
#include "SamsonComscoreDictionary.h" // Own interface
#include <algorithm>

bool match( const char* s1 , const char* s2 )
{
    bool m = ( fnmatch( s1 , s2 , 0 ) == 0 );
    
    /*
     if ( m )
     LM_M(("TRUE Comparing '%s' & '%s' " , s1 , s2));
     else
     LM_M(("FALSE Comparing '%s' & '%s'" , s1 , s2));
     */
    return m;
}


namespace samson {
    namespace comscore {
        
        URL::URL( std::string url )
        {
            
            size_t pos = url.find("/");
            
            // Divide in path and domain
            if( pos == std::string::npos )
            {
                domain = url;
                path = "";
            }
            else
            {
                domain = url.substr( 0 , pos );
                path = url.substr( pos+1 );
            }
            
            // Divide domain in core and 
            size_t pos2 = std::string::npos;
            size_t tmp_pos = domain.find_last_of(".");
            if( tmp_pos != std::string::npos )
                pos2 = domain.find_last_of("." , tmp_pos - 1 );
            
            if ( pos2 == std::string::npos )
            {
                pre_domain = "";
                core_domain = domain;
            }
            else
            {
                pre_domain = domain.substr( 0 , pos2 );
                core_domain = domain.substr( pos2+1 );
            }
            
            
            // toUpper for all strings
            std::transform(domain.begin(), domain.end(),domain.begin(), ::toupper);
            std::transform(pre_domain.begin(), pre_domain.end(),pre_domain.begin(), ::toupper);
            std::transform(core_domain.begin(), core_domain.end(),core_domain.begin(), ::toupper);
            std::transform(path.begin(), path.end(),path.begin(), ::toupper);
            
        }        
        
        
        
        
    }
}
