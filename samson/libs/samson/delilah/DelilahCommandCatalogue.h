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
#ifndef _H_DELILAH_COMMAND_CATALOGE
#define _H_DELILAH_COMMAND_CATALOGE

#include "au/console/CommandCatalogue.h"

namespace samson {
/**
 * \brief \ref DelilahCommandCatalogue is the catalogue of valid commands for delilah console
 */

class DelilahCommandCatalogue : public au::console::CommandCatalogue {
public:

  // Constructor with commands definitions
  DelilahCommandCatalogue();
};
}

#endif