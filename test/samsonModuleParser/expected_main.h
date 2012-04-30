

/*

	Module  (example)

	File: main.h
	NOTE: This file has been generated with the samson_module tool, please do not modify

*/



#ifndef _H_SS_example_Module
#define _H_SS_example_Module



#include <samson/module/samson.h>
#include <samson/modules/example/MyData.h>
#include <samson/modules/example/ExampleData.h>
#include "operations/generator.h"
#include "operations/generator_error_operation_crashes.h"
#include "operations/generator_error_timeout.h"
#include "operations/generator_error_user_generated.h"
#include "operations/generator_error_wrong_data_type.h"
#include "operations/select.h"
#include "operations/map.h"
#include "operations/reduce.h"
#include "operations/reduce2.h"
#include "operations/test.h"
#include "operations/generator2.h"
#include "operations/parser.h"
#include "operations/script.h"
#include "operations/error.h"
#include "operations/map_error.h"
#include "operations/map_error_user.h"
#include "operations/map_traces.h"
#include "operations/generator_list.h"
#include "operations/map_list.h"
#include "operations/splitter.h"


namespace samson{
namespace example{

// Module definition

	class Module : public samson::Module
	{
	public:
		Module();

		std::string help(){
			std::ostringstream o;
			return o.str();
		}
	};


} // end of namespace example
} // end of namespace samson



#endif
