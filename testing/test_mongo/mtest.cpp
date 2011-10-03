#include <sys/types.h>
#include <signal.h>

#include "mongo/db/json.h"
#include "mongo/client/dbclient.h"
#include "mongo/client/dbclientcursor.h"

#include "parseArgs.h"
#include "logMsg.h"



/* ****************************************************************************
*
* definitions - 
*/ 
#define MAXUSER 40000000



/* ****************************************************************************
*
* Results - 
*/
typedef struct Results
{
	int     queries;
	int     seconds;
	int     microseconds;
	double  qps;
} Results;



/* ****************************************************************************
*
* global vars - 
*/
mongo::DBClientConnection*  mdbConnection;
std::string                 dbAndColl;
bool                        connected = false;
pid_t                       pid       = 0;
struct timeval              sleepTime = { 0, 0 };
struct timeval              startTime;
struct timeval              stopTime;
int                         recordsInserted = 0;



/* ****************************************************************************
*
* Option variables
*/
bool  location;
bool  mobility;
bool  upload;
char  server[256];
char  db[256];
char  collection[256];
int   kvs;
int   times;
bool  bulk;
int   bulksize;
bool  indexed;
bool  burst;
bool  sleepsec;
int   maxUserId;
int   minUserId;
int   userId;
int   days;
int   threads;
int   test;
int   queries;
int   initrecs;


#define M512 (512 * 1024 * 1024)
#define M40   (40 * 1000 * 1000)
#define M10   (10 * 1000 * 1000)
/* ****************************************************************************
*
* parse arguments
*/
PaArgument paArgs[] =
{
	{ "-location", &location,   "LOCATION",    PaBool,    PaOpt,  false,                    false, true,  "user location query"        },
	{ "-mobility", &mobility,   "MOBILITY",    PaBool,    PaOpt,  false,                    false, true,  "user mobility query"        },
	{ "-upload",   &upload,     "UPLOAD",      PaBool,    PaOpt,  false,                    false, true,  "upload data to mongodb"     },
	{ "-server",   server,      "SERVER",      PaString,  PaOpt,  (long) "localhost:27017", PaNL,  PaNL,  "data base server:port"      },
	{ "-db",       db,          "DB_PATH",     PaString,  PaReq,  PaND,                     PaNL,  PaNL,  "name of data base"          },
	{ "-coll",     collection,  "COLLECTION",  PaString,  PaReq,  PaND,                     PaNL,  PaNL,  "name of collection"         },
	{ "-kvs",      &kvs,        "KEYVALUES",   PaInt,     PaOpt,  40000,                    1,     M512,  "no of keyvalues per second" },
	{ "-times",    &times,      "TIMES",       PaInt,     PaOpt,  0,                        0,     PaNL,  "times"                      },
	{ "-bulk",     &bulk,       "BULK",        PaBool,    PaOpt,  false,                    false, true,  "bulk load"                  },
	{ "-bulksize", &bulksize,   "BULKSIZE",    PaInt,     PaOpt,  60000,                    PaNL,  PaNL,  "bulk size"                  },
	{ "-indexed",  &indexed,    "INDEXED",     PaBool,    PaOpt,  false,                    false, true,  "create index"               },
	{ "-burst",    &burst,      "BURST",       PaBool,    PaOpt,  false,                    false, true,  "burst data send"            },
	{ "-sleep",    &sleepsec,   "SLEEP",       PaBool,    PaOpt,  false,                    false, true,  "sleep rest of second"       },
	{ "-queries",  &queries,    "QUERIES",     PaInt,     PaOpt,  10000,                    1,     M512,  "no of queries"              },
	{ "-maxuid",   &maxUserId,  "MAXUSERID",   PaInt,     PaOpt,  M40,                      0,     M512,  "max user id"                },
	{ "-minuid",   &minUserId,  "MINUSERID",   PaInt,     PaOpt,  1,                        0,     M512,  "min user id"                },
	{ "-uid",      &userId,     "USERID",      PaInt,     PaOpt,  -1,                      -1,     M512,  "user id"                    },
	{ "-days",     &days,       "DAYS",        PaInt,     PaOpt,  0,                        0,     M512,  "days"                       },
	{ "-threads",  &threads,    "THREADS",     PaInt,     PaOpt,  1,                        1,       20,  "number of threads"          },
	{ "-test",     &test,       "TEST",        PaInt,     PaOpt,  0,                        0,      100,  "test case"                  },
	{ "-initrecs", &initrecs,   "INITRECS",    PaInt,     PaOpt,  0,                        0,     M512,  "records initially"          },

	PA_END_OF_ARGS
};



/* ****************************************************************************
*
* timediff - 
*/
void timediff(struct timeval* start, struct timeval* stop, struct timeval* diff)
{
	diff->tv_sec  = stop->tv_sec  - start->tv_sec;
	diff->tv_usec = stop->tv_usec - start->tv_usec;

	if (diff->tv_usec < 0)
	{
		diff->tv_usec += 1000000;
		diff->tv_sec  -= 1;
	}
}



/* ****************************************************************************
*
* stopTimeMeasuring - 
*/
void stopTimeMeasuring(void)
{
	struct timeval  diff;
	double          rate;
	double          dstart;
	double          dend;
	double          ddiff;
	double          dsleep;

	gettimeofday(&stopTime, NULL);
	timediff(&startTime, &stopTime, &diff);

	dstart = (double) startTime.tv_sec + (double) startTime.tv_usec / 1000000;
	dend   = (double) stopTime.tv_sec + (double) stopTime.tv_usec / 1000000;
	dsleep = (double) sleepTime.tv_sec + (double) sleepTime.tv_usec / 1000000;
	ddiff  = dend - dstart;

	LM_M(("Total execution time: %.2f seconds", ddiff));
	LM_M(("Total sleep time:     %.2f seconds", dsleep));

	LM_M(("Effective execution time: %.2f%%", 100 * (double) ((ddiff - dsleep) / ddiff)));

	if (upload == true)
	{
		rate = (double) recordsInserted / ddiff;
		LM_M(("KVs per second: %.2f", rate));
	}
	else
	{
		rate = (double) queries / ddiff;
		LM_M(("Queries per second: %.2f", rate));
	}

	if (pid != 0)
		kill(pid, SIGTERM);
}


/* ****************************************************************************
*
* sigHandler - 
*/
void sigHandler(int sigNo)
{
	stopTimeMeasuring();
	if (pid != 0)
		kill(pid, SIGTERM);
	pid = 0;
	exit(0);
}



/* ****************************************************************************
*
* dbConnect  - 
*/
void dbConnect(void)
{
	if (connected == true)
		return;

	mdbConnection = new mongo::DBClientConnection();
	LM_M(("Connecting to mongo at '%s'", server));
	mdbConnection->connect(server);

	dbAndColl = (std::string) db + "." + (std::string) collection;
	connected = true;
}



/* ****************************************************************************
*
* dbEmpty - 
*/ 
void dbEmpty(void)
{
	long long items;

	items = mdbConnection->count(dbAndColl);

	LM_M(("Emptying collection %s in database %s (%d items)", collection, db, items));
	mdbConnection->dropCollection(dbAndColl);
	mdbConnection->createCollection(dbAndColl);
}



/* ****************************************************************************
*
* locationQuery - 
*/ 
void* locationQuery(void* x)
{
	long long int       userId;
	struct timeval      start;
	struct timeval      stop;
	struct timeval      diff;
	struct timeval      lastTrace;
	Results*            results = (Results*) x;
	int                 notfound = 0;

	if (!connected)
		dbConnect();

	gettimeofday(&startTime, NULL);
	gettimeofday(&start, NULL);
	gettimeofday(&lastTrace, NULL);

	for (int ix = 0; ix < queries; ix++)
	{
		std::auto_ptr<mongo::DBClientCursor> cursor = mdbConnection->query(dbAndColl, QUERY("I" << userId));

		userId = rand() % MAXUSER;

		if (cursor->more() == false)
		{
			notfound++;
#if 0
			if (notfound % 1000 == 0)
				LM_W(("%d users not found!", notfound));
#endif
		}

		int hits = 0;

		while (cursor->more() != false)
		{
			cursor->next();
			++hits;
		}

		gettimeofday(&stop, NULL);
		timediff(&lastTrace, &stop, &diff);
		if (diff.tv_sec > 1)
		{
			LM_M(("Made %d queries (of %d)", ix, queries));
			lastTrace.tv_sec = stop.tv_sec;
			lastTrace.tv_usec = stop.tv_usec;
		}
	}

	gettimeofday(&stop, NULL);

	timediff(&start, &stop, &diff);

	double ddiff = diff.tv_sec + (double) diff.tv_usec / 1000000;

	LM_M(("%d queries in %d.%06d seconds - %f queries per second", queries, diff.tv_sec, diff.tv_usec, (double) queries / ddiff));

	if (results != NULL)
	{
		results->queries      = queries;
		results->seconds      = diff.tv_sec;
		results->microseconds = diff.tv_usec;
		results->qps          = (double) queries / ddiff;
	}

	return NULL;
}



/* ****************************************************************************
*
* mobilityQuery - 
*/ 
void* mobilityQuery(void* x)
{
	struct timeval   start;
	struct timeval   stop;
	struct timeval   diff;
	time_t           now = time(NULL);

	if (userId == -1)
		LM_X(1, ("please indicate a user identifier, using the '-uid' option"));

	if (days == 0)
		LM_X(1, ("please indicate number of days, using the '-days' option"));

	dbConnect();

	while (times-- > 0)
	{
		int times;

		gettimeofday(&start, NULL);

		times = 0;
		for (int ix = 0; ix < queries; ix++)
		{
			std::auto_ptr<mongo::DBClientCursor> cursor = mdbConnection->query(dbAndColl, QUERY("I" << (long long int) userId));

			if (cursor->more() == false)
				LM_X(1, ("No user with id %u found", userId));
			else
			{
				int hits = 0;

				while (cursor->more() != false)
				{
					mongo::BSONObj bo;

					int ts;

					bo = cursor->next();
					
					ts = bo.getIntField("T");
					if (ts + days * 3600 * 24 < now)
						continue;
					++hits;
				}

				++times;
				LM_V(("Found %d hits for user %d the last %d days (%d times)", hits, userId, days, times));
			}
		}

		gettimeofday(&stop, NULL);

		timediff(&start, &stop, &diff);

		if (diff.tv_sec != 0)
			LM_W(("operation took longer than ONE second - %d.%06d secs!", diff.tv_sec, diff.tv_usec));
		else if (burst == false)
		{
			LM_D(("sleeping 0.%06d seconds", 1000000 - diff.tv_usec));
			usleep(1000000 - diff.tv_usec);
		}
	}

	return NULL;
}



/* ****************************************************************************
*
* uploadData - 
*/
void* uploadData(void* x)
{
	int             operation = 0;
	int             kvs2;
	struct timeval  diff;
	bool            oneshot = false;

	dbConnect();

	gettimeofday(&startTime, NULL);

	if (x != NULL)
	{
		kvs2 = *((int*) x);
		oneshot = true;
	}
	else
		kvs2 = kvs;

	if (bulk)
		LM_M(("Bulk-mode-inserting %d kvs/second to server '%s', DB '%s', collection '%s'", kvs2, server, db, collection));
	else
		LM_M(("Slow-inserting %d kvs/second to server '%s', DB '%s', collection '%s'", kvs2, server, db, collection));

	while (1)
	{
		struct timeval      start;
		struct timeval      stop;
		long long int       user;
		long long int       position    = 1234560;
		long long int       timestamp   = (long long int) time(NULL);
		
		gettimeofday(&start, NULL);

		std::vector<mongo::BSONObj>  bulk_data;
		int                          inserts = 0;
			
		if (oneshot == true)
			LM_M(("Uploading initial data of %d kvs (once only)", kvs2));
		else
			LM_M(("Uploading %d kvs per second", kvs2));

		for (int ix = 0; ix < kvs2; ix++)
		{
			mongo::BSONObj  record;

			user = rand() % MAXUSER;

			LM_D(("I:%d, P:%d, T:%d", user, position, timestamp));
			record =  BSON("I" << user << "P" << position << "T" << timestamp);
			++position;
			++timestamp;
			++recordsInserted;

			bulk_data.push_back(record);

			++inserts;
			if ((inserts % bulksize) == 0)
			{
				mdbConnection->insert(dbAndColl, bulk_data);
				bulk_data.clear();
				inserts = 0;
			}
		}				

		if (inserts != 0)
		{
			mdbConnection->insert(dbAndColl, bulk_data);
			bulk_data.clear();
		}

		if (indexed)
			mdbConnection->ensureIndex(dbAndColl, mongo::fromjson("{I:1}"));

		if (oneshot == true)
			return NULL;

		++operation;

		gettimeofday(&stop, NULL);

		timediff(&start, &stop, &diff);

		if (diff.tv_sec != 0)
			LM_W(("operation %d took longer than ONE second - %d.%06d secs!", operation, diff.tv_sec, diff.tv_usec));
		else if (sleepsec == true)
		{
			LM_D(("sleeping 0.%06d seconds", 1000000 - diff.tv_usec));
			usleep(1000000 - diff.tv_usec);

			sleepTime.tv_usec += (1000000 - diff.tv_usec);
			if (sleepTime.tv_usec >= 1000000)
			{
				++sleepTime.tv_sec;
				sleepTime.tv_usec -= 1000000;
			}
		}
	}

	return NULL;
}



/* ****************************************************************************
*
* upLoad - 
*/
void upLoad(int kvsPerSecond)
{
	char  kvsV[16];
	char  timesV[16];

	pid = fork();
	if (pid == 0)
	{
		sprintf(kvsV, "%d", kvsPerSecond);

		const char* const args[] = 
		{
			"uploader",
			"-bulk",
			"-kvs",
			(const char*) kvsV,
			"-indexed",
			"-upload",
			"-coll",
			collection,
			"-db",
			db,
			"-sleep",
			NULL
		};

		LM_M(("Executing '%s' -upload -kvs %s -times %s ...", progName, kvsV, timesV));
		execvp(progName, (char* const*) args);
		LM_X(1, ("Back from EXEC ..."));
	}

	// srand(time(NULL) + 1);
}



/* ****************************************************************************
*
* test1 - 
*/
void test1(int speed, int noOfQueries)
{
	Results results;

	if (initrecs != 0)
	{
		bulk    = true;
		indexed = true;

		dbEmpty();
		uploadData(&initrecs);
	}
	upLoad(speed);

	sleep(1);
	LM_M(("Starting queries ..."));
	queries = noOfQueries;
	locationQuery(&results);

	LM_M(("Test 1\n-------\no collection initially empty\no %d kvs/second uploading\no %d queries\n==========================\n",
		  speed, queries));
	LM_M(("\nResult:\n  %f queries per second\n", results.qps));
}



/* ****************************************************************************
*
* testSwitch - 
*/
void* testSwitch(void* x)
{
	dbConnect();

	switch (test)
	{
	case 1:
		test1(kvs, queries);
		break;

	default:
		LM_X(1, ("Test case %d not implemented", test));
		break;
	}

	return NULL;
}



/* ****************************************************************************
*
* main - 
*/
int main(int argC, char* argV[])
{
	paConfig("prefix",                        (void*) "MONGO");
	paConfig("usage and exit on any warning", (void*) true);
    paConfig("log to screen",                 (void*) "only errors");
    paConfig("log file line format",          (void*) "TYPE:DATE:EXEC/FILE[LINE] FUNC: TEXT");
    paConfig("screen line format",            (void*) "TYPE@TIME  EXEC: TEXT");
    paConfig("log to file",                   (void*) true);

	paParse(paArgs, argC, (char**) argV, 1, false);

	if (!location && !mobility && !upload && test == 0)
	{
		printf("Please pick one if the operating modes: '-location', '-mobility', '-upload' or '-test'\n");
		exit(1);
	}

	signal(SIGINT,  sigHandler);
	signal(SIGTERM, sigHandler);

	srand(time(NULL));

	if (location)
		locationQuery(NULL);
	else if (mobility)
		mobilityQuery(NULL);
	else if (upload)
		uploadData(NULL);
	else if (test != 0)
		testSwitch(NULL);

	stopTimeMeasuring();
	if (pid != 0)
		kill(pid, SIGTERM);

	return 0;
}
