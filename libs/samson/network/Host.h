#ifndef HOST_H
#define HOST_H

/* ****************************************************************************
*
* FILE                     Host.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Feb 16 2011
*
*/



/* ****************************************************************************
*
* Host - 
*/
class Host
{
    public:
    
	char* name;
	char* ip;
	char* alias[10];

    bool match( const char* _ip )
    {
        if (ip == NULL)
            return NULL;
        
        if (ip[0] == 0)
            return NULL;
        
        if ((name != NULL) && (strcmp(name, ip) == 0))
            return true;
        
        if ((ip != NULL) && (strcmp(_ip, ip) == 0))
            return true;
        
        for (unsigned int aIx = 0; aIx < sizeof(alias) / sizeof(alias[0]); aIx++)
        {
            if (alias[aIx] == NULL)
                continue;
            
            if (strcmp(alias[aIx], ip) == 0)
                return true;
        }
        
        return false;
    }
    
} ;

#endif
