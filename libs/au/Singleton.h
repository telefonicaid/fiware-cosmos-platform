
#include "au/mutex/Token.h"
#include "au/mutex/TokenTaker.h"

#ifndef _H_AU_SINGLETON
#define _H_AU_SINGLETON


namespace au {
    
    template< class C >
    class Singleton
    {
        static au::Token token;
        static C* instance;
        
    public:
        
        static C* shared()
        {
            au::TokenTaker tt(&token);
            if( !instance )
                instance = new C();
            return instance;
        }
        
    };
    
    // Static members
    template <class C> C* Singleton<C>::instance;
    template <class C> au::Token Singleton<C>::token("singleton");
    
} // end of au namesapce

#endif
