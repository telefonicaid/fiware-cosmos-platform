#ifndef USER_MGR_H
#define USER_MGR_H

/* ****************************************************************************
*
* FILE                     UserMgr.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Feb 03 2011
*
*/
class User;



/* ****************************************************************************
*
* UserMgr
*/
class UserMgr
{
public:
	UserMgr(unsigned int size);

	int           users(void);
	void          insert(const char* name, const char* password, long permissions);
	User*         lookup(const char* name);
	User*         allowToEnter(char* name, char* password);

private:
	User**        userV;
	unsigned int  size;
};

#endif
