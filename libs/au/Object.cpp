
#include "au/mutex/TokenTaker.h"

#include "Object.h" // Own interface


namespace au {

    
    
    void Object::release()
    {
        bool run_self_destroy = false;
        
        {
            TokenTaker tt(&token);
            retain_counter--;
            if( retain_counter == 0 )
                run_self_destroy =true;
        }
        
        if( run_self_destroy )
            self_destroy();
    }
    
    void Object::retain()
    {
        TokenTaker tt(&token);
        retain_counter++;
    }
    
    void Object::self_destroy()
    {
        delete this;
    }
    
    int Object::get_retain_counter()
    {
        return retain_counter;
    }

    
    

    
}