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


#ifndef _H_MAIN_DELILAH_QT
#define _H_MAIN_DELILAH_QT

#include "tables/Tree.h"

class MainWindow;
class SamsonConnect;
class SamsonQueryWidget;

extern MainWindow *mainWindow;               // Main window....
extern SamsonConnect *samsonConnect;         // Connection dialog...
extern SamsonQueryWidget* samsonQueryWidget; // Query widget used in the background

#endif
