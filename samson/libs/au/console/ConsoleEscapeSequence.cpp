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

#include "au/console/ConsoleEscapeSequence.h"  // Own interface
#include "au/string/StringUtilities.h"

namespace au {
ConsoleEscapeSequence::ConsoleEscapeSequence() {
  init();

  // Suported sequences
  addSequence(127);    // Delete

  addSequence(91, 65);      // move_up
  addSequence(91, 66);      // move_down
  addSequence(91, 67);      // move_forward
  addSequence(91, 68);      // move_backward
  addSequence("au");

  addSequence("h");   // History...

  addSequence("b");   // Block background messages
}
}