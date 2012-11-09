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
 * FILE            EngineElementCollectionTest.cpp
 *
 * AUTHOR          Javier Lois
 *
 * DATE            January 2012
 *
 * DESCRIPTION
 *
 * unit testing of the EngineElementCollection class in the engine library
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

#include "engine/EngineElementCollection.h"
#include "engine/Notification.h"
#include "engine/NotificationElement.h"

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

// -----------------------------------------------------------------------------
// idTest -
//
TEST(engine_EngineElementCollection, engine_idTest) {
  engine::EngineElementCollection engine_element_collection;

  EXPECT_TRUE(engine_element_collection.IsEmpty());
  EXPECT_EQ(0ULL, engine_element_collection.GetNumEngineElements());
  EXPECT_EQ(0ULL, engine_element_collection.GetNumNormalEngineElements());

  engine_element_collection.Add(
     new engine::NotificationElement(
        new engine::Notification("notification_name")));

  EXPECT_FALSE(engine_element_collection.IsEmpty());
  EXPECT_EQ(1ULL, engine_element_collection.GetNumNormalEngineElements());
  EXPECT_EQ(NULL, engine_element_collection.NextRepeatedEngineElement());

  engine::EngineElement *element = engine_element_collection.NextNormalEngineElement();

  EXPECT_EQ(0ULL, engine_element_collection.GetNumNormalEngineElements());
  EXPECT_TRUE(element != NULL);
  delete element;

  EXPECT_EQ(0ULL, engine_element_collection.ExtraElements().size());

  // Add extra element
  engine::NotificationElement* element2 =
     new engine::NotificationElement(new engine::Notification("notification_name"));

  element2->SetAsExtra();
  engine_element_collection.Add(element2);

  EXPECT_EQ(1ULL, engine_element_collection.ExtraElements().size());
  EXPECT_EQ(1ULL, engine_element_collection.GetNumEngineElements());
  EXPECT_EQ(0ULL, engine_element_collection.GetNumNormalEngineElements());

  EngineElementTest1* engine_element1 = new EngineElementTest1("Engine Element Test 1");
  EngineElementTest2* engine_element2 = new EngineElementTest2("Engine Element Test 2", 10);
  EngineElementTest1* engine_element3 = new EngineElementTest1("Engine Element Test 3");
  engine_element3->SetAsExtra();

  EXPECT_TRUE(engine_element1 != NULL);
  EXPECT_TRUE(engine_element2 != NULL);
  EXPECT_TRUE(engine_element3 != NULL);

  EXPECT_TRUE(engine_element1->IsNormal());
  EXPECT_TRUE(engine_element2->IsRepeated());
  EXPECT_TRUE(engine_element3->IsExtra());

  engine_element_collection.Add(engine_element1);
  engine_element_collection.Add(engine_element2);
  engine_element_collection.Add(engine_element3);

  EXPECT_EQ(4, engine_element_collection.GetNumEngineElements());

  std::string s = engine_element_collection.GetTableOfEngineElements();
  engine_element_collection.PrintElements();
}
