/* ****************************************************************************
*
* FILE                     main.cpp - main program for samson
*/
#include <stdio.h>
#include <fstream>
#include <iostream>

#include "au.h"
#include "samson.h"
#include "KVFormat.h"
#include "KVSet.h"
#include "MemoryController.h"
#include "KVManager.h"
#include "Task.h"
#include "KVSet.h"
#include "KVQueue.h"
#include "Data.h"
#include "Module.h"
#include "TaskManager.h"
#include "Tasks.h"
#include "SSManager.h"
#include "NetworkManager.h"
#include "KVFormat.h"


 
/* ****************************************************************************
*
*
*/
int main(int arg , const char *argv[])
{
	ss::SSManager::shared()->run(arg, argv);
}
