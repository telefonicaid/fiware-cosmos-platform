/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */
#ifndef QSI_EXPAND_LIST_H
#define QSI_EXPAND_LIST_H

/* ****************************************************************************
*
* FILE                     QsiExpandList.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            May 17 2011
*
*/
#include "QsiBox.h"             // Box



namespace Qsi
{



/* ****************************************************************************
*
* - 
*/
class Manager;
class Base;
class Block;



/* ****************************************************************************
*
* ExpandList - 
*/
class ExpandList : public Box
{
private:
	char*      titleString;
	Block*     title;
	Box*       memberBox;
	Base**     member;
	int        members;
	int        autoExCo;
	bool       frame;
	int        xmargin;
	int        ymargin;
	int        memberSpace;

public:
	ExpandList(Manager* manager, Box* owner, const char* _title, int x, int y, int _xmargin = 10, int _ymargin = 10, MenuFunction onClick = NULL, bool _frame = false);
	~ExpandList();

	Base*   addMember(const char* string, MenuFunction callback = NULL, const void* dataP = NULL, const char* mVec[] = NULL);
	Base*   addMember(Base* _member,   MenuFunction callback = NULL, const void* dataP = NULL, const char* mVec[] = NULL);
	void    expand(void);
	void    compress(void);
	void    exCoOnButtonPress(bool _autoExCo);
	void    titleSet(const char* _title);
	void    menu(MenuFunction callback, const char* mVec[]);
	Block*  titleGet(void) { return title; };
	void    setFrame(int padding);
};

}

#endif
