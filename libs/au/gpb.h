#ifndef _H_AU_GPB
#define _H_AU_GPB


namespace au
{
    
    
    struct GPBHeader
    {
        int magic;
        ssize_t size;
 
        void init( ssize_t _size )
        {
            size = _size;
            magic = 8537465;
        }
        
        bool check( )
        {
            return ( magic == 8537465 );
        }
        
    };

    
    
    // Read a google protocol buffer message    
    
    template <class T>
    int readGPB( int fd , T** t , int time_out )
    {
        
        //LM_M(("Reading a GPB message from fd %d ", fd ));
        
        int iom = iomMsgAwait( fd , time_out, 0);// Wait until this is ready
        
        if (iom != 1)
            return 1;   // Error in the isolated process
        
        GPBHeader header;
        int nb = read( fd , &header , sizeof(header) );

        if( nb == 0)
            return 7;
        
        if( nb < 0)
            return 8;

        if( !header.check() )
            return 8;
        
        // If not received the rigth size, return NULL
        if (nb != sizeof(header))
            return 2;   // Error reading the size
        
        //LM_M(("Reading a GPB message from fd %d (size %d)", fd , (int) size ));
        
        if( header.size > 1000000)
		{
		   LM_W(("Large size %l for a background process message",header.size));
           //return 3;    // Too much bytes to read
		}
        
        void *data  = (void*) malloc( header.size );
        
        nb = read( fd , data , header.size );
        
        if( nb != header.size )
            return 5;
        
        *t = new T();
        bool parse = (*t)->ParseFromArray( data , header.size );
        
        free( data );
        
        if( !parse )
        {
            LM_W(("Error parsing a GPB message of %d bytes", header.size));
            delete (*t);
            return 4; // Error parsing the document
        }
        
        return 0;   // Correct
        
    }
    
    // Write a google protocol buffer message    
    
    template <class T>
    int writeGPB( int fd , T* t  )
    {
        //LM_M(  ("Writing a GPB message to fd %d ( Size: %d )", fd , (int) t->ByteSize() ));
        
        if( !t->IsInitialized()  )
            return 9;
        
        GPBHeader header;
        header.init(  t->ByteSize() );
        
        if ( header.size > 100000 )
		{
		   LM_W(("Large size %l for a background process message",header.size));
           //return 3;
		}
        
        void *data = (void*) malloc( header.size );
        
        bool serialize = t->SerializeToArray( data , header.size );
        
        if( !serialize )
        {
            free( data );
            return 4;
        }
        
        // If not received the rigth size, return NULL
        int nw = write( fd , &header , sizeof(header) );
        if (nw != sizeof(header))
        {
            free( data );
            return 2;
        }
        
        // If not received the rigth size, return NULL
        nw = write( fd , data , header.size );
        if (nw != header.size)
        {
            free( data );
            return 5;
        }

        free( data );
        
        return 0;
    } 
    
}

#endif
