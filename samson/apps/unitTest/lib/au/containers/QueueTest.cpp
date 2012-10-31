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
#include "au/containers/Queue.h"
#include "au/containers/SharedPointer.h"
#include "gtest/gtest.h"
#include "unitTest/TestClasses.h"

TEST(au_containers_Queue, assignation) {
  EXPECT_EQ(TestBase::num_instances(),
            0) << "Error in number of instances of test class";

  {
    au::Queue<TestBase> queue;

    queue.Push(new TestBase(10));
    queue.Push(new TestBase(11));
    queue.Push(new TestBase(12));
    queue.Push(new TestBase(13));
    // Make sure the counter works

    EXPECT_EQ(TestBase::num_instances() > 0,
              true) <<
    "Something wrong with the instance counter for test class";


    EXPECT_EQ(queue.Pop()->value(), 10) << "Error in au::Queue basic operations";
    EXPECT_EQ(queue.Pop()->value(), 11) << "Error in au::Queue basic operations";
    EXPECT_EQ(queue.Pop()->value(), 12) << "Error in au::Queue basic operations";
    EXPECT_EQ(queue.Pop()->value(), 13) << "Error in au::Queue basic operations";
  }

  EXPECT_EQ(TestBase::num_instances(),
            0) << "Error in number of instances of test class";
}

