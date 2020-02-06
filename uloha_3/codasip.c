#include <linux/init.h>		/* __init and __exit macroses */
#include <linux/kernel.h>	/* KERN_INFO macros */
#include <linux/module.h>	/* required for all kernel modules */
#include <linux/moduleparam.h>	/* module_param() and MODULE_PARM_DESC() */

#include <linux/fs.h>		/* struct file_operations, struct file */
#include <linux/miscdevice.h>	/* struct miscdevice and misc_[de]register() */
#include <linux/mutex.h>	/* mutexes */
#include <linux/string.h>	/* memchr() function */
#include <linux/slab.h>		/* kzalloc() function */
#include <linux/sched.h>	/* wait queues */
#include <linux/uaccess.h>	/* copy_{to,from}_user() */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Marek Vitula");
MODULE_DESCRIPTION("Codasip test module");


static unsigned long buffer_size = 128; // 128 chars
module_param(buffer_size, ulong, (S_IRUSR | S_IRGRP | S_IROTH));
MODULE_PARM_DESC(buffer_size, "Internal buffer size");

struct buffer {
	wait_queue_head_t read_queue;
	struct mutex lock;
	char *data, *end;
	char *read_ptr;
	unsigned long size;
};

static struct buffer *buffer_alloc(unsigned long size) 
{
	struct buffer *buf = NULL;

	buf = kzalloc(sizeof(*buf), GFP_KERNEL);
	if (unlikely(!buf))
		goto out;

	buf->data = kzalloc(size, GFP_KERNEL);
	if (unlikely(!buf->data))
		goto out_free;

	init_waitqueue_head(&buf->read_queue);

	mutex_init(&buf->lock);

	/* It's unused for now, but may appear useful later */
	buf->size = size;

 out:
	return buf;

 out_free:
	kfree(buf);
	return NULL;
}

static void buffer_free(struct buffer *buffer)
{
	kfree(buffer->data);
	kfree(buffer);
}

static int codasip_open(struct inode *inode, struct file *file)
{
	struct buffer *buf;
	int err = 0;

	/*
	 * Real code can use inode to get pointer to the private
	 * device state.
	 */

	buf = buffer_alloc(buffer_size);
	if (unlikely(!buf)) {
		err = -ENOMEM;
		goto out;
	}

	file->private_data = buf;

 out:
	return err;
}

static ssize_t codasip_read(struct file *file, char __user * out,
			    size_t size, loff_t * off)
{
	struct buffer *buf = file->private_data;
	ssize_t result;

	if (mutex_lock_interruptible(&buf->lock)) {
		result = -ERESTARTSYS;
		goto out;
	}

	while (buf->read_ptr == buf->end) {
		mutex_unlock(&buf->lock);
		if (file->f_flags & O_NONBLOCK) {
			result = -EAGAIN;
			goto out;
		}
		if (wait_event_interruptible
		    (buf->read_queue, buf->read_ptr != buf->end)) {
			result = -ERESTARTSYS;
			goto out;
		}
		if (mutex_lock_interruptible(&buf->lock)) {
			result = -ERESTARTSYS;
			goto out;
		}
	}

	size = min(size, (size_t) (buf->end - buf->read_ptr));
	if (copy_to_user(out, buf->read_ptr, size)) {
		result = -EFAULT;
		goto out_unlock;
	}

	buf->read_ptr += size;
	result = size;

 out_unlock:
	mutex_unlock(&buf->lock);
 out:
	return result;
}

static ssize_t codasip_write(struct file *file, const char __user * in,
			     size_t size, loff_t * off)
{
	struct buffer *buf = file->private_data;
	ssize_t result;

	if (size > buffer_size) {
		result = -EFBIG;
		goto out;
	}

	if (mutex_lock_interruptible(&buf->lock)) {
		result = -ERESTARTSYS;
		goto out;
	}

	if (copy_from_user(buf->data, in, size)) {
		result = -EFAULT;
		goto out_unlock;
	}

	buf->end = buf->data + size;
	buf->read_ptr = buf->data;

	if (buf->end > buf->data)
		buf->data, buf->end - 1;

	wake_up_interruptible(&buf->read_queue);

	result = size;
 out_unlock:
	mutex_unlock(&buf->lock);
 out:
	return result;
}

static int codasip_close(struct inode *inode, struct file *file)
{
	struct buffer *buf = file->private_data;

	buffer_free(buf);

	return 0;
}

static struct file_operations codasip_fops = {
	.owner = THIS_MODULE,
	.open = codasip_open,
	.read = codasip_read,
	.write = codasip_write,
	.release = codasip_close,
	.llseek = noop_llseek
};

static struct miscdevice codasip_misc_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "codasip",
	.fops = &codasip_fops
};

static int __init codasip_init(void)
{
	if (!buffer_size)
		return -1;

	misc_register(&codasip_misc_device);
	printk(KERN_INFO
	       "codasip device has been registered, buffer size is %lu bytes\n",
	       buffer_size);

	return 0;
}

static void __exit codasip_exit(void)
{
	misc_deregister(&codasip_misc_device);
	printk(KERN_INFO "codasip device has been unregistered\n");
}

module_init(codasip_init);
module_exit(codasip_exit);



