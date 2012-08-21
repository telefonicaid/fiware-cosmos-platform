/* ****************************************************************************
 *
 * FILE                     arcanumConsumer.cpp
 *
 * DESCRIPTION
 *
 * AUTHOR                   Ken Zangelin
 *
 * CREATION DATE            Dec 9 2011
 *
 *
 * ToDo
 *
 */
#include <arpa/inet.h>          // inet_ntoa
#include <fcntl.h>              // fcntl, F_SETFD
#include <netdb.h>              // gethostbyname
#include <netinet/in.h>         // struct sockaddr_in
#include <netinet/tcp.h>        // TCP_NODELAY
#include <stdlib.h>             // free
#include <sys/socket.h>         // socket, bind, listen
#include <sys/types.h>          // types needed by socket include files
#include <sys/un.h>             // sockaddr_un
#include <unistd.h>             // close

#include "logMsg/logMsg.h"
#include "logMsg/traceLevels.h"
#include "parseArgs/paConfig.h"
#include "parseArgs/parseArgs.h"



/* ****************************************************************************
 *
 * Option variables
 */
char host[512];
unsigned short port;
int bufSize;




#define HOST "172.17.200.200"
/* ****************************************************************************
 *
 * parse arguments
 */
PaArgument paArgs[] =
{
  { "-host", host,     "HOST",     PaStr,         PaOpt,         _i HOST,        PaNL,         PaNL,
    "host name of server"                            },
  { "-port", &port,    "PORT",     PaShortU,      PaOpt,         1099,           1,            65000,
    "port for server where to connect"               },
  { "-size", &bufSize, "BUF_SIZE", PaInt,         PaOpt,         1024,           10,           16 * 1024,
    "size of read buffer"                            },

  PA_END_OF_ARGS
};



/* ****************************************************************************
 *
 * connectToServer -
 */
int connectToServer(const char *host, unsigned short port) {
  struct hostent *hp;
  struct sockaddr_in peer;
  int fd;

  if (host == NULL)
    LM_RE(-1, ("no hostname given")); if (port == 0)
    LM_RE(-1, ("Cannot connect to '%s' - port is ZERO", host)); if ((hp = gethostbyname(host)) == NULL)
    LM_RE(-1, ("gethostbyname(%s): %s", host, strerror(errno))); if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    LM_RE(-1, ("socket: %s", strerror(errno))); memset((char *)&peer, 0, sizeof(peer));

  peer.sin_family      = AF_INET;
  peer.sin_addr.s_addr = ((struct in_addr *)(hp->h_addr))->s_addr;
  peer.sin_port        = htons(port);

  LM_V(("Connecting to %s:%d", host, port));
  int retries = 200;
  int tri     = 0;

  while (1) {
    if (connect(fd, (struct sockaddr *)&peer, sizeof(peer)) == -1) {
      ++tri;
      LM_E(("connect intent %d failed: %s", tri, strerror(errno)));
      usleep(50000);
      if (tri > retries) {
        close(fd);
        LM_RE(-1, ("Cannot connect to %s, port %d (even after %d retries)", host, port, retries));
      }
    } else {
      break;
    }
  }

  return fd;
}

char buf[16 * 1024 + 1];

/* ****************************************************************************
 *
 * readFromServer -
 */
void readFromServer(int fd) {
  int nb;
  int sz;

  sz = bufSize;
  while (1) {
    memset(buf, 0, bufSize + 1);
    nb = read(fd, buf, sz);
    if (nb == -1) {
      LM_X(1, ("error reading from socket: %s", strerror(errno)));
    } else if (nb == 0) {
      LM_E(("Read zero bytes"));
    } else {
      printf(buf);
      fflush(stdout);
    }
  }
}

/* ****************************************************************************
 *
 * main -
 */
int main(int argC, char *argV[]) {
  paConfig("usage and exit on any warning", (void *)true);
  paConfig("log to screen",                 (void *)true);
  paConfig("log file line format",          (void *)"TYPE:DATE:EXEC-AUX/FILE[LINE](p.PID)(t.TID) FUNC: TEXT");
  paConfig("screen line format",            (void *)"TYPE@TIME  EXEC: TEXT");
  paConfig("log to file",                   (void *)true);

  // paConfig("man synopsis",                  (void*) manSynopsis);
  // paConfig("man shortdescription",          (void*) manShortDescription);
  // paConfig("man description",               (void*) manDescription);
  // paConfig("man exitstatus",                (void*) manExitStatus);
  // paConfig("man author",                    (void*) manAuthor);
  // paConfig("man reportingbugs",             (void*) manReportingBugs);
  // paConfig("man copyright",                 (void*) manCopyright);
  // paConfig("man version",                   (void*) manVersion);


  paParse(paArgs, argC, (char **)argV, 1, false);

  lmAux((char *)"father");

  int fd = connectToServer(host, port);

  if (fd == -1)
    LM_X(1, ("error connecting to host '%s', port %d", host, port)); readFromServer(fd);

  return 0;
}

