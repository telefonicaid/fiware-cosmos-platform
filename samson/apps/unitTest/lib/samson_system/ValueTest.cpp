#include <stdint.h>
#include <stdlib.h>


#include "gtest/gtest.h"

#include "samson_system/Value.h"
#include "samson_system/ValueContainer.h"

void check_serialization(samson::system::Value *value, char *line, size_t max_size) {
  size_t s = value->serialize(line);

  EXPECT_TRUE(s <= max_size) << au::str("Excesive size to serialize value %s ( used %lu when max is %lu) ",
                                         value->str().c_str(), s, max_size).c_str();

  samson::system::Value *value2 = samson::system::Value::getInstance();
  size_t s2 = value2->parse(line);

  EXPECT_TRUE(s == s2) << au::str("Different serialization size for value %s (write %lu read %lu)",
                                   value->str().c_str(), s, s2).c_str();
  EXPECT_TRUE(*value == *value2) << au::str("Different write and read value %s != %s",
                                   value->str().c_str(), value2->str().c_str()).c_str();

  samson::system::Value::reuseInstance(value2);
}

TEST(samson_system_Value, basic) {
  samson::system::ValueContainer value_container;
  value_container.value->SetAsVoid();
  EXPECT_TRUE(value_container.value->IsVoid()) << "Wrong type assignment for void";
  EXPECT_FALSE(value_container.value->IsVector()) << "Wrong type detection for void";

  value_container.value->SetDouble(1.5);
  EXPECT_TRUE(value_container.value->IsNumber()) << "Wrong type detection for number";
  EXPECT_TRUE(value_container.value->GetDouble() == 1.5) << "Wrong value for double";
  std::string string_value = value_container.value->GetString();
  EXPECT_TRUE(strcmp(string_value.c_str(), "1.500000") == 0) << "Wrong value for double to string: " << string_value;

  const uint64_t kTefNumber = 34931233286;
  value_container.value->SetUint64(kTefNumber);
  EXPECT_TRUE(value_container.value->IsNumber()) << "Wrong type detection for number";
  EXPECT_TRUE(value_container.value->GetUint64() == kTefNumber) << "Wrong value for uint64";

  uint64_t now = static_cast<uint64_t>(::time(NULL));
  value_container.value->SetUint64(now);
  EXPECT_TRUE(value_container.value->IsNumber()) << "Wrong type detection for number";
  EXPECT_TRUE(value_container.value->GetUint64() == now) << "Wrong value for uint64";

  const std::string kTestString = "test string";
  value_container.value->SetString(kTestString.c_str());
  EXPECT_TRUE(value_container.value->IsString()) << "Wrong type detection for string";
  EXPECT_TRUE(value_container.value->GetString() == kTestString) << "Wrong value for string";

  value_container.value->SetAsVector();
  EXPECT_TRUE(value_container.value->IsVector()) << "Wrong type detection for vector";
  EXPECT_TRUE(value_container.value->GetVectorSize() == 0);
  value_container.value->AddValueToVector()->SetString("item 1");
  value_container.value->AddValueToVector()->SetString("item 2");
  value_container.value->AddValueToVector()->SetDouble(3);
  EXPECT_TRUE(value_container.value->IsVector()) << "Wrong type detection for vector";
  EXPECT_TRUE(value_container.value->GetVectorSize() == 3) << "Wrong vector size";

  value_container.value->SetAsMap();
  EXPECT_TRUE(value_container.value->IsMap()) << "Wrong type detection for map";
  EXPECT_TRUE(value_container.value->GetVectorSize() == 0);
  EXPECT_TRUE(value_container.value->GetNumMapValues() == 0) << "Wrong map size";
  value_container.value->SetStringForMap("key1", "item 1");
  value_container.value->SetStringForMap("key2", "item 2");
  value_container.value->SetDoubleForMap("key3", 3);
  EXPECT_TRUE(value_container.value->IsMap()) << "Wrong type detection for map";
  EXPECT_TRUE(value_container.value->GetNumMapValues() == 3) << "Wrong map size";
  value_container.value->SetStringForMap("key2", "item 2bis");
  EXPECT_TRUE(value_container.value->GetNumMapValues() == 3) << "Wrong map size";
  string_value = value_container.value->GetStringFromMap("key2");
  EXPECT_TRUE(strcmp(string_value.c_str(), "item 2bis") == 0) << "Wrong map insert: " << string_value;
}

TEST(samson_system_Value, ser_string) {
  char *line = (char *) malloc(sizeof(char) * 64000);

  samson::system::ValueContainer value_container;

  // Check simple serialization
  value_container.value->SetString("SAMSON");
  check_serialization(value_container.value, line, 8);

  value_container.value->SetDouble(1);
  check_serialization(value_container.value, line, 6);

  value_container.value->SetDouble(1.5);
  check_serialization(value_container.value, line, 6);

  value_container.value->SetStringForMap("app", "top");
  check_serialization(value_container.value, line, 100);

  value_container.value->SetAsVector();
  value_container.value->AddValueToVector()->SetString("item 1");
  value_container.value->AddValueToVector()->SetString("item 2");
  value_container.value->AddValueToVector()->SetString("last item");
  check_serialization(value_container.value, line, 100);

  free(line);
}

TEST(samson_system_Value, ser_vector) {
  char *line = (char *) malloc(sizeof(char) * 64000);

  samson::system::ValueContainer value_container;

  value_container.value->SetAsVector();
  value_container.value->AddValueToVector()->SetString("item 1");
  value_container.value->AddValueToVector()->SetString("item 2");
  value_container.value->AddValueToVector()->SetString("last item");
  check_serialization(value_container.value, line, 100);

  value_container.value->SetAsVector();

  samson::system::Value *v1 = value_container.value->AddValueToVector();
  v1->SetStringForMap("map1", "value");
  v1->SetStringForMap("map2", "value2");

  samson::system::Value *v2 = value_container.value->AddValueToVector();
  v2->SetStringForMap("map1", "value");
  v2->SetStringForMap("map2", "value2");

  value_container.value->AddValueToVector()->SetString("New item");

  check_serialization(value_container.value, line, 100);

  free(line);
}

TEST(samson_system_Value, ser_full) {
  char *line = (char *) malloc(sizeof(char) * 64000);

  samson::system::ValueContainer value_container;

  value_container.value->SetAsVector();

  samson::system::Value *v1 = value_container.value->AddValueToVector();
  v1->SetStringForMap("map1", "value");
  v1->SetStringForMap("map2", "value2");

  samson::system::Value *v2 = value_container.value->AddValueToVector();
  v2->SetStringForMap("map1", "value");
  v2->SetStringForMap("map2", "value2");

  value_container.value->AddValueToVector()->SetString("New item");

  check_serialization(value_container.value, line, 100);

  free(line);
}

TEST(samson_system_Value, sort) {

  samson::system::ValueContainer value_container;
  value_container.value->SetAsVector();
  value_container.value->AddValueToVector()->SetDouble(3.0);
  value_container.value->AddValueToVector()->SetDouble(9.0);
  value_container.value->AddValueToVector()->SetDouble(1.0);
  value_container.value->AddValueToVector()->SetDouble(7.0);
  EXPECT_TRUE(value_container.value->GetVectorSize() == 4);
  value_container.value->SortVectorInAscendingOrder();
  EXPECT_TRUE(value_container.value->GetVectorSize() == 4);
  double double_value = value_container.value->GetValueFromVector(1)->GetDouble();
  EXPECT_TRUE(double_value == 3.0) << "Error sorting doubles, second element: " << double_value
      << " vector: " << value_container.value->str();
  value_container.value->SortVectorInDescendingOrder();
  EXPECT_TRUE(value_container.value->GetVectorSize() == 4);
  double_value = value_container.value->GetValueFromVector(1)->GetDouble();
  EXPECT_TRUE(double_value == 7.0) << "Error sorting doubles, second element: " << double_value
      << " vector: " << value_container.value->str();

  value_container.value->SetAsVoid();
  value_container.value->SetAsVector();
  value_container.value->AddValueToVector()->SetString("b");
  value_container.value->AddValueToVector()->SetString("d");
  value_container.value->AddValueToVector()->SetString("c");
  value_container.value->AddValueToVector()->SetString("a");
  EXPECT_TRUE(value_container.value->GetVectorSize() == 4);
  value_container.value->SortVectorInAscendingOrder();
  EXPECT_TRUE(value_container.value->GetVectorSize() == 4);
  std::string string_value = value_container.value->GetValueFromVector(1)->GetString();
  EXPECT_TRUE(strcmp(string_value.c_str(), "b") == 0) << "Error sorting strings, second element: "
      << string_value << " vector: " << value_container.value->str();
  value_container.value->SortVectorInDescendingOrder();
  EXPECT_TRUE(value_container.value->GetVectorSize() == 4);
  string_value = value_container.value->GetValueFromVector(1)->GetString();
  EXPECT_TRUE(strcmp(string_value.c_str(), "c") == 0) << "Error sorting strings, second element: "
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
  EXPECT_TRUE(value_container.value->GetVectorSize() == 4);
  value_container.value->SortVectorOfMapsInAscendingOrder("age");
  EXPECT_TRUE(value_container.value->GetVectorSize() == 4);
  string_value = value_container.value->GetValueFromVector(1)->GetStringFromMap("name");
  EXPECT_TRUE(strcmp(string_value.c_str(), "Andreu") == 0) << "Error sorting strings, second element: "
      << string_value << " vector: " << value_container.value->str();
  value_container.value->SortVectorOfMapsInAscendingOrder("name");
  string_value = value_container.value->GetValueFromVector(1)->GetStringFromMap("name");
  EXPECT_TRUE(strcmp(string_value.c_str(), "Goyo") == 0) << "Error sorting strings, second element: "
      << string_value << " vector: " << value_container.value->str();
  value_container.value->SortVectorOfMapsInDescendingOrder("age");
  string_value = value_container.value->GetValueFromVector(1)->GetStringFromMap("name");
  EXPECT_TRUE(strcmp(string_value.c_str(), "Ken") == 0) << "Error sorting strings, second element: "
      << string_value << " vector: " << value_container.value->str();
  value_container.value->SortVectorOfMapsInDescendingOrder("name");
  string_value = value_container.value->GetValueFromVector(1)->GetStringFromMap("name");
  EXPECT_TRUE(strcmp(string_value.c_str(), "Grant") == 0) << "Error sorting strings, second element: "
      << string_value << " vector: " << value_container.value->str();

  value_container.value->SetAsVoid();
  value_container.value->SetAsVector();
  value_container.value->AddValueToVector()->SetDouble(3.0);
  value_container.value->AddValueToVector()->SetDouble(9.0);
  value_container.value->AddValueToVector()->SetDouble(1.0);
  value_container.value->AddValueToVector()->SetDouble(7.0);
  EXPECT_TRUE(value_container.value->GetVectorSize() == 4);
  value_container.value->PartialSortVectorInAscendingOrder(2);
  EXPECT_TRUE(value_container.value->GetVectorSize() == 4);
  double_value = value_container.value->GetValueFromVector(1)->GetDouble();
  EXPECT_TRUE(double_value == 3.0) << "Error sorting doubles, second element: " << double_value
      << " vector: " << value_container.value->str();
  value_container.value->PartialSortVectorInDescendingOrder(2);
  EXPECT_TRUE(value_container.value->GetVectorSize() == 4);
  double_value = value_container.value->GetValueFromVector(1)->GetDouble();
  EXPECT_TRUE(double_value == 7.0) << "Error sorting doubles, second element: " << double_value
      << " vector: " << value_container.value->str();

  value_container.value->SetAsVoid();
  value_container.value->SetAsVector();
  value_container.value->AddValueToVector()->SetString("b");
  value_container.value->AddValueToVector()->SetString("d");
  value_container.value->AddValueToVector()->SetString("c");
  value_container.value->AddValueToVector()->SetString("a");
  EXPECT_TRUE(value_container.value->GetVectorSize() == 4);
  value_container.value->PartialSortVectorInAscendingOrder(2);
  EXPECT_TRUE(value_container.value->GetVectorSize() == 4);
  string_value = value_container.value->GetValueFromVector(1)->GetString();
  EXPECT_TRUE(strcmp(string_value.c_str(), "b") == 0) << "Error sorting strings, second element: "
      << string_value << " vector: " << value_container.value->str();
  value_container.value->PartialSortVectorInDescendingOrder(2);
  EXPECT_TRUE(value_container.value->GetVectorSize() == 4);
  string_value = value_container.value->GetValueFromVector(1)->GetString();
  EXPECT_TRUE(strcmp(string_value.c_str(), "c") == 0) << "Error sorting strings, second element: "
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
  EXPECT_TRUE(value_container.value->GetVectorSize() == 4);
  value_container.value->PartialSortVectorOfMapsInAscendingOrder("age", 2);
  EXPECT_TRUE(value_container.value->GetVectorSize() == 4);
  string_value = value_container.value->GetValueFromVector(1)->GetStringFromMap("name");
  EXPECT_TRUE(strcmp(string_value.c_str(), "Andreu") == 0) << "Error sorting strings, second element: "
      << string_value << " vector: " << value_container.value->str();
  value_container.value->PartialSortVectorOfMapsInAscendingOrder("name", 2);
  string_value = value_container.value->GetValueFromVector(1)->GetStringFromMap("name");
  EXPECT_TRUE(strcmp(string_value.c_str(), "Goyo") == 0) << "Error sorting strings, second element: "
      << string_value << " vector: " << value_container.value->str();
  value_container.value->PartialSortVectorOfMapsInDescendingOrder("age", 2);
  string_value = value_container.value->GetValueFromVector(1)->GetStringFromMap("name");
  EXPECT_TRUE(strcmp(string_value.c_str(), "Ken") == 0) << "Error sorting strings, second element: "
      << string_value << " vector: " << value_container.value->str();
  value_container.value->PartialSortVectorOfMapsInDescendingOrder("name", 2);
  string_value = value_container.value->GetValueFromVector(1)->GetStringFromMap("name");
  EXPECT_TRUE(strcmp(string_value.c_str(), "Grant") == 0) << "Error sorting strings, second element: "
      << string_value << " vector: " << value_container.value->str();

}
