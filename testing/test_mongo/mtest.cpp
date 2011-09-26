#include "mongo/db/json.h"
#include "mongo/client/dbclient.h"

#include "parseArgs.h"
#include "logMsg.h"



/* ****************************************************************************
*
* global vars - 
*/
mongo::DBClientConnection*  mdbConnection;



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



#define M512 (512 * 1024 * 1024)
/* ****************************************************************************
*
* parse arguments
*/
PaArgument paArgs[] =
{
	{ "-server",   server,      "SERVER",      PaString,  PaOpt,  (long) "localhost:27017", PaNL,  PaNL,  "data base server:port"      },
	{ "-db",       db,          "DB_PATH",     PaString,  PaOpt,  (long) "test",            PaNL,  PaNL,  "name of data base"          },
	{ "-coll",     collection,  "COLLECTION",  PaString,  PaReq,  PaND,                     PaNL,  PaNL,  "name of collection"         },
	{ "-kvs",      &kvs,        "KEYVALUES",   PaInt,     PaOpt,  40000,                    1,     M512,  "no of keyvalues per second" },
	{ "-secs",     &secs,       "SECS",        PaInt,     PaOpt,  1,                        1,     PaNL,  "seconds"                    },
	{ "-bulk",     &bulk,       "BULK",        PaBool,    PaOpt,  false,                    false, true,  "bulk load"                  },
	{ "-bulksize", &bulksize,   "BULKSIZE",    PaInt,     PaOpt,  30000,                    PaNL,  PaNL,  "bulk size"                  },
	{ "-indexed",  &indexed,    "INDEXED",     PaBool,    PaOpt,  false,                    false, true,  "create index"               },
	{ "-ipt",      &ipt,        "UK_DATA",     PaBool,    PaOpt,  false,                    false, true,  "UK Pilot-like data"         },
	{ "-burst",    &burst,      "BURST",       PaBool,    PaOpt,  false,                    false, true,  "burst data send"            },

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
* main - 
*/
int main(int argC, char* argV[])
{
	std::string dbAndColl;

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

	if (bulk)
		LM_M(("Bulk-mode-inserting %d kvs/second to server %s during %d seconds in DB %s, collection %s", kvs, server, secs, db, collection));
	else
		LM_M(("Slow-inserting %d kvs/second to server %s during %d seconds in DB %s, collection %s", kvs, server, secs, db, collection));


	while (secs-- > 0)
	{
		mongo::BSONObj      bo;
		std::string         key     = "K";
		long long           value   = 1;
		struct timeval      start;
		struct timeval      stop;
		struct timeval      diff;
		long long int       key2        = 0;
		long long int       position    = 0;
		long long int       timestamp   = 0;
		
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
					// LM_M(("I:%d, P:%d, T:%d", key2, position, timestamp));
					record =  BSON("I" << key2 << "P" << position << "T" << timestamp);
					++key2;
					++position;
					++timestamp;
				}
				else
				{
					record = BSON("K" << key << "V" << value);
					++value;
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
				bo = BSON("K" << key << "V" << value);
				value += 1;

				mdbConnection->insert(dbAndColl, bo);
			}
		}


		if (burst == false)
		{
			gettimeofday(&stop, NULL);

			timediff(&start, &stop, &diff);

			if (diff.tv_sec != 0)
				LM_X(1, ("Sorry, this operation takes too long (%d.%06d seconds). Please lower the number of keyvalues per 'burst' (using option '-kvs')", diff.tv_sec, diff.tv_usec));
			LM_M(("sleeping 0.%06d seconds", 1000000 - diff.tv_usec));
			usleep(1000000 - diff.tv_usec);
		}
	}

	if (indexed)
		mdbConnection->ensureIndex(dbAndColl, mongo::fromjson("{K:1}"));

	return 0;
}
