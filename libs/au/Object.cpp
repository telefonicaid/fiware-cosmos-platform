
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

    
    ObjectContainer::ObjectContainer()
    {
        object_ = NULL;
    }
    
    ObjectContainer::~ObjectContainer()
    {
        if( object_ )
            object_->release();
    }
    
    Object* ObjectContainer::getObject()
    {
        return object_;
    }
    
    void ObjectContainer::setObject( Object * object )
    {
        // If we have internally the same buffer, do nothing
        if( object == object_ )
            return;
        
        // If previous buffer, just release
        if( object_ )
            object_->release();
        
        // If no input buffer, do nothing
        if( !object )
            return;
        
        // Keep a retained copy of this
        object_ = object;
        object_->retain();
    }
    
    void ObjectContainer::release()
    {
        if( object_ )
        {
            object_->release();
            object_ = NULL;
        }
    }
    

    
}