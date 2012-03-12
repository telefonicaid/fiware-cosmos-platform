

#include "Filter.h" // Own interface


namespace samson{
    namespace system{

        const char* str_Comparisson( Comparisson c )
        {
            switch (c) {
                case equal:                  return "==";
                case greater_than:           return ">";
                case less_than:              return "<";
                case greater_or_equal_than:  return ">=";
                case less_or_equal_than:     return "<=";
                case different_than:         return "!=";
                case unknown:                return "?";
            }
            
            LM_X(1, ("Internal error"));
            return "Unknown";
        }
        
        Comparisson comparition_from_string( std::string s )
        {
            Comparisson c = unknown;
            
            if( s == "==" )
                c = equal;
            else if( s == "<" )
                c = less_than;
            else if( s == ">" )
                c = greater_than;
            else if( s == ">=" )
                c = greater_or_equal_than;
            else if( s == "<=" )
                c = less_or_equal_than;
            else if( s == "!=" )
                c = different_than;
            
            return c;
        }
        
        SamsonTokenVector::SamsonTokenVector()
        {
            addSingleCharTokens("()[]{}<> ;&|:");
            addToken(":[");
            addToken("<=");
            addToken(">=");
            addToken("==");
            addToken("!=");

        }


      
        
    }
}