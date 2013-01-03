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

#include "gpb.h"  // Own interface


namespace au {
Status iomMsgAwait(int fd, int secs) {
  struct timeval tv;
  struct timeval *tvP;
  int fds;
  fd_set rFds;

  if (secs == -1) {
    tvP = NULL;
  } else {
    tv.tv_sec  = secs;
    tv.tv_usec = 0;

    tvP        = &tv;
  }

  do {
    FD_ZERO(&rFds);
    FD_SET(fd, &rFds);
    fds = select(fd + 1, &rFds, NULL, NULL, tvP);
  } while ((fds == -1) && (errno == EINTR));

  if (fds == -1) {
    LM_RP(SelectError, ("iomMsgAwait: select returns -1 with errno:%d for fd:%d in %d seconds", errno, fd, secs));
  } else if (fds == 0) {
    LM_RE(Timeout,
          ("iomMsgAwait: timeout in select returns 0 for fd:%d in %d seconds", fd,
           secs));
  } else if ((fds > 0) && (!FD_ISSET(fd, &rFds))) {
    LM_X(1,
         ("iomMsgAwait: some other fd has a read pending - this is impossible ! (select for fd:%d)",
          fd));
  } else if ((fds > 0) && (FD_ISSET(fd, &rFds))) {
    return OK;
  }

  LM_X(1, ("iomMsgAwait: Other very strange error"));
  return Error;
}
}