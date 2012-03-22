#include <string>
#include <vector>


class Service
{

    std::vector <std::string> httpPatterns;
    std::vector <std::string> dnsPatterns;

public:
    
    std::string name;
    int serviceId;
    
    Service( std::string _name, int _serviceId )
    {
        name = _name;
        serviceId = _serviceId;
    }
    
    void addHTTPPattern(std::string _httpPattern)
    {
        httpPatterns.push_back(_httpPattern);
    }

    void addDNSPattern(std::string _dnsPattern)
    {
        dnsPatterns.push_back(_dnsPattern);
    }
    
    bool checkHTTP(std::string url);

    bool checkDNS(std::string dns);
    
    
};

