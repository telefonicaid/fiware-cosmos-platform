/* ****************************************************************************
*
* FILE                     UserMgr.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Feb 03 2011
*
*/
#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // Trace Levels

#include "Popup.h"              // Popup
#include "user.h"               // User
#include "UserMgr.h"            // Own interface



/* ****************************************************************************
*
* UserMgr
*/
UserMgr::UserMgr(unsigned int size)
{
	this->size = size;

	userV = (User**) calloc(size, sizeof(User*));
	if (userV == NULL)
		LM_X(1, ("error allocating room for %d delilah users", size));
}



/* ****************************************************************************
*
* UserMgr::users - 
*/
int UserMgr::users(void)
{
	unsigned int  ix;
	int           userNo = 0;

	for (ix = 0; ix < size; ix++)
	{
		if (userV[ix] != NULL)
			++userNo;
	}

	return userNo;
}



/* ****************************************************************************
*
* UserMgr::insert - 
*/
void UserMgr::insert(const char* name, const char* password, long permissions)
{
	unsigned int  ix;

	for (ix = 0; ix < size; ix++)
	{
		if (userV[ix] == NULL)
		{
			userV[ix] = (User*) calloc(1, sizeof(User));

			if (userV[ix] == NULL)
				LM_X(1, ("error allocating user of %d bytes: %s", sizeof(User), strerror(errno)));

			userV[ix]->name        = strdup(name);
			userV[ix]->password    = strdup(password);
			userV[ix]->permissions = permissions;

			return;
		}
	}

	LM_TODO(("realloc user vector ..."));
	new Popup("Out of users", "Sorry, no room for more users.\nTo fix this, you need to contact the developer (kzangeli@tid.es).");
}



/* ****************************************************************************
*
* UserMgr::lookup - 
*/
User* UserMgr::lookup(char* name)
{
	unsigned int  ix;

	for (ix = 0; ix < size; ix++)
	{
		if (userV[ix] == NULL)
			continue;

		if (strcmp(userV[ix]->name, name) == 0)
			return userV[ix];
	}

	return NULL;
}



/* ****************************************************************************
*
* UserMgr::allowToEnter - 
*/
bool UserMgr::allowToEnter(char* name, char* password)
{
	User* user = lookup(name);

	if (user == NULL)
	{
		LM_T(LmtUser, ("Cannot find user '%s'", name));
		return false;
	}

	if (strcmp(user->password, password) == 0)
	{
		LM_T(LmtUser, ("User '%s' accepted", name));
		return true;
	}

	LM_T(LmtUser, ("Bad password for user '%s'", name));
	return false;
}
