/* ****************************************************************************
 *
 * FILE            MemoryCheckTest.cpp
 *
 * AUTHOR          Ken Zangelin
 *
 * DATE            Nov 2012
 *
 *
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 *
 * DESCRIPTION
 * unit testing of the common class in the samson  library
 *
 */
#include "gtest/gtest.h"

#include "samson/common/MemoryCheck.h"

// -----------------------------------------------------------------------------
// Test the MemoryCheck function
//
TEST(samson_common_MemoryCheck, MemoryCheck) {
  bool ok = samson::MemoryCheck();
  EXPECT_TRUE(ok);

  LM_M(("MemoryCheck Exercising 'if (samson_required_mem > max_memory_size)'"));
  // Exercising 'if (samson_required_mem > max_memory_size)'
  samson::SamsonSetup* setup = au::Singleton<samson::SamsonSetup>::shared();
  setup->Set("general.num_processess", "17");
  setup->Set("general.shared_memory_size_per_buffer", "536870912"); // 512Mb
  ok = samson::MemoryCheck();
  EXPECT_FALSE(ok);

  LM_M(("MemoryCheck Exercising 'if (shared_memory_size_per_buffer > kernel_shmmax)'"));
  // Exercising 'if (shared_memory_size_per_buffer > kernel_shmmax)'
  setup->Set("general.shared_memory_size_per_buffer", "3221225472"); // 3Gb
  setup->Set("general.num_processess", "1");
  ok = samson::MemoryCheck();
  EXPECT_FALSE(ok);
}
