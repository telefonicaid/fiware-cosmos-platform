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
#include "au/containers/SharedPointer.h"
#include "gtest/gtest.h"
#include "unitTest/TestClasses.h"


TEST(au_containers_SharedPointer, assignation) {
  au::SharedPointer< TestBase > a(new TestBase(10));
  au::SharedPointer< TestBase > b = a;

  EXPECT_EQ(b->value(), 10) << "Error in au::SharedPointer assignation";

  EXPECT_EQ(2, a.reference_count()) <<  "Error in au::SharedPointer assignation";
  EXPECT_EQ(2, b.reference_count()) <<  "Error in au::SharedPointer assignation";
}

TEST(au_containers_SharedPointer, operators) {
  au::SharedPointer< TestBase > a(new TestBase(10));
  au::SharedPointer< TestBase > b = a;

  au::SharedPointer< TestBase > c(new TestBase(2));

  TestBase& aa = *a;
  TestBase& bb = *b;
  TestBase& cc = *c;

  EXPECT_EQ(&aa == &bb, true) << "Error in au::SharedPointer operator *";
  EXPECT_EQ(a.shared_object() == b.shared_object(), true) << "Error in au::SharedPointer::shared_object() ";
  EXPECT_EQ(a == b, true) << "Error in au::SharedPointer operator == ";
  EXPECT_EQ(a != b, false) << "Error in au::SharedPointer operator != ";

  EXPECT_EQ(&aa == &cc, false) << "Error in au::SharedPointer operator *";
  EXPECT_EQ(a.shared_object() == c.shared_object(),
            false) << "Error in au::SharedPointer::shared_object() ";
  EXPECT_EQ(a == c, false) << "Error in au::SharedPointer operator == ";
  EXPECT_EQ(a != c, true) << "Error in au::SharedPointer operator != ";

  EXPECT_EQ(a == b.shared_object(),
            true) << "Error in au::SharedPointer operator ==";
  EXPECT_EQ(a == c.shared_object(),
            false) << "Error in au::SharedPointer operator ==";
}

TEST(au_containers_SharedPointer, static_castings) {
  au::SharedPointer< TestDerived > a(new TestDerived(10));
  au::SharedPointer< TestBase > b = a.static_pointer_cast<TestBase>();
  au::SharedPointer< TestBase > c(new TestBase(11));

  EXPECT_EQ(a == b, true) << "Error in au::SharedPointer operator ==";
  EXPECT_EQ(a == c, false) << "Error in au::SharedPointer operator ==";
}

TEST(au_containers_SharedPointer, dynamic_castings) {
  au::SharedPointer< TestDerived > a(new TestDerived(10));

  au::SharedPointer< TestBase > base_a = a.static_pointer_cast<TestBase>();

  // Dynamic casting to derived classes
  au::SharedPointer< TestDerived > b = base_a.dynamic_pointer_cast<TestDerived>();
  au::SharedPointer< TestDerived2 > c =
    base_a.dynamic_pointer_cast<TestDerived2>();


  EXPECT_EQ(a == b, true) << "Error in au::SharedPointer dynamic casting (1)";
  EXPECT_EQ(c == NULL,
            true) << "Error in au::SharedPointer dynamic casting (2)";
  EXPECT_EQ(a == c, false) << "Error in au::SharedPointer dynamic casting (3)";
}

class TestSharedPointerReturnFunction {
public:

  TestSharedPointerReturnFunction(au::SharedPointer< TestBase > a) {
    a_ = a;
  }

  au::SharedPointer< TestBase > a() {
    return a_;
  }

private:

  au::SharedPointer< TestBase > a_;
};

TEST(au_containers_SharedPointer, return_function) {
  EXPECT_EQ(0, TestBase::num_instances());

  // Create one
  au::SharedPointer< TestBase > a(new TestBase(10));

  TestSharedPointerReturnFunction *test = new TestSharedPointerReturnFunction(a);
  a.Reset();

  EXPECT_EQ(1, TestBase::num_instances());

  // Use with temporal shared pointer
  test->a()->set_value(4);

  EXPECT_EQ(1, TestBase::num_instances());
  delete test;

  EXPECT_EQ(0, TestBase::num_instances());
}

