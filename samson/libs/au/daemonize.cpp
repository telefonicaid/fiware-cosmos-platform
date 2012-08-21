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

/*
 * FILE                     daemonize.cpp
 *
 * AUTHOR                   Ken Zangelin
 *
 * CREATION DATE            Feb 25 2011
 */

#include <stdio.h>              // stdin, stdout, stderr
#include <stdlib.h>             // free
#include <sys/stat.h>           // umask
#include <sys/types.h>          // pid_t
#include <unistd.h>             // getppid, fork, setsid

#include "./daemonize.h"        // Own interface
#include "logMsg/logMsg.h"      // LM_*


extern "C" void exit(int status);
/* ****************************************************************************
 *
 * daemonize -
 */
void daemonize(void) {
  pid_t pid;
  pid_t sid;

  // already daemon
  if (getppid() == 1)
    return;

  pid = fork();
  if (pid == -1)
    LM_X(1, ("fork: %s", strerror(errno)));

  // Exiting father process
  if (pid > 0)
    exit(0);

  // Change the file mode mask */
  umask(0);

  // Removing the controlling terminal
  sid = setsid();
  if (sid == -1)
    LM_X(1, ("setsid: %s", strerror(errno)));

  // Change current working directory.
  // This prevents the current directory from being locked; hence not being able to remove it.
  if (chdir("/") == -1)
    LM_X(1, ("chdir: %s", strerror(errno)));
}

void deamonize_close_all(void) {
  // Redirect standard files to /dev/null
  if (freopen("/dev/null", "r", stdin) == NULL)
    LM_X(1, ("Error setting STDIN redirect to /dev/null: %s", strerror(errno)));
  if (freopen("/dev/null", "r", stdout) == NULL)
    LM_X(1, ("Error setting STDOUT redirect to /dev/null: %s", strerror(errno)));
  if (freopen("/dev/null", "r", stderr) == NULL)
    LM_X(1, ("Error setting STDERR redirect to /dev/null: %s", strerror(errno)));
}

