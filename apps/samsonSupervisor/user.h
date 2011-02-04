#ifndef USER_H
#define USER_H

/* ****************************************************************************
*
* FILE                     User.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Feb 04 2011
*
*/



/* ****************************************************************************
*
* User - 
*/
typedef struct User
{
	char*  name;
	char*  password;
	long   permissions;
} User;

#endif
