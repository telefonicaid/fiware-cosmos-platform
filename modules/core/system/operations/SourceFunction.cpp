

#include "SourceFunction.h" // Own interface


namespace samson {
    namespace system
    {

        SourceFunction* SourceFunction::getSourceForFunction( std::string function_name , au::vector<Source>& _input_sources , au::ErrorManager * error )
        {
            SourceFunction* source_function = NULL;
            
            if( function_name == "strlen" )
                source_function = new SourceFunctionStrlen();
            else if( function_name == "str" )
                source_function = new SourceFunctionStr();
            else if (function_name == "isAlpha")
                source_function = new SourceFunctionisAlpha();
            
            if ( source_function )
            {
                source_function->setInputSource( _input_sources );
                return source_function;
            }
            
            error->set(au::str("Unkown function %s" , function_name.c_str() ));
            return NULL;
            
        }
        
    }
}