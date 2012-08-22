
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


#include <arpa/inet.h>          // inet_ntoa
#include <errno.h>
#include <fcntl.h>              // fcntl, F_SETFD
#include <netdb.h>              // gethostbyname
#include <netinet/in.h>         // struct sockaddr_in
#include <netinet/tcp.h>        // TCP_NODELAY
#include <stdio.h>
#include <stdlib.h>             // free
#include <sys/socket.h>         // socket, bind, listen
#include <sys/types.h>          // types needed by socket include files
#include <sys/un.h>             // sockaddr_un
#include <unistd.h>             // close

#include "au/mutex/TokenTaker.h"

#include "logMsg/logMsg.h"      // LM_T
#include "logMsg/traceLevels.h"  // LmtFileDescriptors, etc.

#include "FileDescriptor.h"  // Own interface

namespace au {
FileDescriptor::FileDescriptor(const std::string& name, int fd)
  : token_("FileDescriptor")
    , name_(name)
    , fd_(fd) {
}

FileDescriptor::~FileDescriptor() {
  Close();   // Close if still open
}

int FileDescriptor::fd()  const {
  return fd_;
}

std::string FileDescriptor::name() const {
  return name_;
}

au::rate::Rate& FileDescriptor::rate_in() {
  return rate_in_;
}

au::rate::Rate& FileDescriptor::rate_out() {
  return rate_out_;
}

void FileDescriptor::set_name(const std::string& name) {
  name_ = name;
}

// Disconnect
void FileDescriptor::Close() {
  au::TokenTaker tt(&token_);

  if (fd_ != -1) {
    LM_LT(LmtFileDescriptors, ("Closing FileDescriptor fd:%d", fd_));
    int r = ::close(fd_);
    if (r != 0) {
      LM_W(("Error closing fd %d in au::FileDescriptor %s", fd_, name_.c_str()));
    }
    fd_ = -1;
  }
}

bool FileDescriptor::IsClosed() const {
  return ( fd_ == -1 );
}

std::string FileDescriptor::str() {
  return au::str("FileDescriptor (fd=%d)", fd_);
}

Status FileDescriptor::okToSend(int tries, int tv_sec, int tv_usec) {
  int fds;
  fd_set wFds;
  struct timeval timeVal;
  int tryh;

  for (tryh = 0; tryh < tries; tryh++) {
    timeVal.tv_sec  = tv_sec;
    timeVal.tv_usec = tv_usec;

    // Connection previously closed by someone
    if (fd_ == -1) {
      return ConnectionClosed;
    }

    FD_ZERO(&wFds);
    FD_SET(fd_, &wFds);

    do {
      fds = select(fd_ + 1, NULL, &wFds, NULL, &timeVal);
    } while ((fds == -1) && (errno == EINTR));

    if ((fds == 1) && (FD_ISSET(fd_, &wFds))) {
      return OK;
    }

    if (fds == -1) {
      return SelectError;  // LM_RE(Error, ("Select over fd %d: %s", fd , strerror(errno)));
    }
    if (tryh > 3) {
      if (tryh % 10 == 0) {
        // Traces canceled since it is used to send traces to a server
        LM_LW(("Problems to send to %s (%d/%d secs)", name_.c_str(), tryh,
               tries ));
      }
    }
  }

  return Timeout;
}

Status FileDescriptor::partWrite(const void *dataP, int dataLen,
                                 const char *what, int retries, int tv_sec,
                                 int tv_usec) {
  int nb;
  int tot  = 0;
  char *data = (char *)dataP;
  Status s;

  while (tot < dataLen) {
    s = okToSend(retries, tv_sec, tv_usec);
    if (s != OK) {
      return s;
    }

    // Traces removed since this class is used to send traces to log server
    // LM_RE(s, ("Cannot write to '%s' after %d tries ( waiting %f seconds ) (fd %d)", name.c_str(), retries , (double) tv_sec + ((double)tv_usec/1000000.0)  , fd ));

    nb = ::write(fd_, &data[tot], dataLen - tot);
    if (nb == -1) {
      // LM_RE(WriteError, ("error writing to '%s' (fd: %d): %s", name.c_str(), fd, strerror(errno)));
      return WriteError;
    } else if (nb == 0) {
      // LM_RE(WriteError, ("part-write written ZERO bytes to '%s' (total: %d)", name.c_str(), tot));
      return WriteError;
    }

    // Add bytes to the count
    rate_out_.Push(nb);
    tot += nb;
  }

  return OK;
}

Status FileDescriptor::msgAwait(int secs, int usecs, const char *what) {
  struct timeval tv;
  struct timeval *tvP;
  int fds;
  fd_set rFds;

  do {
    if (secs == -1) {
      tvP = NULL;
    } else {
      tv.tv_sec  = secs;
      tv.tv_usec = usecs;

      tvP        = &tv;
    }

    FD_ZERO(&rFds);
    FD_SET(fd_, &rFds);
    fds = select(fd_ + 1, &rFds, NULL, NULL, tvP);
  } while ((fds == -1) && (errno == EINTR));

  if (fds == -1) {
    // LM_RP(SelectError, ("select error awaiting '%s' from '%s", what, name.c_str()));
    return SelectError;
  } else if (fds == 0) {
    // LM_RE(Timeout, ("timeout awaiting '%s' from '%s' (%d.%06d seconds)", what, host.c_str(), secs, usecs));
    return Timeout;
  } else if ((fds > 0) && (!FD_ISSET(fd_, &rFds))) {
    return Error;  // LM_RE(Error, ("some other fd has a read pending - this is impossible ! (awaiting '%s' from '%s')", what, name.c_str()));
  } else if ((fds > 0) && (FD_ISSET(fd_, &rFds))) {
    return OK;
  }







  LM_LX(1, ("Very strange error awaiting '%s' from '%s'", what, name_.c_str()));

  return Error;
}

Status FileDescriptor::WriteLine(const char *line, int retries, int tv_sec,
                                 int tv_usec) {
  size_t nb = strlen(line);
  Status s = partWrite(line, nb, "write line", retries, tv_sec, tv_usec);

  if (s != OK) {
    Close();
  }
  rate_out_.Push(nb);

  return s;
}

Status FileDescriptor::readBuffer(char *line, size_t max_size, int tmoSecs) {
  int nb;

  nb = read(fd_, line, max_size);

  if (nb > 0) {
    // Add bytes to the count
    rate_in_.Push(nb);
    return OK;
  }
  return ReadError;
}

Status FileDescriptor::ReadLine(char *line, size_t max_size, int max_seconds) {
  au::Cronometer c;
  size_t tot = 0;

  while (true) {
    // Read a byte
    Status s = partRead(line + tot, 1, "web line character",
                        max_seconds - c.seconds());

    if (s != OK) {
      Close();
      return s;
    }

    // Add 1 char to the count
    rate_in_.Push(1);

    if (line[tot] == '\n') {
      line[tot + 1] = '\0';  // Keep the \n at the end of the line
      LM_READS("REST Interface", "Web Line", line, tot, LmfByte);
      return OK;
    }

    // A new letter has been read
    tot++;

    // Check excesive line length
    if (tot >= ( max_size - 2 )) {
      return Error;
    }
  }
}

Status FileDescriptor::partRead(void *vbuf, size_t bufLen, const char *what,
                                int max_seconds,
                                size_t *read_size) {
  size_t tot = 0;
  Status s;
  char *buf = (char *)vbuf;

  // Cronometer to measure absolute max time
  au::Cronometer cronometer;

  while (tot < bufLen) {
    ssize_t nb;

    // Wait until OK to read
    do {
      if (fd_ == -1) {
        return ConnectionClosed;
      }

      s = msgAwait(1, 0, what);   // Continous try with 1 second timeout to check max_seconds

      if (( s != OK ) && ( s != Timeout )) {
        if (read_size) {
          *read_size = tot;
        }
        return s;                       // Different error, just report
      }

      // Report timeout if max seconds is excedded
      if (max_seconds > 0) {
        if (cronometer.seconds() > max_seconds) {
          if (read_size) {
            *read_size = tot;
          }
          return Timeout;
        }
      }
    } while (s != OK);

    // Read call
    nb = read(fd_, (void *)&buf[tot], bufLen - tot);

    // Report connection closed if so
    if (nb == -1) {
      if (errno == EBADF) {
        LM_LE((
                "read: %s (treating as Connection Closed), expecting '%s' from %s"
                , strerror(errno), what, name_.c_str()));
        return ConnectionClosed;
      }

      LM_LE(("read: %s, expecting '%s' from %s", strerror(errno), what,
             name_.c_str()));
      return ConnectionClosed;
    } else if (nb == 0) {
      if (read_size) {
        *read_size = tot;
      }
      return ConnectionClosed;
    }

    // Add readed bytes to the count
    rate_in_.Push(nb);

    tot += nb;
  }

  if (read_size) {
    *read_size = tot;
  }
  return OK;
}
}
