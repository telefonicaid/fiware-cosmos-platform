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
#include <fcntl.h>

#include "au/string/Descriptors.h"
#include "gtest/gtest.h"


TEST(au_Descriptors, basic) {
  au::Descriptors descriptors;

  descriptors.Add("Hola");
  descriptors.Add("Hola");
  descriptors.Add("Adios");
  descriptors.Add("Adios");
  descriptors.Add("Adios");

  EXPECT_EQ((size_t)2, descriptors.size());
  EXPECT_EQ("3 X Adios 2 X Hola", descriptors.str());
}
