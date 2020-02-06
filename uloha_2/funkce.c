int max(const int* arr, const int len) {
    int max = arr[len-1], i;
    
    for(i=len-1; i; i--) {
        max = max-((max-arr[i-1])&((max-arr[i-1])>>(sizeof(int)*8-1)));
    }
    
    return max;
}

/* Vysvetleni:
 *	je pouzit trik pro komparaci dvou cisel bitovym posuvem o 31 bitu,
 *	kde je u signed intu ulozeny znamenkovy bit.
 *	A toto je nasledne zpracovano rekurzivne v cyklu for. Vracena je hodnota max
 *
 */
