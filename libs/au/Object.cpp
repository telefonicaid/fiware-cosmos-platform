
#include "au/ObjectPool.h"

#include "au/Object.h"  // Own interface

NAMESPACE_BEGIN(au)

ObjectPool pool; // Common pool protected with multithread for auto-release


Object* Object::auto_release()
{
    
    // Add to the pool
    pool.add( this );
    
    {
        // Mutex protection
        au::TokenTaker tt( &token );
        
        // One less in the count
        retain_count--;
    }
    
    // NOTE: it is not necessary to check for remove since it is included in the pool
    
    return this;
}

NAMESPACE_END
