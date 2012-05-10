/* ****************************************************************************
*
*  FILE                 Verb.cpp -  
*
*
*
*
*/
#include <string>

#include "Verb.h"                       // Own interface

using namespace std;



/* ****************************************************************************
*
* verbName - 
*/
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
