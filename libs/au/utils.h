#ifndef _H_AU_UTILS
#define _H_AU_UTILS

namespace au {
    
    template<typename T,typename T2>
    void replaceIfLower(T &t,T2 v)
    {
        if( (T)v < t )
            t = v;
    }
    
    template<typename T,typename T2>
    void replaceIfHiger(T &t,T2 v)
    {
        if( (T)v > t )
            t = v;
    }
    
    int getColumns();
    void clear_line();
    
    
    
    
}

#endif
