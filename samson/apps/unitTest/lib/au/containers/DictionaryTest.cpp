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
#include "au/containers/Dictionary.h"
#include "au/containers/SharedPointer.h"
#include "gtest/gtest.h"
#include "unitTest/TestClasses.h"

TEST(au_containers_Dictionary, assignation) {
  au::Dictionary<std::string, TestBase> dictionary;

  dictionary.Set("hola", new TestBase(10));
  au::SharedPointer<TestBase> p = dictionary.Get("hola");


  EXPECT_EQ(p->value(), 10) << "Error in au::Dictionary basic operations";
}

TEST(au_containers_Dictionary, duplication) {
  au::Dictionary<std::string, TestBase> dictionary;

  dictionary.Set("hola", new TestBase(10));
  dictionary.Set("adios", dictionary.Get("hola"));

  au::SharedPointer<TestBase> p = dictionary.Get("hola");
  EXPECT_EQ(p->value(), 10) << "Error in au::Dictionary basic operations";

  EXPECT_EQ(dictionary.Get("hola") == dictionary.Get("adios"),
            true) << "Error in au::Dictionary duplication";
}

