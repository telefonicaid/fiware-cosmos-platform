/* ****************************************************************************
 *
 * FILE            object_Test.cpp
 *
 * AUTHOR          Javier Lois
 *
 * DATE            December 2011
 *
 * DESCRIPTION
 *
 * unit testing of the Object class in the engine library
 *
 */

#include "gtest/gtest.h"

#include "engine/Engine.h"
#include "engine/Object.h"

#include "unitTest/common_engine_test.h"

// Tests size_t getEngineId();
TEST(objecttest, getEngineIdTest) {
  init_engine_test();

  engine::Object *object1 = new engine::Object("engine");
  engine::Object *object2 = new engine::Object();
  engine::Object *object3 = new engine::Object();

  // getEngineId() should return a non zero value, different for each onject
  size_t id1 = object1->getEngineId();
  size_t id2 = object2->getEngineId();
  size_t id3 = object3->getEngineId();

  EXPECT_TRUE(id1 != 0);
  EXPECT_TRUE(id1 != id2);
  EXPECT_TRUE(id1 != id3);
  EXPECT_TRUE(id2 != id3);

  delete object1;
  delete object2;
  delete object3;

  close_engine_test();
}


