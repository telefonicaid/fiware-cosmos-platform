#ifndef DATABASE_H
#define DATABASE_H

/* ****************************************************************************
*
* FILE                        database.h - 
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            May 16 2012
*
*
*
*/
#include <mysql.h>                              // mysql



/* ****************************************************************************
*
* db - 
*/
extern MYSQL*     db;



/* ****************************************************************************
*
* dbInit - 
*/
extern int dbInit(void);



/* ****************************************************************************
*
* dbReset - 
*/
extern int dbReset(void);

#endif


 
