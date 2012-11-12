/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */
#include "logMsg.h"
#include "parseArgs.h"

int main(int argC, char* argV[])
{
	char* fName = "main";

	paConfig("trace levels", "0-255");
	paConfig("help file",     "/users/kzangeli/systems/parseArgs/parseTest3.help");
	paConfig("log to file",   (void*) TRUE);

	paParse(NULL, argC, argV, 1, FALSE);

	lmVerbose = true;
	LM_V(("line with newlines ...\n 1\n2\n3\n4\n"));

	return 0;
}
