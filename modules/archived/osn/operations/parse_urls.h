
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_osn_parse_urls
#define _H_SAMSON_osn_parse_urls


#include <samson/module/samson.h>

#undef DEBUG_FILES
#ifdef DEBUG_FILES
#include <iostream>
#include <fstream>
#endif /* de DEBUG_FILES */
#undef DEBUG_FILES


namespace samson{
namespace osn{


#define CHAR_TO_INT(v) (v-48)

	bool parseDate_O2UK_osn (char *dateTxt, samson::system::Date *date, samson::system::Time *time)
	{
		if ((strlen(dateTxt) != 14) && (strlen(dateTxt) != 15))
		{
			// In the test logs, we have found "30Nov2010133246" and "1Dec2010000000"
#undef DEBUG_FILES
#ifdef DEBUG_FILES
		{
		std::string filename = "/tmp/parser_urls.log";
		std::ofstream fs(filename.c_str(), std::ios::app);
		fs << "Error parsing Date:" << dateTxt << " strlen: " << strlen(dateTxt) << std::endl;
		fs.close();
		}
#endif /* de DEBUG_FILES */
#undef DEBUG_FILES
			return false;
		}

		date->unassignAllOptionals();

		if (isdigit(dateTxt[1]))
		{
			date->day.value = CHAR_TO_INT( dateTxt[0] ) *10 + CHAR_TO_INT( dateTxt[1] );
			dateTxt += 2;
		}
		else
		{
			date->day.value = CHAR_TO_INT( dateTxt[0] );
			dateTxt++;
		}

		if (!strncmp(dateTxt, "Jan", 3))
		{
			date->month.value = 1;
		}
		else if (!strncmp(dateTxt, "Feb", 3))
		{
			date->month.value = 2;
		}
		else if (!strncmp(dateTxt, "Mar", 3))
		{
			date->month.value = 3;
		}
		else if (!strncmp(dateTxt, "Apr", 3))
		{
			date->month.value = 4;
		}
		else if (!strncmp(dateTxt, "May", 3))
		{
			date->month.value = 5;
		}
		else if (!strncmp(dateTxt, "Jun", 3))
		{
			date->month.value = 6;
		}
		else if (!strncmp(dateTxt, "Jul", 3))
		{
			date->month.value = 7;
		}
		else if (!strncmp(dateTxt, "Aug", 3))
		{
			date->month.value = 8;
		}
		else if (!strncmp(dateTxt, "Sep", 3))
		{
			date->month.value = 9;
		}
		else if (!strncmp(dateTxt, "Oct", 3))
		{
			date->month.value = 10;
		}
		else if (!strncmp(dateTxt, "Nov", 3))
		{
			date->month.value = 11;
		}
		else if (!strncmp(dateTxt, "Dec", 3))
		{
			date->month.value = 12;
		}
		else
		{
#undef DEBUG_FILES
#ifdef DEBUG_FILES
		{
                std::string filename = "/tmp/parser_urls.log";
		std::ofstream fs(filename.c_str(), std::ios::app);
		fs << "Error in month date: " << dateTxt << std::endl;
                fs.close();
		}
#endif /* de DEBUG_FILES */
#undef DEBUG_FILES
		}
		dateTxt += 3;

		const int YEAR_2000 = 2000;
		date->year.value          = (CHAR_TO_INT( dateTxt[0] ) *1000 + CHAR_TO_INT( dateTxt[1] ) *100 + CHAR_TO_INT( dateTxt[2] ) *10 + CHAR_TO_INT( dateTxt[3] )) - YEAR_2000;
		dateTxt += 4;

		time->hour.value          = CHAR_TO_INT( dateTxt[0] ) *10 + CHAR_TO_INT( dateTxt[1] );
#ifdef DEBUG_FILES
		{
                std::string filename = "/tmp/parser_urls.log";
		std::ofstream fs(filename.c_str(), std::ios::app);
		fs << "From dateTxt: " << dateTxt << " Hour: " << int(time->hour.value) << std::endl;
                fs.close();
		}
#endif /* de DEBUG_FILES */
#undef DEBUG_FILES
		time->minute.value        = CHAR_TO_INT( dateTxt[2] ) *10 + CHAR_TO_INT( dateTxt[3] );
		time->seconds.value       = CHAR_TO_INT( dateTxt[4] ) *10 + CHAR_TO_INT( dateTxt[5] );

		date->daysFrom2000_01_01(); //As a side effect, computes and stores the days_2000 and week_day fields
		return true;
	}


	bool getURLConnectionFromLine( char *line, samson::system::UInt64 *msisdn, samson::osn::URLConnection *URLConnection)
	{
		//LINE --> "msisdn URL DateTime status MIME agent method"
		//Separator is a tab character

		char *pLine;
		char *pNextLine;
		char *pSep;
		char *pDomain;
		char *pDir;
		char *pPage;
		char *pQuery;

		pLine = line;
		if ((pSep = strchr(pLine, '\t')) != NULL)
		{
			*pSep = '\0';
			msisdn->value = strtoll(pLine, NULL, 16);
			URLConnection->msisdn.value = msisdn->value;
			pLine = pSep+1;
		}
		else
		{
#undef DEBUG_FILES
#ifdef DEBUG_FILES
		{
                std::string filename = "/tmp/parser_urls.log";
		std::ofstream fs(filename.c_str(), std::ios::app);
		fs << "Error in msisdn: " << pLine << std::endl;
                fs.close();
		}
#endif /* de DEBUG_FILES */
#undef DEBUG_FILES
			return false;
		}

		if ((pSep = strchr(pLine, '\t')) != NULL)
		{
			*pSep = '\0';
			pNextLine = pSep+1;
#ifdef DEBUG_FILES
		{
                std::string filename = "/tmp/parser_urls.log";
		std::ofstream fs(filename.c_str(), std::ios::app);
		fs << "pLine: " << pLine << std::endl;
                fs.close();
		}
#endif /* de DEBUG_FILES */
#undef DEBUG_FILES
			URLConnection->url.completeURL.value.assign(pLine);

			if ((pSep = strchr(pLine, '?')) != NULL)
			{
				*pSep = '\0';
				URLConnection->url.url_locDoc.value.assign(pLine);
				*pSep = '?';
			}
			else
			{
				URLConnection->url.url_locDoc.value.assign(pLine);
			}
			
			if (!strncmp(pLine, "http://", strlen("http://")))
			{
				pDomain = pLine + strlen("http://");
			}
			else
			{
				pDomain = pLine;
			}
			if ((pSep = strchr(pDomain, '/')) != NULL)
			{
				*pSep = '\0';
#ifdef DEBUG_FILES
{
std::string filename = "/tmp/parser_urls.log";
std::ofstream fs(filename.c_str(), std::ios::app);
fs << "pDomain: " << pDomain << std::endl;
fs.close();
}
#endif /* de DEBUG_FILES */
#undef DEBUG_FILES
				URLConnection->url.url_host.value.assign(pDomain);
				*pSep = '/';
				pQuery = pSep;
#ifdef DEBUG_FILES
{
std::string filename = "/tmp/parser_urls.log";
std::ofstream fs(filename.c_str(), std::ios::app);
fs << "pQuery: " << pQuery << std::endl;
fs.close();
}
#endif /* de DEBUG_FILES */
#undef DEBUG_FILES
				URLConnection->url.url_query.value.assign(pQuery);
				pDir = pSep+1;
				if ((pSep = strrchr(pDir, '/')) != NULL)
				{
					*pSep = '\0';
					URLConnection->url.url_dir.value.assign(pDir);
					*pSep = '/';
					pPage = pSep+1;
					if ((pSep = strchr(pPage, '?')) != NULL)
					{
						*pSep = '\0';
					}
					URLConnection->url.url_page.value.assign(pPage);
				}
				else
				{
					pPage = pDir;
					if ((pSep = strchr(pDir, '?')) != NULL)
					{
						*pSep = '\0';
					}
#ifdef DEBUG_FILES
		{
                std::string filename = "/tmp/parser_urls.log";
		std::ofstream fs(filename.c_str(), std::ios::app);
		fs << "pPage: " << pPage << std::endl;
                fs.close();
		}
#endif /* de DEBUG_FILES */
#undef DEBUG_FILES
					URLConnection->url.url_dir.value.assign(pDir);
					URLConnection->url.url_page.value.assign(pPage);
				}
			}
			else
			{
#undef DEBUG_FILES
#ifdef DEBUG_FILES
		{
                std::string filename = "/tmp/parser_urls.log";
		std::ofstream fs(filename.c_str(), std::ios::app);
		fs << "Error in pDomain: " << pLine << std::endl;
                fs.close();
		}
#endif /* de DEBUG_FILES */
#undef DEBUG_FILES
				return false;
			}

			pLine = pNextLine;
		}
		else
		{
#undef DEBUG_FILES
#ifdef DEBUG_FILES
		{
                std::string filename = "/tmp/parser_urls.log";
		std::ofstream fs(filename.c_str(), std::ios::app);
		fs << "Error in URL: " << pLine << std::endl;
                fs.close();
		}
#endif /* de DEBUG_FILES */
#undef DEBUG_FILES
			return false;
		}

		if ((pSep = strchr(pLine, '\t')) != NULL)
		{
			*pSep = '\0';
			if (!parseDate_O2UK_osn(pLine, &(URLConnection->date), &(URLConnection->time)))
			{
#undef DEBUG_FILES
#ifdef DEBUG_FILES
		{
                std::string filename = "/tmp/parser_urls.log";
		std::ofstream fs(filename.c_str(), std::ios::app);
		fs << "Error in Date: " << pLine << std::endl;
                fs.close();
		}
#endif /* de DEBUG_FILES */
#undef DEBUG_FILES
				return false;
			}
			pLine = pSep+1;
		}
		else
		{
#undef DEBUG_FILES
#ifdef DEBUG_FILES
		{
                std::string filename = "/tmp/parser_urls.log";
		std::ofstream fs(filename.c_str(), std::ios::app);
		fs << "Error in Date: " << pLine << std::endl;
                fs.close();
		}
#endif /* de DEBUG_FILES */
#undef DEBUG_FILES
			return false;
		}

		if ((pSep = strchr(pLine, '\t')) != NULL)
		{
			*pSep = '\0';
			URLConnection->status.value = strtol(pLine, NULL, 10);
			pLine = pSep+1;
		}
		else
		{
#undef DEBUG_FILES
#ifdef DEBUG_FILES
		{
                std::string filename = "/tmp/parser_urls.log";
		std::ofstream fs(filename.c_str(), std::ios::app);
		fs << "Error in status: " << pLine << std::endl;
                fs.close();
		}
#endif /* de DEBUG_FILES */
#undef DEBUG_FILES
			return false;
		}

		if ((pSep = strchr(pLine, '\t')) != NULL)
		{
			*pSep = '\0';
			URLConnection->MIMEcontent.value.assign(pLine);
			pLine = pSep+1;
		}
		else
		{
#undef DEBUG_FILES
#ifdef DEBUG_FILES
		{
                std::string filename = "/tmp/parser_urls.log";
		std::ofstream fs(filename.c_str(), std::ios::app);
		fs << "Error in MIME: " << pLine << std::endl;
                fs.close();
		}
#endif /* de DEBUG_FILES */
#undef DEBUG_FILES
			return false;
		}

		if ((pSep = strchr(pLine, '\t')) != NULL)
		{
			*pSep = '\0';
			URLConnection->userAgent.value.assign(pLine);
			pLine = pSep+1;
		}
		else
		{
#undef DEBUG_FILES
#ifdef DEBUG_FILES
		{
                std::string filename = "/tmp/parser_urls.log";
		std::ofstream fs(filename.c_str(), std::ios::app);
		fs << "Error in agent: " << pLine << std::endl;
                fs.close();
		}
#endif /* de DEBUG_FILES */
#undef DEBUG_FILES
			return false;
		}

		URLConnection->method.value = strtol(pLine, NULL, 10);

		return true;
	}

	class parse_urls : public samson::Parser
	{

	public:
		samson::system::UInt64 msisdn;               //Unique identifier of a MSISDN over time
		samson::osn::URLConnection URLConnection;    //All the information in the log file. Could be optimized for specific applications

		void run( char *data , size_t length , samson::KVWriter *writer )
		{
			char *pData;
			char *pDataBegin;
			char *pDataEnd;
#undef DEBUG_FILES
#ifdef DEBUG_FILES
		{
                std::string filename = "/tmp/parser_urls.log";
		std::ofstream fs(filename.c_str(), std::ios::app);
		fs << "Process length: " << length << std::endl;
                fs.close();
		}
#endif /* de DEBUG_FILES */
#undef DEBUG_FILES

			pData = pDataBegin = data;
			pDataEnd = pData + length;
			while (pData < pDataEnd)
			{
				if ((*pData == '\n') || (*pData == '\0'))
				{
					*pData = '\0';
#ifdef DEBUG_FILES
		{
                std::string filename = "/tmp/parser_urls.log";
		std::ofstream fs(filename.c_str(), std::ios::app);
		fs << "Line: " << pDataBegin << std::endl;
                fs.close();
		}
#endif /* de DEBUG_FILES */
#undef DEBUG_FILES
					if (getURLConnectionFromLine(pDataBegin, &msisdn, &URLConnection))
					{
						writer->emit(0, &msisdn, &URLConnection);
					}
					else
					{
#undef DEBUG_FILES
#ifdef DEBUG_FILES
		{
                std::string filename = "/tmp/parser_urls.log";
		std::ofstream fs(filename.c_str(), std::ios::app);
		fs << "Error in Line: " << pDataBegin << std::endl;
                fs.close();
		}
#endif /* de DEBUG_FILES */
#undef DEBUG_FILES
						;
					}
					pDataBegin = pData+1;
				}
				pData++;
			}
			if (pDataBegin < (pDataEnd - 1))
			{
				*(pDataEnd -1) = '\0';
				if (getURLConnectionFromLine(pDataBegin, &msisdn, &URLConnection))
				{
					writer->emit(0, &msisdn, &URLConnection);
				}
				else
				{
#undef DEBUG_FILES
#ifdef DEBUG_FILES
	{
	std::string filename = "/tmp/parser_urls.log";
	std::ofstream fs(filename.c_str(), std::ios::app);
	fs << "Error in Line: " << pDataBegin << std::endl;
	fs.close();
	}
#endif /* de DEBUG_FILES */
#undef DEBUG_FILES
					;
				}
			}
		}
	};


} // end of namespace samson
} // end of namespace osn

#endif