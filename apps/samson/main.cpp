/* ****************************************************************************
*
* FILE                     main.cpp - main program for samson
*/
#include "SSManager.h"


 
/* ****************************************************************************
*
*
*/
int main(int arg , const char *argv[])
{
	ss::SSManager::shared()->run(arg, argv);
}
