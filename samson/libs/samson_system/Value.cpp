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
 * FILE            Value.cpp
 *
 * AUTHOR          Andreu Urruela
 *
 * PROJECT         SAMSON samson_system library
 *
 * DATE            2012
 *
 * DESCRIPTION
 *
 *  Definition of Value class methods to create a flexible data type that
 *  can be used as a number, a string, a vector of Values or a map of Values
 *
 */

#include "samson_system/Value.h"  // Own interface

#include <string.h>

#include <algorithm>

#include "samson_system/smaz.h"

#include "samson_system/MapCompareFunctor.h"

namespace samson {
namespace system {

const std::string Value::kSystemValueName("system.Value");
const size_t Value::kValueCode = 1219561887489248771ULL;

bool IsValidDouble(const char *data) {
  bool found_sign = false;
  bool found_dot = false;
  bool found_digit = false;

  size_t pos = 0;

  while (true) {
    int c = data[pos];
    ++pos;

    if (c == '\0') {
      return true;
    }

    // Dot
    if (c == '.') {
      if (found_dot)
        return false;
      found_dot = true;
      continue;
    }

    // Signs
    if ((c == '-') || (c == '+')) {
      if (found_dot) {
        return false;  // sign after dot
      }
      if (found_sign) {
        return false;  // Double sign
      }
      if (found_digit) {
        return false;  // Sign after digits
      }
      found_sign = true;
      continue;
    }

    if (isdigit(c)) {
      found_digit = true;
      continue;
    }

    return false;
  }

  // Impossible condition to silent valgrind
  // In any case, LM_X will exit the program
  LM_X(1, ("Internal error"));
  return true;
}

// --------------------------------------------------------
// Constant word serialization stuff
// --------------------------------------------------------

// Preferred names for fields
const std::string Value::kAppField("app");
const std::string Value::kCategoriesField("categories");
const std::string Value::kCategoryField("category");
const std::string Value::kCellIdField("cellId");
const std::string Value::kConceptField("concept");
const std::string Value::kCounterField("counter");
const std::string Value::kCountField("count");
const std::string Value::kDomainField("domain");
const std::string Value::kGlobalCountField("global_count");
const std::string Value::kHitsField("hits");
const std::string Value::kImeiField("imei");
const std::string Value::kImsiField("imsi");
const std::string Value::kInstantProfileField("instant_profile");
const std::string Value::kItemField("item");
const std::string Value::kLanguageField("language");
const std::string Value::kLinkField("link");
const std::string Value::kMemberField("member");
const std::string Value::kNameField("name");
const std::string Value::kQueryField("query");
const std::string Value::kQueryWordsField("query_words");
const std::string Value::kServiceField("service");
const std::string Value::kTimestampField("timestamp");
const std::string Value::kTopCategoryField("top.category");
const std::string Value::kTotalField("total");
const std::string Value::kUpdatedCountField("updated_count");
const std::string Value::kUrlField("url");
const std::string Value::kUserField("user");
const std::string Value::kUserIdField("userId");
const std::string Value::kUsrField("usr");
const std::string Value::kValueField("value");
const std::string Value::kVectorProfileField("vector_profile");
const std::string Value::kWeightField("weight");
const std::string Value::kWordField("word");

// Constant sorted alphabetically for fast search
const char *kConstantWords[] = { Value::kAppField.c_str(),
                                 Value::kCategoryField.c_str(),
                                 Value::kCategoriesField.c_str(),
                                 Value::kCellIdField.c_str(),
                                 Value::kConceptField.c_str(),
                                 Value::kCounterField.c_str(),
                                 Value::kCountField.c_str(),
                                 Value::kDomainField.c_str(),
                                 Value::kGlobalCountField.c_str(),
                                 Value::kHitsField.c_str(),
                                 Value::kImeiField.c_str(),
                                 Value::kImsiField.c_str(),
                                 Value::kInstantProfileField.c_str(),
                                 Value::kItemField.c_str(),
                                 Value::kLanguageField.c_str(),
                                 Value::kLinkField.c_str(),
                                 Value::kMemberField.c_str(),
                                 Value::kNameField.c_str(),
                                 Value::kQueryWordsField.c_str(),
                                 Value::kQueryField.c_str(),
                                 Value::kServiceField.c_str(),
                                 Value::kTimestampField.c_str(),
                                 Value::kTopCategoryField.c_str(),
                                 Value::kTotalField.c_str(),
                                 Value::kUpdatedCountField.c_str(),
                                 Value::kUrlField.c_str(),
                                 Value::kUserField.c_str(),
                                 Value::kUserIdField.c_str(),
                                 Value::kUsrField.c_str(),
                                 Value::kValueField.c_str(),
                                 Value::kVectorProfileField.c_str(),
                                 Value::kWeightField.c_str(),
                                 Value::kWordField.c_str() };

int InternalGetConstantWordCode(const char *word, int min, int max) {
  if (max <= (min + 1)) {
    return -1;
  }

  int mid = (min + max) / 2;

  int c = strcmp(word, kConstantWords[mid]);
  if (c == 0) {
    return mid;
  } else if (c > 0) {
    return InternalGetConstantWordCode(word, mid, max);
  } else {
    return InternalGetConstantWordCode(word, min, mid);
  }
}

// Find if this word is a constant word ( serialized with 2 bytes )
// Return -1 if not found
int GetConstantWordCode(const char *word) {
  int max = sizeof(kConstantWords) / sizeof(word);

  if (strcmp(word, kConstantWords[0]) == 0) {
    return 0;
  }
  if (strcmp(word, kConstantWords[max - 1]) == 0) {
    return max - 1;
  }
  return InternalGetConstantWordCode(word, 0, max - 1);
}

const char *GetConstantWord(int c) {
  if ((c < 0) || (c > 255)) {
    return "Unknown";
  }
  return kConstantWords[c];
}

// Parse operations
// ------------------------------------------------------

inline int Value::ParseVoid(const char *data) {
  ChangeValueType(value_void);
  return 1;  // Void is always serialized in 1 byte
}

inline int Value::ParseNumber(const char *data) {
  SerializationCode code = static_cast<SerializationCode> (data[0]);

  // Common init to value int
  ChangeValueType(value_number);

  // Variables used only in some "cases", but cleaner declaring them here
  size_t tmp = 0;
  int total = 0;
  switch (code) {
    // Constant values
    case ser_int_value_0:
      value_double_ = 0;
      return 1;  // Codified in the serialization code
    case ser_int_value_1:
      value_double_ = 1;
      return 1;  // Codified in the serialization code
    case ser_int_value_2:
      value_double_ = 2;
      return 1;  // Codified in the serialization code
    case ser_int_value_3:
      value_double_ = 3;
      return 1;  // Codified in the serialization code
    case ser_int_value_4:
      value_double_ = 4;
      return 1;  // Codified in the serialization code
    case ser_int_value_5:
      value_double_ = 5;
      return 1;  // Codified in the serialization code
    case ser_int_value_6:
      value_double_ = 6;
      return 1;  // Codified in the serialization code
    case ser_int_value_7:
      value_double_ = 7;
      return 1;  // Codified in the serialization code
    case ser_int_value_8:
      value_double_ = 8;
      return 1;  // Codified in the serialization code
      break;
    case ser_int_value_9:
      value_double_ = 9;
      return 1;  // Codified in the serialization code
    case ser_int_value_10:
      value_double_ = 10;
      return 1;  // Codified in the serialization code
    case ser_int_value_minus_1:
      value_double_ = -1;
      return 1;  // Codified in the serialization code
    case ser_int_positive:
      total = 1 + samson::staticVarIntParse(data + 1, &tmp);
      value_double_ = static_cast<double> (tmp);
      return total;
    case ser_int_negative:
      total = 1 + samson::staticVarIntParse(data + 1, &tmp);
      // The sign change must be done outside the static cast, otherwise error happens
      value_double_ = -static_cast<double> (tmp);
      return total;
    case ser_double_positive_1_decimal:
      total = 1 + samson::staticVarIntParse(data + 1, &tmp);
      value_double_ = static_cast<double> (tmp) / 10.0;
      return total;
    case ser_double_positive_2_decimal:
      total = 1 + samson::staticVarIntParse(data + 1, &tmp);
      value_double_ = static_cast<double> (tmp) / 100.0;
      return total;
    case ser_double_positive_3_decimal:
      total = 1 + samson::staticVarIntParse(data + 1, &tmp);
      value_double_ = static_cast<double> (tmp) / 1000.0;
      return total;

    case ser_double_positive_4_decimal:
      total = 1 + samson::staticVarIntParse(data + 1, &tmp);
      value_double_ = static_cast<double> (tmp) / 10000.0;
      return total;
      break;
    case ser_double_positive_5_decimal:
      total = 1 + samson::staticVarIntParse(data + 1, &tmp);
      value_double_ = static_cast<double> (tmp) / 100000.0;
      return total;
      break;
    case ser_double_negative_1_decimal:
      total = 1 + samson::staticVarIntParse(data + 1, &tmp);
      // The sign change must be done outside the static cast, otherwise error happens
      value_double_ = -1.0 * static_cast<double> (tmp) / 10.0;
      return total;
    case ser_double_negative_2_decimal:
      total = 1 + samson::staticVarIntParse(data + 1, &tmp);
      value_double_ = -1.0 * static_cast<double> (tmp) / 100.0;
      return total;
    case ser_double_negative_3_decimal:
      total = 1 + samson::staticVarIntParse(data + 1, &tmp);
      value_double_ = -1.0 * static_cast<double> (tmp) / 1000.0;
      return total;
    case ser_double_negative_4_decimal:
      total = 1 + samson::staticVarIntParse(data + 1, &tmp);
      value_double_ = -1.0 * static_cast<double> (tmp) / 10000.0;
      return total;
    case ser_double_negative_5_decimal:
      size_t tmp;
      total = 1 + samson::staticVarIntParse(data + 1, &tmp);
      value_double_ = -1.0 * static_cast<double> (tmp) / 100000.0;
      return total;
    case ser_double:
      value_double_ = *(reinterpret_cast<const double *> (data + 1));
      return 1 + sizeof(value_double_);
    default:
      LM_E(("Internal error, unknown number serialization_code:%d", static_cast<int>(code)));
      return 0;
  }
  // Impossible condition to silent valgrind
  // In any case, LM_X will exit the program
  LM_X(1, ("Internal error"));
  return 0;
}

inline int Value::ParseString(const char *data) {
  // printf("Parsing string %p\n" , data);

  SerializationCode code = static_cast<SerializationCode> (data[0]);

  // Common init to value string
  ChangeValueType(value_string);

  switch (code) {
    case ser_string:
      value_string_ = &data[1];
      return 1 + value_string_.length() + 1;   // Serialization code, string, '\0'

    case ser_string_constant:
      value_string_ = GetConstantWord(static_cast<unsigned char> (data[1]));
      return 2;   // Always length 2 ( serialization char and word index )

    case ser_string_smaz: {
      // Create a compilation unit to avoid problems with the local variables
      const int kMaxLengthLine = 8192;
      char line[kMaxLengthLine];

      // We skip the serialization char
      size_t offset = 1;

      size_t len_in = 0;
      offset += samson::staticVarIntParse(data + offset, &len_in);

      int len = smaz_decompress(data + offset, len_in, line, kMaxLengthLine);

      // printf("Parsing compress string length %lu // Original string length %d\n" , len_in , len );

      line[len] = '\0';
      value_string_ = line;

      offset += len_in;
      return offset;
    }

    default:
      LM_X(1, ("Internal error, unknown string serialization_code:%d", static_cast<int>(code)));
  }
  // Impossible condition to silent valgrind
  // In any case, LM_X will exit the program
  LM_X(1, ("Internal error"));
  return 0;
}

inline int Value::ParseVector(const char *data) {
  SerializationCode code = static_cast<SerializationCode> (data[0]);

  // We want to start with an empty Value object
  // (We have tried to made a clear() always in ChangeValueType(),
  // but it is used too widely to be safe)
  SetAsVoid();

  // Common init to value vector
  SetAsVector();

  // Skip serialization code
  size_t offset = 1;

  // Length of the vector ( decoded in different ways )
  size_t length = 0;

  switch (code) {
    case ser_vector_len_0:
      length = 0;
      break;
    case ser_vector_len_1:
      length = 1;
      break;
    case ser_vector_len_2:
      length = 2;
      break;
    case ser_vector_len_3:
      length = 3;
      break;
    case ser_vector_len_4:
      length = 4;
      break;
    case ser_vector_len_5:
      length = 5;
      break;
    case ser_vector:
      // Recover the number of elements we have serialized
      offset += samson::staticVarIntParse(data + offset, &length);
      break;
    default:
      LM_X(1, ("Internal error, unknown vector serialization_code:%d", static_cast<int>(code)));
  }

  // Parse all components of the vector
  for (size_t i = 0; i < length; ++i) {
    Value *new_value = AddValueToVector();
    offset += new_value->parse(const_cast<char *> (data + offset));
  }
  return offset;
}

inline int Value::ParseMap(const char *data) {
  SerializationCode code = static_cast<SerializationCode> (data[0]);

  // We want to start with an empty Value object
  // (We have tried to made a clear() always in ChangeValueType(),
  // but it is used too widely to be safe)
  // SetAsVoid(); is not safe neither for maps

  // Common init to value map
  SetAsMap();

  // Skip serialization code
  size_t offset = 1;

  // Length of the vector ( decoded in different ways )
  size_t length = 0;

  switch (code) {
    case ser_map_len_0:
      length = 0;
      break;
    case ser_map_len_1:
      length = 1;
      break;
    case ser_map_len_2:
      length = 2;
      break;
    case ser_map_len_3:
      length = 3;
      break;
    case ser_map_len_4:
      length = 4;
      break;
    case ser_map_len_5:
      length = 5;
      break;
    case ser_map:
      // Recover the number of elements we have serialized
      offset += samson::staticVarIntParse(data + offset, &length);
      break;
    default:
      LM_X(1, ("Internal error, unknown map serialization_code:%d", static_cast<int>(code)));
  }

  // Parse all components of the vector
  Value tmp_key;   // Component to parse keys...
  for (size_t i = 0; i < length; ++i) {
    // Parse key
    // We have to const_cast because parse() does not receive const (yet)
    offset += tmp_key.parse(const_cast<char *> (data + offset));

    Value *new_value = AddValueToMap(tmp_key.GetString());
    offset += new_value->parse(const_cast<char *> (data + offset));
    // LM_M(("ParseMap component for first:'%s' value:'%s', offset:%d", tmp_key.str().c_str(), new_value->str().c_str(), offset));
  }
  // LM_M(("ParseMap for value:'%s', offset:%d", this->str().c_str(), offset));
  return offset;
}

// TODO: To be updated to const char * when DataInstance be updated
int Value::parse(char *data) {
  clear();
  // Get serialization code
  SerializationCode code = static_cast<SerializationCode> (data[0]);

  switch (code) {
    case ser_void:
      return ParseVoid(data);

    case ser_int_positive:
    case ser_int_negative:
    case ser_int_value_0:
    case ser_int_value_1:
    case ser_int_value_2:
    case ser_int_value_3:
    case ser_int_value_4:
    case ser_int_value_5:
    case ser_int_value_6:
    case ser_int_value_7:
    case ser_int_value_8:
    case ser_int_value_9:
    case ser_int_value_10:
    case ser_int_value_minus_1:
    case ser_double:
    case ser_double_positive_1_decimal:
    case ser_double_positive_2_decimal:
    case ser_double_positive_3_decimal:
    case ser_double_positive_4_decimal:
    case ser_double_positive_5_decimal:
    case ser_double_negative_1_decimal:
    case ser_double_negative_2_decimal:
    case ser_double_negative_3_decimal:
    case ser_double_negative_4_decimal:
    case ser_double_negative_5_decimal:
      return ParseNumber(data);

    case ser_string:
    case ser_string_constant:
    case ser_string_smaz:
      return ParseString(data);

    case ser_vector:
    case ser_vector_len_0:
    case ser_vector_len_1:
    case ser_vector_len_2:
    case ser_vector_len_3:
    case ser_vector_len_4:
    case ser_vector_len_5:
      return ParseVector(data);

    case ser_map:
    case ser_map_len_0:
    case ser_map_len_1:
    case ser_map_len_2:
    case ser_map_len_3:
    case ser_map_len_4:
    case ser_map_len_5:
      return ParseMap(data);
    default:
      LM_X(1, ("Internal error, unknown serialization_code:%d", static_cast<int>(code)));
  }

  // Impossible condition to silent valgrind
  // In any case, LM_X will exit the program
  LM_X(1, ("Internal error"));
  return 0;
}

// ------------------------------------------------------------
// SERIALIZE Operations
// ------------------------------------------------------------

int Value::SerializeVoid(char *data) {
  data[0] = static_cast<char> (ser_void);
  return 1;
}

int Value::SerializeNumber(char *data) {
  if (value_double_ == 0) {
    data[0] = static_cast<char> (ser_int_value_0);
    return 1;   // Codified in 1 byte
  }
  if (value_double_ == 1) {
    data[0] = static_cast<char> (ser_int_value_1);
    return 1;   // Codified in 1 byte
  }
  if (value_double_ == 2) {
    data[0] = static_cast<char> (ser_int_value_2);
    return 1;   // Codified in 1 byte
  }
  if (value_double_ == 3) {
    data[0] = static_cast<char> (ser_int_value_3);
    return 1;   // Codified in 1 byte
  }
  if (value_double_ == 4) {
    data[0] = static_cast<char> (ser_int_value_4);
    return 1;   // Codified in 1 byte
  }
  if (value_double_ == 5) {
    data[0] = static_cast<char> (ser_int_value_5);
    return 1;   // Codified in 1 byte
  }
  if (value_double_ == 6) {
    data[0] = static_cast<char> (ser_int_value_6);
    return 1;   // Codified in 1 byte
  }
  if (value_double_ == 7) {
    data[0] = static_cast<char> (ser_int_value_7);
    return 1;   // Codified in 1 byte
  }
  if (value_double_ == 8) {
    data[0] = static_cast<char> (ser_int_value_8);
    return 1;   // Codified in 1 byte
  }
  if (value_double_ == 9) {
    data[0] = static_cast<char> (ser_int_value_9);
    return 1;   // Codified in 1 byte
  }
  if (value_double_ == 10) {
    data[0] = static_cast<char> (ser_int_value_10);
    return 1;   // Codified in 1 byte
  }

  if (value_double_ == -1) {
    data[0] = static_cast<char> (ser_int_value_minus_1);
    return 1;   // Codified in 1 byte
  }

  // Generic variable length codification
  // It was made with a pair of castings, but apparently it is better to use modf()
  // ... and with static_cast, it would be impossible to be read

  // Integer numbers...
  double int_part;
  if (modf(value_double_, &int_part) == 0.0) {
    if (int_part >= 0) {
      data[0] = static_cast<char> (ser_int_positive);
      return 1 + samson::staticVarIntSerialize(data + 1, static_cast<size_t>(int_part));
    } else {
      data[0] = static_cast<char> (ser_int_negative);
      return 1 + samson::staticVarIntSerialize(data + 1, static_cast<size_t>(-int_part));
    }
  }

  // Limited number of decimals
  if (modf((10.0 * value_double_), &int_part) == 0.0) {
    if (int_part >= 0) {
      data[0] = static_cast<char> (ser_double_positive_1_decimal);
      return 1 + samson::staticVarIntSerialize(data + 1, static_cast<size_t>(int_part));
    } else {
      data[0] = static_cast<char> (ser_double_negative_1_decimal);
      return 1 + samson::staticVarIntSerialize(data + 1, static_cast<size_t>(-int_part));
    }
  }

  if (modf((100.0 * value_double_), &int_part) == 0.0) {
    if (int_part >= 0) {
      data[0] = static_cast<char> (ser_double_positive_2_decimal);
      return 1 + samson::staticVarIntSerialize(data + 1, static_cast<size_t>(int_part));
    } else {
      data[0] = static_cast<char> (ser_double_negative_2_decimal);
      return 1 + samson::staticVarIntSerialize(data + 1, static_cast<size_t>(-int_part));
    }
  }

  if (modf((1000.0 * value_double_), &int_part) == 0.0) {
    if (int_part >= 0) {
      data[0] = static_cast<char> (ser_double_positive_3_decimal);
      return 1 + samson::staticVarIntSerialize(data + 1, static_cast<size_t>(int_part));
    } else {
      data[0] = static_cast<char> (ser_double_negative_3_decimal);
      return 1 + samson::staticVarIntSerialize(data + 1, static_cast<size_t>(-int_part));
    }
  }

  if (modf((10000.0 * value_double_), &int_part) == 0.0) {
    if (int_part >= 0) {
      data[0] = static_cast<char> (ser_double_positive_4_decimal);
      return 1 + samson::staticVarIntSerialize(data + 1, static_cast<size_t>(int_part));
    } else {
      data[0] = static_cast<char> (ser_double_negative_4_decimal);
      return 1 + samson::staticVarIntSerialize(data + 1, static_cast<size_t>(-int_part));
    }
  }

  if (modf((100000.0 * value_double_), &int_part) == 0.0) {
    if (int_part >= 0) {
      data[0] = static_cast<char> (ser_double_positive_5_decimal);
      return 1 + samson::staticVarIntSerialize(data + 1, static_cast<size_t>(int_part));
    } else {
      data[0] = static_cast<char> (ser_double_negative_5_decimal);
      return 1 + samson::staticVarIntSerialize(data + 1, static_cast<size_t>(-int_part));
    }
  }

  // Generic double codification
  data[0] = static_cast<char> (ser_double);
  *(reinterpret_cast<double *> (data + 1)) = value_double_;
  return 1 + sizeof(value_double_);
}

int Value::SerializeString(char *data) {
  // If constant word, preferred method
  int index = GetConstantWordCode(value_string_.c_str());

  if (index != -1) {
    data[0] = static_cast<char> (ser_string_constant);
    data[1] = static_cast<unsigned char> (index);
    return 2;   // Always 2 length serialization ( serialization code + index word )
  }

  if (value_string_.length() < 4096) {
    // Try compressed version
    const int kMaxLengthLine = 8192;
    char line[kMaxLengthLine];
    size_t len = ::smaz_compress(const_cast<char *>(value_string_.c_str()), value_string_.length(), line, kMaxLengthLine);

    if (len < value_string_.length()) {
      // Serialize using compression
      size_t offset = 0;

      data[offset] = static_cast<char> (ser_string_smaz);
      ++offset;

      offset += samson::staticVarIntSerialize(data + offset, len);

      memcpy(data + offset, line, len);
      offset += len;

      // printf("Serialized string length %lu // Sent len %lu" , value_string_.length() , len  );

      return offset;
    }
  }

  // Default serialization of string
  data[0] = static_cast<char> (ser_string);
  strcpy(data + 1, value_string_.c_str());
  int total = 1 + (value_string_.length() + 1);   // Serialization code - string - '\0'
  return total;
}

int Value::SerializeVector(char *data) {
  size_t offset = 1;

  if (value_vector_.size() == 0) {
    data[0] = static_cast<char> (ser_vector_len_0);
  } else if (value_vector_.size() == 1) {
    data[0] = static_cast<char> (ser_vector_len_1);
  } else if (value_vector_.size() == 2) {
    data[0] = static_cast<char> (ser_vector_len_2);
  } else if (value_vector_.size() == 3) {
    data[0] = static_cast<char> (ser_vector_len_3);
  } else if (value_vector_.size() == 4) {
    data[0] = static_cast<char> (ser_vector_len_4);
  } else if (value_vector_.size() == 5) {
    data[0] = static_cast<char> (ser_vector_len_5);
  } else {
    data[0] = static_cast<char> (ser_vector);
    offset += samson::staticVarIntSerialize(data + offset, value_vector_.size());
  }

  // Serialize all the components
  for (size_t i = 0; i < value_vector_.size(); ++i) {
    Value *value = value_vector_[i];
    offset += value->serialize(data + offset);
  }
  return offset;
}

int Value::SerializeMap(char *data) {
  size_t offset = 1;

  if (value_map_.size() == 0) {
    data[0] = static_cast<char> (ser_map_len_0);
  } else if (value_map_.size() == 1) {
    data[0] = static_cast<char> (ser_map_len_1);
  } else if (value_map_.size() == 2) {
    data[0] = static_cast<char> (ser_map_len_2);
  } else if (value_map_.size() == 3) {
    data[0] = static_cast<char> (ser_map_len_3);
  } else if (value_map_.size() == 4) {
    data[0] = static_cast<char> (ser_map_len_4);
  } else if (value_map_.size() == 5) {
    data[0] = static_cast<char> (ser_map_len_5);
  } else {
    data[0] = static_cast<char> (ser_map);
    offset += samson::staticVarIntSerialize(data + offset, value_map_.size());
  }

  // Serialize all the components
  Value tmp_key;   // Value to serialize key
  au::map<std::string, Value>::iterator it;
  for (it = value_map_.begin(); it != value_map_.end(); ++it) {
    tmp_key.SetString(it->first);
    Value *value = it->second;
    offset += tmp_key.serialize(data + offset);
    offset += value->serialize(data + offset);
    // LM_M(("SerializeMap component for first:'%s' value:'%s', offset:%d", tmp_key.str().c_str(), value->str().c_str(), offset));
  }
  // LM_M(("SerializeMap for value:'%s', offset:%d", this->str().c_str(), offset));
  return offset;
}

int Value::serialize(char *data) {
  // printf("Serialize %s\n" , str().c_str() );

  switch (value_type_) {
    case value_void:
      return SerializeVoid(data);
    case value_number:
      return SerializeNumber(data);
    case value_string:
      return SerializeString(data);
    case value_vector:
      return SerializeVector(data);
    case value_map:
      return SerializeMap(data);
    default:
      LM_X(1, ("Internal error, unknown value_type:%d", static_cast<int>(value_type_)));
  }

  // Impossible condition to silent valgrind
  // In any case, LM_X will exit the program
  LM_X(1, ("Internal error"));
  return 0;
}

int Value::HashVoid(int max_num_partitions) {
  return 0;
}

int Value::HashNumber(int max_num_partitions) {
  if (value_double_ >= 0) {
    return (static_cast<size_t> (value_double_)) % max_num_partitions;
  } else {
    return (static_cast<size_t> (-value_double_)) % max_num_partitions;
  }
}

int Value::HashString(int max_num_partitions) {
  static const size_t kInitialFNV = 2166136261U;
  static const size_t kFNVMultiple = 16777619;

  size_t hash = kInitialFNV;

  for (size_t i = 0; i < value_string_.length(); ++i) {
    hash = hash ^ (value_string_[i]);
    hash = hash * kFNVMultiple;
  }
  return hash % max_num_partitions;
}

int Value::HashVector(int max_num_partitions) {
  if (value_vector_.size() == 0) {
    return 0;
  }
  return value_vector_[0]->hash(max_num_partitions);
}

int Value::HashMap(int max_num_partitions) {
  const int kNoPartitions = 2147483647;   // 2^31 - 1
  size_t accumulated_hash = 0;

  au::map<std::string, Value>::iterator it;
  for (it = value_map_.begin(); it != value_map_.end(); ++it) {
    accumulated_hash ^= (au::HashString(it->first, kNoPartitions) * it->second->hash(kNoPartitions));
  }
  return accumulated_hash % max_num_partitions;
}

int Value::hash(int max_num_partitions) {
  switch (value_type_) {
    case value_void:
      return HashVoid(max_num_partitions);
    case value_number:
      return HashNumber(max_num_partitions);
    case value_string:
      return HashString(max_num_partitions);
    case value_vector:
      return HashVector(max_num_partitions);
    case value_map:
      return HashMap(max_num_partitions);
    default:
      LM_E(("Internal error, unknown value_type:%d", static_cast<int>(value_type_)));
  }
  return 0;
}

int Value::size(char *data) {
  // Simple but crappy implementation...
  Value value;
  return value.parse(data);
}

int Value::compare(char *data1, char *data2, size_t *offset1, size_t *offset2) {
  Value tmp_value1;
  Value tmp_value2;
  size_t size1 = tmp_value1.parse(data1 + *offset1);
  size_t size2 = tmp_value2.parse(data2 + *offset2);

  *offset1 = *offset1 + size1;
  *offset1 = *offset1 + size2;

  return tmp_value1.compare(tmp_value2);
}

int Value::compare(char *data1, char *data2) {
  size_t offset1 = 0;
  size_t offset2 = 0;

  return compare(data1, data2, &offset1, &offset2);
}

int Value::serial_compare(char *data1, char *data2) {
  return compare(data1, data2);
}

// ------------------------------------------------------------
// Checks
// ------------------------------------------------------------

const char *Value::GetTypeStatic() {
  return (Value::kSystemValueName.c_str());
}

const char *Value::getType() {
  return (Value::kSystemValueName.c_str());
}

bool Value::CheckTypeStatic(const char *type) {
  if (strcmp(type, Value::kSystemValueName.c_str()) == 0) {
    return true;
  }
  return false;
}

bool Value::checkType(const char *type) {
  if (strcmp(type, Value::kSystemValueName.c_str()) == 0) {
    return true;
  }
  return false;
}

size_t Value::GetHashTypeStatic() {
  return (Value::kValueCode);
}

size_t Value::getHashType() {
  return (Value::kValueCode);
}

bool Value::CheckHashTypeStatic(size_t valType) {
  if (valType == Value::kValueCode) {
    return true;
  }
  return false;
}

bool Value::checkHashType(size_t valType) {
  if (valType == Value::kValueCode) {
    return true;
  }
  return false;
}

// Operations

bool Value::operator==(const Value &other) const {
  return compare(other) == 0;
}

bool Value::operator!=(const Value &other) const {
  return compare(other) != 0;
}

bool Value::operator<=(const Value &other) const {
  return compare(other) <= 0;
}

bool Value::operator>=(const Value &other) const {
  return compare(other) >= 0;
}

bool Value::operator<(const Value &other) const {
  return compare(other) < 0;
}

bool Value::operator>(const Value &other) const {
  return compare(other) > 0;
}

const Value Value::operator+(Value &other) {   // const & for other not possible because of GetString()
  Value result = *this;   // Make a copy of myself.

  switch (value_type_) {
    case value_number:
      result.SetDouble(GetDouble() + other.GetDouble());
      break;

    case value_string:
      result.SetString(GetString() + other.GetString());
      break;

    case value_void:
    case value_vector:
    case value_map:
      result.SetAsVoid();
      break;
  }
  return result;   // All done!
}

const Value Value::operator-(const Value &other) const {
  Value result = *this;   // Make a copy of myself.

  switch (value_type_) {
    case value_number:
      result.SetDouble(GetDouble() - other.GetDouble());
      break;

    case value_string:
    case value_void:
    case value_vector:
    case value_map:
      result.SetAsVoid();
      break;
  }

  return result;   // All done!
}

const Value Value::operator*(const Value &other) const {
  Value result = *this;   // Make a copy of myself.

  switch (value_type_) {
    case value_number:
      result.SetDouble(GetDouble() * other.GetDouble());
      break;

    case value_string:
    case value_void:
    case value_vector:
    case value_map:
      result.SetAsVoid();
      break;
  }

  return result;   // All done!
}

const Value Value::operator/(const Value &other) const {
  Value result = *this;   // Make a copy of myself.

  switch (value_type_) {
    case value_number:
      result.SetDouble(GetDouble() / other.GetDouble());
      break;

    case value_string:
    case value_void:
    case value_vector:
    case value_map:
      result.SetAsVoid();
      break;
  }

  return result;   // All done!
}

inline int Value::compare(const Value& other) const {
  if (value_type_ != other.value_type_) {
    return value_type_ - other.value_type_;
  }

  // Same type
  switch (value_type_) {
    case value_void:
      return 0;

    case value_number:
      if (value_double_ > other.value_double_) {
        return 1;
      } else if (value_double_ < other.value_double_) {
        return -1;
      } else {
        return 0;
      }

    case value_string:
      if (value_string_ > other.value_string_) {
        return 1;
      } else if (value_string_ < other.value_string_) {
        return -1;
      } else {
        return 0;
      }

    case value_vector:
      if (value_vector_.size() != other.value_vector_.size()) {
        if (value_vector_.size() > other.value_vector_.size()) {
          return 1;
        } else {
          return -1;
        }
      }

      for (size_t i = 0; i < value_vector_.size(); ++i) {
        int c = value_vector_[i]->compare(*other.value_vector_[i]);
        if (c != 0) {
          return c;
        }
      }
      return 0;   // all the elements are the same.

    case value_map: {
      // Create a compilation unit to avoid problems with the local variables

      if (value_map_.size() != other.value_map_.size()) {
        if (value_map_.size() > other.value_map_.size()) {
          return 1;
        } else {
          return -1;
        }
      }

      au::map<std::string, Value>::const_iterator it = value_map_.begin();
      au::map<std::string, Value>::const_iterator it2 = other.value_map_.begin();

      while (it != value_map_.end()) {
        // compare the first key
        std::string k = it->first;
        std::string k2 = it2->first;
        if (k < k2) {
          return -1;
        }
        if (k > k2) {
          return 1;
        }

        // Compare both values
        int c = it->second->compare(*it2->second);
        if (c != 0) {
          return c;
        }

        ++it;
        ++it2;
      }
      return 0;   // all the elements are the same.
    }

    default:
      LM_X(1, ("Internal error, unknown value_type:%d", static_cast<int>(value_type_)));
  }

  // Impossible condition to silent valgrind
  // In any case, LM_X will exit the program
  LM_X(1, ("Internal error"));
  return 0;
}

void Value::SetFrom(struct json_object *json) {
  json_type type = json_object_get_type(json);

  switch (type) {
    case json_type_boolean:
      SetDouble(1);
      break;
    case json_type_string:
      SetString(json_object_get_string(json));
      break;
    case json_type_double:
      SetDouble(json_object_get_double(json));
      break;
    case json_type_int:
      SetDouble(json_object_get_int(json));
      break;
    case json_type_object: {
      // Create a compilation unit to avoid problems with the local variables

      SetAsMap();
      struct lh_table *table = json_object_get_object(json);
      lh_entry *entry = table->head;
      while (entry) {
        // Add entry from map
        char *key = static_cast<char *> (entry->k);
        struct json_object *json2 = json_object_object_get(json, key);
        AddValueToMap(key)->SetFrom(json2);

        // Go to the next one
        entry = entry->next;
      }
      break;
    }
    case json_type_array:
      SetAsVector();
      for (int i = 0; i < json_object_array_length(json); ++i) {
        struct json_object *json2 = json_object_array_get_idx(json, i);
        AddValueToVector()->SetFrom(json2);
      }
      break;
    case json_type_null:
      SetAsVoid();
      break;
    default:
      LM_E(("Internal error, unknown json_type:%d", static_cast<int>(type)));
  }
}

void Value::setFromString(const char *_data) {
  // BNy default json format is used
  SetFromJSONString(_data);
}

void Value::SetFromJSONString(const char *_data) {
  struct json_object *json = json_tokener_parse(_data);

  if (is_error(json)) {
    SetAsVoid();
    return;
  }

  if (json) {
    SetFrom(json);
    json_object_put(json);
  } else {
    SetAsVoid();
  }
}

void Value::copyFrom(Value *other) {
  // Change the type to the other value type
  ChangeValueType(other->value_type_);

  switch (value_type_) {
    case value_void:
      break;
    case value_number:
      value_double_ = other->value_double_;
      break;
    case value_string:
      value_string_ = other->value_string_;
      break;
    case value_vector:
      SetAsVector();
      for (size_t i = 0; i < other->value_vector_.size(); ++i) {
        AddValueToVector()->copyFrom(other->value_vector_[i]);
      }
      break;
    case value_map: {
      // Create a compilation unit to avoid problems with the local variables
      SetAsMap();
      au::map<std::string, Value>::iterator it;
      for (it = other->value_map_.begin(); it != other->value_map_.end(); ++it) {
        AddValueToMap(it->first)->copyFrom(it->second);
      }
      break;
    }
    default:
      LM_X(1, ("Internal error, unknown value_type:%d", static_cast<int>(value_type_)));
  }
}

std::string Value::str() {
  std::ostringstream output;

  switch (value_type_) {
    case value_void:
      return "<void>";

    case value_number:
      double intpart;
      if (modf(value_double_, &intpart) == 0.0) {
        output << static_cast<size_t> (intpart);
      } else {
        output << value_double_;
      }
      return output.str();

    case value_string:
      return "\"" + value_string_ + "\"";

    case value_vector:
      output << "[";
      for (size_t i = 0; i < value_vector_.size(); ++i) {
        output << value_vector_[i]->str();
        if (i != (value_vector_.size() - 1)) {
          output << ",";
        }
      }
      output << "]";
      return output.str();

    case value_map: {
      // Create a compilation unit to avoid problems with the local variables
      output << "{";
      au::map<std::string, Value>::iterator it;
      for (it = value_map_.begin(); it != value_map_.end();) {
        output << "\"" << it->first << "\"" << ":";
        output << it->second->str();
        ++it;
        if (it != value_map_.end()) {
          output << ",";
        }
      }
      output << "}";
      return output.str();
    }

    default:
      LM_E(("Internal error, unknown value_type:%d", static_cast<int>(value_type_)));
  }

  // Impossible condition to silent valgrind
  // In any case, LM_X will exit the program
  LM_E(("Internal error"));
  return "unknown";
}

std::string Value::strJSON() {
  std::ostringstream output;

  _strJSON(output);
  return output.str();
}

void Value::_strJSON(std::ostream &output) {
  switch (value_type_) {
    case value_number:
      double intpart;
      if (modf(value_double_, &intpart) == 0.0) {
        output << static_cast<size_t> (intpart);
      } else {
        output << value_double_;
      }
      break;
    case value_string:
      output << "\"" << value_string_ << "\"";
      break;
    case value_vector:
      output << "[";
      for (size_t i = 0; i < value_vector_.size();) {
        value_vector_[i]->_strJSON(output);
        ++i;
        if (i != value_vector_.size()) {
          output << ",";
        }
      }
      output << "]";
      break;
    case value_map: {
      // Create a compilation unit to avoid problems with the local variables
      output << "{";
      au::map<std::string, Value>::iterator it;
      for (it = value_map_.begin(); it != value_map_.end(); ++it) {
        if (it != value_map_.begin()) {
          output << ",";
        }
        output << it->first << ":";
        it->second->_strJSON(output);
      }
      output << "}";
      break;
    }
    case value_void:
      output << "null";
      break;
    default:
      LM_X(1, ("Internal error, unknown value_type:%d", static_cast<int>(value_type_)));
  }
}

std::string Value::strXML() {
  std::ostringstream output;

  _strXML(output);
  return output.str();
}

void Value::_strXML(std::ostream &output) {
  switch (value_type_) {
    case value_void:
      output << "null";
      break;
    case value_number:
      double intpart;
      if (modf(value_double_, &intpart) == 0.0) {
        output << static_cast<size_t> (intpart);
      } else {
        output << value_double_;
      }
      break;
    case value_string:
      output << "<![CDATA[" << value_string_ << "]]>" << "\n";
      break;
    case value_vector:
      for (unsigned int i = 0; i < value_vector_.size(); ++i) {
        output << "<values>" << value_vector_[i]->strXML() << "</values>\n";
      }
      break;
    case value_map: {
      // Create a compilation unit to avoid problems with the local variables
      au::map<std::string, Value>::iterator it;
      for (it = value_map_.begin(); it != value_map_.end(); ++it) {
        output << "<value first=\"" << it->first << "\" second=\"" << it->second->str() << "\"/>\n";
      }
      break;
    }
    default:
      output << "Error, unsupported " << Value::kSystemValueName << " type";
      break;
  }
}
void Value::_strHTML(int level_html_heading, std::ostream &output) {
  switch (value_type_) {
    case value_number:
      double intpart;
      if (modf(value_double_, &intpart) == 0.0) {
        output << static_cast<size_t> (intpart);
      } else {
        output << value_double_;
      }
      break;
    case value_string:
      output << "\"" << value_string_ << "\"";
      break;
    case value_vector:
      output << "<table id=\"table-5\">";
      for (size_t i = 0; i < value_vector_.size(); ++i) {
        output << "<tr><td>";
        output << (i + 1) << " / " << value_vector_.size();
        output << "</td><td>";

        value_vector_[i]->_strHTML(level_html_heading + 1, output);
        output << "</tr></td>";
      }
      output << "</table>";
      break;
    case value_map: {
      // Create a compilation unit to avoid problems with the local variables
      output << "<table id=\"table-5\">";
      au::map<std::string, Value>::iterator it;
      for (it = value_map_.begin(); it != value_map_.end(); ++it) {
        output << "<tr><td>";
        output << it->first;
        output << "</td><td>";
        it->second->_strHTML(level_html_heading + 1, output);
        output << "</tr></td>";
      }
      output << "</table>";
      break;
    }
    case value_void:
      output << "null";
      break;
    default:
      LM_X(1, ("Internal error, unknown value_type:%d", static_cast<int>(value_type_)));
  }
}

std::string Value::strHTML(int level_html_heading) {
  std::ostringstream output;

  output << "<style>";
  output
      << "#table-5{font-family:\"Lucida Sans Unicode\", \"Lucida Grande\", Sans-Serif;font-size:12px;background:#fff;border-collapse:collapse;text-align:left;margin:20px;}";
  output
      << "#table-5 th{font-size:14px;font-weight:normal;color:#039;border-bottom:2px solid #6678b1;padding:10px 8px;}";
  output
      << "#table-5 tr{font-size:14px;font-weight:normal;color:#039;border-top:1px solid #6678b1;border-bottom:1px solid #6678b1;padding:10px 8px;}";
  output
      << "#table-5 td{ color:#669;padding:9px 8px 0;border-top:1px solid #6678b1;border-bottom:1px solid #6678b1;border-left:1px solid #6678b1;border-right:1px solid #6678b1;}";
  output << "#table-5 tbody tr:hover td{color:#009;}";
  output << "</style>";

  _strHTML(level_html_heading, output);
  return output.str();
}

std::string Value::strHTMLTable(std::string _varNameInternal) {
  std::ostringstream output;

  output << "<style>";
  output
      << "#table-5{font-family:\"Lucida Sans Unicode\", \"Lucida Grande\", Sans-Serif;font-size:12px;background:#fff;border-collapse:collapse;text-align:left;margin:20px;}";
  output
      << "#table-5 th{font-size:14px;font-weight:normal;color:#039;border-bottom:2px solid #6678b1;padding:10px 8px;}";
  output
      << "#table-5 tr{font-size:14px;font-weight:normal;color:#039;border-top:1px solid #6678b1;border-bottom:1px solid #6678b1;padding:10px 8px;}";
  output
      << "#table-5 td{ color:#669;padding:9px 8px 0;border-top:1px solid #6678b1;border-bottom:1px solid #6678b1;border-left:1px solid #6678b1;border-right:1px solid #6678b1;}";
  output << "#table-5 tbody tr:hover td{color:#009;}";
  output << "</style>";

  _strHTMLTable(_varNameInternal, output);
  return output.str();
}

void Value::_strHTMLTable(const std::string& _varNameInternal, std::ostream &output) {
  switch (value_type_) {
    case value_number:
    case value_void:
    case value_string:
      output << "<table id=\"table-5\">\n";
      output << "<caption>" << _varNameInternal << "</caption>\n";
      output << "<tr>\n";
      output << "<th>" << _varNameInternal << "</th>\n";
      output << "</tr>\n";
      output << "<tr>\n";
      output << "<th>" << getName() << "</th>\n";
      output << "</tr>\n";
      output << "<td>" << str() << "</td>\n";
      output << "</tr>\n";
      output << "</table>\n";
      output << value_double_;
      break;
    case value_vector: {
      // Create a compilation unit to avoid problems with the local variables

      output << "<table id=\"table-5\">\n";
      output << "<caption>" << _varNameInternal << "</caption>\n";
      int init_col = 0;
      int end_col = 0;
      output << "<tr>\n";
      {   // paint_header  vector values
        if (value_vector_.size() > 0) {
          end_col = init_col + value_vector_[0]->num_basic_fields();
          output << "<th colspan=" << end_col << ">values</th>\n";
          init_col = end_col + 1;
        } else {
          output << "<th colspan=" << end_col << "></th>\n";
        }
      }
      output << "</tr>\n";
      output << "<tr>\n";
      {   // paint_header_basic  vector values
        if (value_vector_.size() > 0) {
          if (value_vector_[0]->is_terminal()) {
            output << "<th>values</th>\n";
          } else {
            output << value_vector_[0]->paint_header_basic(init_col);
          }
        } else {
          output << "<th></th>\n";
        }
      }
      output << "</tr>\n";
      int m_num_values = max_num_values();
      for (int index_row = 0; (index_row < m_num_values); ++index_row) {
        output << "<tr>\n";
        {   // paint_value of vector values
          if ((index_row >= 0) && (index_row < static_cast<int> (value_vector_.size()))) {
            output << value_vector_[index_row]->paint_value(0) << "\n";
          } else {
            if (value_vector_.size() > 0) {
              output << value_vector_[0]->paint_value(-1) << "\n";
            } else {
              output << "<td></td>\n";
            }
          }
        }
        output << "</tr>\n";
      }
      output << "</table>\n";
      break;
    }

    case value_map: {
      // Create a compilation unit to avoid problems with the local variables
      output << "<table id=\"table-5\">\n";
      output << "<caption>" << _varNameInternal << "</caption>\n";
      int level_html_heading = 1;

      output << "<table id=\"table-5\">";
      au::map<std::string, Value>::iterator it;
      for (it = value_map_.begin(); it != value_map_.end(); ++it) {
        output << "<tr><td>";
        output << it->first;
        output << "</td><td>";
        it->second->_strHTML(level_html_heading + 1, output);
        output << "</tr></td>";
      }
      output << "</table>";
      break;
    }
    default:
      output << output << "Error, unsupported " << Value::kSystemValueName << " type";
  }
}

std::string Value::paint_header(int init_col) {
  return "Term";
}

std::string Value::paint_header_basic(int init_col) {
  return "Term";
}

std::string Value::paint_value(int index_row) {
  std::ostringstream o;

  if (index_row >= 0) {
    o << "<td>" << str() << "</td>";
  } else {
    o << "<td></td>";
  }
  return o.str();
}

int Value::num_fields() {
  return 1;
}

int Value::num_basic_fields() {
  return 1;
}

int Value::max_depth() {
  return 1;
}

int Value::max_num_values() {
  return 1;
}

bool Value::is_terminal() {
  return true;
}


Value::ValueType Value::GetValueType() const {
  return value_type_;
}
void Value::SetAsVoid() {
  ChangeValueType(value_void);
}

bool Value::IsVoid() const {
  return (value_type_ == value_void);
}

// ----------------------------------------------------------------------------------------
// number functions
// ----------------------------------------------------------------------------------------

bool Value::IsNumber() const {
  return (value_type_ == value_number);
}

// ----------------------------------------------------------------------------------------
// Vector functions
// ----------------------------------------------------------------------------------------

bool Value::IsVector() const {
  return (value_type_ == value_vector);
}

void Value::clear() {
  // Clear elements in the map
  au::map<std::string, Value>::iterator it;
  for (it = value_map_.begin(); it != value_map_.end(); ++it) {
    it->second->clear();   // Recursive reuse
    reuseInstance(it->second);
  }
  value_map_.clear();

  // Clear elements in the vector
  for (size_t i = 0; i < value_vector_.size(); ++i) {
    value_vector_[i]->clear();   // Recursive reuse
    reuseInstance(value_vector_[i]);
  }
  value_vector_.clear();
}

void Value::SetAsVector() {
  ChangeValueType(value_vector);
}

void Value::Vectorize() {
  Value *value = getInstance();

  value->copyFrom(this);

  clear();   // Clear all elements used here
  SetAsVector();

  value_vector_.push_back(value);   // Put the copy of me
}

void Value::SwapVectorComponents(size_t pos, size_t pos2) {
  if (value_type_ != value_vector) {
    return;
  }

  if (value_vector_.size() < pos) {
    return;
  }

  if (value_vector_.size() < pos2) {
    return;
  }

  Value *value = value_vector_[pos];
  value_vector_[pos] = value_vector_[pos2];
  value_vector_[pos2] = value;
}

void Value::PopBackFromVector() {
  if (value_type_ != value_vector) {
    return;
  }
  if (value_vector_.size() == 0) {
    return;
  }

  // Reuse the last element and remove it
  reuseInstance(value_vector_[value_vector_.size() - 1]);
  value_vector_.pop_back();
}

Value *Value::AddValueToVector() {
  // Just make sure we are in vector mode
  if (value_type_ != value_vector) {
    SetAsVector();
  }

  // Get a new instance of Value and push it to the vector
  Value *value = getInstance();
  value_vector_.push_back(value);

  // Always return a void object
  value->SetAsVoid();
  return value;
}

Value *Value::AddValueToVector(size_t pos) {
  // Just make sure we are in vector mode
  if (value_type_ != value_vector) {
    SetAsVector();
  }

  // Get a new instance of Value and push it to the vector
  Value *value = getInstance();
  value_vector_.insert(value_vector_.begin() + pos, value);

  // Always return a void object
  value->SetAsVoid();
  return value;
}

Value *Value::GetValueFromVector(size_t pos) {
  if (value_type_ != value_vector) {
    if (pos == 0) {
      return this;
    } else {
      return NULL;
    }
  }

  if (pos >= value_vector_.size()) {
    return NULL;
  } else {
    return value_vector_[pos];
  }
}

size_t Value::GetVectorSize() const {
  return value_vector_.size();
}


bool StringCompareAscending(const std::string& left, const std::string& right) {
  if (strcmp(left.c_str(), right.c_str()) < 0) {
    return true;
  }
  return false;
}

void Value::SortVectorInAscendingOrder() {
  if (value_type_ != value_vector) {
    return;
  }
  if (value_vector_.size() < 2) {
    return;
  }
  std::sort(value_vector_.begin(), value_vector_.end(), Value::Less);
  return;
}

void Value::SortVectorInDescendingOrder() {
  if (value_type_ != value_vector) {
    return;
  }
  if (value_vector_.size() < 2) {
    return;
  }
  std::sort(value_vector_.begin(), value_vector_.end(), Value::Greater);
  return;
}

void Value::SortVectorOfMapsInAscendingOrder(const std::string& field) {
  if (value_type_ != value_vector) {
    return;
  }
  if (value_vector_.size() < 2) {
    return;
  }
  if (value_vector_[0]->value_type_ != value_map) {
    return;
  }
  MapCompareAscendingFunctor comp_functor(field);
  std::sort(value_vector_.begin(), value_vector_.end(), comp_functor);
  return;
}

void Value::SortVectorOfMapsInDescendingOrder(const std::string& field) {
  if (value_type_ != value_vector) {
    return;
  }
  if (value_vector_.size() < 2) {
    return;
  }
  if (value_vector_[0]->value_type_ != value_map) {
    return;
  }
  MapCompareDescendingFunctor comp_functor(field);
  std::sort(value_vector_.begin(), value_vector_.end(), comp_functor);
  return;
}

void Value::PartialSortVectorInAscendingOrder(uint64_t num_items_sorted) {
  if (value_type_ != value_vector) {
    return;
  }
  if (value_vector_.size() < 2) {
    return;
  }
  if (num_items_sorted > value_vector_.size()) {
    num_items_sorted = value_vector_.size();
  }
  std::partial_sort(value_vector_.begin(), value_vector_.begin() + num_items_sorted, value_vector_.end(),
                    Value::Less);
  return;
}

void Value::PartialSortVectorInDescendingOrder(uint64_t num_items_sorted) {
  if (value_type_ != value_vector) {
    return;
  }
  if (value_vector_.size() < 2) {
    return;
  }
  if (num_items_sorted > value_vector_.size()) {
    num_items_sorted = value_vector_.size();
  }
  std::partial_sort(value_vector_.begin(), value_vector_.begin() + num_items_sorted, value_vector_.end(),
                    Value::Greater);
  return;
}

void Value::PartialSortVectorOfMapsInAscendingOrder(const std::string& field, uint64_t num_items_sorted) {
  if (value_type_ != value_vector) {
    return;
  }
  if (value_vector_.size() < 2) {
    return;
  }
  if (value_vector_[0]->value_type_ != value_map) {
    return;
  }
  if (num_items_sorted > value_vector_.size()) {
    num_items_sorted = value_vector_.size();
  }
  MapCompareAscendingFunctor comp_functor(field);
  std::partial_sort(value_vector_.begin(), value_vector_.begin() + num_items_sorted, value_vector_.end(),
                    comp_functor);
  return;
}

void Value::PartialSortVectorOfMapsInDescendingOrder(const std::string& field, uint64_t num_items_sorted) {
  if (value_type_ != value_vector) {
    return;
  }
  if (value_vector_.size() < 2) {
    return;
  }
  if (value_vector_[0]->value_type_ != value_map) {
    return;
  }
  if (num_items_sorted > value_vector_.size()) {
    num_items_sorted = value_vector_.size();
  }
  MapCompareDescendingFunctor comp_functor(field);
  std::partial_sort(value_vector_.begin(), value_vector_.begin() + num_items_sorted, value_vector_.end(),
                    comp_functor);
  return;
}

// ----------------------------------------------------------------------------------------
// Map functions
// ----------------------------------------------------------------------------------------

bool Value::IsMap() const {
  return (value_type_ == value_map);
}

bool Value::CheckMapValue(const char *key, const char *value) {
  if (value_type_ != value_map) {
    return false;
  }

  std::string _key = key;
  Value *map_value = value_map_.findInMap(_key);
  if (!map_value) {
    return false;
  }

  return map_value->IsString(value);
}

void Value::SetAsMap() {
  ChangeValueType(value_map);
}

Value::ValueType Value::GetTypeFromMap(const char *key) {
  if (value_type_ != value_map) {
    return value_void;
  }
  const std::string _key = key;
  return value_map_.findInMap(_key)->GetValueType();
}

size_t Value::GetNumMapValues() {
  if (value_type_ != value_map) {
    return 0;
  }
  return value_map_.size();
}

Value *Value::AddValueToMap(const std::string& key) {
  // Just make sure we are in map mode
  if (value_type_ != value_map) {
    SetAsMap();
  }
  // Get a new instance of Value and insert it in the map
  Value *value = getInstance();
  value_map_.insertInMap(key, value);

  // Always return a void object
  value->SetAsVoid();
  return value;
}

Value *Value::GetValueFromMap(const char *key) {
  if (value_type_ != value_map) {
    return NULL;
  }
  std::string _key = key;
  return value_map_.findInMap(_key);
}

const char *Value::GetStringFromMap(const char *key) {
  if (value_type_ != value_map) {
    LM_E(("Error looking for key '%s' in map, Value is not a map", key));
    return NULL;
  }

  std::string _key = key;
  Value *value = value_map_.findInMap(_key);

  if (!value) {
    LM_E(("Error looking for key in map, key:'%s' not found", key));
    return NULL;
  }

  // LM_M(("OK looking for key in map, key:'%s' value:%p('%s')", key, value, value->c_str()));
  return value->c_str();
}

Value *Value::GetOrAddValueFromMap(const char *key) {
  Value *v = GetValueFromMap(key);

  if (v) {
    return v;
  }
  return AddValueToMap(key);
}

size_t Value::GetUint64FromMap(const char *key, size_t default_value) {
  if (value_type_ != value_map) {
    return default_value;
  }

  std::string _key = key;
  Value *v = value_map_.findInMap(_key);
  if (v) {
    return v->GetUint64(default_value);
  }

  return default_value;
}

double Value::GetDoubleFromMap(const char *key, double default_value) {
  std::string _key = key;
  Value *v = value_map_.findInMap(_key);

  if (v) {
    return v->GetDouble();
  }

  return default_value;
}

Value *Value::GetValueFromMap(const std::string& key) {
  return value_map_.findInMap(key);
}

void Value::SetStringForMap(const char *key, const char *str) {
  ChangeValueType(value_map);   // Make sure this is a map
  GetOrAddValueFromMap(key)->SetString(str);
}

void Value::SetUint64ForMap(const char *key, size_t value) {
  ChangeValueType(value_map);   // Make sure this is a map
  GetOrAddValueFromMap(key)->SetUint64(value);
}

void Value::SetDoubleForMap(const char *key, double value) {
  ChangeValueType(value_map);   // Make sure this is a map
  GetOrAddValueFromMap(key)->SetDouble(value);
}

std::vector<std::string> Value::GetKeysFromMap() {
  return value_map_.getKeysVector();
}

void Value::ConvertToNumber() {
  switch (value_type_) {
    case value_void:
    case value_number:
      // It is already a number
      return;

    case value_vector:
      SetDouble(0);
      return;

    case value_map:
      SetDouble(0);
      return;

    case value_string:
      SetDouble(atof(value_string_.c_str()));
      return;

    default:
      LM_X(1, ("Internal error, unknown value_type:%d", static_cast<int>(value_type_)));
      return;
  }
  // Impossible condition to silent valgrind
  // In any case, LM_X will exit the program
  LM_X(1, ("Internal error"));
  return;
}

void Value::ConvertToString() {
  if (value_type_ != value_string) {
    SetString(GetString());
  }
}

void Value::ChangeValueType(ValueType new_value_type) {
  if (value_type_ == new_value_type) {
    return;   // Nothing to do
  }
  // Now, if we change (or think we change) type, we want to be sure data are cleaned
  // But perhaps we are using set_as_...() in too many places to be sure,
  // so it is returned back to its original place
  // Reuse all elements
  clear();

  // Assign the new value_type
  value_type_ = new_value_type;
}

void Value::AddDouble(double v) {
  if (value_type_ != value_number) {
    SetDouble(v);
  }
  value_double_ += v;
}

// ----------------------------------------------------------------------------------------
// SET AND GET functions
// ----------------------------------------------------------------------------------------

Value& Value::operator=(const char *value) {
  // Set as integer
  ChangeValueType(value_string);
  value_string_ = value;
  return *this;
}

void Value::SetString(const std::string& value) {
  ChangeValueType(value_string);
  value_string_ = value;
}

Value& Value::operator=(double value) {
  ChangeValueType(value_number);
  value_double_ = value;
  return *this;
}

void Value::SetDouble(double value) {
  ChangeValueType(value_number);
  value_double_ = value;
}

void Value::SetUint64(size_t value) {
  ChangeValueType(value_number);
  value_double_ = value;
}

void Value::SetString(const char *value) {
  ChangeValueType(value_string);
  value_string_ = value;
}

void Value::SetString(const char *value, size_t len) {
  ChangeValueType(value_string);
  value_string_ = "";
  value_string_.append(value, len);
}

// prefix form
Value& Value::operator++() {
  switch (value_type_) {
    case value_number:
      ++value_double_;
      break;
    default:
      break;
  }
  return *this;
}

// postfix form
const Value Value::operator++(int /* just to distinguish prefix and postfix increment */) {
  Value old_value = *this;
  ++(*this);
  return old_value;
}

// Comparison operators (mainly for numbers and strings)
bool Value::Less(const Value* const left, const Value* const right) {
  if (left->value_type_ < right->value_type_) {
    return true;
  } else if (left->value_type_ < right->value_type_) {
    return true;
  } else {
    switch (left->value_type_) {
      case value_number:
        return left->value_double_ < right->value_double_;
      case value_string:
        if (strcmp(left->value_string_.c_str(), right->value_string_.c_str()) < 0) {
          return true;
        }
        return false;
      case value_vector:
        return left->value_vector_.size() < right->value_vector_.size();
      case value_map:
        return left->value_vector_.size() < right->value_vector_.size();
      default:
        LM_E(("Internal Error, unknown value_type:%d", left->value_type_));
        std::abort();
        break;
    }
    return false;
  }
}

bool Value::Greater(const Value* const left, const Value* const right) {
  return Less(right, left);
}

bool Value::LessOrEqual(const Value* const left, const Value* const right) {
  return !Less(right, left);
}

bool Value::GreaterOrEqual(const Value* const left, const Value* const right) {
  return !Less(left, right);
}

bool Value::IsString() const {
  return (value_type_ == value_string);
}

bool Value::IsString(const char *str) const {
  if (value_type_ == value_string) {
    if (value_string_ == str) {
      return true;
    }
  }
  return false;
}

size_t Value::GetUint64(size_t default_value) const {
  if (value_type_ != value_number) {
    return default_value;
  }
  return value_double_;
}

double Value::GetDouble(double default_value) const {
  if (value_type_ != value_number) {
    return default_value;
  }
  return value_double_;
}

std::string Value::GetString() {  // const not possible because of str()
  switch (value_type_) {
    case value_void:
      return "";

    case value_number:
      double intpart;
      if (modf(value_double_, &intpart) == 0.0) {
        return au::str("%lu", static_cast<size_t> (intpart));
      } else {
        return au::str("%f", value_double_);
      }

    case value_string:
      return value_string_;

    case value_vector: {
      // Create a compilation unit to avoid problems with the local variables

      std::ostringstream output;
      output << "[";
      for (size_t i = 0; i < value_vector_.size(); ++i) {
        output << value_vector_[i]->str() << ",";
      }
      output << "]";
      return output.str();
    }

    case value_map:
      return str();

    default:
      LM_X(1, ("Internal error, unknown value_type:%d", static_cast<int>(value_type_)));
      return NULL;
  }

  // Impossible condition to silent valgrind
  // In any case, LM_X will exit the program
  LM_X(1, ("Internal error"));
  return NULL;
}

// Access to the string char*
const char *Value::c_str() const {
  if (value_type_ != value_string) {
    LM_W(("Error recovering c_str() from Value; it is not a string, but value_type_:%d (value_string:%d)",
          value_type_, value_string));
    return NULL;
  }
  return value_string_.c_str();
}

void Value::AppendString(Value *value) {
  // Convert this to string
  ConvertToString();
  value_string_.append(value->GetString());
}

const char *Value::StrSerializationCode(SerializationCode code) {
  switch (code) {
    case ser_void:
      return "ser_void";
    case ser_int_positive:
      return "ser_int_positive";
    case ser_int_negative:
      return "ser_int_negative";
    case ser_int_value_0:
      return "ser_int_value_0";
    case ser_int_value_1:
      return "ser_int_value_1";
    case ser_int_value_2:
      return "ser_int_value_2";
    case ser_int_value_3:
      return "ser_int_value_3";
    case ser_int_value_4:
      return "ser_int_value_4";
    case ser_int_value_5:
      return "ser_int_value_5";
    case ser_int_value_6:
      return "ser_int_value_6";
    case ser_int_value_7:
      return "ser_int_value_7";
    case ser_int_value_8:
      return "ser_int_value_8";
    case ser_int_value_9:
      return "ser_int_value_9";
    case ser_int_value_10:
      return "ser_int_value_10";
    case ser_int_value_minus_1:
      return "ser_int_value_minus_1";
    case ser_double_positive_1_decimal:
      return "ser_double_positive_1_decimal";
    case ser_double_positive_2_decimal:
      return "ser_double_positive_2_decimal";
    case ser_double_positive_3_decimal:
      return "ser_double_positive_3_decimal";
    case ser_double_positive_4_decimal:
      return "ser_double_positive_4_decimal";
    case ser_double_positive_5_decimal:
      return "ser_double_positive_5_decimal";
    case ser_double_negative_1_decimal:
      return "ser_double_negative_1_decimal";
    case ser_double_negative_2_decimal:
      return "ser_double_negative_2_decimal";
    case ser_double_negative_3_decimal:
      return "ser_double_negative_3_decimal";
    case ser_double_negative_4_decimal:
      return "ser_double_negative_4_decimal";
    case ser_double_negative_5_decimal:
      return "ser_double_negative_5_decimal";
    case ser_double:
      return "ser_double";
    case ser_string:
      return "string";
    case ser_string_constant:
      return "string_constant";
    case ser_string_smaz:
      return "string_smaz";
    case ser_vector:
      return "vector";
    case ser_vector_len_0:
      return "ser_vector_len_0";
    case ser_vector_len_1:
      return "ser_vector_len_1";
    case ser_vector_len_2:
      return "ser_vector_len_2";
    case ser_vector_len_3:
      return "ser_vector_len_3";
    case ser_vector_len_4:
      return "ser_vector_len_4";
    case ser_vector_len_5:
      return "ser_vector_len_5";
    case ser_map:
      return "ser_map";
    case ser_map_len_0:
      return "ser_map_len_0";
    case ser_map_len_1:
      return "ser_map_len_1";
    case ser_map_len_2:
      return "ser_map_len_2";
    case ser_map_len_3:
      return "ser_map_len_3";
    case ser_map_len_4:
      return "ser_map_len_4";
    case ser_map_len_5:
      return "ser_map_len_5";
  }
  return "Unknown";
}

const char *Value::StrType() {
  switch (value_type_) {
    case value_void:
      return "void";
    case value_string:
      return "string";
    case value_number:
      return "number";
    case value_vector:
      return "vector";
    case value_map:
      return "map";
  }
  return "Unknown";
}
}
}
