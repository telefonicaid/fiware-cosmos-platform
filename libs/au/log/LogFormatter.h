

#ifndef _H_AU_LOG_FORMATTER
#define _H_AU_LOG_FORMATTER


#define DEFAULT_FORMAT "TYPE : date : time : EXEC : FILE[LINE] : FUNC : TEXT"

namespace au 
{
    
    class LogFormatter
    {
        
        std::string definition;
        std::vector<std::string> fields;
        
    public:
        
        LogFormatter( std::string _definition = DEFAULT_FORMAT );        
        std::string get( Log* log );
        
    };
}

#endif