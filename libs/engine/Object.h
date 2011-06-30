
#ifndef _H_ENGINE_OBJECT
#define _H_ENGINE_OBJECT

#include <string>   // size_t

namespace engine
{
    
    class Notification;
    
    /**
     
     Base class for any element in the engine library to receive notifications
     
     */
    
    class Object
    {
        // Class the managers this objects
        friend class ObjectsManager;
        
        // Unique identifier of this listener
        size_t engine_id;
        
    public:        
        
        Object();                   // Constructor
        virtual ~Object();          // Destructor. ( virtual destructor is mandatory for generic remov

    public:
        
        // Get my id as listener
        size_t getEngineId();
        
    protected:

        
        // Start listening a particular notification
        void listen( const char* notification_name );

        // Function to send a notification to this object
        virtual void notify( Notification* notification );
        
    };
    
}

#endif
