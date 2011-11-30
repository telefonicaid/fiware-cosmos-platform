/* ****************************************************************************
*
*
*
* Operation Modes
* - Empty
* - Fill
* - Upload
* - Query
* - update
*/
#include <sys/types.h>
#include <signal.h>

#include "mongo/db/json.h"
#include "mongo/client/dbclient.h"
#include "mongo/client/dbclientcursor.h"

#include "parseArgs.h"
#include "logMsg.h"



using namespace mongo;



/* ****************************************************************************
*
* definitions - 
*/ 
#define MAXUSER 10000000



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
int                         recordsInserted  = 0;
int                         queriesPerformed = 0;




/* ****************************************************************************
*
* Option variables
*/
char  server[256];
char  db[256];
char  collection[256];

bool  empty;
bool  dbfill;
bool  upload;
bool  query;
bool  update;

int   kvs;
int   times;
int   bulksize;
bool  indexed;
bool  burst;
bool  sleepsec;
int   userId;
int   timestamp;
int   days;
int   test;
int   queries;

#define M512 (512 * 1024 * 1024)
#define M40   (40 * 1000 * 1000)
#define M10   (10 * 1000 * 1000)
/* ****************************************************************************
*
* parse arguments
*/
PaArgument paArgs[] =
{
	{ "-server",   server,      "SERVER",      PaString,  PaOpt,  (long) "localhost:27017", PaNL,  PaNL,  "data base server:port"      },
	{ "-db",       db,          "DB_PATH",     PaString,  PaReq,  PaND,                     PaNL,  PaNL,  "name of data base"          },
	{ "-coll",     collection,  "COLLECTION",  PaString,  PaReq,  PaND,                     PaNL,  PaNL,  "name of collection"         },

	{ "-clear",    &::empty,      "EMPTY",       PaBool,    PaOpt,  false,                    false, true,  "clear data base collection" },
	{ "-fill",     &dbfill,     "FILL",        PaBool,    PaOpt,  false,                    false, true,  "initial filling of db"      },
	{ "-upload",   &upload,     "UPLOAD",      PaBool,    PaOpt,  false,                    false, true,  "upload data to mongodb"     },
	{ "-query",    &query,      "QUERY",       PaBool,    PaOpt,  false,                    false, true,  "db query"                   },
	{ "-update",   &update,     "UPDATE",      PaBool,    PaOpt,  false,                    false, true,  "db update"                  },

	{ "-kvs",      &kvs,        "KEYVALUES",   PaInt,     PaOpt,  40000,                    1,     M512,  "keyvalues"                  },
	{ "-times",    &times,      "TIMES",       PaInt,     PaOpt,  0,                        0,     PaNL,  "times"                      },
	{ "-bulksize", &bulksize,   "BULKSIZE",    PaInt,     PaOpt,  60000,                    PaNL,  PaNL,  "bulk size"                  },
	{ "-indexed",  &indexed,    "INDEXED",     PaBool,    PaOpt,  false,                    false, true,  "create index"               },
	{ "-burst",    &burst,      "BURST",       PaBool,    PaOpt,  false,                    false, true,  "burst data send"            },
	{ "-sleep",    &sleepsec,   "SLEEP",       PaBool,    PaOpt,  false,                    false, true,  "sleep rest of second"       },
	{ "-queries",  &queries,    "QUERIES",     PaInt,     PaOpt,  10000,                    1,     M512,  "no of queries"              },
	{ "-uid",      &userId,     "USERID",      PaInt,     PaOpt,  -1,                      -1,     M512,  "user id"                    },
	{ "-ts",       &timestamp,  "TIMESTAMP",   PaInt,     PaOpt,  -1,                      -1,     M512,  "timestamp"                  },
	{ "-days",     &days,       "DAYS",        PaInt,     PaOpt,  0,                        0,     M512,  "days"                       },
	{ "-test",     &test,       "TEST",        PaInt,     PaOpt,  0,                        0,      100,  "test case"                  },

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
* report - 
*/
void report(void)
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

	LM_M(("-----------------------------------------------------------------"));
	LM_M(("Total execution time: %.2f seconds", ddiff));
	LM_M(("Total sleep time:     %.2f seconds", dsleep));

	LM_M(("Effective execution time: %.2f%%", 100 * (double) ((ddiff - dsleep) / ddiff)));

	if (upload == true)
	{
		rate = (double) recordsInserted / ddiff;
		LM_M(("Inserted %d kvs (in %.2f seconds). Rate: %.2f kvs/second", recordsInserted, ddiff, rate));
	}
	else if (dbfill == true)
	{
		rate = (double) recordsInserted / ddiff;
		LM_M(("Filled db collection with %d kvs (in %.2f seconds). Rate: %.2f kvs/second", kvs, ddiff, rate));
	}
	else if (query == true)
	{
		rate = (double) queriesPerformed / ddiff;
		LM_M(("Queries per second: %.2f", rate));
	}

	if (pid != 0)
		kill(pid, SIGTERM);
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

	LM_M(("Starting to measure time ..."));
	gettimeofday(&startTime, NULL);
}



/* ****************************************************************************
*
* dbClear - 
*/ 
void dbClear(void)
{
	long long items;

	dbConnect();

	items = mdbConnection->count(dbAndColl);

	LM_M(("Clearing collection %s in database %s (%d items)", collection, db, items));
	mdbConnection->dropCollection(dbAndColl);
	mdbConnection->createCollection(dbAndColl);
}



Results  results;
/* ****************************************************************************
*
* dbQuery - 
*/ 
void dbQuery(void)
{
	long long int       userId;
	struct timeval      start;
	struct timeval      stop;
	struct timeval      diff;
	struct timeval      lastTrace;
	int                 lastQueriesPerformed = 0;

	dbConnect();

	gettimeofday(&start, NULL);
	gettimeofday(&lastTrace, NULL);

	for (int ix = 0; ix < queries; ix++)
	{
		int                                  hits;
		std::auto_ptr<mongo::DBClientCursor> cursor;

		userId = 666666666 + rand() % MAXUSER;

		if (strcmp(collection, "History") == 0)		  
			cursor = mdbConnection->query(dbAndColl, QUERY("I" << userId));
		else
			cursor = mdbConnection->query(dbAndColl, QUERY("_id" << userId));

		hits = 0;
		while (cursor->more() != false)
		{
			cursor->next();
			++hits;
		}

		++queriesPerformed;

		gettimeofday(&stop, NULL);
		timediff(&lastTrace, &stop, &diff);
		if (diff.tv_sec > 1)
		{
			double timespan    = (double) diff.tv_sec + (double) diff.tv_usec / 1000000;
			double partialRate = ((double) (queriesPerformed - lastQueriesPerformed)) / timespan;
			double accRate;

			timediff(&start, &stop, &diff);
			timespan     = (double) diff.tv_sec + (double) diff.tv_usec / 1000000;
			accRate      = ((double) queriesPerformed) / timespan;

			LM_M(("Made %d queries (out of %d). Last query: userId %d, %d hits (part rate: %.2f qps, acc rate: %.2f)", 
				  ix, queries, userId, hits, partialRate, accRate));
			lastTrace.tv_sec = stop.tv_sec;
			lastTrace.tv_usec = stop.tv_usec;

			lastQueriesPerformed = queriesPerformed;
		}
	}

	gettimeofday(&stop, NULL);

	timediff(&start, &stop, &diff);

	double ddiff = diff.tv_sec + (double) diff.tv_usec / 1000000;

	LM_M(("%d queries in %d.%06d seconds - %f queries per second", queries, diff.tv_sec, diff.tv_usec, (double) queries / ddiff));

	results.queries      = queries;
	results.seconds      = diff.tv_sec;
	results.microseconds = diff.tv_usec;
	results.qps          = (double) queries / ddiff;
}



/* ****************************************************************************
*
* dbUpdate - 
*/ 
void dbUpdate(void)
{
	mongo::Query    query;
	mongo::BSONObj  record;
	long long int   ts;
	long long int   uid;

	dbConnect();

	if (timestamp == -1)
		timestamp = time(NULL);

	uid     = (long long int) userId;
	ts      = (long long int) timestamp;

	if (strcmp(collection, "History") == 0)
		record = BSON("I"   << uid                <<
					  "T"   << ts                 <<
					  "C"   << (long long int) 1  <<
					  "X"   << (float) 2.0        <<
					  "Y"   << (float) 3.0);
	else
		record = BSON("_id" << uid                <<
					  "T"   << ts                 <<
					  "C"   << (long long int) 1  <<
					  "X"   << (float) 2.0        <<
					  "Y"   << (float) 3.0);


	// db.LastKnownLocation.update( { _id:3, T : { $lt: 3 }  }, { _id:3, T:3, C:1, X:1, Y:1 }, true  )
	if (strcmp(collection, "History") == 0)
		query = BSON("I" << uid << "T" << BSON( "$lt" << ts));
	else
		query = BSON("_id" << uid << "T" << BSON( "$lt" << ts));

	mdbConnection->update(dbAndColl, query, record, true);
}



/* ****************************************************************************
*
* dbUpload - 
*/
void dbUpload(int kvs, bool oneshot)
{
	int             operation = 0;
	struct timeval  diff;

	dbConnect();

	LM_M(("Bulk-inserting %d kvs/second to server '%s', DB '%s', collection '%s'", kvs, server, db, collection));

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
			LM_M(("Filling collection '%s' of data base '%s' with %d keyvalues", collection, db, kvs));
		else
			LM_M(("Uploading %d kvs per second to collection '%s' of data base '%s'", kvs, collection, db));

		for (int ix = 0; ix < kvs; ix++)
		{
			mongo::BSONObj  record;

			user = 666666666 + rand() % MAXUSER;

			LM_D(("I:%d, P:%d, T:%d", user, position, timestamp));
			if (strcmp(collection, "History") == 0)
				record =  BSON("I" << user << "T" << timestamp << "C" << (long long int) 1 << "X"   << (float) 2.0 << "Y"   << (float) 3.0);
			else
				record =  BSON("_id" << user << "T" << timestamp << "C" << (long long int) 1 << "X"   << (float) 2.0 << "Y"   << (float) 3.0);

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
			return;

		++operation;

		gettimeofday(&stop, NULL);

		timediff(&start, &stop, &diff);

		if (diff.tv_sec != 0)
			LM_W(("operation %d took longer than ONE second - %d.%06d secs!", operation, diff.tv_sec, diff.tv_usec));
		else if (sleepsec == true)
		{
			LM_V(("sleeping 0.%06d seconds", 1000000 - diff.tv_usec));
			usleep(1000000 - diff.tv_usec);

			sleepTime.tv_usec += (1000000 - diff.tv_usec);
			if (sleepTime.tv_usec >= 1000000)
			{
				++sleepTime.tv_sec;
				sleepTime.tv_usec -= 1000000;
			}
		}

		if ((times != 0) && (operation == times))
			break;
	}
}



#if 0
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
			std::auto_ptr<mongo::DBClientCursor> cursor;

			if (strcmp(collection, "History") == 0)
				cursor = mdbConnection->query(dbAndColl, QUERY("I" << (long long int) userId));
			else
				cursor = mdbConnection->query(dbAndColl, QUERY("_id" << (long long int) userId));

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
#endif



/* ****************************************************************************
*
* sigHandler - 
*/
void sigHandler(int sigNo)
{
	report();
	if (pid != 0)
		kill(pid, SIGTERM);
	pid = 0;
	exit(0);
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

	if (!::empty && !dbfill && !query && !upload && !update)
	{
		printf("Please pick one if the operating modes: '-clear', '-fill', '-query' or '-upload' or '-upload'\n");
		exit(1);
	}

	signal(SIGINT,  sigHandler);
	signal(SIGTERM, sigHandler);

	srand(time(NULL));

	if (::empty)
		dbClear();
	else if (dbfill)
		dbUpload(kvs, true);
	else if (upload)
		dbUpload(kvs, false);
	else if (query != 0)
		::dbQuery();
	else if (update != 0)
		::dbUpdate();

	report();

	if (pid != 0)
		kill(pid, SIGTERM);

	return 0;
}
