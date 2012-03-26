


#include "au/string.h"
#include "au/Console.h"
#include "au/ConsoleAutoComplete.h"

#include "au/charset.h"

int main()
{
    
    for ( int i = 0 ; i < 256 ; i++ )
    {

        if( au::iso_8859_is_letter( i ) )
            printf(" %s," , au::str("'%c'", i ).c_str() );
        else
            printf(" %3d," , i);
        
        if( i > 0 )
            if( ((i+1)%32) == 0 )
                printf("     \\\n");
        
    }

    return 0;
}


