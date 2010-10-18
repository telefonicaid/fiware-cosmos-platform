#ifndef _H_COMMON
#define _H_COMMON


#define TRACE_DALILAH				101
#define TRACE_SAMSON_WORKER			102
#define TRACE_SAMSON_CONTROLLER		103

#define TRACE_SAMSON_DEMO			104

#define LMT_READY		21
#define LMT_CONFIG		22


namespace ss {
	void samsonInitTrace( int argc, const char *argv[] );
}

#endif