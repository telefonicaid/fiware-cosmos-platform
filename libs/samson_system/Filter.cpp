

#include "Filter.h" // Own interface


namespace samson{
    namespace system{

        
        
        SamsonTokenVector::SamsonTokenVector()
        {
            addSingleCharTokens("()[]{}<> ;&|?:,+-*/'");
            addToken(":[");
            addToken("<=");
            addToken(">=");
            addToken("==");
            addToken("!=");

        }


      
        
    }
}