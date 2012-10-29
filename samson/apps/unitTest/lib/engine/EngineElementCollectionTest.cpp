#include "engine/EngineElementCollection.h"
#include "engine/Notification.h"
#include "engine/NotificationElement.h"
#include "gtest/gtest.h"



class EngineElementTest1 : public engine::EngineElement
{
public:
  EngineElementTest1(std::string name) : EngineElement(name) {};
  ~EngineElementTest1() { LM_M(("engine element of type 1 is destroyed")); }
  void run() { LM_M(("running ...")); }
};


class EngineElementTest2 : public engine::EngineElement
{
public:
  EngineElementTest2(std::string name, int seconds) : EngineElement(name, seconds) {};
  ~EngineElementTest2() { LM_M(("engine element of type 2 is destroyed")); }
  void run() { LM_M(("running ...")); }
};



//
// idTest
//
TEST(engine_EngineElementCollection, engine_idTest) {
  engine::EngineElementCollection engine_element_collection;

  EXPECT_TRUE(engine_element_collection.IsEmpty());
  EXPECT_EQ(0ULL, engine_element_collection.GetNumEngineElements());
  EXPECT_EQ(0ULL, engine_element_collection.GetNumNormalEngineElements());

  engine_element_collection.Add(new engine::NotificationElement(new engine::Notification("notification_name")));

  EXPECT_FALSE(engine_element_collection.IsEmpty());
  EXPECT_EQ(1ULL, engine_element_collection.GetNumNormalEngineElements());
  EXPECT_EQ(NULL, engine_element_collection.NextRepeatedEngineElement());

  engine::EngineElement *element = engine_element_collection.NextNormalEngineElement();

  EXPECT_EQ(0ULL, engine_element_collection.GetNumNormalEngineElements());
  EXPECT_TRUE(element != NULL);
  delete element;

  EXPECT_EQ(0ULL, engine_element_collection.ExtraElements().size());

  // Add extra element
  engine::NotificationElement *element2 = new engine::NotificationElement(new engine::Notification("notification_name"));
  element2->SetAsExtra();
  engine_element_collection.Add(element2);

  EXPECT_EQ(1ULL, engine_element_collection.ExtraElements().size());
  EXPECT_EQ(1ULL, engine_element_collection.GetNumEngineElements());
  EXPECT_EQ(0ULL, engine_element_collection.GetNumNormalEngineElements());


  EngineElementTest1* engine_element1 = new EngineElementTest1("Engine Element Test 1");
  EngineElementTest2* engine_element2 = new EngineElementTest2("Engine Element Test 2", 10);
  EngineElementTest1* engine_element3 = new EngineElementTest1("Engine Element Test 3");
  engine_element3->SetAsExtra();

  engine_element_collection.Add(engine_element1);
  engine_element_collection.Add(engine_element2);
  engine_element_collection.Add(engine_element3);

  std::string s = engine_element_collection.GetTableOfEngineElements();
  engine_element_collection.PrintElements();
}
