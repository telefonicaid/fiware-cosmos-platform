
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this data
*/

#ifndef _H_SAMSON_url_ServerPathCount
#define _H_SAMSON_url_ServerPathCount


#include <samson/modules/url/ServerPathCount_base.h>


namespace ss{
namespace url{


	class ServerPathCount : public ServerPathCount_base
	{
	public:
		static bool compare_by_count(const ServerPathCount &spc1, const ServerPathCount &spc2)
		{
			return(spc1.count.value > spc2.count.value);
		}

		static bool compare_by_weight(const ServerPathCount &spc1, const ServerPathCount &spc2)
		{
			return(spc1.weight.value > spc2.weight.value);
		}

		static bool compare_by_weight_and_count(const ServerPathCount &spc1, const ServerPathCount &spc2)
		{
			return(sqrt(spc1.count.value*spc1.weight.value) > sqrt(spc2.count.value*spc2.weight.value));
		}

		bool isEqual( ServerPathCount *serverPath)
		{
			if ((server == serverPath->server) && (!path.value.compare(serverPath->path.value)))
			{
				return true;
			}
			return false;
		}

	};


} // end of namespace ss
} // end of namespace url

#endif
