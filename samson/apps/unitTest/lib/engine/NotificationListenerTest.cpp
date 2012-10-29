/* ****************************************************************************
*
* FILE            NotificationListenerTest.cpp
*
* AUTHOR          Javier Lois
*
* DATE            December 2011
*
* DESCRIPTION
*
* unit testing of the NotificationListenerTest class in the engine library
*
*/
#include "gtest/gtest.h"

#include "engine/Engine.h"
#include "engine/NotificationListener.h"
#include "engine/Notification.h"

#include "unitTest/common_engine_test.h"

int x;

class NotificationListenerTest : public engine::NotificationListener
{
public:
  ~NotificationListenerTest() {}
  void notify2(engine::Notification *notification) { notify(notification);      }
  void listen2(const char *notification_name)      { listen(notification_name); }
};

TEST(engine_NotificationListener, engine_idTest) {
  init_engine_test();

  NotificationListenerTest*     nlTest  = new NotificationListenerTest();
  engine::NotificationListener *object1 = new engine::NotificationListener();
  engine::NotificationListener *object2 = new engine::NotificationListener();
  engine::NotificationListener *object3 = new engine::NotificationListener();

  // engine_id() should return a non zero value, different for each onject
  size_t id1 = object1->engine_id();
  size_t id2 = object2->engine_id();
  size_t id3 = object3->engine_id();

  EXPECT_TRUE(id1 != 0);
  EXPECT_TRUE(id2 != 0);
  EXPECT_TRUE(id3 != 0);

  EXPECT_TRUE(id1 != id2);
  EXPECT_TRUE(id1 != id3);
  EXPECT_TRUE(id2 != id3);

  engine::Notification* nP = new engine::Notification("Test");
  nlTest->notify2(nP);
  nlTest->listen2("Test");
  delete nP;

  delete object1;
  delete object2;
  delete object3;
  delete nlTest;

  close_engine_test();
}
