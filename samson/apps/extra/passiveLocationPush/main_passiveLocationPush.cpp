/* ****************************************************************************
 *
 * FILE                     passiveLocationPush - example app using samsonClient lib
 *
 * AUTHOR                   Andreu Urruela, Ken Zangelin
 *
 * LAST MODIFICATION DATE   Jan 10 2012
 *
 *
 * Generate random xml documents simulating information from OSS Passive Location pilot
 * OR: connects to tektronixTunnel to receive real data and injects that data in samson
 *
 */
#include <arpa/inet.h>          // inet_ntoa
#include <dirent.h>             // opendir, scandir
#include <errno.h>              // errno
#include <fcntl.h>              // fcntl, F_SETFD
#include <inttypes.h>           // uint64_t etc.
#include <iostream>             // std::cout
#include <netdb.h>              // gethostbyname
#include <netinet/in.h>         // struct sockaddr_in
#include <netinet/tcp.h>        // TCP_NODELAY
#include <signal.h>             // signal, SIGINT, ...
#include <stdio.h>              // printf
#include <stdlib.h>             // exit
#include <stdlib.h>             // free
#include <string.h>             // memcpy
#include <sys/socket.h>         // socket, bind, listen
#include <sys/stat.h>           // stat
#include <sys/types.h>          // types needed by socket include files
#include <sys/un.h>             // sockaddr_un
#include <time.h>               // strptime, struct tm
#include <unistd.h>             // close

#include "au/CommandLine.h"     // au::CommandLine
#include "au/statistics/Cronometer.h"      // au::Cronometer
#include "au/string/StringUtilities.h"          // au::str()
#include "au/time.h"            // au::todatString()

#include "logMsg/logMsg.h"
#include "logMsg/traceLevels.h"
#include "parseArgs/paConfig.h"
#include "parseArgs/paUsage.h"
#include "parseArgs/parseArgs.h"

#include "samson/client/SamsonClient.h"         // samson::SamsonClient
#include "samson/client/SamsonPushBuffer.h"
#include "samson/common/samsonVars.h"


/* ****************************************************************************
 *
 * File -
 */
typedef struct File {
  char name[256];
  time_t date;
  uint64_t size;
  int fd;
} File;


/* ****************************************************************************
 *
 * Option variables
 */
SAMSON_ARG_VARS;

char controller[256];
char user[1024];
char password[1024];
char file[256];
char directory[256];
char queueName[256];
bool tektronix;
bool fake;
bool toFile;
char host[256];
unsigned short port       = 0;
int timeout    = 0;
double rate       = 1;


#define K(k) (1024 * k)
#define M(m) (1024 * 1024 * m)
/* ****************************************************************************
 *
 * parse arguments
 */
PaArgument paArgs[] =
{
  SAMSON_ARGS,

  { "-dir",        directory,         "SS_PLP_DIRECTORY",                   PaString,
    PaOpt,
    _i "nodir",    PaNL,              PaNL,
    "directory"                              },
  { "-tektronix",  &tektronix,        "SS_PLP_TEKTRONIX",                   PaBool,
    PaOpt,
    false,         false,             true,
    "act as tektronix consumer"              },
  { "-fake",       &fake,             "SS_PLP_FAKE",                        PaBool,
    PaOpt,
    false,         false,             true,
    "fake data"                              },
  { "-host",       host,              "SS_PLP_TEKTRONIX_HOST",              PaString,
    PaOpt,
    PaND,          PaNL,              PaNL,
    "tektronix tunnel host"                  },
  { "-port",       &port,             "SS_PLP_TEKTRONIX_PORT",              PaShortU,
    PaOpt,
    1099,          1024,
    65535,         "tektronix tunnel port"                  },
  { "-queue",      queueName,         "SS_PLP_QUEUE_NAME",                  PaString,
    PaOpt,
    PaND,          PaNL,              PaNL,
    "name of the queue"                      },
  { "-timeout",    &timeout,          "SS_PLP_TIMEOUT",                     PaInt,
    PaOpt,
    0,             0,                 3600,
    "timeout"                                },
  { "-rate",       &rate,             "SS_PLP_RATE",                        PaDouble,
    PaOpt,
    _i 1.0,        PaNL,              PaNL,
    "rate"                                   },
  { "-node",       controller,        "",                                   PaString,
    PaOpt,
    _i "localhost",PaNL,              PaNL,
    "SAMSON node"                            },
  { "-user",       user,              "",                                   PaString,
    PaOpt,
    _i "anonymous",PaNL,              PaNL,
    "User to connect to SAMSON cluster"      },
  { "-password",   password,          "",                                   PaString,
    PaOpt,
    _i "anonymous",PaNL,              PaNL,
    "Password to connect to SAMSON cluster"  },
  { "-file",       file,              "SS_PLP_FILE",                        PaString,
    PaOpt,
    _i "generator",PaNL,              PaNL,
    "file"                                   },

  PA_END_OF_ARGS
};



/* ****************************************************************************
 *
 * Global variables
 */
unsigned int bufSize = M(8) + 1;
char buf[M(8) + 1];            // MAX size: 8 megas ... ?
char buffer[M(8) + 1];         // MAX size: 8 megas ... ?
char savedBuffer[128 * 1024];
int savedMissing      = 0;
int savedLen          = 0;
int storageFd         = -1;
int nbAccumulated     = 0;
int packets           = 0;
int totalFileSize     = 0;
int pushed            = 0;
int nextIndex         = 0;
int files;
time_t newest  = 0;
File *fileV;
struct timeval startTime;
struct timeval endTime;
struct timeval diffTime;
unsigned long bytesPushed = 0;

/* ****************************************************************************
 *
 * timeDiff -
 */
void timeDiff(struct timeval *end, struct timeval *start, struct timeval *diff) {
  diff->tv_sec  = end->tv_sec  - start->tv_sec;
  diff->tv_usec = end->tv_usec - start->tv_usec;

  if (diff->tv_usec < 0) {
    diff->tv_sec  -= 1;
    diff->tv_usec += 1000000;
  }
}

/* ****************************************************************************
 *
 * toFile -
 */
void toFileF(const char *buf, int bufLen) {
  static int fd  = -1;
  int tot = 0;
  int nb;

  if (toFile == false) {
    return;
  }

  if (fd == -1) {
    LM_V(("Opening plp.bin"));
    fd = open("plp.bin", O_WRONLY | O_CREAT | O_TRUNC, 0777);
    if (fd == -1) {
      LM_X(1, ("open('plp.bin): %s", strerror(errno)));
    }
  }

  while (tot < bufLen) {
    nb = write(fd, &buf[tot], bufLen - tot);
    if (nb == -1) {
      LM_X(1, ("Error writing to 'plp.bin': %s", strerror(errno)));
    } else if (nb == 0) {
      LM_RVE(("written ZERO bytes to 'plp.bin'"));
    }
    tot += nb;
  }

  totalFileSize += bufLen;
  // LM_M(("Written %d bytes to file (%d bytes in total)", bufLen, totalFileSize));
}

/* ********************************************************************************
 *
 * Handler for SIGINT
 */
sig_atomic_t signaled_quit = false;
void captureSIGINT(int s) {
  s = 0;

  signaled_quit = true;
  lmVerbose     = false;
}

/* ****************************************************************************
 *
 * connectToServer -
 */
int connectToServer(void) {
  struct hostent *hp;
  struct sockaddr_in peer;
  int fd;

  if (host == NULL) {
    LM_RE(-1, ("no hostname given"));
  }
  if (port == 0) {
    LM_RE(-1, ("Cannot connect to '%s' - port is ZERO", host));
  }
  if ((hp = gethostbyname(host)) == NULL) {
    LM_RE(-1, ("gethostbyname(%s): %s", host, strerror(errno)));
  }
  if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    LM_RE(-1, ("socket: %s", strerror(errno)));
  }
  memset((char *)&peer, 0, sizeof(peer));

  peer.sin_family      = AF_INET;
  peer.sin_addr.s_addr = ((struct in_addr *)(hp->h_addr))->s_addr;
  peer.sin_port        = htons(port);

  LM_V(("Connecting to %s:%d", host, port));
  int retries = 200;
  int tri     = 0;

  while (1) {
    if (connect(fd, (struct sockaddr *)&peer, sizeof(peer)) == -1) {
      ++tri;
      LM_W(("connect intent %d failed: %s", tri, strerror(errno)));
      usleep(500000);
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

/* ****************************************************************************
 *
 * bufPresent -
 */
void bufPresent(const char *title, char *buf, int bufLen, int verboseLevel) {
  int ix = 0;

  if (lmVerbose < verboseLevel) {
    return;
  }

  printf("----- %s -----\n", title);

  while (ix < bufLen) {
    if (ix % 16 == 0) {
      printf("%08x:  ", ix);
    }
    printf("%02x ", buf[ix] & 0xFF);
    ++ix;
    if (ix % 16 == 0) {
      printf("\n");
    }
  }

  printf("\n");
  printf("\n");
}

/* ****************************************************************************
 *
 * bufPush -
 */
int bufPush(char *buf, unsigned int size, samson::SamsonPushBuffer *pushBuffer) {
  unsigned int packetLen;
  char *initialBuf = buf;
  unsigned long totalLen   = 0;
  unsigned long dataLen;
  static int calls = 0;
  int relPacket = 0;

  ++calls;

  LM_V3(("Pushing a buffer of %d bytes", size));

  //
  // Loop over all packets in the buf and write them to file/push
  //
  while (size > 0) {
    packetLen = ntohl(*((int *)buf));
    dataLen   = packetLen + 4;

    // NOT Enough bytes for an entire packet?
    if ((size < 4) || size < dataLen) {
      if (size >= 4) {
        // LM_W(("bytes read: %d, packetLen == %d (totalLen == %d)", size, packetLen, totalLen));
        if (packetLen > 50000) {
          bufPresent("bad packetLen", buf, size, 1);
          LM_X(1,
               ("Bad packetLen in packet %d: %d (original: 0x%x, htohl: 0x%x)", packets, packetLen, *((int *)buf),
                ntohl(*((int *)buf))));
        }  // else
      }
      //    LM_W(("bytes read: %d (totalLen == %d)", size, totalLen));

      if (totalLen != 0) {
        if (fake == false) {
          pushBuffer->push(initialBuf, totalLen, false);
          pushed += totalLen;
          LM_V2(("Pushed buffer of %d bytes to samson (total: %lu bytes)", totalLen, pushed));
        }

        toFileF(initialBuf, totalLen);
      }

      memcpy(initialBuf, buf, size);
      return size;
    }

    if (packetLen > 50000) {
      bufPresent("Bad Packet Len (>50000)", buf, size, 1);
      LM_X(1,
           ("Bad packetLen in packet %d: %d (original: 0x%x, htohl: 0x%x)", packets + 1, packetLen, *((int *)buf),
            ntohl(*((int *)buf))));
    }

    ++packets;
    ++relPacket;

    size     -= dataLen;
    totalLen += dataLen;

    LM_V2(("Got packet %d.%d (%d) of %d bytes (%d bytes read). %d bytes left", calls, relPacket, packets, packetLen + 4,
           totalLen, size));
    bufPresent("packet", buf, packetLen + 4, 1);

    buf       = &buf[dataLen];
  }

  if (totalLen != 0) {
    if (fake == false) {
      pushBuffer->push(initialBuf, totalLen, false);
      pushed += totalLen;
      LM_V(("Pushed buffer of %d bytes to samson (total: %lu bytes)", totalLen, pushed));
    }
    toFileF(initialBuf, totalLen);
  }
  // else
  //   LM_W(("totalLen == %d", totalLen));

  return size;
}

/* ****************************************************************************
 *
 * dirPresent -
 */
void dirPresent(void) {
  int ix;

  for (ix = 0; ix < files; ix++) {
    printf("  %s\n", fileV[ix].name);
  }
}

/* ****************************************************************************
 *
 * dirScan -
 */
int dirScan(const char *dirPath, const char *suffix) {
  DIR *dir;
  struct dirent *entry;
  int files;

  LM_V(("Opening directory '%s'", dirPath));

  if (chdir(dirPath) == -1) {
    LM_X(1, ("Error changing to directory '%s': %s", dirPath, strerror(errno)));
  }
  dir  = opendir(".");
  if (dir == NULL) {
    LM_X(1, ("opendir(%s): %s", dirPath, strerror(errno)));  //
  }
  // Count number of files in the directory
  //
  files = 0;
  while ((entry = readdir(dir)) != NULL) {
    char *suff;

    if (entry->d_name[0] == '.') {
      continue;
    }
    suff = strrchr(entry->d_name, '.');
    if (suff == NULL) {
      continue;
    }
    ++suff;
    if (strcmp(suff, suffix) != 0) {
      continue;
    }
    ++files;
  }
  LM_V(("Initially found %d files in '%s'", files, dirPath));

  //
  // Create a vector to hold the files
  //
  fileV = (File *)calloc(files, sizeof(File));
  if (fileV == NULL) {
    LM_X(1, ("error allocating vector for %d Files: %s", files, strerror(errno)));  //
  }
  // Gather info on the files and fill the vector
  //
  rewinddir(dir);
  int ix = 0;
  while ((entry = readdir(dir)) != NULL) {
    char *suff;
    struct stat statBuf;

    if (entry->d_name[0] == '.') {
      continue;
    }
    suff = strrchr(entry->d_name, '.');
    if (suff == NULL) {
      continue;
    }
    ++suff;
    if (strcmp(suff, suffix) != 0) {
      continue;
    }
    if (stat(entry->d_name, &statBuf) == -1) {
      LM_E(("stat(%s): %s", entry->d_name, strerror(errno)));
      continue;
    }

    if (!S_ISREG(statBuf.st_mode)) {
      continue;
    }
    if (newest != 0) {               // newest == 0 only the first time ...
      LM_D(("comparing newest=%lu to %s:%lu", newest, entry->d_name, (unsigned long)statBuf.st_mtime));
      if (statBuf.st_mtime <= newest) {
        LM_D(("skipping '%s' as it is old", entry->d_name));
        continue;
      }
    } else {
      LM_D(("newest == 0 - first time in function?"));
    }

    if (strlen(entry->d_name) > sizeof(fileV[ix].name)) {
      LM_E(("File name too long: '%s'", entry->d_name));
      continue;
    }

    LM_V2(("Adding file '%s'", entry->d_name));

    strcpy(fileV[ix].name, entry->d_name);
    fileV[ix].date      = statBuf.st_mtime;
    fileV[ix].size      = statBuf.st_size;

    ++ix;
  }
  closedir(dir);

  files = ix;
  LM_V(("%d files in vector", files));


  //
  // Sort the vector in name order (smallest first)
  //
  int first;
  int second;
  int smallestIx = -1;
  File *smallest;
  File copy;

  for (first = 0; first < files - 1; first++) {
    smallest = &fileV[first];

    if (fileV[first].date > newest) {
      newest = fileV[first].date;
      LM_D(("newest: %lu", newest));
    }

    for (second = first + 1; second < files; second++) {
      if (strcmp(fileV[second].name, smallest->name) < 0) {
        smallest = &fileV[second];
        smallestIx = second;
      }
    }
    LM_V(("%05d: '%s'", first, smallest->name));

    // Swap
    if (smallest != &fileV[first]) {
      LM_VV(("Copying smallest from index %d to %d", smallestIx, first));
      memcpy(&copy, &fileV[first], sizeof(copy));
      memcpy(&fileV[first], smallest, sizeof(fileV[first]));
      memcpy(smallest, &copy, sizeof(copy));
    }
  }

  LM_V(("Done scanning directory '%s'", dirPath));

  return files;
}

int pushes    = 0;
int nonsleeps = 0;
/* ****************************************************************************
 *
 * injectFromFile -
 */
void injectFromFile(File *fileP, samson::SamsonPushBuffer *pushBuffer, char *buffer, int bufLen) {
  int nb;
  unsigned int total;
  float wantedTime;
  float executionTime;
  int64_t sleepTime;
  float actualRate;

  LM_V2(("reading chunks of %d bytes from %s (fd %d)", bufLen, fileP->name, fileP->fd));
  total = 0;
  while (total < fileP->size) {
    nb = read(fileP->fd, buffer, bufLen);
    if (nb == -1) {
      LM_X(1, ("read(%s): %s", fileP->name, strerror(errno)));
    } else if (nb == 0) {
      LM_X(1, ("read ZERO bytes from %s (fd %d)", fileP->name, fileP->fd));
    }
    if (fake == false) {
      pushBuffer->push(buffer, nb, false);
    }
    bytesPushed += nb;
    total       += nb;
    pushes      += 1;


    gettimeofday(&endTime, NULL);
    timeDiff(&endTime, &startTime, &diffTime);

    wantedTime    = ((float)bytesPushed / 1024 / 1024) / rate;
    executionTime = float(diffTime.tv_sec) + ((float)diffTime.tv_usec) / 1000000;
    sleepTime     = (int64_t)((wantedTime - executionTime) * 1000000);
    actualRate    = ((float)bytesPushed / 1024 / 1024) / ((float)diffTime.tv_sec + (float)diffTime.tv_usec / 1000000);

    if ((pushes != 0) && (pushes % 500 == 0)) {
      LM_V(("------------------------------------------------------------------------------"));
      LM_V(("Pushes:                       %d/%d (%d%%)",   pushes, nonsleeps, (nonsleeps / pushes) * 100));
      LM_V(("Bytes pushed so far:          %ldMB",   bytesPushed / 1024 / 1024));
      LM_V(("Actual rate:                  %f Mb/s", actualRate));
      LM_V(("Wanted rate:                  %f Mb/s", rate));
      LM_V(("Time we've been executing:    %.6fs",   executionTime));
      LM_V(("Time to get to correct rate:  %.6fs",   wantedTime));
      LM_V(("Time to sleep:                %dus",    sleepTime));
    }

    if (sleepTime > 0) {
      usleep(sleepTime);
    } else {
      ++nonsleeps;
    }
  }
}

/* ****************************************************************************
 *
 * injectFromDirectory -
 */
void injectFromDirectory(const char *dirPath, samson::SamsonPushBuffer *pushBuffer) {
  int fileIx;
  char *buffer;
  float executionTime;
  float actualRate;

  buffer = (char *)malloc(bufSize);
  if (buffer == NULL) {
    LM_X(1, ("error allocating buffer of %d bytes", bufSize));
  }
  LM_V(("Reading from directory '%s'", dirPath));

  files = dirScan(dirPath, "blob");

  for (fileIx = 0; fileIx < files; fileIx++) {
    fileV[fileIx].fd = open(fileV[fileIx].name, O_RDONLY);
    LM_V2(("file %d (%s) opened as file descriptor %d", fileIx, fileV[fileIx].name, fileV[fileIx].fd));
    if (fileV[fileIx].fd == -1) {
      LM_E(("error opening '%s': %s", fileV[fileIx], strerror(errno)));
      continue;
    }
    injectFromFile(&fileV[fileIx], pushBuffer, buffer, bufSize);
  }

  gettimeofday(&endTime, NULL);
  timeDiff(&endTime, &startTime, &diffTime);

  executionTime = float(diffTime.tv_sec) + ((float)diffTime.tv_usec) / 1000000;
  actualRate    = ((float)bytesPushed / 1024 / 1024) / ((float)diffTime.tv_sec + (float)diffTime.tv_usec / 1000000);

  LM_V(("============ Injected %d files to samson =====================================", files));
  LM_V(("Pushes:                       %d",          pushes));
  LM_V(("Bytes pushed:                 %ld (%ldMB)", bytesPushed, bytesPushed / 1024 / 1024));
  LM_V(("Final rate:                   %f Mb/s",     actualRate));
  LM_V(("Execution Time:               %.6fs",       executionTime));
}

/* ****************************************************************************
 *
 * injectFromServer -
 */
void injectFromServer(int fd, samson::SamsonPushBuffer *pushBuffer) {
  int nb;

  LM_VVV(("Reading from server"));
  while (signaled_quit == false) {
    // Read as much as we possibly can ...
    nb = read(fd, &buffer[nextIndex], sizeof(buffer) - nextIndex);
    if (nb > 0) {
      nbAccumulated += nb;
      LM_V2(("Read %d bytes from tunnel (%d bytes in total)", nb, nbAccumulated));

      bufPresent("Read from Tunnel", buffer, nb + nextIndex, 2);
      nextIndex = bufPush(buffer, nb + nextIndex, pushBuffer);
    } else {
      if (nb == -1) {
        LM_E(("Error reading from the Tunnel: %s", strerror(errno)));
      } else {
        LM_E(("The Tunnel closed the connection?"));
      } close(fd);
      fd        = connectToServer();
      nextIndex = 0;
    }
  }
}

/* ****************************************************************************
 *
 * directoryData -
 */
void directoryData(samson::SamsonPushBuffer *pushBuffer, const char *dirPath) {
  injectFromDirectory(dirPath, pushBuffer);
}

/* ****************************************************************************
 *
 * readFromServer -
 */
void readFromServer(int fd, samson::SamsonPushBuffer *pushBuffer) {
  int nb;

  LM_VVV(("Reading from server"));
  while (signaled_quit == false) {
    // Read as much as we possibly can ...
    nb = read(fd, buffer, sizeof(buffer));
    LM_VVV(("Read %d bytes from tunnel", nb));
    if (nb > 0) {
      nbAccumulated += nb;

      LM_D(("Read %d bytes of data from tunnel (grand total: %d)", nb, nbAccumulated));
      bufPresent("Read from Tunnel", buffer, nb, 2);

      LM_VVV(("Calling bufPush with a buffer of %d bytes", nb));
      bufPush(buffer, nb, pushBuffer);
    } else {
      if (nb == -1) {
        LM_E(("Error reading from the Tunnel: %s", strerror(errno)));
      } else {
        LM_E(("The Tunnel closed the connection?"));
      } close(fd);
      fd       = connectToServer();
      savedLen = 0;
    }
  }
}

/* ****************************************************************************
 *
 * tektronixData -
 */
void tektronixData(samson::SamsonPushBuffer *pushBuffer) {
  int fd;

  // 1. connect to host:port ...
  LM_V(("connecting to %s:%d", host, port));
  fd = connectToServer();
  if (fd == -1) {
    LM_X(1, ("Error connecting to server '%s', port %d", host, port));
  }
  LM_V(("calling readFromServer"));
  readFromServer(fd, pushBuffer);
}

/* ****************************************************************************
 *
 * fakeData -
 */
void fakeData(samson::SamsonPushBuffer *pushBuffer) {
  struct tm tm;
  char line[2048];
  const char *lineP = line;
  au::Cronometer cronometer;
  unsigned long int total_size      = 0;
  unsigned long int num_messages    = 0;
  FILE *fp              = NULL;
  time_t timeStamp       = 0;
  time_t firstTimeStamp  = 0;
  time_t offset_secs     = 0;

  if (strcmp(file, "generator") != 0) {
    if ((fp = fopen(file, "r")) == NULL) {
      LM_X(1, ("Error opening file '%s' (errno:%d): %s", file, errno, strerror(errno)));
    }
  }
  while (true) {
    char *pTime;
    char *p_sep;

    if (fp == NULL) {
      unsigned long int used_id = rand() % 40000000;
      int cell    = rand() % 20000;

      // snprintf(line, 1024 , "<message> <id> %lu </id> <cell> %d </cell> <time> %s </time>\n", used_id , cell , au::todayString().c_str() );
      snprintf(
        line, sizeof(line),
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?><ns0:AMRReport xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'  xmlns:ns0='http://O2.arcanum.vitria.com'  xsi:schemaLocation='http://O2.arcanum.vitria.com AMR.xsd'>  <SubscriberReport>    <User>      <IMSI>%lu</IMSI>      <IMEI>3563190407579307</IMEI>    </User>    <Authentication>      <Location>        <LocationArea>12115</LocationArea>        <CellID>%d</CellID>        <RoutingArea>134</RoutingArea>      </Location>    </Authentication>  </SubscriberReport>  <Timestamp>%s</Timestamp></ns0:AMRReport>\n",
        used_id, cell, au::todayString().c_str());
    } else {
      if (fgets(line, sizeof(line), fp) == NULL) {
        rewind(fp);
        if (firstTimeStamp != 0) {
          offset_secs += (timeStamp - firstTimeStamp);
        }
        continue;
      }
#define STR_TIMESTAMP_BEGIN "<Timestamp>"
      if ((pTime = strstr(line, STR_TIMESTAMP_BEGIN)) != NULL) {
        pTime += strlen(STR_TIMESTAMP_BEGIN);
        if ((p_sep = strchr(pTime, '<')) != NULL) {
          *p_sep = '\0';
          strptime(pTime, "%Y-%m-%dT%H:%M:%S", &tm);
          timeStamp = mktime(&tm);
          if (offset_secs != 0) {
            timeStamp += offset_secs;
            localtime_r(&timeStamp, &tm);
            strftime(pTime, 20, "%Y-%m-%dT%H:%M:%S", &tm);
          } else if (firstTimeStamp == 0) {
            firstTimeStamp = timeStamp;
          }
          *p_sep = '<';
        }
      }
    }

    size_t line_length  = strlen(line) + 1;
    total_size += line_length;
    num_messages++;

    pushBuffer->push(lineP, line_length, true);

    // Detect if we need to sleep....
    int theoretical_seconds  = ((double)total_size / ((double)(1024 * 1024) * (double)rate));
    int elapsed_seconds      = cronometer.seconds();

    // Sleep some time to simulate a particular rate
    if (elapsed_seconds < theoretical_seconds) {
      int sleep_seconds = theoretical_seconds - elapsed_seconds;
      std::cerr << "Sleeping " << sleep_seconds << " seconds... num messages " << au::str(num_messages) <<
      " size " << au::str(total_size, "bytes") << " time " << au::str_time(elapsed_seconds) <<
      " theoretical time " << au::str_time(theoretical_seconds) << "\n";
      sleep(sleep_seconds);
    }
  }
}

/* ****************************************************************************
 *
 * main -
 */
int main(int argC, const char *argV[]) {
  paConfig("builtin prefix",                (void *)"SS_PLP_");
  paConfig("usage and exit on any warning", (void *)true);
  paConfig("log to screen",                 (void *)"only errors");
  paConfig("log file line format",          (void *)"TYPE:DATE:EXEC/FILE[LINE] FUNC: TEXT");
  paConfig("screen line format",            (void *)"TYPE@TIME  EXEC: TEXT (FUNC)");
  paConfig("log to file",                   (void *)true);
  paConfig("exit on usage",                 (void *)false);

  paParse(paArgs, argC, (char **)argV, 1, false);

  if (signal(SIGINT, captureSIGINT) == SIG_ERR) {
    LM_W(("SIGINT cannot be handled"));
  }
  signaled_quit = false;

  gettimeofday(&startTime, NULL);

  // Check queue is specified
  if (strcmp(queueName, "null") == 0) {
    LM_X(1, ("Please, specify a queue to push data to"));
  }
  samson::SamsonClient::general_init();

  // Instance of the client to connect to SAMSON system
  LM_V(("Connecting to '%s'", controller));
  samson::SamsonClient client("push");
  if (!client.connect(controller)) {
    LM_X(1, ("Not possible to connect with %s", controller ));  // Set 1G RAM for uploading content
  }
  uint64_t mem = 1024 * 1024 * 1024;
  mem *= 4;

  // Initialize connection
  // Init connection


  samson::SamsonPushBuffer *pushBuffer;
  if (((tektronix == true) || (strcmp(directory, "nodir") != 0)) && (fake == true)) {
    pushBuffer = NULL;
  } else {
    pushBuffer = new samson::SamsonPushBuffer(&client, queueName);
  } LM_V(("rate: %f MBs", rate));
  if (timeout > 0) {
    LM_V(("timeout: %ds", timeout));
  }
  if (tektronix == true) {
    tektronixData(pushBuffer);
  } else if (strcmp(directory, "nodir") != 0) {
    directoryData(pushBuffer, directory);
  } else if (fake == true) {
    fakeData(pushBuffer);
  } else {
    fprintf(stderr, "%s: bad parameters - either '-tektronix' or '-fake' must be used", paProgName);
    paUsage();
  }

  // Only here if we received a SIGQUIT
  // Last push
  pushBuffer->flush();

  LM_V(("--------------------------------------------------------------------------------"));
  LM_V(("PushBuffer info:   %s", pushBuffer->rate_.str().c_str()));
  LM_V(("SamsonClient info: %s", client.push_rate().str().c_str()));
  LM_V(("--------------------------------------------------------------------------------"));


  // Wait until all operations are complete
  LM_V(("Waiting for all the push operations to complete..."));
  client.waitFinishPushingData();

  LM_V(("DONE"));
  return 0;
}

