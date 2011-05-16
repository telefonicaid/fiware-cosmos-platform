
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_url_benchmark_map_visits
#define _H_SAMSON_url_benchmark_map_visits


#include <samson/samson.h>

/****************************************************************
map map_visits
{
	in system.UInt system.String  # UserId - URL visited
	out system.String system.String   # ServerName - Path (URL without ServerName)
}
*****************************************************************/

namespace ss{
namespace url_benchmark{


	class map_visits : public ss::Map
	{

            ss::system::UInt user;
            ss::system::String url;
            ss::system::String serverName;
            ss::system::String path;

	public:


		void run(  ss::KVSetStruct* inputs , ss::KVWriter *writer )
		{

            bool found;

            for (size_t i = 0; (i < inputs[0].num_kvs); ++i)
            {
                    // Parse the input of each input key-value
            	user.parse(inputs[0].kvs[i]->key);
            	url.parse(inputs[0].kvs[i]->value);

            	char *url_c = strdup(url.value.c_str()); /* To be realeased in the emit->writer? */
            	char *p_server;
            	char *p_path;

            	if (!strncmp(url_c, "http://", strlen("http://")))
            	{
            		p_server = url_c + strlen("http://");
            	}
            	else if (!strncmp(url_c, "https://", strlen("https://")))
            	{
            		p_server = url_c + strlen("https://");
            	}
            	else
            	{
            		p_server = url_c;
            	}

            	if ((p_path = strchr(p_server, '/')) != NULL)
            	{
            		*p_path = '\0';
            		p_path++;
            	}
            	else
            	{
            		OLM_E(("Error, badly formed URL:%s", url_c));
			free(url_c);
            		return;
            	}

            	serverName.value = p_server;
            	path.value = p_path;

                writer->emit( 0 , &serverName, &path);

            }
		}


	};


} // end of namespace ss
} // end of namespace url_benchmark

#endif
