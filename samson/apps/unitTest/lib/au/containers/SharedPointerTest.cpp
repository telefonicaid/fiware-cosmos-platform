#include "au/containers/SharedPointer.h"
#include "gtest/gtest.h"
#include "unitTest/test_common.h"


TEST(au_containers_SharedPointer, assignation) {
  au::SharedPointer< TestBase > a;
  a = new TestBase(10);
  au::SharedPointer< TestBase > b = a;

  EXPECT_EQ(b->v(), 10) << "Error in au::SharedPointer assignation";

  EXPECT_EQ(a.reference_count(),
            2) <<  "Error in au::SharedPointer assignation";
  EXPECT_EQ(b.reference_count(),
            2) <<  "Error in au::SharedPointer assignation";
}

TEST(au_containers_SharedPointer, operators) {
  au::SharedPointer< TestBase > a(new TestBase(10));
  au::SharedPointer< TestBase > b = a;

  au::SharedPointer< TestBase > c(new TestBase(2));

  TestBase& aa = *a;
  TestBase& bb = *b;
  TestBase& cc = *c;

  EXPECT_EQ(&aa == &bb, true) << "Error in au::SharedPointer operator *";
  EXPECT_EQ(a.shared_object() == b.shared_object(),
            true) << "Error in au::SharedPointer::shared_object() ";
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
