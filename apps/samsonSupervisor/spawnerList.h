#if 0

typedef struct Process
{
	char             name[32];
	char             alias[32];
	char             ip[32];
	ProcessType      type;
	bool             sendsLogs;
	bool             verbose;
	bool             debug;
	bool             reads;
	bool             writes;
	bool             toDo;
	char             traceV[256];

	Host*            hostP;
	ss::Endpoint*    endpoint;
	Starter*         starterP;
}
#endif
