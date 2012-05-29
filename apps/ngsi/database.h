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



/* ****************************************************************************
*
* dbRegistrationAdd - 
*/
extern int dbRegistrationAdd(std::string id);

#endif


 
