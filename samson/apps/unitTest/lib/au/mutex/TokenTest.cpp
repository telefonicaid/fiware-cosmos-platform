#include <fcntl.h>

#include "au/network/RESTService.h"
#include "au/network/RESTServiceCommand.h"
#include "gtest/gtest.h"
#include "unitTest/test_common.h"

TEST(au_mutex_Token, basic) {
  au::Token token;

  EXPECT_FALSE(token.IsRetainedByMe());

  {
    au::TokenTaker tt(&token);
    EXPECT_TRUE(token.IsRetainedByMe());
  }

  EXPECT_FALSE(token.IsRetainedByMe());
}
