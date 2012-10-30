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

#include "engine/SimpleBuffer.h"
#include "gtest/gtest.h"

TEST(engine_SimpleBuffer, engine_idTest) {
  engine::SimpleBuffer simple_buffer;

  EXPECT_EQ(NULL, simple_buffer.data());
  EXPECT_EQ(0ULL, simple_buffer.size());


  char *data = (char *)malloc(10);

  engine::SimpleBuffer simple_buffer2(data, 10);
  EXPECT_EQ(data, simple_buffer2.data());
  EXPECT_EQ(10ULL, simple_buffer2.size());

  EXPECT_TRUE(simple_buffer2.checkSize(5));
  EXPECT_TRUE(simple_buffer2.checkSize(10));
  EXPECT_FALSE(simple_buffer2.checkSize(20));

  free(data);
}
