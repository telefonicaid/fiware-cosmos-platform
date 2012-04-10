

#ifndef _H_AU_LOG_FORMATTER
#define _H_AU_LOG_FORMATTER


namespace au 
{
    
    class LogFormatter
    {
        
        std::vector<std::string> fields;
        
    public:
        
        LogFormatter( std::string definition );        
        std::string get( Log* log );
        
        
    };
}

#endif