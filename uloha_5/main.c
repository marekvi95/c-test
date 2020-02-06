#include <stdio.h>

const char * query_conf_string(const char * config_string_addr, const char * query_string)
{

    char *newstr = malloc(strlen(config_string_addr)+1);
    char *np = newstr, *op = config_string_addr;

    // vymazeme mezery

    do
    {
        if (*op != ' ')
            *np++ = *op;
    }
    while (*op++);

    //printf("%s",newstr);
    char *p1, *p2, *p3;
    int i=0,j=0,flag=0;

    p1 = newstr;
    p2 = query_string;

    // vyhledame substring

    for(i = 0; i<strlen(newstr); i++)
    {
        if(*p1 == *p2)
        {
            p3 = p1;
            for(j = 0; j<strlen(query_string); j++)
            {
                if(*p3 == *p2)
                {
                    p3++;
                    p2++;
                }
                else
                    break;
            }
            p2 = query_string;

            if(j == strlen(query_string))
            {

                flag = 1;
             
                int index = i+strlen( query_string), index2=0;
                char *rtr = malloc(strlen(newstr)+1);

                while(newstr[index]!=';')
                {
                    //printf("%c",newstr[index]);
                    rtr[index2] = newstr[index];
                    index++;
                    index2++;
                }

                rtr[index2+1] = '\0';
                printf("%s ", rtr);
                free(newstr);
                return rtr;
            }
        }
        p1++;


    }
    if(flag==0)
    {
        free(newstr);
        return NULL;
    }
}

char* str =
    "rtc {\
        addr 0x48001000;\
    }\
\
    ram {\
        0 {\
            addr 0x80000000;\
            size 0x00100000;\
        };\
    };\
\
    core {\
        0 {\
            0 {\
                isa rv32im;\
                timecmp 0x48001008;\
            };\
        };\
    }";

//char* sub = "core{0{0{isa";


int main()
{

    printf("%s ",query_conf_string(str,"core{0{0{isa"));

    return 0;
}


