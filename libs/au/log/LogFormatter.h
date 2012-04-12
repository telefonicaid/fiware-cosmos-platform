

#ifndef _H_AU_LOG_FORMATTER
#define _H_AU_LOG_FORMATTER


namespace au 
{
    
    class LogFormatter
    {
        
        std::string definition;
        std::vector<std::string> fields;
        
    public:
        
        LogFormatter( std::string _definition );        
        std::string get( Log* log );
        
        
    };
}

#endif