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
#ifndef _H_DELILAH_BASE
#define _H_DELILAH_BASE

/* ****************************************************************************
 *
 * FILE                     DelilahBase.h
 *
 * DESCRIPTION			   Basic stuff for delilah client
 *
 */


#include <string>                      // std::string
#include <vector>                      // std::vector

#include "au/console/ConsoleAutoComplete.h"
#include "au/containers/map.h"
#include "au/mutex/Token.h"            // au::Token
#include "au/statistics/Cronometer.h"  // au::Cronometer

#include "au/tables/DataBase.h"

#include "samson/network/NetworkInterface.h"


#include "au/tables/pugi.h"            // pugi::... node_to_string_function

namespace samson {

}


#endif  // ifndef _H_DELILAH_BASE

