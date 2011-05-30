#include "Object.h" // Own interface
#include <string>
namespace au
{
    
    void Object::add( const char *name , Runner* runner  )
    {
        runners.insertInMap( name , runner );
    }
    
    void Object::run( const char * name )
    {
        Runner *runner = runners.findInMap( name );
        if( runner )
            runner->run();
    }    
    
}