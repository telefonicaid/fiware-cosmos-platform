
#include "gtest/gtest.h"
#include "engine/Notification.h"
#include "engine/NotificationElement.h"
#include "engine/EngineElementCollection.h"

TEST(engine_EngineElementCollection, engine_idTest) {
  
  engine::EngineElementCollection engine_element_collection;
  
  EXPECT_TRUE( engine_element_collection.IsEmpty() );
  EXPECT_EQ(0,engine_element_collection.GetNumEngineElements());
  EXPECT_EQ(0,engine_element_collection.GetNumNormalEngineElements());
  
  engine_element_collection.Add( new engine::NotificationElement(  new engine::Notification("notification_name") ) );

  EXPECT_FALSE( engine_element_collection.IsEmpty() );
  EXPECT_EQ(1,engine_element_collection.GetNumNormalEngineElements());
  EXPECT_EQ(NULL, engine_element_collection.NextRepeatedEngineElement() );
  
  engine::EngineElement* element = engine_element_collection.NextNormalEngineElement();
  
  EXPECT_EQ(0,engine_element_collection.GetNumNormalEngineElements());
  EXPECT_TRUE( element!=NULL);
  delete element;

  EXPECT_EQ(0,engine_element_collection.ExtraElements().size());
  
  // Add extra element
  engine::NotificationElement * element2 = new engine::NotificationElement(  new engine::Notification("notification_name") );
  element2->SetAsExtra();
  engine_element_collection.Add(element2);
  
  EXPECT_EQ(1,engine_element_collection.ExtraElements().size());
  EXPECT_EQ(1,engine_element_collection.GetNumEngineElements());
  EXPECT_EQ(0,engine_element_collection.GetNumNormalEngineElements());
  
}
