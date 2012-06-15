
#include <stdlib.h> // malloc
#include "logMsg/logMsg.h"
#include "KVFile.h" // Own interface

namespace samson {

KVFile::KVFile( char *_data )
{
    if (_data == NULL)
    {
        info = NULL;
        data = NULL;
        offsets = NULL;
        LM_E(("NULL _data"));
        error.set("NULL _data");
        return;
    }
    // Keep a pointer to data
    data   = _data;

    // Keep a pointer to the header
    header = (KVHeader*) data;

    // Check valid header
    if( !header->check() )
    {
        info = NULL;
        data = NULL;
        offsets = NULL;
        LM_W(("Not valid KVHeader when creating a KVFile"));
        error.set("KVHeader error: wrong magic number");
        return;
    }

    // Create the key-value data
    info   = createKVInfoVector(_data , &error);

    if( info )
    {
        // Vector containing offsets to each hash-group
        offsets = (size_t *) malloc( sizeof(size_t) * KVFILE_NUM_HASHGROUPS );

        offsets[0] = sizeof( KVHeader );
        for (int i = 1 ; i < KVFILE_NUM_HASHGROUPS ; i++ )
            offsets[i] = offsets[i-1] + info[i-1].size;

    }
    else
    {
        offsets = NULL;
    }
}

KVFile::~KVFile()
{
    if( info )
    {
        free( info );
        info = NULL;
    }

    if( offsets )
    {
        free(offsets);
        offsets = NULL;
    }
}

// Get pointer to data for a particular hash-group
char * KVFile::dataForHashGroup( int hg )
{
    return data + offset(hg);
}

KVInfo KVFile::getKVInfoForHashGroup( int hg )
{
    if( !info )
    {
        return KVInfo(0,0);
    }
    else
        return info[hg];
}

KVHeader* KVFile::getKVHeader()
{
    return header;
}

size_t KVFile::offset( int hg )
{
    return offsets[hg];
}

}
