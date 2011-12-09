

#include "engine/EngineService.h"							// Own interface

NAMESPACE_BEGIN(engine)

std::vector<EngineService*> engine_services;

void destroy_engine_services()
{
    
    for ( size_t i = 0 ; i < engine_services.size() ; i++ )
    {
        EngineService * service = engine_services[engine_services.size()-1-i];
        LM_V(("Destroying EngineServide '%s'" , service->getEngineServiceName().c_str() ));
        delete service;
    }
    
    // Clear vector
    engine_services.clear();
}

EngineService::EngineService( std::string _name )
{
    name = _name;
    LM_V(("Init EngineService '%s'" , name.c_str()));
    engine_services.push_back(this);
}

NAMESPACE_END