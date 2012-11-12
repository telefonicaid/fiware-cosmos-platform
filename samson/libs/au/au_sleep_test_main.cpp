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



#include "parseArgs/parseArgs.h"

#include "au/string.h"
#include "au/time.h"
#include "au/Token.h"
#include "au/Cronometer.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


int logFd;


/* ****************************************************************************
 *
 * parse arguments
 */
PaArgument paArgs[] =
{
	PA_END_OF_ARGS
};


int counter;
double time_sleep;


void run_chindren()
{
    
    //    for (int i = 0 ; i < 100 ; i++ )
    //    LM_M(("Label %d",i));
    LM_M(("Start %d...",counter));
    {
        au::ExecesiveTimeAlarm alarm("sleep 0.1 secs (100000 usecs)" , 0.2 );
        usleep(100000);
    }
    LM_M(("Finish %d...",counter));
    exit(0);
}

/* ****************************************************************************
 *
 * main - 
 */

int main(int argC, const char *argV[])
{
    
	paConfig("usage and exit on any warning", (void*) true);
	paConfig("log to screen",                 (void*) true);
	paConfig("log file line format",          (void*) "TYPE:DATE:EXEC-AUX/FILE[LINE](p.PID)(t.TID) FUNC: TEXT");
	paConfig("log to file",                   (void*) true);
    
	paParse(paArgs, argC, (char**) argV, 1, false);
	logFd = lmFirstDiskFileDescriptor();
    
    time_sleep = 0.5;

    pid_t pids[100];    
    
    au::Cronometer c;
    
    counter = 0;
    
    while( true )
    {
        for (int i = 0; i < 100 ; i++ )
        {
            counter++;
            pid_t pid = fork();
            
            if ( pid == 0 )
                run_chindren();
            else
                pids[i] = pid;
        }
        
        // Waid for all childrens
        for (int i = 0; i < 100 ; i++ )
            waitpid(pids[i], NULL, 0);
        
        LM_M(("Testing sleeps (t=%f) --> counter %d " , time_sleep , counter ));
        au::ExecesiveTimeAlarm alarm("sleep 0.1 secs (100000 usecs)" , 2*time_sleep );
        usleep( au::seconds_to_microseconds( time_sleep ) );
        
        
    }
    
}
