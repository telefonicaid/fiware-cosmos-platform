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

#include "au/log/LogMain.h"
#include "engine/EngineElement.h"
#include "gtest/gtest.h"

class EngineElementTest1 : public engine::EngineElement {
public:
  explicit EngineElementTest1(const std::string& name) : EngineElement(name) {
  }

  virtual ~EngineElementTest1() {
    LOG_SM(("engine element of type 1 is destroyed"));
  }

  void run() {
    LOG_SM(("running ..."));
  }
};

class EngineElementTest2 : public engine::EngineElement {
public:
  explicit EngineElementTest2(const std::string& name, int seconds) :
    EngineElement(name, seconds) {
  }

  virtual ~EngineElementTest2() {
    LOG_SM(("engine element of type 2 is destroyed"));
  }

  void run() {
    LOG_SM(("running ..."));
  }
};

class EngineElementTest3 : public engine::EngineElement {
public:
  explicit EngineElementTest3(const std::string& name, int seconds) :
    EngineElement(name, seconds) {
  }

  virtual ~EngineElementTest3() {
    LOG_SM(("engine element of type 3 is destroyed"));
  }

  void run() {
    LOG_SM(("running ..."));
  }
};

// -----------------------------------------------------------------------------
// engine_idTest - test of the Engine elements
//
TEST(engine_EngineElement, engine_idTest) {
  EngineElementTest1 engine_element1("Engine Element Test 1");
  EngineElementTest2 engine_element2("Engine Element Test 2", 10);
  EngineElementTest3 engine_element3("Engine Element Test 3", 10);

  std::string elementString = engine_element1.str();

  std::string description = engine_element1.short_description();

  EXPECT_STREQ(description.c_str(), "Engine element to be executed once");
  EXPECT_STREQ(engine_element2.short_description().c_str(), "Engine element repeated every 10 seconds");
  EXPECT_STREQ(engine_element1.str().c_str(), "Engine element to be executed once");
  EXPECT_STREQ("[Repeat 10 secs] Engine element repeated every 10 seconds", engine_element2.str().c_str());
  EXPECT_EQ(engine_element1.IsNormal(), true);
  EXPECT_EQ(engine_element2.IsNormal(), false);

  engine_element2.SetAsExtra();
  EXPECT_EQ(engine_element2.IsExtra(), true);
  EXPECT_STREQ("Extra] Engine element repeated every 10 seconds", engine_element2.str().c_str());
}
