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

#include "au/network/FileDescriptor.h"
#include "gtest/gtest.h"
#include "unitTest/TestClasses.h"

TEST(au_network_FileDescriptor, basic) {
  au::FileDescriptor file_descriptor("name_of_fd", -1);

  EXPECT_EQ(file_descriptor.name() == "name_of_fd",
            true) << "Error in FileDescriptor";
  EXPECT_EQ(file_descriptor.fd(), -1) << "Error in FileDescriptor";

  EXPECT_EQ((size_t)0, file_descriptor.rate_in().size()) << "Error in FileDescriptor";
  EXPECT_EQ((size_t)0, file_descriptor.rate_out().size()) << "Error in FileDescriptor";

  EXPECT_EQ(file_descriptor.IsClosed(), true) << "Error in FileDescriptor";
}
