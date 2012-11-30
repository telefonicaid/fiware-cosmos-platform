/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) 2012 Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */

/*
 * FILE            ValueTest.cpp
 *
 * AUTHOR          Gregorio Escalada
 *
 * PROJECT         SAMSON samson_system unit test
 *
 * DATE            september 2012
 *
 * DESCRIPTION
 *
 *  Definition of unit tests on the Value class
 *
 */
#include <stdint.h>
#include <stdlib.h>

#include <string>

#include "gtest/gtest.h"

#include "samson_system/Value.h"
#include "samson_system/ValueContainer.h"

void check_serialization(samson::system::Value *value, char *line, size_t max_size) {
  size_t s = value->serialize(line);

  EXPECT_EQ(max_size, s) << au::str("Excesive size to serialize value %s ( used %lu when max is %lu) ",
                                         value->str().c_str(), s, max_size).c_str();

  samson::system::Value *value2 = new samson::system::Value();
  size_t s2 = value2->parse(line);

  EXPECT_EQ(s, s2) << au::str("Different serialization size for value %s (write %lu read %lu)",
                                   value->str().c_str(), s, s2).c_str();
  EXPECT_EQ(*value, *value2) << au::str("Different write and read value %s != %s",
                                   value->str().c_str(), value2->str().c_str()).c_str();

  delete value2;
}

TEST(samson_system_Value, basic) {
  samson::system::ValueContainer value_container;
  value_container.value->SetAsVoid();
  EXPECT_TRUE(value_container.value->IsVoid()) << "Wrong type assignment for void";
  EXPECT_FALSE(value_container.value->IsVector()) << "Wrong type detection for void";

  value_container.value->SetDouble(1.5);
  EXPECT_TRUE(value_container.value->IsNumber()) << "Wrong type detection for number";
  EXPECT_EQ(1.5, value_container.value->GetDouble()) << "Wrong value for double";
  std::string string_value = value_container.value->GetString();
  EXPECT_STREQ("1.500000", string_value.c_str()) << "Wrong value for double to string: " << string_value;

  const uint64_t kTefNumber = 34931233286;
  value_container.value->SetUint64(kTefNumber);
  EXPECT_TRUE(value_container.value->IsNumber()) << "Wrong type detection for number";
  EXPECT_EQ(kTefNumber, value_container.value->GetUint64()) << "Wrong value for uint64";

  uint64_t now = static_cast<uint64_t>(::time(NULL));
  value_container.value->SetUint64(now);
  EXPECT_TRUE(value_container.value->IsNumber()) << "Wrong type detection for number";
  EXPECT_EQ(now, value_container.value->GetUint64()) << "Wrong value for uint64";

  const char *kTestString = "test string";
  value_container.value->SetString(kTestString);
  EXPECT_TRUE(value_container.value->IsString()) << "Wrong type detection for string";
  EXPECT_STREQ(kTestString, value_container.value->GetString().c_str());

  value_container.value->SetAsVector();
  EXPECT_TRUE(value_container.value->IsVector()) << "Wrong type detection for vector";
  EXPECT_EQ(0, value_container.value->GetVectorSize());
  value_container.value->AddValueToVector()->SetString("item 1");
  value_container.value->AddValueToVector()->SetString("item 2");
  value_container.value->AddValueToVector()->SetDouble(3);
  EXPECT_TRUE(value_container.value->IsVector()) << "Wrong type detection for vector";
  EXPECT_EQ(3, value_container.value->GetVectorSize()) << "Wrong vector size";

  value_container.value->SetAsMap();
  EXPECT_TRUE(value_container.value->IsMap()) << "Wrong type detection for map";
  EXPECT_EQ(0, value_container.value->GetVectorSize());
  EXPECT_EQ(0, value_container.value->GetNumMapValues()) << "Wrong map size";
  value_container.value->SetStringForMap("key1", "item 1");
  value_container.value->SetStringForMap("key2", "item 2");
  value_container.value->SetDoubleForMap("key3", 3);
  EXPECT_TRUE(value_container.value->IsMap()) << "Wrong type detection for map";
  EXPECT_EQ(3, value_container.value->GetNumMapValues()) << "Wrong map size";
  value_container.value->SetStringForMap("key2", "item 2bis");
  EXPECT_EQ(3, value_container.value->GetNumMapValues()) << "Wrong map size";
  string_value = value_container.value->GetStringFromMap("key2");
  EXPECT_STREQ("item 2bis", string_value.c_str()) << "Wrong map insert: " << string_value;
}

TEST(DISABLED_samson_system_Value, ser_string) {
  char *line = reinterpret_cast<char *>(malloc(sizeof(*line) * 64000));

  samson::system::ValueContainer value_container;

  // Check simple serialization
  value_container.value->SetAsVoid();
  check_serialization(value_container.value, line, 1);

  value_container.value->SetString("SAMSON");
  check_serialization(value_container.value, line, 8);

  value_container.value->SetDouble(1);
  check_serialization(value_container.value, line, 6);

  value_container.value->SetDouble(1.5);
  check_serialization(value_container.value, line, 6);

  value_container.value->SetStringForMap("app", "top");
  check_serialization(value_container.value, line, 100);

  value_container.value->SetStringForMap("bones", "esternocleidomastoideo");
  check_serialization(value_container.value, line, 100);

  value_container.value->SetAsVector();
  value_container.value->AddValueToVector()->SetString("item 1");
  value_container.value->AddValueToVector()->SetString("item 2");
  value_container.value->AddValueToVector()->SetString("item 3");
  value_container.value->AddValueToVector()->SetString("item 4");
  value_container.value->AddValueToVector()->SetString("last item");
  check_serialization(value_container.value, line, 100);

  free(line);
}

TEST(DISABLED_samson_system_Value, ser_full) {
  char *line = reinterpret_cast<char *>(malloc(sizeof(*line) * 64000));

  samson::system::ValueContainer value_container;

  value_container.value->SetAsVector();
  value_container.value->AddValueToVector()->SetString("item 1");
  value_container.value->AddValueToVector()->SetString("last item");
  check_serialization(value_container.value, line, 100);

  value_container.value->SetAsVector();

  samson::system::Value *v1 = value_container.value->AddValueToVector();
  v1->SetStringForMap("map1", "value");
  v1->SetStringForMap("map2", "value2");

  std::string string_value = value_container.value->GetValueFromVector(2)->GetStringFromMap("map2");
  EXPECT_STREQ("value2", string_value.c_str());

  samson::system::Value *v2 = value_container.value->AddValueToVector(2);
  v2->SetStringForMap("map1", "value2_1");
  v2->SetStringForMap("map2", "value2_2");

  string_value = value_container.value->GetValueFromVector(2)->GetStringFromMap("map2");
  EXPECT_STREQ("value2_2", string_value.c_str());

  string_value = value_container.value->GetValueFromVector(3)->GetStringFromMap("map2");
  EXPECT_STREQ("value2", string_value.c_str());

  value_container.value->AddValueToVector()->SetString("New item");
  value_container.value->AddValueToVector()->SetDouble(-1.0);
  value_container.value->AddValueToVector()->SetDouble(12.0);
  value_container.value->AddValueToVector()->SetDouble(-2.0);
  value_container.value->AddValueToVector()->SetDouble(-25.7);

  check_serialization(value_container.value, line, 100);

  free(line);
}

TEST(samson_system_Value, str) {
  samson::system::ValueContainer value_container;

  value_container.value->SetAsVector();
  value_container.value->AddValueToVector()->SetString("item 1");
  value_container.value->AddValueToVector()->SetString("last item");

  value_container.value->SetAsVector();

  samson::system::Value *v1 = value_container.value->AddValueToVector();
  v1->SetStringForMap("map1", "value");
  v1->SetStringForMap("map2", "value2");

  samson::system::Value *v2 = value_container.value->AddValueToVector();
  v2->SetStringForMap("map1", "value");
  v2->SetStringForMap("map2", "value2");

  value_container.value->AddValueToVector()->SetString("New item");
  value_container.value->AddValueToVector()->SetDouble(-2.0);
  value_container.value->AddValueToVector()->SetDouble(-25.7);

  std::string string_value = value_container.value->str();
  const char *exp_str1 = "[\"item 1\",\"last item\",{\"map1\":\"value\",\"map2\":\"value2\"},{\"map1\":\"value\",\"map2\":\"value2\"},\"New item\",18446744073709551614,-25.7]";
  EXPECT_STREQ(exp_str1, string_value.c_str());

  std::string string_value_JSON = value_container.value->strJSON();
  const char *exp_str2 = "[\"item 1\",\"last item\",{map1:\"value\",map2:\"value2\"},{map1:\"value\",map2:\"value2\"},\"New item\",18446744073709551614,-25.7]";
  EXPECT_STREQ(exp_str2, string_value_JSON.c_str());

  std::string string_value_XML = value_container.value->strXML();
  const char *exp_str3 = "<values><![CDATA[item 1]]>\n</values>\n<values><![CDATA[last item]]>\n</values>\n<values><value first=\"map1\" second=\"\"value\"\"/>\n<value first=\"map2\" second=\"\"value2\"\"/>\n</values>\n<values><value first=\"map1\" second=\"\"value\"\"/>\n<value first=\"map2\" second=\"\"value2\"\"/>\n</values>\n<values><![CDATA[New item]]>\n</values>\n<values>18446744073709551614</values>\n<values>-25.7</values>\n";
  EXPECT_STREQ(exp_str3, string_value_XML.c_str());

  std::string string_value_HTML = value_container.value->strHTML(1);
  const char *exp_str4 = "<style>#table-5{font-family:\"Lucida Sans Unicode\", \"Lucida Grande\", Sans-Serif;font-size:12px;background:#fff;border-collapse:collapse;text-align:left;margin:20px;}#table-5 th{font-size:14px;font-weight:normal;color:#039;border-bottom:2px solid #6678b1;padding:10px 8px;}#table-5 tr{font-size:14px;font-weight:normal;color:#039;border-top:1px solid #6678b1;border-bottom:1px solid #6678b1;padding:10px 8px;}#table-5 td{ color:#669;padding:9px 8px 0;border-top:1px solid #6678b1;border-bottom:1px solid #6678b1;border-left:1px solid #6678b1;border-right:1px solid #6678b1;}#table-5 tbody tr:hover td{color:#009;}</style><table id=\"table-5\"><tr><td>1 / 7</td><td>\"item 1\"</tr></td><tr><td>2 / 7</td><td>\"last item\"</tr></td><tr><td>3 / 7</td><td><table id=\"table-5\"><tr><td>map1</td><td>\"value\"</tr></td><tr><td>map2</td><td>\"value2\"</tr></td></table></tr></td><tr><td>4 / 7</td><td><table id=\"table-5\"><tr><td>map1</td><td>\"value\"</tr></td><tr><td>map2</td><td>\"value2\"</tr></td></table></tr></td><tr><td>5 / 7</td><td>\"New item\"</tr></td><tr><td>6 / 7</td><td>18446744073709551614</tr></td><tr><td>7 / 7</td><td>-25.7</tr></td></table>";
  EXPECT_STREQ(exp_str4, string_value_HTML.c_str());

  std::string string_value_HTMLTable = value_container.value->strHTMLTable("test");
  const char *exp_str5 = "<style>#table-5{font-family:\"Lucida Sans Unicode\", \"Lucida Grande\", Sans-Serif;font-size:12px;background:#fff;border-collapse:collapse;text-align:left;margin:20px;}#table-5 th{font-size:14px;font-weight:normal;color:#039;border-bottom:2px solid #6678b1;padding:10px 8px;}#table-5 tr{font-size:14px;font-weight:normal;color:#039;border-top:1px solid #6678b1;border-bottom:1px solid #6678b1;padding:10px 8px;}#table-5 td{ color:#669;padding:9px 8px 0;border-top:1px solid #6678b1;border-bottom:1px solid #6678b1;border-left:1px solid #6678b1;border-right:1px solid #6678b1;}#table-5 tbody tr:hover td{color:#009;}</style><table id=\"table-5\">\n<caption>test</caption>\n<tr>\n<th colspan=1>values</th>\n</tr>\n<tr>\n<th>values</th>\n</tr>\n<tr>\n<td>\"item 1\"</td>\n</tr>\n</table>\n";
  EXPECT_STREQ(exp_str5, string_value_HTMLTable.c_str()) << "Error printing Value: "
      << string_value_HTMLTable << " vector: " << value_container.value->str();
}

TEST(samson_system_Value, copy) {
  samson::system::ValueContainer value_container;

  value_container.value->SetAsVector();
  value_container.value->AddValueToVector()->SetString("item 1");
  value_container.value->AddValueToVector()->SetString("last item");

  samson::system::Value *v1 = value_container.value->AddValueToVector();
  v1->SetStringForMap("map1", "value");
  v1->SetStringForMap("map2", "value2");

  samson::system::Value *v2 = value_container.value->AddValueToVector();
  v2->SetStringForMap("map1", "value");
  v2->SetStringForMap("map2", "value2");

  value_container.value->AddValueToVector()->SetString("New item");
  value_container.value->AddValueToVector()->SetDouble(-2.0);
  value_container.value->AddValueToVector()->SetDouble(-25.7);

  samson::system::ValueContainer value_container_copy;

  value_container_copy.value->copyFrom(value_container.value);

  EXPECT_EQ(7, value_container.value->GetVectorSize());
  value_container_copy.value->PopBackFromVector();
  EXPECT_EQ(6, value_container_copy.value->GetVectorSize());
  std::string string_value = value_container_copy.value->GetValueFromVector(4)->GetString();
  EXPECT_STREQ("New item", string_value.c_str()) << "Error copying Value: "
      << string_value << " vector: " << value_container_copy.value->str();
}

TEST(samson_system_Value, hash) {
  samson::system::ValueContainer value_container;

  value_container.value->SetAsVoid();
  int hash = value_container.value->hash(65536);
  EXPECT_EQ(0, hash) << "Error hash for void: " << hash;

  value_container.value->SetUint64(34931233286);
  hash = value_container.value->hash(65536);
  EXPECT_EQ(20998, hash) << "Error hash for Uint64: " << hash;

  value_container.value->SetUint64(34931233287);
  hash = value_container.value->hash(65536);
  EXPECT_EQ(20999, hash) << "Error hash for second Uint64: " << hash;

  value_container.value->SetDouble(42.0);
  hash = value_container.value->hash(65536);
  EXPECT_EQ(42, hash) << "Error hash for Double: " << hash;

  value_container.value->SetString("SAMSON unit test");
  hash = value_container.value->hash(65536);
  EXPECT_EQ(46930, hash) << "Error hash for String: " << hash;

  value_container.value->SetAsVector();
  value_container.value->AddValueToVector()->SetUint64(34931233286);
  value_container.value->AddValueToVector()->SetDouble(42.0);
  value_container.value->AddValueToVector()->SetString("SAMSON unit test");
  hash = value_container.value->hash(65536);
  EXPECT_EQ(20998, hash) << "Error hash for Vector: " << hash;

  value_container.value->SetAsMap();
  value_container.value->SetDoubleForMap("a", 1.0);
  value_container.value->SetDoubleForMap("b", 2.0);
  hash = value_container.value->hash(65536);
  EXPECT_EQ(45713, hash) << "Error hash for Map: " << hash;

  samson::system::Value *p_value = value_container.value->GetValueFromMap("a");
  EXPECT_EQ(2.0, (++(*p_value)).GetDouble(0.0)) << "Error in prefix increment";
  EXPECT_EQ(2.0, ((*p_value)++).GetDouble(0.0)) << "Error in postfix increment";
  double double_value = value_container.value->GetDoubleFromMap("a");
  EXPECT_EQ(3.0, double_value) << "Error in postfix increment: " << double_value;
  hash = p_value->hash(65536);
  EXPECT_EQ(3.0, hash) << "Error hash for Map: " << hash;
}

TEST(samson_system_Value, sort) {
  samson::system::ValueContainer value_container;
  value_container.value->SetAsVector();
  value_container.value->AddValueToVector()->SetDouble(3.0);
  value_container.value->AddValueToVector()->SetDouble(9.0);
  value_container.value->AddValueToVector()->SetDouble(1.0);
  value_container.value->AddValueToVector()->SetDouble(7.0);
  EXPECT_EQ(4, value_container.value->GetVectorSize());
  value_container.value->SortVectorInAscendingOrder();
  EXPECT_EQ(4, value_container.value->GetVectorSize());
  double double_value = value_container.value->GetValueFromVector(1)->GetDouble();
  EXPECT_EQ(3.0, double_value) << "Error sorting doubles, second element: " << double_value
      << " vector: " << value_container.value->str();
  value_container.value->SortVectorInDescendingOrder();
  EXPECT_EQ(4, value_container.value->GetVectorSize());
  double_value = value_container.value->GetValueFromVector(1)->GetDouble();
  EXPECT_EQ(7.0, double_value) << "Error sorting doubles, second element: " << double_value
      << " vector: " << value_container.value->str();

  value_container.value->SetAsVoid();
  value_container.value->SetAsVector();
  value_container.value->AddValueToVector()->SetString("b");
  value_container.value->AddValueToVector()->SetString("d");
  value_container.value->AddValueToVector()->SetString("c");
  value_container.value->AddValueToVector()->SetString("a");
  EXPECT_EQ(4, value_container.value->GetVectorSize());
  value_container.value->SortVectorInAscendingOrder();
  EXPECT_EQ(4, value_container.value->GetVectorSize());
  std::string string_value = value_container.value->GetValueFromVector(1)->GetString();
  EXPECT_STREQ("b", string_value.c_str()) << "Error sorting strings, second element: "
      << string_value << " vector: " << value_container.value->str();
  value_container.value->SortVectorInDescendingOrder();
  EXPECT_EQ(4, value_container.value->GetVectorSize());
  string_value = value_container.value->GetValueFromVector(1)->GetString();
  EXPECT_STREQ("c", string_value.c_str()) << "Error sorting strings, second element: "
      << string_value << " vector: " << value_container.value->str();

  value_container.value->SetAsVoid();
  value_container.value->SetAsVector();
  samson::system::Value *v1 = value_container.value->AddValueToVector();
  v1->SetDoubleForMap("age", 20);
  v1->SetStringForMap("name", "Ken");
  v1 = value_container.value->AddValueToVector();
  v1->SetDoubleForMap("age", 18);
  v1->SetStringForMap("name", "Grant");
  v1 = value_container.value->AddValueToVector();
  v1->SetDoubleForMap("age", 19);
  v1->SetStringForMap("name", "Andreu");
  v1 = value_container.value->AddValueToVector();
  v1->SetDoubleForMap("age", 21);
  v1->SetStringForMap("name", "Goyo");
  EXPECT_EQ(4, value_container.value->GetVectorSize());
  value_container.value->SortVectorOfMapsInAscendingOrder("age");
  EXPECT_EQ(4, value_container.value->GetVectorSize());
  string_value = value_container.value->GetValueFromVector(1)->GetStringFromMap("name");
  EXPECT_STREQ("Andreu", string_value.c_str()) << "Error sorting strings, second element: "
      << string_value << " vector: " << value_container.value->str();
  value_container.value->SortVectorOfMapsInAscendingOrder("name");
  string_value = value_container.value->GetValueFromVector(1)->GetStringFromMap("name");
  EXPECT_STREQ("Goyo", string_value.c_str()) << "Error sorting strings, second element: "
      << string_value << " vector: " << value_container.value->str();
  value_container.value->SortVectorOfMapsInDescendingOrder("age");
  string_value = value_container.value->GetValueFromVector(1)->GetStringFromMap("name");
  EXPECT_STREQ("Ken", string_value.c_str()) << "Error sorting strings, second element: "
      << string_value << " vector: " << value_container.value->str();
  value_container.value->SortVectorOfMapsInDescendingOrder("name");
  string_value = value_container.value->GetValueFromVector(1)->GetStringFromMap("name");
  EXPECT_STREQ("Grant", string_value.c_str()) << "Error sorting strings, second element: "
      << string_value << " vector: " << value_container.value->str();

  value_container.value->SetAsVoid();
  value_container.value->SetAsVector();
  value_container.value->AddValueToVector()->SetDouble(3.0);
  value_container.value->AddValueToVector()->SetDouble(9.0);
  value_container.value->AddValueToVector()->SetDouble(1.0);
  value_container.value->AddValueToVector()->SetDouble(21.0);
  value_container.value->AddValueToVector()->SetDouble(121.0);
  value_container.value->AddValueToVector()->SetDouble(7.0);
  EXPECT_EQ(6, value_container.value->GetVectorSize());
  value_container.value->PartialSortVectorInAscendingOrder(2);
  EXPECT_EQ(6, value_container.value->GetVectorSize());
  double_value = value_container.value->GetValueFromVector(1)->GetDouble();
  EXPECT_EQ(3.0, double_value) << "Error sorting doubles, second element: " << double_value
      << " vector: " << value_container.value->str();
  value_container.value->PartialSortVectorInDescendingOrder(2);
  EXPECT_EQ(6, value_container.value->GetVectorSize());
  double_value = value_container.value->GetValueFromVector(1)->GetDouble();
  EXPECT_EQ(21.0, double_value) << "Error sorting doubles, second element: " << double_value
      << " vector: " << value_container.value->str();

  value_container.value->SetAsVoid();
  value_container.value->SetAsVector();
  value_container.value->AddValueToVector()->SetString("b");
  value_container.value->AddValueToVector()->SetString("d");
  value_container.value->AddValueToVector()->SetString("c");
  value_container.value->AddValueToVector()->SetString("a");
  value_container.value->AddValueToVector()->SetString("e");
  EXPECT_EQ(5, value_container.value->GetVectorSize());
  value_container.value->PartialSortVectorInAscendingOrder(2);
  EXPECT_EQ(5, value_container.value->GetVectorSize());
  string_value = value_container.value->GetValueFromVector(1)->GetString();
  EXPECT_STREQ("b", string_value.c_str()) << "Error sorting strings, second element: "
      << string_value << " vector: " << value_container.value->str();
  value_container.value->PartialSortVectorInDescendingOrder(2);
  EXPECT_EQ(5, value_container.value->GetVectorSize());
  string_value = value_container.value->GetValueFromVector(1)->GetString();
  EXPECT_STREQ("d", string_value.c_str()) << "Error sorting strings, second element: "
      << string_value << " vector: " << value_container.value->str();

  value_container.value->SetAsVoid();
  value_container.value->SetAsVector();
  v1 = value_container.value->AddValueToVector();
  v1->SetDoubleForMap("age", 20);
  v1->SetStringForMap("name", "Ken");
  v1 = value_container.value->AddValueToVector();
  v1->SetDoubleForMap("age", 18);
  v1->SetStringForMap("name", "Grant");
  v1 = value_container.value->AddValueToVector();
  v1->SetDoubleForMap("age", 19);
  v1->SetStringForMap("name", "Andreu");
  v1 = value_container.value->AddValueToVector();
  v1->SetDoubleForMap("age", 21);
  v1->SetStringForMap("name", "Goyo");
  EXPECT_EQ(4, value_container.value->GetVectorSize());
  value_container.value->PartialSortVectorOfMapsInAscendingOrder("age", 2);
  EXPECT_EQ(4, value_container.value->GetVectorSize());
  string_value = value_container.value->GetValueFromVector(1)->GetStringFromMap("name");
  EXPECT_STREQ("Andreu", string_value.c_str()) << "Error sorting strings, second element: "
      << string_value << " vector: " << value_container.value->str();
  value_container.value->PartialSortVectorOfMapsInAscendingOrder("name", 2);
  string_value = value_container.value->GetValueFromVector(1)->GetStringFromMap("name");
  EXPECT_STREQ("Goyo", string_value.c_str()) << "Error sorting strings, second element: "
      << string_value << " vector: " << value_container.value->str();
  value_container.value->PartialSortVectorOfMapsInDescendingOrder("age", 2);
  string_value = value_container.value->GetValueFromVector(1)->GetStringFromMap("name");
  EXPECT_STREQ("Ken", string_value.c_str()) << "Error sorting strings, second element: "
      << string_value << " vector: " << value_container.value->str();
  value_container.value->PartialSortVectorOfMapsInDescendingOrder("name", 2);
  string_value = value_container.value->GetValueFromVector(1)->GetStringFromMap("name");
  EXPECT_STREQ("Grant", string_value.c_str()) << "Error sorting strings, second element: "
      << string_value << " vector: " << value_container.value->str();
}
