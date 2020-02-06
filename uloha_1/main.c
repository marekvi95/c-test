#include <stdio.h>
#include <stdlib.h>
#include <math.h>

static char *units[] =
{
    "", "jeden", "dve", "tri", "ctyri",
    "pet", "sest", "sedm", "osm", "devet"
};

static char *teens[] =
{
    "deset", "jedenact", "dvanact", "trinact", "ctrnact",
    "patnact", "sestnact", "sedmnact", "osmnact", "devatenact"
};

static char *decades[] =
{
    "", "", "dvacet", "tricet", "ctyricet", "padesat", "sedesat",
    "sedmdesat", "osmdesat", "devadesat"
};

static char *magnitudes[] =
{
    "", "tisic", "milionu", "miliard",
};


// vrati pocet digitu v cisle

int getDigits(long val)
{
    return (int) (floor( log10( (double) abs( val )))) + 1;
    
}

// rozdelime cisla na skupiny po trech 

void printCentury( int val )
{
    if ( val > 99 )
    {
        int cent = val / 100;
        if (cent == 1)
            printf( "%ssto ", units[cent] );
        if (cent == 2)
            printf( "%sste ", units[cent] );
        if (cent == 3 || cent == 4)
            printf( "%ssta ", units[cent] );
        else if (cent > 4 && cent < 10)
            printf( "%sset ", units[cent] );
        val -= cent * 100;

    }

    if ( val > 19 )
    {
        printf( "%s", decades[val / 10] );
        if ( val % 10 > 0 )
            printf( "-%s", units[val % 10] );

        else
            putchar(' ');
    }

    else if ( val >= 10 )
    {
        printf( "%s", teens[val % 10] );
    }

    else if ( val > 0 )
    {
        printf( "%s", units[val] );
    }
}

// parametr main je cislo ktere chceme prevest, max velikost je 999 999 999 999 

int main( int argc, char **argv )
{
    long val = 0;

    if(argc > 1)
    {
        val = strtol( argv[1], NULL, 10 ); 
    }

    else
    {
        fprintf( stderr, "Pouziti: %s <cislo>\n", argv[0] );
        return 0;
    }

    if (val < 0)
    {
        printf( "minus " );
        val = -val;
    }

    if ( val == 0 )
        printf( "nula" );


    else
    {
        char *sep = "";

        while ( val > 100L )
        {
            int digits = getDigits( val );
            int mag = (digits - 1) / 3;
            long order = (long) ( pow (10.0, 3 * mag )); // rad cisla
            long cent = val / order;
            printf( "%s", sep );
            sep = " ";
            printCentury( (int) cent );
            printf( " %s", magnitudes[mag] );
            val -= cent * order;
        }

    }

    putchar('\n');
    return 0;

}

