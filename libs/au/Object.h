
#ifndef _AU_OBJECT
#define _AU_OBJECT

#include "logMsg/logMsg.h"

#include "au/namespace.h"
#include "au/Token.h"
#include "au/TokenTaker.h"

NAMESPACE_BEGIN(au)

class Object
{
    
    au::Token token;
    int retain_count;

private:
    
    // Make sure destructor is not called directly.
    // release should be calle instead
    
    
public:
    
    Object() : token("Object")
    {
        retain_count = 1;
    }

    virtual ~Object()
    {        
        //LM_M(("Destroying object"));
        if( retain_count != 0 )
            LM_X(1, ("Error in managing au::Object"));
    }
    
public:
    
    Object* retain()
    {
        // Mutex protection
        au::TokenTaker tt( &token );
        retain_count++;
        return this;
    }
    
    void release()
    {
        
        bool remove_object;
        {
            au::TokenTaker tt( &token );
            retain_count--;
            remove_object = ( retain_count == 0 );
        }

        // Note: Make this outside the scpde of TokenTaker to avoid operations over internal data after removing object
        if( remove_object )
            delete this;
    }
    
    Object* auto_release();
    
    
    int getRetainCount()
    {
        return retain_count;
    }
    
};



NAMESPACE_END

#endif

