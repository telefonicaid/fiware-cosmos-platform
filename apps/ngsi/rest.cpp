/* ****************************************************************************
*
* FILE                rest.cpp - 
*
*
*
*
*/
#include <sstream>                          // std::ostringstream
#include <string>                           // std::stream

#include "logMsg/logMsg.h"                  // LM_*

#include "traceLevels.h"                    // Trace levels for log msg library
#include "logRequest.h"                     // treat REST message for logging
#include "Format.h"                         // Format (XML, JSON)
#include "Verb.h"                           // Verb
#include "httpData.h"                       // httpDataLines, httpData
#include "Parameter.h"                      // Parameter, parametersParse, ...
#include "registerContext.h"                // registerContext()
#include "discoverContextAvailability.h"    // discoverContextAvailability()
#include "logRequest.h"                     // logRequest
#include "versionRequest.h"                 // versionRequest
#include "rest.h"                           // Own interface

using namespace std;



/* ****************************************************************************
*
* Globals
*/
std::string   allow = "";
char          headerBuf[8 * 1024];
char          dataBuf[32 * 1024];



/* ****************************************************************************
*
* linesPresent - just for debugging ...
*/
static void linesPresent(char** input, int lines)
{
    int ix;

	if (lmReads == false)
		return;

    for (ix = 0; ix < lines; ix++)
        LM_F(("input: '%s'", input[ix]));
}



/* ****************************************************************************
*
* componentsPresent - 
*/
static void componentsPresent(int components, std::string* component)
{
    int ix;

    for (ix = 0; ix < components; ix++)
        LM_T(LmtComponents, ("component[%d]: '%s'", ix, component[ix].c_str()));
}



/* ****************************************************************************
*
* restReplySend - 
*/
bool restReplySend(int fd, Format format, int httpCode, std::string data)
{
    int                 dataLen = strlen(data.c_str());
    std::ostringstream  header;
    std::ostringstream  output;
    int                 outputLen;
    int                 nb;

    switch (httpCode)
    {
    case 200:
        header << "HTTP/1.1 200 OK\n";
        break;
        
    case 400:
        header << "HTTP/1.1 400 Bad Request\n";
        break;
        
    case 404:
        header << "HTTP/1.1 404 Not Found\n";
        break;
        
    case 405:
        header << "HTTP/1.1 405 XXX\n";
		if (allow != "")
		{
			header << "Allow: " << allow << "\n";
			allow = "";
		}
        break;
        
    default:
        header << "HTTP/1.1 Bad Request \n"; 
        break;
    }
    
    if (format == JSON)
        header << "Content-Type:   \"application/json; charset=utf-8\"\n";
    else
        header << "Content-Type:   \"application/xml; charset=utf-8\"\n";

    header << "Content-Length: " << dataLen << "\n";
    header << "\n";
    output << header.str() << data << "\n\n";

	if (lmWrites)
	   LM_F(("Output:\n%s\n", output.str().c_str()));

    outputLen  = strlen(output.str().c_str());
    nb         = write(fd, output.str().c_str(), outputLen);

    if (nb != outputLen)
        LM_RE(false, ("Written only %d bytes of %d - implement a loop to write the entire message ...", nb, outputLen));

    return true;
}



/* ****************************************************************************
*
* restReply - 
*/
bool restReply(int fd, Format format, int httpCode, const char* key, const char* value)
{
    std::ostringstream  data;

    if (format == JSON)
        data << "{" << '"' << key << '"' << " : " << '"' << value << '"' << "}";
    else
        data << "<ngsi><key>" << key << "</key>" << "<value>" << value << "</value></ngsi>";

    return restReplySend(fd, format, httpCode, data.str());
}



/* ****************************************************************************
*
* restTreat - 
*/
static int restTreat(int fd, Verb verb, Format format, int components, std::string* component)
{
	//
	// log request?
	//
	if (component[0] == "log")
		return logRequest(fd, verb, format, components, component);


	//
	// version request?
	//
	if (component[0] == "version")
		return versionRequest(fd, verb, format, components, component);


    //
    // Sanity Check
    //
    if (components < 2)
        LM_RE(-1, ("Too few components in path (%d)", components));



	//
	// If not NGSI9 nor NGSI10 - error!
	//
    if ((component[0] != "NGSI10") && (component[0] != "NGSI9"))
        LM_RE(-1, ("Not an NGSI10 nor NGSI9 request. First component is '%s'", component[0].c_str()));

	//
	// Too many components in path?
	//
    if (components > 2)
        LM_RE(-1, ("Too many components in path (%d)", components));


	//
	// /NGSI9/registerContext
	//
	if ((component[0] == "NGSI9") && (components == 2) && (component[1] == "registerContext"))
		return registerContext(fd, verb, format, httpData[0]);
	else if ((component[0] == "NGSI9") && (components == 2) && (component[1] == "discoverContextAvailability"))
		return discoverContextAvailability(fd, verb, format, httpData[0]);

	LM_RE(-1, ("Unknown request"));
}



/* ****************************************************************************
*
* sanityCheck - 
*/
static bool sanityCheck(const char* s)
{
    if ((s == NULL) || (s[0] == 0))
        return false;

    if ((strncmp(s, "/NGSI10/", 8) != 0) && (strncmp(s, "/NGSI9/", 7) != 0) && (strncmp(s, "/log/", 5) != 0) && (strncmp(s, "/version", 8) != 0))
        LM_RE(false, ("Sorry, '%s' is not a valid request", s));

    return true;
}



/* ****************************************************************************
*
* stringSplit - 
*/
int stringSplit(char* path, std::string* component, char separator)
{
    int    ix  = 0;
    char*  end = (char*) 4;  // not NULL ...

    while (end != NULL)
    {
        end = strchr(path, separator);
        if (end != NULL)
        {
            *end          = 0;
            component[ix] = path;
            path          = &end[1];
        }
        else
            component[ix] = path;

        ++ix;
    }

    return ix;
}



/* ****************************************************************************
*
* restParse - 
*/
static int restParse(Verb verb, char* path, int fd)
{
    char*        httpVersionString;
    int          httpCode     = 200;
    std::string  jsonSuffix   = ".json";
    std::string  xmlSuffix    = ".xml";
    Format       format       = XML;    // Default format is XML

	LM_T(LmtRestPath, ("------------- %s -------------", path));

    //
    // Check the input
    //
    if ((httpVersionString = strstr(path, "HTTP/1.1")) != NULL)
    {
        --httpVersionString;
        *httpVersionString = 0;
        ++httpVersionString;
    }
    else if ((httpVersionString = strstr(path, "HTTP/1.0")) != NULL)
	{
		--httpVersionString;
		*httpVersionString = 0;
		++httpVersionString;
	}
	else
	{
		httpVersionString  = (char*) "Unknown";
		httpCode           = 400;

		restReply(fd, XML, 400, "error", "Bad HTTP Version");
		LM_RE(-1, ("Bad HTTP Version in path '%s'", path));
	}


    LM_D(("HTTP Version: '%s'", httpVersionString));
    LM_T(LmtInput, ("path: '%s'", path));

    if (sanityCheck(path) == false)
    {
        restReply(fd, XML, 400, "error", "Not an NGSI request");
        LM_RE(-1, ("Not an NGSI request: '%s'", path));
    }


    //
    // Any parameters?
    //
    char* parameters = strchr(path, '?');

    if (parameters != NULL)
    {
        *parameters = 0;
        ++parameters;
        LM_T(LmtParameters2, ("parameters: '%s'", parameters));
        parametersParse(parameters);
    }



    //
    // Check whether request ends in '.xml' or '.json'
    //
    char* suffix = &path[strlen(path) - 5];
    LM_T(LmtSuffix, ("suffix: '%s'", suffix));
    if (strcmp(suffix, ".json") == 0)
    {
        LM_T(LmtSuffix, ("XML reply requested"));
        format  = JSON;
        *suffix = 0;
    }
    else
    {
        ++suffix;
        LM_T(LmtSuffix, ("suffix: '%s'", suffix));
        if (strcmp(suffix, ".xml") == 0)
        {
            LM_T(LmtSuffix, ("XML reply requested"));
            format  = XML;
            *suffix = 0;
        }
        else
            LM_T(LmtSuffix, ("Default reply is XML"));
    }



    //
    // split path in components
    //
    std::string  component[20];
    int          components;
	std::string  initialPath = std::string(path);

    components = stringSplit(&path[1], component, '/');
    componentsPresent(components, component);

    if (restTreat(fd, verb, format, components, component) == -1)
    {
		LM_E(("Error treating REST request '%s'", initialPath.c_str()));
        restReply(fd, format, 400, "error", "Badly formed NGSI request");
        return -1;
    }

    return 0;
}



/* ****************************************************************************
*
* lineSplit - 
*/
int lineSplit(char* buf, char** lineX)
{
    int    ix  = 0;
    char*  end = (char*) 4;  // not NULL ...
    char*  br;

    while (end != NULL)
    {
        lineX[ix] = buf;
        br        = strchr(buf, '\r');
        end       = strchr(buf, '\n');
        if (end != NULL)
        {
            *end = 0;
            buf = &end[1];
        }

        if (br != NULL)
            *br = 0;

        ++ix;
    }

    return ix;
}



/* ****************************************************************************
*
* restServe - 
*/
int restServe(int fd)
{
    int   nb;
	bool  dataSeparate;
	char* lineV[100];
	int   lines;

    nb = read(fd, headerBuf, sizeof(headerBuf));
    if (nb == -1)
		LM_RE(-1, ("read: %s", strerror(errno)));
    else if (nb == 0)
        LM_RE(-1, ("read: connection closed"));


	LM_T(LmtRead, ("Read %d bytes of REST message", nb));

	lines = lineSplit(headerBuf, lineV);
	linesPresent(lineV, lines);
	dataSeparate = httpDataParse(lineV, lines);

	if (dataSeparate == true)
	{
		LM_T(LmtHttpDataSeparate, ("Data comes in a separate package - reading again"));
		nb = read(fd, dataBuf, sizeof(dataBuf) - 1);
		if (nb == -1)
			LM_RE(-1, ("read: %s", strerror(errno)));
		else if (nb == 0)
			LM_RE(-1, ("read: connection closed"));
		
		dataBuf[nb] = 0;
		LM_T(LmtHttpDataSeparate, ("Read %d bytes of data", nb));
		httpData[0]   = dataBuf;
		httpDataLines = 1;
		
		LM_T(LmtHttpDataSeparate, ("Got data line: '%s'", httpData[0]));
	}

	if      (strncmp(lineV[0], "GET ",    4) == 0)  restParse(GET,    &lineV[0][4], fd);
	else if (strncmp(lineV[0], "POST ",   5) == 0)  restParse(POST,   &lineV[0][5], fd);
	else if (strncmp(lineV[0], "PUT ",    4) == 0)  restParse(PUT,    &lineV[0][4], fd);
	else if (strncmp(lineV[0], "DELETE ", 7) == 0)  restParse(DELETE, &lineV[0][7], fd);
	else
	{
		restReply(fd, XML, 400, "error", "BAD VERB");
		LM_RE(-1, ("Bad verb: '%s'", lineV[0]));
	}

    return 0;
}
