
#ifndef _H_ENGINE_OBJECT
#define _H_ENGINE_OBJECT

namespace engine
{
    
    /**
     Base class for all the objects that goes in a notification
     */
    
    class Object
    {
        
    public:        
        virtual ~Object(){};      // Force virtual destrutor for correct release of memory at the end of the notification
    };
    
    
}

#endif
