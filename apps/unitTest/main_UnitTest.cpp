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
* initLog() 
* Initialise log
*/

LmStatus initLog(char* pname)
{
    LmStatus s;
    int      i2;
    char w[512];

    //
    // 1. create the progName variable, removing path etc.
    //    progName is used to create the logfile among other things
    /*if ((progName = lmProgName(pname, 1, false)) == NULL)
        return LmsPrognameError;
        */
	progName = (char*)malloc(512);
	strcpy(progName, "unitTest");
        

    //
    // 2. Register fd 1 (stdout) so that I get traces on the console (fourth parameter is just a name)
    //    If you don't want traces to stdout /or any file descriptor), don't use this call ...
    /*int      i1;
    if ((s = lmFdRegister(1, "TYPE:EXEC/FUNC: TEXT", "DEF", "nul", &i1)) != LmsOk)
    {
       sprintf(w, "lmInit: %s", lmStrerror(s));
        std::cerr << w << std::endl;
         return s;
    }*/
    

    //
    // 3. Register a log file, first parameter is the directory. The name of the log file uses progName and 'Log' is appended
    //
    if ((s = lmPathRegister("/tmp", "DEF", "DEF", &i2)) != LmsOk)
    {
       sprintf(w, "lmInit: %s", lmStrerror(s));
        std::cerr << w << std::endl;
        return s;
    }

    //
    // 4. Start the whole thing ...
    //
    if ((s = lmInit()) != LmsOk)
    {
       sprintf(w, "lmInit: %s", lmStrerror(s));
        std::cerr << w << std::endl;
        return s;
    }

    //
    // set the trace level, according to the variable tLevel
    //
    if ((s = lmTraceSet(0)) != LmsOk)
    {
       sprintf(w, "lmInit: %s", lmStrerror(s));
        std::cerr << w << std::endl;
        return s;
    }
   
    return LmsOk;
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
    initLog(argv[0]);
    //int logfdp;
    //samson::samsonInitTrace(argc, const_cast<const char**>(argv), &logfdp, true, false);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}


