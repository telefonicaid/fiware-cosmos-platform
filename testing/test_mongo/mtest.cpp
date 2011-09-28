#include "mongo/db/json.h"
#include "mongo/client/dbclient.h"
#include "mongo/client/dbclientcursor.h"

#include "parseArgs.h"
#include "logMsg.h"



/* ****************************************************************************
*
* global vars - 
*/
mongo::DBClientConnection*  mdbConnection;
std::string                 dbAndColl;



/* ****************************************************************************
*
* Option variables
*/
char  server[256];
char  db[256];
char  collection[256];
int   kvs;
int   secs;
bool  bulk;
int   bulksize;
bool  indexed;
bool  ipt;
bool  burst;
bool  location;
bool  mobility;
int   qps;
int   maxUserId;
int   minUserId;
int   userId;
int   days;



#define M512 (512 * 1024 * 1024)
/* ****************************************************************************
*
* parse arguments
*/
PaArgument paArgs[] =
{
	{ "-server",   server,      "SERVER",      PaString,  PaOpt,  (long) "localhost:27017", PaNL,  PaNL,  "data base server:port"      },
	{ "-db",       db,          "DB_PATH",     PaString,  PaReq,  PaND,                     PaNL,  PaNL,  "name of data base"          },
	{ "-coll",     collection,  "COLLECTION",  PaString,  PaReq,  PaND,                     PaNL,  PaNL,  "name of collection"         },
	{ "-kvs",      &kvs,        "KEYVALUES",   PaInt,     PaOpt,  40000,                    1,     M512,  "no of keyvalues per second" },
	{ "-secs",     &secs,       "SECS",        PaInt,     PaOpt,  1,                        1,     PaNL,  "seconds"                    },
	{ "-bulk",     &bulk,       "BULK",        PaBool,    PaOpt,  false,                    false, true,  "bulk load"                  },
	{ "-bulksize", &bulksize,   "BULKSIZE",    PaInt,     PaOpt,  30000,                    PaNL,  PaNL,  "bulk size"                  },
	{ "-indexed",  &indexed,    "INDEXED",     PaBool,    PaOpt,  false,                    false, true,  "create index"               },
	{ "-ipt",      &ipt,        "UK_DATA",     PaBool,    PaOpt,  false,                    false, true,  "UK Pilot-like data"         },
	{ "-burst",    &burst,      "BURST",       PaBool,    PaOpt,  false,                    false, true,  "burst data send"            },
	{ "-location", &location,   "LOCATION",    PaBool,    PaOpt,  false,                    false, true,  "user location query"        },
	{ "-mobility", &mobility,   "MOBILITY",    PaBool,    PaOpt,  false,                    false, true,  "user mobility query"        },
	{ "-qps",      &qps,        "QPS",         PaInt,     PaOpt,  10000,                    1,     M512,  "no of queries per second"   },
	{ "-maxuid",   &maxUserId,  "MAXUSERID",   PaInt,     PaOpt,  0,                        0,     M512,  "max user id"                },
	{ "-minuid",   &minUserId,  "MINUSERID",   PaInt,     PaOpt,  0,                        0,     M512,  "min user id"                },
	{ "-uid",      &userId,     "USERID",      PaInt,     PaOpt,  -1,                      -1,     M512,  "user id"                    },
	{ "-days",     &days,       "DAYS",        PaInt,     PaOpt,  0,                        0,     M512,  "days"                       },
	
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
* upload - 
*/
void upload(void)
{
	if (bulk)
		LM_M(("Bulk-mode-inserting %d kvs/second to server %s during %d seconds in DB %s, collection %s", kvs, server, secs, db, collection));
	else
		LM_M(("Slow-inserting %d kvs/second to server %s during %d seconds in DB %s, collection %s", kvs, server, secs, db, collection));


	while (secs-- > 0)
	{
		mongo::BSONObj      bo;
		long long           key     = 1;
		long long           value   = 1;
		struct timeval      start;
		struct timeval      stop;
		struct timeval      diff;
		long long int       key2        = 0;
		long long int       position    = 1234560;
		long long int       timestamp   = (long long int) time(NULL);
		
		gettimeofday(&start, NULL);

		if (bulk)
		{
			std::vector<mongo::BSONObj>  bulk_data;
			int                          inserts = 0;
			
			for (int ix = 0; ix < kvs; ix++)
			{
				mongo::BSONObj  record;

				if (ipt)
				{
					LM_M(("I:%d, P:%d, T:%d", key2, position, timestamp));
					record =  BSON("I" << key2 << "P" << position << "T" << timestamp);
					++key2;
					++position;
					++timestamp;
				}
				else
				{
					record = BSON("K" << key << "V" << value);
					++value;
					++key;
				}

				bulk_data.push_back(record);

				++inserts;
				if ((inserts % bulksize) == 0)
				{
					LM_V(("Bulk-inserting ..."));
					mdbConnection->insert(dbAndColl, bulk_data);
					LM_V(("... Bulk-insert finished"));
					bulk_data.clear();
					inserts = 0;
				}
			}				

			if (inserts != 0)
			{
				mdbConnection->insert(dbAndColl, bulk_data);
				bulk_data.clear();
			}
		}
		else
		{
			for (int ix = 0; ix < kvs; ix++)
			{
				if (ipt)
				{
					bo =  BSON("I" << key2 << "P" << position << "T" << timestamp);
					++key2;
					++position;
					++timestamp;
				}
				else
				{
					bo = BSON("K" << key << "V" << value);
					value += 1;
					key   += 1;
				}

				mdbConnection->insert(dbAndColl, bo);
			}
		}

		if (burst == false)
		{
			gettimeofday(&stop, NULL);

			timediff(&start, &stop, &diff);

			if (diff.tv_sec != 0)
				LM_X(1, ("Sorry, this operation takes too long (%d.%06d seconds)."
						 "Please lower the number of keyvalues per 'burst' (using option '-kvs')",
						 diff.tv_sec, diff.tv_usec));
			LM_M(("sleeping 0.%06d seconds", 1000000 - diff.tv_usec));
			usleep(1000000 - diff.tv_usec);
		}
	}

	if (indexed)
	{
		if (ipt)
			mdbConnection->ensureIndex(dbAndColl, mongo::fromjson("{I:1}"));
		else
			mdbConnection->ensureIndex(dbAndColl, mongo::fromjson("{K:1}"));
	}
}



/* ****************************************************************************
*
* locationQuery - 
*/ 
void locationQuery(void)
{
	int                 times  = 0;
	long long int       userId = minUserId;
	struct timeval      start;
	struct timeval      stop;
	struct timeval      diff;


	gettimeofday(&start, NULL);

	while (secs-- > 0)
	{
		for (int ix = 0; ix < qps; ix++)
		{
			std::auto_ptr<mongo::DBClientCursor> cursor = mdbConnection->query(dbAndColl, QUERY("I" << userId));
			++times;
			++userId;
			if (userId > maxUserId)
				userId = minUserId;

			if (cursor->more() == false)
				LM_X(1, ("No user with id %u found", userId));
			else
			{
				int hits = 0;

				while (cursor->more() != false)
				{
					cursor->next();
					++hits;
				}

				LM_V(("Found %d hits for user %d (%d times)", hits, userId, times));
			}
		}

		if (burst == false)
		{
			gettimeofday(&stop, NULL);

			timediff(&start, &stop, &diff);

			if (diff.tv_sec != 0)
				LM_X(1, ("Sorry, this operation takes too long (%d.%06d seconds)."
						 "Please lower the number of keyvalues per 'burst' (using option '-kvs')",
						 diff.tv_sec, diff.tv_usec));
			LM_M(("sleeping 0.%06d seconds", 1000000 - diff.tv_usec));
			usleep(1000000 - diff.tv_usec);
		}
	}
}



/* ****************************************************************************
*
* mobilityQuery - 
*/ 
void mobilityQuery(void)
{
	struct timeval   start;
	struct timeval   stop;
	struct timeval   diff;
	time_t           now = time(NULL);

	if (userId == -1)
		LM_X(1, ("please indicate a user identifier, using the '-uid' option"));

	if (days == 0)
		LM_X(1, ("please indicate number of days, using the '-days' option"));

	while (secs-- > 0)
	{
		int times;

		gettimeofday(&start, NULL);

		times = 0;
		for (int ix = 0; ix < qps; ix++)
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

		if (burst == false)
		{
			gettimeofday(&stop, NULL);

			timediff(&start, &stop, &diff);

			if (diff.tv_sec != 0)
				LM_X(1, ("Sorry, this operation takes too long (%d.%06d seconds)."
						 "Please lower the number of keyvalues per 'burst' (using option '-kvs')",
						 diff.tv_sec, diff.tv_usec));
			LM_M(("sleeping 0.%06d seconds", 1000000 - diff.tv_usec));
			usleep(1000000 - diff.tv_usec);
		}
	}
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

	mdbConnection = new mongo::DBClientConnection();
	LM_M(("Connecting to mongo at '%s'", server));
	mdbConnection->connect(server);

	dbAndColl = (std::string) db + "." + (std::string) collection;


	if (location)
		locationQuery();
	else if (mobility)
		mobilityQuery();
	else
		upload();

	return 0;
}
