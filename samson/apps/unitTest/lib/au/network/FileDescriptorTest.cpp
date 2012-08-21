#include <fcntl.h>

#include "au/network/FileDescriptor.h"
#include "gtest/gtest.h"
#include "unitTest/test_common.h"

TEST(au_network_FileDescriptor, basic) {
  au::FileDescriptor file_descriptor("name_of_fd", 5);

  EXPECT_EQ(file_descriptor.name() == "name_of_fd",
            true) << "Error in FileDescriptor";
  EXPECT_EQ(file_descriptor.fd(), 5) << "Error in FileDescriptor";

  EXPECT_EQ(file_descriptor.rate_in().getTotalSize(),
            0) << "Error in FileDescriptor";
  EXPECT_EQ(file_descriptor.rate_out().getTotalSize(),
            0) << "Error in FileDescriptor";

  EXPECT_EQ(file_descriptor.IsClosed(), false) << "Error in FileDescriptor";
}
