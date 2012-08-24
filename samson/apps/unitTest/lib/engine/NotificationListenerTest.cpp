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
#include "engine/NotificationListener.h"

#include "unitTest/common_engine_test.h"


TEST(engine_NotificationListener, engine_idTest) {
  
  init_engine_test();

  engine::NotificationListener *object1 = new engine::NotificationListener();
  engine::NotificationListener *object2 = new engine::NotificationListener();
  engine::NotificationListener *object3 = new engine::NotificationListener();

  // engine_id() should return a non zero value, different for each onject
  size_t id1 = object1->engine_id();
  size_t id2 = object2->engine_id();
  size_t id3 = object3->engine_id();

  EXPECT_TRUE(id1 != 0);
  EXPECT_TRUE(id1 != id2);
  EXPECT_TRUE(id1 != id3);
  EXPECT_TRUE(id2 != id3);

  delete object1;
  delete object2;
  delete object3;

  close_engine_test();
}


