

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
            }
            
            LM_X(1, ("Internal error"));
            return "Unknown";
        }
        
    }
}