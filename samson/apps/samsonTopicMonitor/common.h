

#ifndef _H_SAMSON_TOPIC_MONITOR_COMMON
#define _H_SAMSON_TOPIC_MONITOR_COMMON

#include <string>

class Topic
{
public:
    
    std::string concept;
    size_t num;
    
    Topic( std::string _concept , size_t _num )
    {
        concept = _concept;
        num = _num;
    }
    
};

#endif