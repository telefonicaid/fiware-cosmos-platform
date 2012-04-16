

#include "Filter.h" // Own interface


namespace samson{
    namespace system{

        
        
        SamsonTokenizer::SamsonTokenizer()
        {
            addSingleCharTokens("()[]{}<> ;&|?:,+-*/'|");
            addToken(":[");
            addToken("<=");
            addToken(">=");
            addToken("==");
            addToken("!=");

            addToken(" -only_key ");
            addToken("select");
            addToken("emit_key");
            addToken("emit_value");
            
        }


      
        
    }
}