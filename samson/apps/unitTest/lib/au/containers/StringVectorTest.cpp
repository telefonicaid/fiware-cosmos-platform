#include "au/containers/StringVector.h"
#include "gtest/gtest.h"

TEST(au_containers_StringVector, Constructors) {
  au::StringVector a1("a");

  EXPECT_EQ(a1.size(), (size_t)1) << "Error in size of string vector";
  au::StringVector a2("a", "b");
  EXPECT_EQ(a2.size(), (size_t)2) << "Error in size of string vector";
  au::StringVector a3("a", "b", "c");
  EXPECT_EQ(a3.size(), (size_t)3) << "Error in size of string vector";
  au::StringVector a4("a", "b", "c", "d");
  EXPECT_EQ(a4.size(), (size_t)4) << "Error in size of string vector";
  au::StringVector a5("a", "b", "c", "d", "e");
  EXPECT_EQ(a5.size(), (size_t)5) << "Error in size of string vector";
  au::StringVector a6("a", "b", "c", "d", "e", "f");
  EXPECT_EQ(a6.size(), (size_t)6) << "Error in size of string vector";
  au::StringVector a7("a", "b", "c", "d", "e", "f", "g");
  EXPECT_EQ(a7.size(), (size_t)7) << "Error in size of string vector";

  au::StringVector aa6(a6);
  EXPECT_EQ(aa6.size(), (size_t)6) << "Error in size of string vector";
}

TEST(au_containers_StringVector, Assignation) {
  au::StringVector a5("a", "b", "c", "d", "e");
  au::StringVector aa5 = a5;

  EXPECT_EQ(aa5.size(), (size_t)5) << "Error in size of string vector";

  std::set<std::string> set;
  set.insert("hola");
  set.insert("hola");
  set.insert("adios");
  au::StringVector tmp2(set);
  EXPECT_EQ((size_t)2,tmp2.size()) << "Error in size of string vector";
  au::StringVector a1("a");
  tmp2.Append(a1);
  EXPECT_EQ(tmp2.size(), (size_t)3) << "Error in size of string vector";
}

TEST(au_containers_StringVector, Operations) {
  std::vector<std::string> v;
  v.push_back("hola");
  v.push_back("hola");
  v.push_back("adios");
  au::StringVector tmp(v);
  EXPECT_EQ(tmp.size(), (size_t)3) << "Error in size of string vector";
  tmp.RemoveDuplicated();  // Remove duplicated values
  EXPECT_EQ(tmp.size(), (size_t)2) << "Error in size of string vector";

  au::StringVector components =  au::StringVector::ParseFromString(
    "Hola colega de turno");
  EXPECT_EQ(components.size(), (size_t)4) << "Error in size of string vector";
  EXPECT_EQ(components.Get(2), "de") << "Error in au::StringVector::Get";

  au::StringVector t;
  t.Push(1);
  t.Push(1.2);
  t.Push("ja");
  EXPECT_EQ(t.str(), "1 1.2 ja");
}
