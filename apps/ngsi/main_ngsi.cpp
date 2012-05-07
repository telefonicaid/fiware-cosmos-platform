/* ****************************************************************************
*
* FILE                     main_ngsiTest.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            April 23 2012
* 
* 
* NGSI-9
* 
* Register/Update Context Entities, their attributes and their availability
* Query/Notification (discover) Context Entities, their attributes and their availability
* 
* FUNCTIONALITY
*   
*  Context Producers:
*    - publish data/context by 'update context' (message to Broker)
*    - update data/context
*    - send 'query context'  to Broker
*    - subscribe to Context Producer ()
* 
*  Context Broker
*    - treat 'update context'
*    - answer 'context queries'
*    - context data has an expiration time
*    - export 'register context'
*    - notify data/context to Consumers
*    - notify data/context to another Broker
*    - send 'discover context' to Applications
* 
*  Context Consumer
*    - retrieve data/context by querying the Broker
*    - subscribeContext from Broker (with duration)
*    - subscribe to Context Producer ()
* 
*  Application (is this not jsut a combination of Producer/Consumer?)
*    - subscribe to Context
*    - register context
*    - receive 'discover context'
* 
* Consumers must be permanently connected to the Broker in order to ne notified.
* Or should the Broker be able to connect?
* 
*
* REST paths:
*   GET /NGSI10/contextEntities/{entityId}/attributes/{attributeName}/{valueID}
*   GET /NGSI10/contextEntities/{entityId}/attributeDomains/{attributeDomainName}
*   GET /NGSI10/contextEntityTypes/{typeName}/attributes/{attributeName}
*   GET /NGSI10/contextEntityTypes/{typeName}/attributeDomains/{attributeDomainName}
*   GET /NGSI10/contextSubscriptions/{subscriptionId}
*   GET /NGSI10/queryContext
*   GET /NGSI10/subscribeContext
*   GET /NGSI10/updateContextSubscription
*   GET /NGSI10/unsubscribeContext
*   GET /NGSI10/updateContext
*
#if 0
    curl localhost:1025/NGSI10/contextEntities/entity01/attributes/attribute01/value01
    curl localhost:1025/NGSI10/contextEntities/entity01/attributeDomains/attributeDomain01
    curl localhost:1025/NGSI10/contextEntityTypes/type01/attributes/attribute01
    curl localhost:1025/NGSI10/contextEntityTypes/type01/attributeDomains/attributeDomain01
    curl localhost:1025/NGSI10/contextSubscriptions/subscription01
    curl localhost:1025/NGSI10/queryContext
    curl localhost:1025/NGSI10/subscribeContext
    curl localhost:1025/NGSI10/updateContextSubscription
    curl localhost:1025/NGSI10/unsubscribeContext
    curl localhost:1025/NGSI10/updateContext
#endif
*
*
* HTTP Status Codes
*
* Informational
*   100 Continue
*   101 Switching Protocols
*   102 Processing
*   
* Success
*   200 OK
*   201 Created
*   202 Accepted
*   203 Non-Authoritative Information
*   204 No Content
*   205 Reset Content
*   206 Partial Content
*   207 Multi-Status
*   208 Already Reported
*   226 IM Used
*
* Redirection
*   300 Multiple Choices
*   301 Moved Permanently
*   302 Found
*   303 See Other
*   304 Not Modified
*   305 Use Proxy
*   306 Switch Proxy - No longer used
*   307 Temporary Redirect
*   308 Permanent Redirect
*
* Client Error
*   400 Bad Request         - request cannot be fulfilled due to bad syntax
*   402 Payment Required    - this code is not usually used
*   403 Forbidden           - legal request, but the server is refusing to respond to it
*   404 Not Found           - requested resource could not be found but may be available again in the future
*   405 Method Not Allowed  - e.g. using GET on a form which requires POST
*   406 Not Acceptable      - only capable of generating content not acceptable according to the Accept headers sent in the request
*   407 Proxy Authentication Required - The client must first authenticate itself with the proxy
*   408 Request Timeout     - 
*   409 Conflict            - 
*   410 Gone                -
*   411 Length Required     - request did not specify the length of its content, which is required by the requested resource
*   412 Precondition Failed - server does not meet preconditions that the requester put on the request
*   413 Request Entity Too Large
*   414 Request-URI Too Long
*   415 Unsupported Media Type
*   416 Requested Range Not Satisfiable
*   417 Expectation Failed
*   418 I'm a teapot
*   420 Enhance Your Calm
*   422 Unprocessable Entity
*   423 Locked               - resource that is being accessed is locked
*   424 Failed Dependency
*   425 Unordered Collection
*   426 Upgrade Required
*   428 Precondition Required
*   429 Too Many Requests
*   431 Request Header Fields Too Large
*   444 No Response
*   449 Retry With
*   450 Blocked by Windows Parental Controls
*   499 Client Closed Request
*
*
* Server Error
*   500 Internal Server Error
*   501 Not Implemented
*   502 Bad Gateway
*   503 Service Unavailable
*   504 Gateway Timeout
*   505 HTTP Version Not Supported
*   506 Variant Also Negotiates
*   507 Insufficient Storage
*   508 Loop Detected
*   509 Bandwidth Limit Exceeded
*   510 Not Extended
*   511 Network Authentication Required
*   598 Network read timeout error
*   599 Network connect timeout error
*/
#include <sys/types.h>            // system types ...
#include <sys/socket.h>           // socket, bind, listen
#include <sys/un.h>               // sockaddr_un
#include <netinet/in.h>           // struct sockaddr_in
#include <netdb.h>                // gethostbyname
#include <arpa/inet.h>            // inet_ntoa
#include <netinet/tcp.h>          // TCP_NODELAY
#include <unistd.h>               // getppid
#include <sys/stat.h>             // umask
#include <fcntl.h>                // fcntl
#include <string>
#include <sstream>

#include "parseArgs/parseArgs.h"
#include "parseArgs/paConfig.h"
#include "logMsg/logMsg.h"

using namespace std;



/* ****************************************************************************
*
* Producer - 
*/
typedef struct Producer
{
    
} Producer;



/* ****************************************************************************
*
* Consumer - 
*/
typedef struct Consumer
{
    
} Consumer;



/* ****************************************************************************
*
* type of reply - 
*/ 
typedef enum Format
{
    JSON = 1,
    XML  = 2
} Format;



/* ****************************************************************************
*
* rest verb - 
*/ 
typedef enum Verb
{
    GET = 1,
    POST,
    PUT,
    DELETE
} Verb;



/* ****************************************************************************
*
* global variables
*/
char          inBuf[8 * 1024];
static char*  allow = NULL;



/* ****************************************************************************
*
* Option variables
*/
int             port;
bool            fg;


/* ****************************************************************************
*
* parse arguments
*/
PaArgument paArgs[] =
{
    { "-port",      &port,      "PORT",       PaInt,    PaOpt, 1025,     1,      9999,  "port to receive new connections" },
    { "-fg",        &fg,        "FOREGROUND", PaBool,   PaOpt, false,    false,  true,  "don't start as daemon"           },

    PA_END_OF_ARGS
};



const char* verbName(Verb verb)
{
    switch (verb)
    {
    case GET:    return "GET";
    case POST:   return "POST";
    case PUT:    return "PUT";
    case DELETE: return "DELETE";
    }

    return "VERB";
}



/* ****************************************************************************
*
* daemonize - 
*/
void daemonize(void)
{
	pid_t  pid;
	pid_t  sid;

	// already daemon
	if (getppid() == 1)
		return;

	pid = fork();
	if (pid == -1)
		LM_X(1, ("fork: %s", strerror(errno)));

	// Exiting father process
	if (pid > 0)
		exit(0);

	// Change the file mode mask */
	umask(0);

	// Removing the controlling terminal
	sid = setsid();
	if (sid == -1)
		LM_X(1, ("setsid: %s", strerror(errno)));

	// Change current working directory.
	// This prevents the current directory from being locked; hence not being able to remove it.
	if (chdir("/") == -1)
		LM_X(1, ("chdir: %s", strerror(errno)));
}



/* ****************************************************************************
*
* serverInit - 
*/
int serverInit(unsigned short port)
{
	int                 reuse = 1;
	int                 fd;
	struct sockaddr_in  sock;
	struct sockaddr_in  peer;

	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		printf("socket: %s\n", strerror(errno));
		return -1;
	}

	fcntl(fd, F_SETFD, 1);

	memset((char*) &sock, 0, sizeof(sock));
	memset((char*) &peer, 0, sizeof(peer));

	sock.sin_family      = AF_INET;
	sock.sin_addr.s_addr = INADDR_ANY;
	sock.sin_port        = htons(port);
	
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char*) &reuse, sizeof(reuse));
	
	if (bind(fd, (struct sockaddr*) &sock, sizeof(struct sockaddr_in)) == -1)
	{
		close(fd);
		printf("bind to port %d: %s\n", port, strerror(errno));
		return -1;
	}

	if (listen(fd, 10) == -1)
	{
		close(fd);
		printf("listen to port %d\n", port);
		return -1;
	}

	return fd;
}



/* ****************************************************************************
*
* TraceLevels - 
*/
typedef enum TraceLevels
{
	LmtInput        = 21,
	LmtInputLines,
	LmtComponents,
	LmtSuffix,
    LmtOperation,
    LmtParameters,
    LmtParameters2
} TraceLevels;



typedef struct Parameter
{
    char* name;
    char* value;
} Parameter;

Parameter parameterV[100];   // 100 is more than enough ...
int       parameters       = 0;



/* ****************************************************************************
*
* parametersPresent - 
*/
void parametersPresent(void)
{
    int ix;
    int nameLen = 0;

    for (ix = 0; ix < parameters; ix++)
        nameLen = MAX(nameLen, strlen(parameterV[ix].name));

    char format[128];
    snprintf(format, sizeof(format), "param %%02d: %%-%ds %%s", nameLen + 1);

    LM_T(LmtParameters, (""));
    LM_T(LmtParameters, ("----- Parameters -----"));
    for (ix = 0; ix < parameters; ix++)
    {
        std::string name = std::string(parameterV[ix].name) + ":";

        LM_T(LmtParameters, (format, ix, name.c_str(), parameterV[ix].value));
    }
    LM_T(LmtParameters, ("----------------------"));
    LM_T(LmtParameters, (""));
}



/* ****************************************************************************
*
* parametersParse - 
*/
void parametersParse(char* params)
{
    int pIx = 0;

    memset(parameterV, 0, sizeof(parameterV));
    do
    {
        char* next = strchr(params, '&');
        char* eq;

        if (next != NULL)
        {
            *next = 0;
            ++next;
        }

        eq   = strchr(params, '=');
        if (eq)
        {
            *eq = 0;
            ++eq;

            parameterV[pIx].name  = params;
            parameterV[pIx].value = eq;

            LM_T(LmtParameters2, ("Found parameter %d: '%s' - '%s'", pIx, parameterV[pIx].name, parameterV[pIx].value));
            ++pIx;
        }
        else
            LM_W(("No '=' found: '%s'", params));

        params = next;
    } while (params != NULL);

    parameters = pIx;
    parametersPresent();
}



/* ****************************************************************************
*
* linesPresent - just for debugging ...
*/
void linesPresent(int lines, char** input)
{
    int ix;

    for (ix = 0; ix < lines; ix++)
        LM_T(LmtInputLines, ("input: '%s'", input[ix]));
}



/* ****************************************************************************
*
* componentsPresent - 
*/
void componentsPresent(int components, std::string* component)
{
    int ix;

    for (ix = 0; ix < components; ix++)
        LM_T(LmtComponents, ("component[%d]: '%s'", ix, component[ix].c_str()));
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
* stringSplit - 
*/
int stringSplit(char* path, std::string* component, char separator)
{
    int    ix  = 0;
    char*  end = (char*) 4;  // not NULL ...

    while (end != NULL)
    {
        end           = strchr(path, separator);
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
* restReplySend - 
*/
int restReplySend(int fd, std::string data, int httpCode, Format format)
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
		if (allow != NULL)
		{
			header << "Allow: " << allow << "\n";
			allow = NULL;
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
    output << header.str() << data << "\n";

    outputLen  = strlen(output.str().c_str());
    nb         = write(fd, output.str().c_str(), outputLen);

    if (nb != outputLen)
        LM_RE(-1, ("Written only %d bytes of %d", nb, outputLen));

    return 0;
}



/* ****************************************************************************
*
* restReply - 
*/
int restReply(int fd, Format format, int httpCode, const char* key, const char* value)
{
    std::ostringstream  data;

    if (format == JSON)
        data << "{" << '"' << key << '"' << " : " << '"' << value << '"' << "}";
    else
        data << "<ngsi><key>" << key << "</key>" << "<value>" << value << "</value></ngsi>";

    return restReplySend(fd, data.str(), httpCode, format);
}



/* ****************************************************************************
*
* sanityCheck - 
*/
static bool sanityCheck(const char* s)
{
    if ((s == NULL) || (s[0] == 0))
        return false;

    if (s[0] != '/')
        return false;

    if (strlen(s) < strlen("/NGSI10/X"))
        return false;

    if (strncmp(s, "/NGSI10/", 8) != 0)
        return false;

    return true;
}



/* ****************************************************************************
*
* componentSanityCheck - 
*/
bool componentSanityCheck(int components, std::string* component)
{
    if (components < 2)
        return false;
    if (component[0] != "NGSI10")
        return false;

	if ((components == 3) && (component[1] == "contextEntities"))
		return true;
	else if ((components == 4) && (component[1] == "contextEntities") && (component[3] == "attributes"))
		return true;
	else if ((components == 5) && (component[1] == "contextEntities") && (component[3] == "attributes"))
		return true;
	else if ((components == 6) && (component[1] == "contextEntities") && (component[3] == "attributes"))
		return true;
	else if ((components == 5) && (component[1] == "contextEntities") && (component[3] == "attributeDomains"))
		return true;
	else if ((components == 3) && (component[1] == "contextEntityTypes"))
		return true;
	else if ((components == 3) && (component[1] == "contextQuery"))
		return true;
	else if ((components == 3) && (component[1] == "subscribeContext"))
		return true;
	else if ((components == 3) && (component[1] == "updateContextSubscription"))
		return true;
	else if ((components == 3) && (component[1] == "unsubscribeContext"))
		return true;
	else if ((components == 3) && (component[1] == "updateContext"))
		return true;

	return false;
}



/* ****************************************************************************
*
* contextEntity - 
*/
bool contextEntity(int fd, Verb verb, Format format, std::string entityId)
{
    LM_T(LmtOperation, ("entityId: '%s'", entityId.c_str()));

    // Entity* entity = entityLookup(entityId);

    if (verb == GET)
    {
        //
        // Retrieve all available information about the context entity represented by the resource, including all attribute values and metadata.
        // The response body is an instance of ContextElementResponse, and the ContextElement field in this response have to satisfy the following property:
        //   The ID field of the EntityId entry MUST coincide with the {entityID} part of the resource URI.
        //

        LM_T(LmtOperation, ("Retrieve all available information about the context entity '%s'", entityId.c_str()));
    }
    else if (verb == PUT)
    {
        //
        // This operation allows updating multiple attribute values of a Context Entity and the respective metadata.
        // The request message may contain multiple ContextAttribute instances.
        // For each of these instances, the output message SHALL contain an instance of ContextAttributeResponse.
        // In case that a submitted ContextAttribute does not contain a value ID in the form of metadata,
        // or if the contained value ID does not exist in the system,
        // the respective ContextAttributeResponse instance SHALL only consist of an error message.
        // In case all ContextAttribute instance update operations finish with success,
        // the ContextAttributeResponse items of the ContextResponse field in the response can be omitted and the ErrorCode field used instead. 
        // If the request contains domain metadata, then every instance of ContextMetadata in the request replaces any existing metadata
        // having the same name and type.
        //

        LM_T(LmtOperation, ("Replace a number of attribute values for the context entity '%s'", entityId.c_str()));
    }
    else if (verb == POST)
    {
        //
        // This operation allows adding new information like attribute values and metadata about a Context Entity.
        // The new information is added to the existing information.
        // Note that this may result in multiple values of certain attributes,
        // which will be represented in an NGSI ContextElement as multiple attributes having the same name.
        //
        // The request message may contain multiple ContextAttribute instances.
        // For each of these instances, the output message SHALL contain an instance of ContextAttributeResponse containing the respective statuscode.
        //
        // In case all ContextAttribute append operations finish with success,
        // the ContextAttributeResponse items of the ContextResponse field in the response can be omitted and the ErrorCode field used instead.
        //

        LM_T(LmtOperation, ("Append context attribute values for the context entity '%s'", entityId.c_str()));
    }
    else if (verb == DELETE)
    {
        //
        // This operation allows deleting all information about the Context Entity represented by the resource,
        // i.e., all related attribute values and metadata.
        // The request does not have a body, and the response body is an instance of StatusCode.
        //

        LM_T(LmtOperation, ("Delete all entity information for the context entity '%s'", entityId.c_str()));
    }
        
    return restReply(fd, format, 501, __FUNCTION__, "not implemented");
}



/* ****************************************************************************
*
* contextEntityAttributes - 
*/
bool contextEntityAttributes(int fd, Verb verb, Format format, std::string entityId)
{
    LM_T(LmtOperation, ("entityId: '%s'", entityId.c_str()));

    if (verb == GET)
    {
        LM_T(LmtOperation, ("Retrieve all available information about context entity"));
    }
    else if (verb == PUT)
        LM_T(LmtOperation, ("ERROR"));
    else if (verb == POST)
    {
        LM_T(LmtOperation, ("Append context attribute values"));
    }
    else if (verb == DELETE)
    {
        LM_T(LmtOperation, ("Delete all entity information"));
    }

    return restReply(fd, format, 501, __FUNCTION__, "not implemented"); 
}



/* ****************************************************************************
*
* contextEntityAttribute - 
*/
bool contextEntityAttribute(int fd, Verb verb, Format format, std::string entityId, std::string attributeName)
{
    LM_T(LmtOperation, ("entityId: '%s', attributeName: '%s'", entityId.c_str(), attributeName.c_str()));

    if (verb == GET)
    {
        //
        // Retrieve all available information about the attribute represented by this resource,
        // including all available attribute values and the respective metadata.
        // The context entity the attribute belongs to is represented by the parent resource.
        //

        LM_T(LmtOperation, ("Retrieve attribute value(s) and associated metadata"));
    }
    else if (verb == PUT)
    {
        allow = (char*) "GET, POST, DELETE";
        return restReply(fd, format, 405, "error", __FUNCTION__);
    }
    else if (verb == POST)
    {
        //
        // This operation allows adding new value instances, possibly including metadata,
        // for the attribute represented by the resource.
        // The request body is an instance of updateContextAttributeRequest; the response body is an instance of StatusCode.
        //

        LM_T(LmtOperation, ("Append context attribute value"));
    }
    else if (verb == DELETE)
    {
        //
        // This operation allows deleting an Attribute of a Context Entity. It deletes all attribute values and respective metadata.
        // The request does not have a message body. The response body is an instance of StatusCode.
        //

        LM_T(LmtOperation, ("Delete all attribute values"));
    }

    return restReply(fd, format, 501, __FUNCTION__, "not implemented");
}



/* ****************************************************************************
*
* contextEntityAttributeValue - 
*/
bool contextEntityAttributeValue(int fd, Verb verb, Format format, std::string entityId, std::string attributeName, std::string attributeId)
{
    LM_T(LmtOperation, ("entityId: '%s', attributeName: '%s', attributeId: '%s'", entityId.c_str(), attributeName.c_str(), attributeId.c_str()));

    if (verb == GET)
    {
        //
        // Retrieve the attribute value instance provided by this resource together with all metadata.
        // The response body is an instance of ContextAttributeResponse containing a single ContextAttribute,
        // which MUST satisfy the following properties:
        //   o The Name field corresponds to the {attributeName} part of the access URI.
        //   o It contains a piece of ContextMetadata having Name "ID", Type "xsd:string",
        //     and Value corresponding to the {valueID} part of the access URI.
        //

        LM_T(LmtOperation, ("Retrieve specific attribute value"));
    }
    else if (verb == PUT)
    {
        //
        // Update (overwrite) the attribute value instance and associated metadata.
        // This operation can only be used on resources that represent already existing attribute value instances.
        // It cannot be used for creating new value instances.
        // The latter can instead be achieved by a POST on the parent resource.
        //
        // The request body is an instance of updateContextAttributeRequest.
        // In case this instance contains a ContextMetadata instance with Name "ID",
        // the Value must correspond to the {valueID} part of the access URI, or else the server SHALL return an error.
        //
        // The response body is an instance of  StatusCode.
        //

        LM_T(LmtOperation, ("Replace attribute value"));
    }
    else if (verb == POST)
    {
        allow = (char*) "GET";
        return restReply(fd, format, 405, "error", __FUNCTION__);
    }
    else if (verb == DELETE)
    {
        //
        // Delete the attribute value instance. The request does not have a body,
        // and the response body is an instance of StatusCode.
        //

        LM_T(LmtOperation, ("Delete attribute value"));
    }

    return restReply(fd, format, 501, __FUNCTION__, "not implemented");
}



/* ****************************************************************************
*
* contextEntityAttributeDomain - 
*/
bool contextEntityAttributeDomain(int fd, Verb verb, Format format, std::string entityId, std::string attributeDomainName)
{
    LM_T(LmtOperation, ("entityId: '%s', attributeDomainName: '%s'", entityId.c_str(), attributeDomainName.c_str()));

    if (verb == GET)
    {
        //
        // Retrieve all available information about the attributes belonging to the domain represented by this resource,
        // including all attribute values and the respective metadata.
        // The context entity the attributes refer to is represented by the parent resource.
        // The response body is an instance of ContextAttributeResponse.
        // Each ContextAttribute must have a Name that belongs to the attribute domain represented by the {attributeDomainName} part of the access URL.
        //

        LM_T(LmtOperation, ("Retrieve all attribute information belonging to attribute domain '%s' : '%s'", entityId.c_str(), attributeDomainName.c_str()));
    }
    else
    {
        allow = (char*) "GET";
        return restReply(fd, format, 405, "error", __FUNCTION__);
    }

    return restReply(fd, format, 501, __FUNCTION__, "not implemented");
}



/* ****************************************************************************
*
* contextEntityType - 
*/
bool contextEntityType(int fd, Verb verb, Format format, std::string typeName)
{
    LM_T(LmtOperation, ("typeName: '%s'", typeName.c_str()));

    if (verb == GET)
    {
        //
        // Retrieve all available information about context entities of the type represented by this resource;
        // including all attribute value instances and the respective metadata.
        // The response body is an instance of queryContextResponse.
        // The EntityId of each ContextElement instance inside the response must have a type field whose value corresponds to
        // the {typeName} part of the access URI.
        //

        LM_T(LmtOperation, ("Retrieve all available information about all context entities having entity type: '%s'", typeName.c_str()));
    }
    else
    {
        allow = (char*) "GET";
        return restReply(fd, format, 405, "error", __FUNCTION__);
    }

    return restReply(fd, format, 501, __FUNCTION__, "not implemented");
}



/* ****************************************************************************
*
* contextEntityTypeAttributes - 
*/
bool contextEntityTypeAttributes(int fd, Verb verb, Format format, std::string typeName)
{
    LM_T(LmtOperation, ("typeName: '%s'", typeName.c_str()));

    if (verb == GET)
    {
        //
        // The set of allowed interactions with this resource and their effects SHALL be equivalent to the parent resource.
        //
        LM_T(LmtOperation, ("Retrieve all available information about all context entities having entity type: '%s'", typeName.c_str()));
    }
    else
    {
        allow = (char*) "GET";
        return restReply(fd, format, 405, "error", __FUNCTION__);
    }

    return restReply(fd, format, 501, __FUNCTION__, "not implemented");
}



/* ****************************************************************************
*
* contextEntityTypeAttribute - 
*/
bool contextEntityTypeAttribute(int fd, Verb verb, Format format, std::string typeName, std::string attributeName)
{
    LM_T(LmtOperation, ("typeName: '%s', attributeName: '%s'", typeName.c_str(), attributeName.c_str()));

    if (verb == GET)
    {
        //
        // Retrieve all available information about the attribute represented by the resource, including all attribute value instances and metadata.
        // The context entities of interest are all context entities of the type represented by the {typeName} part of the access URI.
        // Further restrictions can be added using the scope parameter.
        // The returned data type is queryContextResponse, and each ContextElement instance contained in it must satisfy the following properties:
        //   o If the EntityId has a type , it SHOULD correspond to the {typeName} fields inside the access URI.
        //   o All Attribute instances MUST have a Name which corresponds to the {attributeName} part of the access URI.
        //

        LM_T(LmtOperation, ("Retrieve all attribute values of the context entities of entity type: '%s'", typeName.c_str()));
    }
    else
    {
        allow = (char*) "GET";
        return restReply(fd, format, 405, "error", __FUNCTION__);
    }

    return restReply(fd, format, 501, __FUNCTION__, "not implemented");
}



/* ****************************************************************************
*
* contextEntityTypeAttributeDomain - 
*/
bool contextEntityTypeAttributeDomain(int fd, Verb verb, Format format, std::string typeName, std::string attributeDomainName)
{
    LM_T(LmtOperation, ("typeName: '%s', attributeDomainName: '%s'", typeName.c_str(), attributeDomainName.c_str()));

    if (verb == GET)
    {
        LM_T(LmtOperation, ("For all context entities of the type '%s', retrieve the values of all attributes belonging to the attribute domain: '%s'",
                            typeName.c_str(),
                            attributeDomainName.c_str()));
    }
    else
    {
        allow = (char*) "GET";
        return restReply(fd, format, 405, "error", __FUNCTION__);
    }

    return restReply(fd, format, 501, __FUNCTION__, "not implemented");
}



/* ****************************************************************************
*
* contextSubscriptionCreate - 
*/
bool contextSubscriptionCreate(int fd, Verb verb, Format format, std::string subscriptionInfo)
{
    LM_T(LmtOperation, ("subscriptionInfo: '%s'", subscriptionInfo.c_str()));

    
    if (verb == POST)
        LM_T(LmtOperation, ("Create a new subscription: '%s'", subscriptionInfo.c_str()));
    else
    {
        LM_W(("Bad verb '%s' for request", verbName(verb)));
        return restReply(fd, format, 405, "Verb not permitted", verbName(verb));
    }

    return restReply(fd, format, 501, __FUNCTION__, "not implemented");
}



/* ****************************************************************************
*
* contextSubscriptionUpdate - 
*/
bool contextSubscriptionUpdate(int fd, Verb verb, Format format, std::string subscriptionId, std::string subscriptionInfo)
{
    LM_T(LmtOperation, ("subscriptionId: '%s', subscriptionInfo: '%s'", subscriptionId.c_str(), subscriptionInfo.c_str()));

    if (verb == PUT)
        LM_T(LmtOperation, ("Update subscription '%s': '%s'", subscriptionId.c_str(), subscriptionInfo.c_str()));
    else if (verb == DELETE)
        LM_T(LmtOperation, ("Cancel subscription '%s': '%s'", subscriptionId.c_str(), subscriptionInfo.c_str()));
    else
    {
        LM_W(("Bad verb '%s' for request", verbName(verb)));
        return restReply(fd, format, 405, "Verb not permitted", verbName(verb));
    }

    return restReply(fd, format, 501, __FUNCTION__, "not implemented");
}




/* ****************************************************************************
*
* unsubscribeContext - 
*/
bool unsubscribeContext(int fd, Verb verb, Format format, std::string subscriptionInfo)
{
    LM_T(LmtOperation, ("subscriptionInfo: '%s'", subscriptionInfo.c_str()));
    return restReply(fd, format, 501, __FUNCTION__, "not implemented");
}



/* ****************************************************************************
*
* updateContextSubscription - 
*/
bool updateContextSubscription(int fd, Verb verb, Format format, std::string subscriptionInfo)
{
    LM_T(LmtOperation, ("subscriptionInfo: '%s'", subscriptionInfo.c_str()));
    return restReply(fd, format, 501, __FUNCTION__, "not implemented");
}



/* ****************************************************************************
*
* subscribeContext - 
*/
bool subscribeContext(int fd, Verb verb, Format format, std::string subscriptionInfo)
{
    LM_T(LmtOperation, ("subscriptionInfo: '%s'", subscriptionInfo.c_str()));
    return restReply(fd, format, 501, __FUNCTION__, "not implemented");
}



/* ****************************************************************************
*
* contextQuery - 
*/
bool contextQuery(int fd, Verb verb, Format format, std::string contextInfo)
{
    LM_T(LmtOperation, ("contextInfo: '%s'", contextInfo.c_str()));
    return restReply(fd, format, 501, __FUNCTION__, "not implemented");
}



/* ****************************************************************************
*
* updateContext - 
*/
bool updateContext(int fd, Verb verb, Format format, std::string contextInfo)
{
    LM_T(LmtOperation, ("contextInfo: '%s'", contextInfo.c_str()));
    return restReply(fd, format, 501, __FUNCTION__, "not implemented");
}



/* ****************************************************************************
*
* restReqTreat - 
*/
bool restReqTreat(int fd, Verb verb, Format format, int components, std::string* component)
{
    //
    // Sanity Check
    //
    if (components < 2)
        return false;
    if (component[0] != "NGSI10")
        return false;


    //
    // /NGSI10/contextEntities
    //
	if ((components == 3) && (component[1] == "contextEntities"))
		return contextEntity(fd, verb, format, component[2]);
	else if ((components == 4) && (component[1] == "contextEntities") && (component[3] == "attributes"))
		return contextEntityAttributes(fd, verb, format, component[2]);
	else if ((components == 5) && (component[1] == "contextEntities") && (component[3] == "attributes"))
		return contextEntityAttribute(fd, verb, format, component[2], component[4]);
	else if ((components == 6) && (component[1] == "contextEntities") && (component[3] == "attributes"))
		return contextEntityAttributeValue(fd, verb, format, component[2], component[4], component[5]);
	else if ((components == 5) && (component[1] == "contextEntities") && (component[3] == "attributeDomains"))
		return contextEntityAttributeDomain(fd, verb, format, component[2], component[4]);


    //
    // /NGSI10/contextEntityTypes
    //
	else if ((components == 3) && (component[1] == "contextEntityTypes"))
		return contextEntityType(fd, verb, format, component[2]);
	else if ((components == 4) && (component[1] == "contextEntityTypes") && (component[3] == "attributes"))
		return contextEntityTypeAttributes(fd, verb, format, component[2]);
	else if ((components == 5) && (component[1] == "contextEntityTypes") && (component[3] == "attributes"))
		return contextEntityTypeAttribute(fd, verb, format, component[2], component[4]);
	else if ((components == 5) && (component[1] == "contextEntityTypes") && (component[3] == "attributeDomains"))
		return contextEntityTypeAttributeDomain(fd, verb, format, component[2], component[4]);


    //
    // /NGSI10/contextSubscriptions
    //
	else if ((components == 3) && (component[1] == "contextSubscriptions"))
		return contextSubscriptionCreate(fd, verb, format, component[2]);
	else if ((components == 4) && (component[1] == "contextSubscriptions"))
		return contextSubscriptionUpdate(fd, verb, format, component[2], component[3]);




	else if ((components == 3) && (component[1] == "contextQuery"))
		return contextQuery(fd, verb, format, component[2]);
	else if ((components == 3) && (component[1] == "subscribeContext"))
		return subscribeContext(fd, verb, format, component[2]);
	else if ((components == 3) && (component[1] == "updateContextSubscription"))
		return updateContextSubscription(fd, verb, format, component[2]);
	else if ((components == 3) && (component[1] == "unsubscribeContext"))
		return unsubscribeContext(fd, verb, format, component[2]);
	else if ((components == 3) && (component[1] == "updateContext"))
		return updateContext(fd, verb, format, component[2]);

	return false;
}



/* ****************************************************************************
*
* restTreat - 
*/
int restTreat(Verb verb, char* path, int fd)
{
    char*        httpVersionString;
    int          httpCode = 200;
    std::string  jsonSuffix = ".json";
    std::string  xmlSuffix  = ".xml";
    Format       format       = JSON;

    //
    // Check the input
    //
    httpVersionString = strstr(path, "HTTP/1.1");
    if (httpVersionString)
    {
        --httpVersionString;
        *httpVersionString = 0;
        ++httpVersionString;
    }
    else
    {
        httpVersionString = strstr(path, "HTTP/1.0");
        if (httpVersionString)
        {
            --httpVersionString;
            *httpVersionString = 0;
            ++httpVersionString;
        }
        else
        {
            httpVersionString  = (char*) "Unknown";
            httpCode           = 400;
        }
    }

    LM_D(("HTTP Version: '%s'", httpVersionString));
    LM_T(LmtInput, ("path: '%s'", path));

    if (sanityCheck(path) == false)
    {
        restReply(fd, JSON, 400, "error", "Not an NGSI request");
        return -1;
    }


    //
    // Check if request ends in '.xml' or '.json'
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
            LM_T(LmtSuffix, ("Default reply is JSON"));
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
    // split path in components
    //
    std::string  component[20];
    int          components;

    components = stringSplit(&path[1], component, '/');
    componentsPresent(components, component);

    if (restReqTreat(fd, verb, format, components, component) == false)
    {
        restReply(fd, JSON, 400, "error", "Badly formed NGSI request");
        return -1;
    }

    return 0;
}



/* ****************************************************************************
*
* restServe - 
*/
int restServe(int fd)
{
    int nb;

    nb = read(fd, inBuf, sizeof(inBuf));
    if (nb == -1)
        LM_RE(-1, ("read: %s", strerror(errno)));
    else if (nb == 0)
        LM_RE(-1, ("read: connection closed"));
    else
    {
        char* lineV[100];
        int   lines;

        lines = lineSplit(inBuf, lineV);
        linesPresent(lines, lineV);
        if (strncmp(lineV[0], "GET ", 4) == 0)
            restTreat(GET, &lineV[0][4], fd);
        else if (strncmp(lineV[0], "POST ", 5) == 0)
            restTreat(POST, &lineV[0][5], fd);
        else if (strncmp(lineV[0], "PUT ", 4) == 0)
            restTreat(PUT, &lineV[0][5], fd);
        else if (strncmp(lineV[0], "DELETE ", 7) == 0)
            restTreat(DELETE, &lineV[0][7], fd);
        else
        {
            LM_E(("Bad verb: '%s'", lineV[0]));
        }
    }

    return 0;
}



/* ****************************************************************************
*
* serve - 
*/
void serve(int fd)
{
	int             fds;
	int             cFd = -1;
	fd_set          rFds;
	struct timeval  timeVal;
	int             max;

	while (1)
	{
		timeVal.tv_sec  = 1;
		timeVal.tv_usec = 0;

		FD_ZERO(&rFds);
		FD_SET(fd, &rFds);
		max = fd;

		if (cFd != -1)
		{
			FD_SET(cFd, &rFds);
			max = (fd > cFd)? fd : cFd;
		}

		do
		{
			fds = select(max + 1, &rFds, NULL, NULL, &timeVal);
		} while ((fds == -1) && (errno == EINTR));

		if ((fds == 1) && (FD_ISSET(fd, &rFds)))
		{
			struct sockaddr_in  sin;
			unsigned int        len         = sizeof(sin);
			
			if (cFd != -1)
				close(cFd);
			cFd = accept(fd, (struct sockaddr*) &sin, &len);
			if (cFd == -1)
				LM_X(1, ("accept: %s", strerror(errno)));
        }

		if (cFd != -1)
        {
            restServe(cFd);
            close(cFd);
            cFd = -1;
        }
    }
}



/* ****************************************************************************
*
* main - 
*/
int main(int argC, const char *argV[])
{
    int fd;

    paConfig("builtin prefix",                    (void*) "SS_NGSI_");
    paConfig("usage and exit on any warning",     (void*) true);
    paConfig("log to screen",                     (void*) true);

    paConfig("log file line format",              (void*) "TYPE:DATE:EXEC-AUX/FILE[LINE](p.PID)(t.TID) FUNC: TEXT");
    paConfig("screen line format",                (void*) "TYPE@TIME  EXEC: TEXT");

    paParse(paArgs, argC, (char**) argV, 1, false);

    if (fg == false)
        daemonize();

    fd = serverInit(port);
    if (fd == -1)
        LM_X(1, ("error opening port %d for connections", port));

    LM_F(("Opened port %d for connections", port));
    serve(fd);

    return 0;
}
