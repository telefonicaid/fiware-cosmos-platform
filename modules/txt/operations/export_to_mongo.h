/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_txt_export_to_mongo
#define _H_SAMSON_txt_export_to_mongo


#include <samson/module/samson.h>
#include "mongo/client/dbclient.h"

using namespace mongo;

namespace samson{
namespace txt{


	class export_to_mongo : public samson::Map
	{

	  std::string          mongo_ip;
	  std::string          mongo_table;
	  DBClientConnection*  mdbConnection;

	public:


#ifdef INFO_COMMENT //Just to include a comment without conflicting anything
// If interface changes and you do not recreate this file, consider updating this information (and of course, the module file)

input: system.String system.UInt  

helpLine: Export data to a mongoDb server at 'mongo.ip' to table 'mongo.table'
#endif // de INFO_COMMENT

		void init(samson::KVWriter* writer)
		{
			// recover parameter from enviroment
			mongo_ip    = environment->get("mongo.ip",    "no-mongo-ip");
			mongo_table = environment->get("mongo.table", "no-mongo-table");

			mdbConnection = new DBClientConnection();
			mdbConnection->connect(mongo_ip);
		}

		void run(samson::KVSetStruct* inputs, samson::KVWriter* writer)
		{
			if (inputs[0].num_kvs == 0)
				return;

			if (mongo_ip == "no-mongo-ip")
			{
				tracer->setUserError("No mongo ip specified. Please specify mongo ip with 'mongo.ip' environment variable");
				return;
			}

			if (mongo_table == "no-mongo-table")
			{
				tracer->setUserError("No table specified. Please specify mongo table with 'mongo.table' environment variable");
				return;
			}


			// Access data and emit with "writer" object
			BSONObj bo;

			for (size_t i = 0 ; i < inputs[0].num_kvs ; i++)
			{
				samson::system::String  key;
				samson::system::UInt    value;

				key.parse(inputs[0].kvs[i]->key);
				value.parse(inputs[0].kvs[i]->value);

				bo = BSON("key" << key.value << "value" << (long long) value.value);

				mdbConnection->insert(mongo_table, bo);
			}

			mdbConnection->ensureIndex(mongo_table, fromjson("{key:0}"));
		}

		void finish(samson::KVWriter* writer)
		{
			delete mdbConnection;
		}



	};


} // end of namespace txt
} // end of namespace samson

#endif
