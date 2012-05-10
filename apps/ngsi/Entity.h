#ifndef ENTITY_H
#define ENTITY_H

/* ****************************************************************************
*
* FILE                  Entity.h 
*
*
*
*
*/
#include <string>

#include "Format.h"

using namespace std;



/* ****************************************************************************
*
* Entity - 
*/
typedef struct Entity
{
	std::string      id;
	std::string      type;
	bool             isPattern;
	std::string      providingApplication;
	time_t           startTime;
	time_t           endTime;
	std::string      registrationId;
	struct Entity*   next;
} Entity;



/* ****************************************************************************
*
* entity functions
*/
extern void    entityInit(void);
extern Entity* entityAdd(std::string id, std::string type, bool isPattern, std::string providingApplication, int duration, std::string registrationId);

#endif
