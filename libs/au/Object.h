#ifndef _H_AU_OBJECT
#define _H_AU_OBJECT

#include "au/map.h"     // au::map and au::smap

namespace au
{
    
    // Generic class to run commands
    
    class Runner
    {
    public:
        virtual void run()=0;
            
    };
    
    // Generic runner to run a specific method of a class
    
    template<class T>
    class ClassRunner : public Runner
    {
        T* t;
        void (T::*f) (void);
        
    public:
        
        ClassRunner( T* _t , void (T::*_f) (void) )
        {
            t = _t;
            f = _f;
        }
        
        void run()
        {
            (t->*f)();
        }
    };
    
    // Generic object to hold Runners to be executed
    class Object
    {
        // String-based map of Runners
        map<const char*,Runner , au::strCompare> runners;
        
    public:
        
        Object()
        {
        }
        
        // Add the runner for a paricular "message"
        void add( const char *name , Runner* runner  );

        // Add particular 
        template<class T>
        void add( const char * name , T*t, void (T::*f) (void) )
        {
            add( name , new ClassRunner<T>( t , f ) );
        }
        
        // Run a message
        void run( const char * name );
        
    };
    

    
    // Example of a derived class
    
    class Derived : public Object
    {
        
    public:
        
        Derived()
        {
            // Add something to execute by "name"
            add("run", this , &Derived::runSomething2 );
        }
        
        void runSomething2()
        {
            printf("runSomething2 method");
        }
        
        
        
    };
    
    
}

#endif
