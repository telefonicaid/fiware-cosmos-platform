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

#include "au/utils.h"  // Own interface

#include <iostream>
#include <signal.h>
#include <sstream>
#include <stdio.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#include "au/string/StringUtilities.h"


namespace au {
const char *valid_chars = "01234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRTSUVXYZ";

int getTerminalColumns() {
  int x, y;

  GetTerminalSize(0, &x, &y);
  return x;
}

void ClearTerminalLine() {
  printf("\r");
  for (int i = 0; i < getTerminalColumns(); ++i) {
    printf(" ");
  }
  printf("\r");
  fflush(stdout);
}

size_t code64_rand() {
  size_t v;
  char *c =  reinterpret_cast<char *>(&v);

  for (size_t i = 0; i < sizeof(size_t) / sizeof(char); ++i) {
    c[i] = rand() % strlen(valid_chars);
  }
  return v;
}

bool code64_is_valid(size_t v) {
  char *c =  reinterpret_cast<char *>(&v);

  for (size_t i = 0; i < sizeof(size_t) / sizeof(char); ++i) {
    if (c[i] >= (int)strlen(valid_chars)) {
      return false;
    }
  }
  return true;
}

int GetCode64Base(char v) {
  for (size_t i = 0; i < strlen(valid_chars); ++i) {
    if (v == valid_chars[i]) {
      return i;
    }
  }
  return -1;
}

size_t code64_num(const std::string& value) {
  size_t v;
  char *c =  reinterpret_cast<char *>(&v);

  if (value.length() != sizeof(size_t)) {
    return static_cast<size_t>(-1);
  }

  for (size_t i = 0; i < sizeof(size_t); ++i) {
    int num_value = GetCode64Base(value[i]);
    if (num_value == -1) {
      return static_cast<size_t>(-1);  // Not possible to transform this letter
    }
    c[i] = num_value;
  }
  return v;
}

std::string code64_str(size_t v) {
  char str[sizeof(size_t) + 1];
  char *c =  reinterpret_cast<char *>(&v);

  for (size_t i = 0; i < sizeof(size_t); ++i) {
    int p = c[i];
    if (p >= (int)strlen(valid_chars)) {
      str[i] = '?';
    } else {
      str[i] = valid_chars[p];
    }
  }

  str[sizeof(size_t)] = '\0';

  return str;
}
}
