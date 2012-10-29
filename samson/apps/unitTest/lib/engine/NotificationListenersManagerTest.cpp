#include "engine/Engine.h"
#include "engine/NotificationListener.h"
#include "engine/NotificationListenersManager.h"
#include "gtest/gtest.h"



//
// idTest
//
TEST(engine_NotificationListenersManager, engine_idTest) {
  engine::Engine::InitEngine(4, 1000000, 1);

  {
    engine::NotificationListenersManager listener_manager;

    engine::NotificationListener listener;
    engine::NotificationListener listener2;
    engine::NotificationListener listener3;
    engine::NotificationListener listener4;

    listener_manager.Add(&listener);
    listener_manager.Add(&listener2);

    listener_manager.AddToChannel(&listener, "channel_name");
    listener_manager.AddToChannel(&listener2, "channel_name");
    listener_manager.AddToChannel(&listener4, "channel_name");
    listener_manager.RemoveFromChannel(&listener4, "channel_name");

    std::set<size_t> ids = listener_manager.GetEndgineIdsForChannel("channel_name");

    EXPECT_EQ(2ULL, ids.size());
    EXPECT_FALSE(ids.find(listener.engine_id()) == ids.end());
    EXPECT_FALSE(ids.find(listener2.engine_id()) == ids.end());
    EXPECT_TRUE(ids.find(listener3.engine_id()) == ids.end());


    //
    // Adding and removing a listener
    //
    listener_manager.AddToChannel(&listener4, "channel_name");
    ids = listener_manager.GetEndgineIdsForChannel("channel_name");
    EXPECT_EQ(3ULL, ids.size());

    listener_manager.RemoveFromChannel(&listener4, "channel_name");
    ids = listener_manager.GetEndgineIdsForChannel("channel_name");
    EXPECT_EQ(2ULL, ids.size());
  }
  engine::Engine::DestroyEngine();
}
