
#ifndef _H_AU_OBJECT
#define _H_AU_OBJECT

#include "au/mutex/Token.h"
#include "au/containers/list.h"

namespace au {

    // Simple retain-relese model for an object ( multi-thread protected )
    
    class Object
    {
        
        Token token; // mutex protection
        
        int retain_counter; // Owner counter
        
    public:
        
        Object() : token("au::Object")
        {
            retain_counter = 1;
        }
        
        virtual ~Object(){};
        
        void release();
        void retain();
        
        int get_retain_counter();
        
        virtual void self_destroy();
        
    };

    template <class C>
    class ObjectContainer
    {
        C* object_;
        
    public:
        
        ObjectContainer()
        {
            object_ = NULL;
        }
        
        ~ObjectContainer()
        {
            if( object_ )
                object_->release();
        }
        
        C* getObject()
        {
            return object_;
        }
        
        void setObject( C * object )
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
        
        void release()
        {
            if( object_ )
            {
                object_->release();
                object_ = NULL;
            }
            
        }
        
    };

    template <class C>
    class ObjectList
    {

    public:

        au::list<C> list; // List of elements
        
        void push_back( C* c )
        {
            c->retain();
            list.push_back( c );
        }
        
        void push_front( C* c )
        {
            c->retain();
            list.push_front( c );
        }
        
        C* front()
        {
            return list.front();
        }
        
        void clear()
        {
            typename au::list<C>::iterator it;
            
            for ( it = list.begin() ; it != list.end() ; it++ )
                (*it)->release();
            
            list.clear();
        }
        
        void pop_front()
        {
            // Relese the front element and remove from list
            list.front()->release();
            list.pop_front();
            
        }
        
        void extract_front( ObjectContainer<C>& container )
        {
            C* c = list.extractFront();
            
            if( !c )
                return;
            
            container.setObject( c ); // Now it is retained by this element
            c->release();
        }

        void extract_back( ObjectContainer<C>& container )
        {
            C* c = list.extractBack();
            
            if( !c )
                return;
            
            container.setObject( c ); // Now it is retained by this element
            c->release();
        }
        
        size_t size()
        {
            return list.size();
        }
        
    };
    
}

#endif