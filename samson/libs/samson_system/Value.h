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
 * FILE            Value.h
 *
 * AUTHOR          Andreu Urruela
 *
 * PROJECT         SAMSON samson_system library
 *
 * DATE            2012
 *
 * DESCRIPTION
 *
 *  Definition of Value class to create a flexible data type that
 *  can be used as a number, a string, a vector of Values or a map of Values
 *
 */

#ifndef _H_SAMSON_SYSTEM_VALUE_Value
#define _H_SAMSON_SYSTEM_VALUE_Value

#include <stdint.h>               // uint64_t

#include <cctype>
#include <string>
#include <vector>

#include "json.h"

#include "au/containers/map.h"
#include "au/Pool.h"
#include "au/tables/pugi.h"
#include "logMsg/logMsg.h"
#include "samson/module/DataInstance.h"

namespace samson {
namespace system {
// Constant word serialization
int GetConstantWordCode(const char *word);
const char *GetConstantWord(int c);

class Value : public samson::DataInstance {
  public:
    // Value types ( on memory )
    typedef enum {
      value_void,   // No content

      value_number,   // Generic number content  ( using value_double_ )
      value_string,   // Generic string content  ( using value_string_ )
      value_vector,   // A vector of values      ( using value_vector_ )
      value_map,
    // A map of values         ( using value_map_    )
    } ValueType;

    // How data is serialized
    typedef enum {
      // Serialization of <void>
      // ------------------------------------------------------------
      ser_void,

      // Serialization of numbers
      // ------------------------------------------------------------
      ser_int_positive,   // Variable length possitive numbers
      ser_int_negative,   // Variable length negative numbers
      ser_int_value_0,   // Concrete values
      ser_int_value_1, ser_int_value_2, ser_int_value_3, ser_int_value_4, ser_int_value_5, ser_int_value_6,
      ser_int_value_7, ser_int_value_8, ser_int_value_9, ser_int_value_10,
      ser_int_value_minus_1,

      ser_double_positive_1_decimal,   // Double possitve or negative with a fixed number of decimals
      ser_double_positive_2_decimal, ser_double_positive_3_decimal, ser_double_positive_4_decimal,
      ser_double_positive_5_decimal,

      ser_double_negative_1_decimal, ser_double_negative_2_decimal, ser_double_negative_3_decimal,
      ser_double_negative_4_decimal, ser_double_negative_5_decimal,

      ser_double,   // Generic double otherwise

      // Serialization of string
      // ------------------------------------------------------------
      ser_string, ser_string_constant,   // Constant words frequently used ( user, log, url, ...)
      ser_string_smaz,   // Compressed using smaz

      // Serialization of vector
      // ------------------------------------------------------------
      ser_vector, ser_vector_len_0,   // Vector with a particular length
      ser_vector_len_1, ser_vector_len_2, ser_vector_len_3, ser_vector_len_4, ser_vector_len_5,

      // Serialization of map
      // ------------------------------------------------------------
      ser_map, ser_map_len_0,   // Map with a particular number of elements inside
      ser_map_len_1, ser_map_len_2, ser_map_len_3, ser_map_len_4, ser_map_len_5,
    } SerializationCode;

    static const std::string kSystemValueName;

    Value() : samson::DataInstance() {}

    ~Value() {
      // This instance will be never reused but the internal values will be reused
      clear();
    }

    static Value *getInstance() {
      // reusing instances caused some problems in the past,
      // but perhaps they are solved in samson_0.7, so let's give it a chance
      if (!pool_values_)
        pool_values_ = new au::Pool<Value>();

      Value * recovered_value = pool_values_->pop();
      LM_M(("Recovering value:%p from pool:%p size:%lu", recovered_value, pool_values_, pool_values_->size()));
      if (recovered_value) {
        return recovered_value;
      }
      return new Value();
    }

    static void reuseInstance(Value *value) {
      // reusing instances caused some problems in the past,
      // but perhaps they are solved in samson_0.7, so let's give it a chance
      if (!pool_values_)
        pool_values_ = new au::Pool<Value>();

      value->clear();
      LM_M(("Pushing value:%p to pool:%p size:%lu", value, pool_values_, pool_values_->size()));
      pool_values_->push(value);
    }

    // Return the name of this data type ( system.Value )
    // Could be const, but it is the implementation of virtual method in DataInstance,
    // should must wait for a larger scope change
    std::string getName() {
      return Value::kSystemValueName;
    }

    // ------------------------------------------------------------
    // PARSE Operations
    // ------------------------------------------------------------

    int ParseVoid(const char *data);
    int ParseNumber(const char *data);
    int ParseString(const char *data);
    int ParseVector(const char *data);
    int ParseMap(const char *data);
    // parse() is implementing DataInstance::parse(), so at the moment we don't change that interface
    int parse(char *data);

    // ------------------------------------------------------------
    // SERIALIZE Operations
    // ------------------------------------------------------------

    int SerializeVoid(char *data);
    int SerializeNumber(char *data);
    int SerializeString(char *data);
    int SerializeVector(char *data);
    int SerializeMap(char *data);
    int serialize(char *data);

    // ------------------------------------------------------------
    // HASH Operations
    // ------------------------------------------------------------

    int HashVoid(int max_num_partitions);
    int HashNumber(int max_num_partitions);
    int HashString(int max_num_partitions);
    int HashVector(int max_num_partitions);
    int HashMap(int max_num_partitions);
    int hash(int max_num_partitions);

    // ------------------------------------------------------------
    // SIZE Operations
    // ------------------------------------------------------------

    static int size(char *data);

    // ------------------------------------------------------------
    // Data compare
    // ------------------------------------------------------------

    static int compare(char *data1, char *data2, size_t *offset1, size_t *offset2);
    static int compare(char *data1, char *data2);
    int serial_compare(char *data1, char *data2);

    // ------------------------------------------------------------
    // Checks
    // ------------------------------------------------------------

    static const char *GetTypeStatic();
    const char *getType();
    static bool CheckTypeStatic(const char *type);
    bool checkType(const char *type);
    static size_t GetHashTypeStatic();
    size_t getHashType();
    static bool CheckHashTypeStatic(size_t valType);
    bool checkHashType(size_t valType);

    // ------------------------------------------------------------
    // Operations
    // ------------------------------------------------------------

    bool operator==(const Value &other) const;
    bool operator!=(const Value &other) const;
    bool operator<=(const Value &other) const;
    bool operator>=(const Value &other) const;
    bool operator<(const Value &other) const;
    bool operator>(const Value &other) const;
    const Value operator+(Value &other);   // const & for other not possible because of GetString()
    const Value operator-(const Value &other) const;
    const Value operator*(const Value &other) const;
    const Value operator/(const Value &other) const;
    inline int compare(const Value& other) const;

    // ------------------------------------------------------------
    // setFromString ( necessary for REST )
    // ------------------------------------------------------------

    void SetFrom(struct json_object *json);
    void setFromString(const char *_data);
    void SetFromJSONString(const char *_data);

    // ------------------------------------------------------------
    // COPY Function
    // ------------------------------------------------------------

    void copyFrom(Value *other);

    // ------------------------------------------------------------
    // STR Function
    // ------------------------------------------------------------

    std::string str();

    // ------------------------------------------------------------
    // JSON & XML Functions
    // ------------------------------------------------------------

    std::string strJSON();
    void _strJSON(std::ostream &output);
    std::string strXML();
    void _strXML(std::ostream &output);
    std::string strHTML(int level_html_heading);
    void _strHTML(int level_html_heading, std::ostream &output);
    std::string strHTMLTable(std::string _varNameInternal);
    void _strHTMLTable(const std::string& _varNameInternal, std::ostream &output);
    std::string paint_header(int init_col);
    std::string paint_header_basic(int init_col);
    std::string paint_value(int index_row);
    int num_fields();
    int num_basic_fields();
    int max_depth();
    int max_num_values();
    bool is_terminal();

    // ----------------------------------------------------------------------------------------
    // void functions
    // ----------------------------------------------------------------------------------------

    void SetAsVoid();
    bool IsVoid() const;

    // ----------------------------------------------------------------------------------------
    // number functions
    // ----------------------------------------------------------------------------------------

    bool IsNumber() const;

    // ----------------------------------------------------------------------------------------
    // Vector functions
    // ----------------------------------------------------------------------------------------

    bool IsVector() const;
    void SetAsVector();
    void Vectorize();
    Value *AddValueToVector();
    Value *AddValueToVector(size_t pos);
    Value *GetValueFromVector(size_t pos);
    size_t GetVectorSize() const;
    void SwapVectorComponents(size_t pos, size_t pos2);
    void PopBackFromVector();

    void SortVectorInAscendingOrder();
    void SortVectorInDescendingOrder();
    void SortVectorOfMapsInAscendingOrder(const std::string& field);
    void SortVectorOfMapsInDescendingOrder(const std::string& field);
    void PartialSortVectorInAscendingOrder(uint64_t num_items_sorted);
    void PartialSortVectorInDescendingOrder(uint64_t num_items_sorted);
    void PartialSortVectorOfMapsInAscendingOrder(const std::string& field, uint64_t num_items_sorted);
    void PartialSortVectorOfMapsInDescendingOrder(const std::string& field, uint64_t num_items_sorted);


    // ----------------------------------------------------------------------------------------
    // Map functions
    // ----------------------------------------------------------------------------------------

    bool IsMap() const;
    bool CheckMapValue(const char *key, const char *value);
    void SetAsMap();

    // GetValueFromMap() could be const, but is calling au::map::findInMap(), which is not
    ValueType GetTypeFromMap(const char *key);

    size_t GetNumMapValues();
    Value *AddValueToMap(const std::string& key);
    Value *GetValueFromMap(const char *key);
    Value *GetOrAddValueFromMap(const char *key);
    const char *GetStringFromMap(const char *key);
    size_t GetUint64FromMap(const char *key, size_t default_value = 0);
    double GetDoubleFromMap(const char *key, double default_value = 0);
    void SetStringForMap(const char *key, const char *str);
    void SetUint64ForMap(const char *key, size_t value);
    void SetDoubleForMap(const char *key, double value);
    Value *GetValueFromMap(const std::string& key);
    std::vector<std::string> GetKeysFromMap();
    void ConvertToNumber();
    void ConvertToString();
    void ChangeValueType(ValueType new_value_type);
    void AddDouble(double v);

    // ----------------------------------------------------------------------------------------
    // CLEAR Function
    // ----------------------------------------------------------------------------------------

    void clear();

    // ----------------------------------------------------------------------------------------
    // SET AND GET functions
    // ----------------------------------------------------------------------------------------

    Value& operator=(const char *_value);
    void SetString(const std::string& _value);
    Value& operator=(double _value);
    void SetDouble(double _value);
    void SetUint64(size_t _value);
    void SetString(const char *_value);
    void SetString(const char *_value, size_t len);
    Value& operator++();
    const Value operator++(int /* dummy argument */);

    static bool Less(const Value* const left, const Value* const right);
    static bool Greater(const Value* const left, const Value* const right);
    static bool LessOrEqual(const Value* const left, const Value* const right);
    static bool GreaterOrEqual(const Value* const left, const Value* const right);

    ValueType GetValueType() const;
    bool IsString() const;
    double GetDouble(double default_value = 0) const;
    size_t GetUint64(size_t default_value = 0) const;
    bool IsString(const char *str) const;
    std::string GetString();   // const not possible because of str()

    // Access to the string char*
    const char *c_str() const;
    void AppendString(Value *value);
    static const char *StrSerializationCode(SerializationCode code);
    const char *StrType();

  private:
    static const size_t kValueCode;
    // Pool of Value instances for vector and map
    static au::Pool<Value> *pool_values_;
    // Global value type
    // ------------------------------------------------------------
    ValueType value_type_;
    // Internal representation of the value
    // ------------------------------------------------------------
    // Number
    double value_double_;
    // String
    std::string value_string_;
    // Vector
    std::vector<Value *> value_vector_;
    // Map
    au::map<std::string, Value> value_map_;
};
}
}   // end of namespace samson.system

#endif  // ifndef _H_SAMSON_SYSTEM_VALUE_Value
