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



/* ****************************************************************************
*
* dbItemsInTable - 
*/
extern int dbItemsInTable(std::string table);

#endif
