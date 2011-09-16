
/* ****************************************************************************
 *
 * FILE            DelilahUtils.h
 *
 * AUTHOR          Andreu Urruela
 *
 * PROJECT         Samson
 *
 * DATE            8/31/11
 *
 * DESCRIPTION
 *
 *
 * COPYRIGTH       Copyright 2011 Andreu Urruela. All rights reserved.
 *
 * ****************************************************************************/

#ifndef _H_Samson_DelilahUtils
#define _H_Samson_DelilahUtils

#include <string>          // std::string
#include "pugi/pugi.h"      // pugi::...

namespace samson {
    
    std::string getBLockListInfo( pugi::node node );
    std::string getStreamOperationInfo( const pugi::xml_node& node );
    std::string getBlockInfo( const pugi::xml_node& queue );
    std::string getQueueInfo( const pugi::xml_node& queue );
    std::string getFormatInfo( const pugi::xml_node& node );
    std::string getDataInfo( const pugi::xml_node& node );
    std::string getOperationInfo( const pugi::xml_node& node );
    std::string getTaskInfo( const pugi::xml_node& node );
    std::string getWorkerTaskInfo( const pugi::xml_node& node );
    std::string getJobInfo( const pugi::xml_node& node );
    std::string getModuleInfo( const pugi::xml_node& node );
    std::string getNetworkInfo( const pugi::xml_node& node );
    std::string getEngineSystemInfo( const pugi::xml_node& node );
    std::string getEngineSimplifiedSystemInfo( const pugi::xml_node& node );
    std::string getSetInfo( const pugi::xml_node& queue );
    std::string getQueueTaskInfo( const pugi::xml_node& queue_task );
    std::string getKVInfoInfo( const pugi::xml_node& node );
    std::string getUpdateTimeInfo( const pugi::xml_node& node ); 
    std::string getBlockManagerInfo( const pugi::xml_node& node ); 
    std::string getOperationRatesInfo( const pugi::xml_node& node ); 
    
    
}

#endif
