#ifndef COMMAND_RESPONSE_H
#define COMMAND_RESPONSE_H

#include "CommandResponse.h"     /* CommandResponse                          */



namespace ss {

    typedef struct CommandResponse
	{
		std::string  message;   // Message (normal or error)
		bool         error;     // Flag to indiciate if there was error in the commnad
	} CommandResponse;
}

#endif
