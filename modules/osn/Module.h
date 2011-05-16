

/*

	Module OSN_data analysis  module (osn)

	File: /home/jges/samson_modules/osn/Module.h
	NOTE: This file has been generated with the samson_module tool, please do not modify

*/



#ifndef _H_SS_osn_Module
#define _H_SS_osn_Module



#include <samson/samsonModuleHeader.h>
#include <samson/modules/osn/Date.h>
#include <samson/modules/osn/Time.h>
#include <samson/modules/osn/URL.h>
#include <samson/modules/osn/URLConnection.h>
#include <samson/modules/osn/Count.h>
#include <samson/modules/osn/CountF.h>
#include "operations/parse_urls.h"
#include "operations/map_urls_to_kv_to_compute_sites_visited.h"
#include "operations/map_site_user.h"
#include "operations/map_url_user.h"
#include "operations/map_urls_by_hour.h"
#include "operations/map_applications.h"
#include "operations/map_extract_logs_by_useragent.h"
#include "operations/map_filter_in_by_status.h"
#include "operations/map_filter_out_by_status.h"
#include "operations/red_compute_sites_visited.h"
#include "operations/red_select_N_sites.h"
#include "operations/red_compute_total_elements.h"
#include "operations/red_compute_total_and_set.h"
#include "operations/export_connections.h"
#include "operations/export_report.h"
#include "operations/export_N_sites.h"
#include "operations/export_total.h"
#include "operations/Bytemobile_req_1.h"
#include "operations/Bytemobile_req_2.h"
#include "operations/Bytemobile_req_3.h"
#include "operations/Bytemobile_req_4.h"
#include "operations/Bytemobile_req_5.h"
#include "operations/Bytemobile_req_6.h"
#include "operations/Bytemobile_req_7.h"


namespace ss{
namespace osn{

// Module definition

	class Module : public ss::Module
	{
	public:
		Module();

		std::string help(){
			std::ostringstream o;
			return o.str();
		}
	};


} // end of namespace osn
} // end of namespace ss



#endif
