/* ****************************************************************************
 *
 * FILE            EngineElementTest.cpp
 *
 * AUTHOR          Javier Lois
 *
 * DATE            January 2012
 *
 * DESCRIPTION
 *
 * unit testing of the EngineElement class in the engine library
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
 */
#include <string>

#include "gtest/gtest.h"

#include "engine/EngineElement.h"

class EngineElementTest1 : public engine::EngineElement {
 public:
  explicit EngineElementTest1(const std::string& name) : EngineElement(name) {}
  virtual ~EngineElementTest1() { LM_M(("engine element of type 1 is destroyed")); }
  void run() { LM_M(("running ...")); }
};

class EngineElementTest2 : public engine::EngineElement {
 public:
  explicit EngineElementTest2(const std::string& name, int seconds) :
  EngineElement(name, seconds) {}

  virtual ~EngineElementTest2() { LM_M(("engine element of type 2 is destroyed")); }
  void run() { LM_M(("running ...")); }
};

class EngineElementTest3 : public engine::EngineElement {
 public:
  explicit EngineElementTest3(const std::string& name, int seconds) :
  EngineElement(name, seconds) {}

  virtual ~EngineElementTest3() { LM_M(("engine element of type 3 is destroyed")); }
  void run() { LM_M(("running ...")); }
};

// -----------------------------------------------------------------------------
// engine_idTest - test of the Engine elements
//
TEST(engine_EngineElement, engine_idTest) {
  EngineElementTest1  engine_element1("Engine Element Test 1");
  EngineElementTest2  engine_element2("Engine Element Test 2", 10);
  EngineElementTest3  engine_element3("Engine Element Test 3", 10);

  std::string elementString = engine_element1.str();

  std::string description = engine_element1.short_description();
  EXPECT_STREQ(description.c_str(), "Engine element to be executed once");

  description = engine_element2.short_description();
  EXPECT_STREQ(description.c_str(), "Engine element repeated every 10 seconds");

  elementString = engine_element1.str();
  EXPECT_STREQ(elementString.c_str(), "Engine element to be executed once [ Engine element ]");

  elementString = engine_element2.str();
  EXPECT_STREQ(elementString.c_str(), "Engine element repeated every 10 seconds "
               "[ Engine element to be executed in 10.00 seconds "
               "( repeat every 10 secs , repeated 0 times )] ");

  EXPECT_EQ(engine_element1.IsNormal(), true);
  EXPECT_EQ(engine_element2.IsNormal(), false);

  engine_element2.SetAsExtra();
  EXPECT_EQ(engine_element2.IsExtra(), true);

  EXPECT_STREQ(engine_element2.str().c_str(),
               "Engine element repeated every 10 seconds [ Engine element EXTRA ]");
}
