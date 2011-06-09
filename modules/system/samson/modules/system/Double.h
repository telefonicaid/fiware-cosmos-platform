
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this data
*/

#ifndef _H_SAMSON_system_Double
#define _H_SAMSON_system_Double


#include <samson/modules/system/FixedLengthDataInstance.h>

namespace samson{
namespace system{


	class Double : public FixedLengthDataInstance<double>
	{
	public:
		int hash(int max_num_partitions)
		{
			return abs((int) 1000*value) % max_num_partitions;
		}

		int *getDataPath(const std::string &dataPathString){
			return(getDataPathStatic(dataPathString));
		}

		static int *getDataPathStatic(const std::string &dataPathString){
			const char *dataPathCharP = dataPathString.c_str();
			int nlevels = 1;
			int *dataPathIntP;

			const char *p_sep = dataPathCharP;
			while ((p_sep = strchr(p_sep, '.')) != NULL)
			{
				nlevels++;
				p_sep++;
			}

			if ((dataPathIntP = (int *)malloc((nlevels + 1)*sizeof(int))) == NULL)
			{
				return ((int *)NULL);
			}

			int retError = getDataPath(dataPathCharP, dataPathIntP);

			if (retError)
			{
				free(dataPathIntP);
				dataPathIntP = NULL;
			}
			return  (dataPathIntP);
		}

		static int getDataPath(const char * dataPathCharP, int *dataPathIntP){
			if (*dataPathCharP == 0)
			{
				*dataPathIntP = -1;
				return (0);
			}

			if (strcmp(dataPathCharP, "Double") == 0)
			{
				*dataPathIntP = -1;
				return (0);
			}
			return -1;
		}

		std::string getTypeFromPath(const std::string &dataPathString){
			const char *dataPathCharP = dataPathString.c_str();
			return(getTypeFromPathStatic(dataPathCharP));
		}

		static std::string getTypeFromPathStatic(const char * dataPathCharP){
			if (*dataPathCharP == 0)
			{
				return ("system.Double");
			}

			if (strcmp(dataPathCharP, "Double") == 0)
			{
				return ("system.Double");
			}
			return("_ERROR_");
		}

		std::string getTypeFromPath(const int *dataPathIntP){
			return(getTypeFromPathStatic(dataPathIntP));
		}

		static std::string getTypeFromPathStatic(const int *dataPathIntP){
			switch(*dataPathIntP)
			{
				case -1:
					return ("system.Double");
					break;
				default:
					return ("_ERROR_");
					break;
			};
		}

		DataInstance * getDataInstanceFromPath(const int *dataPathIntP){
			switch(*dataPathIntP)
			{
				case -1:
					return (this);
					break;
				default:
					return (NULL);
					break;
			};
		}

	

		
	};


} // end of namespace samson
} // end of namespace system

#endif
