/* ****************************************************************************
*
* FILE            main_UnitTest.cpp
*
* AUTHOR          Javier Lois
*
* DATE            December 2011
*
* DESCRIPTION
*
* Main file for the automatic unit testing application
*
*/

#include <stdio.h>

#include "gtest/gtest.h"

#include "parseArgs/parseArgs.h"
#include "samson/common/traces.h"
#include "samson/common/SamsonSetup.h"

#include "logMsg/logMsg.h"

/* ****************************************************************************
*
* initLog - 
*/
void initLog(char* pname)
{
    //Initialise log
	LmStatus    s;
	char        w[512];
	int lmSd;
	int fd = 1;
	progName = (char*)malloc(512);
	strcpy(progName, pname);
    /*if ((progName = lmProgName(pname, 1, false)) == NULL)
    {
        return;
    }*/
    s = lmFdRegister(fd, "DEF", "DEF", "stdout", &lmSd);
    if (s != LmsOk)
    {
        sprintf(w, "lmFdRegister: %s", lmStrerror(s));
        std::cerr << w << std::endl;
        return;
    }

	lmExitFunction(NULL, NULL);
	
    if ((s = lmInit()) != LmsOk)
    {
        sprintf(w, "lmInit: %s", lmStrerror(s));
        std::cerr << w << std::endl;
        return;
    } 
}

/* ****************************************************************************
*
* main - 
*/

int main(int argc, char **argv) {

  /*	paConfig("usage and exit on any warning", (void*) false);
	paConfig("log to screen",                 (void*) true);
	paConfig("log file line format",          (void*) "TYPE:DATE:EXEC-AUX/FILE[LINE] (p.PID) FUNC: TEXT");
	paConfig("screen line format",            (void*) "TYPE: TEXT");
	paConfig("log to file",                   (void*) false);

    samson::SamsonSetup::init("","");
*/    //char* pname = (char*)malloc(255);
    //initLog(argv[0]);
    //int logfdp;
    //samson::samsonInitTrace(argc, const_cast<const char**>(argv), &logfdp, true, false);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}


