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
/* ****************************************************************************
 *
 * FILE            memoryManager_Test.cpp
 *
 * AUTHOR          Javier Lois
 *
 * DATE            December 2011
 *
 * DESCRIPTION
 *
 * Unit testing of the SimpleBuffer class in the engine library
 *
 *
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefonica Investigacion y Desarrollo S.A.U.
 * All rights reserved.
 */
#include "gtest/gtest.h"

#include "engine/SimpleBuffer.h"

TEST(engine_SimpleBuffer, engine_idTest) {
  engine::SimpleBuffer simple_buffer;

  EXPECT_EQ(NULL, simple_buffer.data());
  EXPECT_EQ(0ULL, simple_buffer.size());

  char* data = reinterpret_cast<char *>(malloc(10));

  engine::SimpleBuffer simple_buffer2(data, 10);
  EXPECT_EQ(data, simple_buffer2.data());
  EXPECT_EQ(10ULL, simple_buffer2.size());

  EXPECT_TRUE(simple_buffer2.checkSize(5));
  EXPECT_TRUE(simple_buffer2.checkSize(10));
  EXPECT_FALSE(simple_buffer2.checkSize(20));

  free(data);
}
