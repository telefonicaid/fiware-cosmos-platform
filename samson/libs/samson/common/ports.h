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
#ifndef SAMSON_PORTS_H
#define SAMSON_PORTS_H

/* ****************************************************************************
*
* FILE                     ports.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Jan 13 2011
*
*/



/* ****************************************************************************
*
* Common ports for the Samson platform
*/

/* Judges 13:24
 *
 * And the woman bare a son, and called his name Samson: and the child grew,
 * and the LORD blessed him.
 */
#define SAMSON_WORKER_PORT     1324


/* Genisis 2:2 (Book 1) 
 *
 * And on the seventh day God ended his work which he had made; and he rested
 * on the seventh day from all his work which he had made
 */
#define SAMSON_WORKER_WEB_PORT 1202 //RESTful API

#endif
