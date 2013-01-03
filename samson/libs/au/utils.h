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
#ifndef _H_AU_UTILS
#define _H_AU_UTILS

#include <cstring>
#include <string>


namespace au {
// Information about terminal
int getTerminalColumns();
void ClearTerminalLine();

/**
 * \brief Get a random valid "code64 number"
 * "code64 number" and size_t numbers that satisfy minor restrictions in order to be printable as strings ( see code64_str )
 */
size_t code64_rand();

/**
 * \brief Check if a provided "code64 number" is value
 * This checks if the provided size_t satisfy minor conditions to be printed as string
 */
bool code64_is_valid(size_t v);

/**
 * \brief Get string for a provided "code64 number"
 * All "code64 number" can be transformed to a string in order to be displayed.
 */
std::string code64_str(size_t);

/**
 * \brief Get "code64 number" based on its string representation
 * Inverse of code64_str
 */

size_t code64_num(const std::string& value);
/**
 * \brief Get position in valid_chars vector for a particular char
 * Used inside code64_num to recover original number based on its string representation
 */
int GetCode64Base(char v);
}

#endif  // ifndef _H_AU_UTILS
