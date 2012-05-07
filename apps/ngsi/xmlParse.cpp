#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <libxml/tree.h>

#include "registerContext.h"
#include "discoverContextAvailability.h"
#include "xmlParse.h"                      // Oen interface



/* ****************************************************************************
*
* xmlParse - 
*/
void xmlParse(const char* buf)
{
    xmlDocPtr                            doc;
    xmlNodePtr                           node;
    RegisterContextRequest*              rcrP;
    DiscoverContextAvailabilityRequest*  dcarP;

    doc  = xmlParseDoc((const xmlChar*) buf);
    node = doc->children;

    if (strcmp((char*) node->name, "registerContextRequest") == 0)
    {
        rcrP = registerContextRequestParse(node);
        if (rcrP != NULL)
            registerContextRequestPresent(rcrP);
    }
    else if (strcmp((char*) node->name, "discoverContextAvailabilityRequest") == 0)
    {
        dcarP = discoverContextAvailabilityRequestParse(node);
        if (dcarP != NULL)
            discoverContextAvailabilityRequestPresent(dcarP);
    }
    else
        printf("sorry, unable to parse '%s' - unknown\n", node->name);

    xmlFreeDoc(doc);
}
