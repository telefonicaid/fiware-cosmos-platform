
#include "engine/EngineElement.h"
#include "gtest/gtest.h"

static int test2runs = 0;


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
  void run() { LM_M(("running ...")); ++test2runs; }
};



class EngineElementTest3 : public engine::EngineElement
{
public:
  EngineElementTest3(std::string name, int seconds) : EngineElement(name, seconds) {};
  ~EngineElementTest3() { LM_M(("engine element of type 3 is destroyed")); }
  void run() { LM_M(("running ...")); }
};

TEST(engine_EngineElement, engine_idTest)
{
  EngineElementTest1 engine_element1("Engine Element Test 1");
  EngineElementTest2 engine_element2("Engine Element Test 2", 10);
  EngineElementTest3 engine_element3("Engine Element Test 3", 10);

  std::string s;
  std::string str = engine_element1.str();

  s  = engine_element1.short_description();
  EXPECT_STREQ(s.c_str(), "Engine element to be executed once");

  s  = engine_element2.short_description();
  EXPECT_STREQ(s.c_str(), "Engine element repeated every 10 seconds");

  s = engine_element1.str();
  EXPECT_STREQ(s.c_str(), "Engine element to be executed once [ Engine element ]");

  s = engine_element2.str();
  EXPECT_STREQ(s.c_str(), "Engine element repeated every 10 seconds [ Engine element to be executed in 10.00 seconds ( repeat every 10 secs , repeated 0 times )] ");

  bool b = engine_element1.IsNormal();
  EXPECT_EQ(b, true);

  b = engine_element2.IsNormal();
  EXPECT_EQ(b, false);

  engine_element2.SetAsExtra();
  s = engine_element2.str();
  EXPECT_STREQ(s.c_str(), "Engine element repeated every 10 seconds [ Engine element EXTRA ]");
}
