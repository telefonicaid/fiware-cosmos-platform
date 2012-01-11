#ifndef ERRORCODES_H
#define ERRORCODES_H

#define CS_ERROR_SUCCESS		0
#define	CS_ERROR_EXCEPTION		1001	//Unspecified failure occurred
#define	CS_ERROR_INVALID_ARGS		1002	//One or more commandline arguments were specified
#define CS_ERROR_TOO_MANY_BAD_ROWS	1003	//Too many bad rows were detected in an input file (Q: How many is too many ?)
#define	CS_ERROR_TOO_MANY_EXCEPTIONS	1004	//Too many exceptions have occurred (Q: How many is too many ?)

#define CS_ERROR_RESOURCE_NOT_FOUND	2001	//A dependent input resource was not found
#define CS_ERROR_FILE_NOT_FOUND		2002	//A dependent input file was not found
#define	CS_ERROR_FILE_CREATION_FAILED	2003	//An output file cannot be created
#define CS_ERROR_FILE_MOVE_FAILED	2004	//A file move operation failed
#define CS_ERROR_BAD_FILE_FORMAT	2005	//An input file was not formatted as expected

#define CS_ERROR_EXTERNAL		3001	//An external task failed
#define CS_ERROR_SYNCSORT		3002	//A SyncSort failure occurred
#define CS_ERROR_OS_ERROR		3003	//An operating system error occurred (Example?)

#define CS_ERROR_NO_DATABASE		4001	//A connection to a database could not be opened
#define	CS_ERROR_NO_DATA		4002	//No rows were returned from a database query
#define CS_ERROR_TIMEOUT		4003	//The database connection timedout
#define CS_ERROR_LOAD_FAILED		4004	//A database load failed

//CS_ERROR_APPLICATION	9000 - 9999 Application specific error
#endif
