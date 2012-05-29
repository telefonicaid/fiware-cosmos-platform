#ifndef _H_SAMSON_SETUP
#define _H_SAMSON_SETUP

#include <map>				// std::map
#include <string>			// std::string
#include <iostream>			// std::cout
#include <stdlib.h>         // atoll
#include <sys/stat.h>		// mkdir

#include "logMsg/logMsg.h"

#include "au/containers/map.h"         // au::map
#include "au/containers/StringVector.h"
#include "au/Environment.h" // au::Environment

#include "samson/common/status.h"


NAMESPACE_BEGIN(au)
class ErrorManager;
}

namespace samson 
{

Status createDirectory( std::string path );
Status createFullDirectory( std::string path );
std::string cannonical_path( std::string path );

typedef enum
{
    SetupItem_uint64,
    SetupItem_string
} SamsonItemType;

class SetupItem
{
        std::string name;
        std::string default_value;

        std::string value;

        std::string description;
        SamsonItemType type;

    public:

        SetupItem( std::string _name , std::string _default_value , std::string _description , SamsonItemType _type );

        bool setValue( std::string _value );
        bool check_valid( std::string _value );        
        std::string getValue();
        std::string getDefaultValue();
        std::string getSetValue();
        std::string getDescription();
        std::string getConcept();
        void resetToDefaultValue();
        void clearCustumValue();
};


class SetupItemCollection
{
    protected:

        au::map< std::string , SetupItem > items;

        // Add a new parameter to consider
        void  add( std::string _name , std::string _default_value , std::string _description , SamsonItemType type );

    public:

        ~SetupItemCollection();

        // Load a particular file to include all setup parameters
        void load( std::string file );

        // get the value of a parameter
        std::string getValueForParameter( std::string name );

        // Set manually a particular parameter ( special case )
        bool setValueForParameter( std::string name ,std::string value );

        // Check if a particular property if defined
        bool isParameterDefined( std::string name );

        // Come back to default parameters
        void resetToDefaultValues();

        std::string str();

};


class SamsonSetup : public SetupItemCollection
{
        SamsonSetup( std::string samson_home , std::string samson_working );

        std::string _samson_home;           // Home directory for SAMSON system
        std::string _samson_working;        // Working directory for SAMSON system

    public:

        static void init( std::string samson_home , std::string samson_working );
        static void destroy();
        static SamsonSetup *shared();

        // Used only in unitTests, to have them more complete
        void addItem(std::string _name , std::string _default_value , std::string _description , SamsonItemType type);

        // Get access to parameters
        std::string get( std::string name );
        size_t getUInt64( std::string name );
        int getInt( std::string name );

        std::string get_default( std::string name );


        // Get names fo files
        std::string setupFileName();                         // Get the Steup file
        std::string sharedMemoryLogFileName();
        std::string modulesDirectory();
        std::string blocksDirectory();

        std::string blockFileName( size_t worker_id , size_t id );
        bool blockIdFromFileName( std::string fileName , size_t* worker_id , size_t *id );

        std::string streamManagerLogFileName();

        std::string clusterInformationFileName();

        std::vector<std::string> getItemNames();


        // Create working directories
        void createWorkingDirectories();

        // Clear values specified manually
        void clearCustumValues();

        int save();

};

}

#endif
