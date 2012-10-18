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
 * FILE            Filter.h
 *
 * AUTHOR          Andreu Urruela
 *
 * PROJECT         SAMSON samson_system library
 *
 * DATE            2012
 *
 * DESCRIPTION
 *
 *  Declaration of Filter (and descendants) class to process system.Value in command line
 *
 */

#ifndef _H_SAMSON_SYSTEM_FILTER
#define _H_SAMSON_SYSTEM_FILTER

#include <string>
#include <vector>

#include "au/string/StringComponents.h"
#include "au/string/Tokenizer.h"
#include "au/containers/vector.h"
#include "au/string/string.h"

#include "samson/module/samson.h"

#include "samson_system/KeyValue.h"
#include "samson_system/Source.h"
#include "samson_system/SourceFunction.h"
#include "samson_system/Value.h"

namespace samson {
namespace system {
// -----------------------------------------------------------------------
// Filter for map-like operations over system.Value system.Value queues
// -----------------------------------------------------------------------

class Filter {
  public:
    Filter() : next_(NULL) {}

    virtual ~Filter() {}

    virtual void run(KeyValue kv) = 0;

    std::string str() {
      std::string output = _str();

      if (next_) {
        output.append(" | ");
        output.append(next_->str());
      }
      return output;
    }

    virtual std::string _str() = 0;

    Filter* next() const { return next_; }
    void set_next(Filter *next) { next_ = next; }
    bool IsThereNext() const { return (next_ != NULL); }

  private:
    Filter *next_;   // Pointer to the next filter
};

// --------------------------------------------------------
// FilterEmit
// --------------------------------------------------------

class FilterEmit : public Filter {
  public:
    FilterEmit(int channel, samson::KVWriter* const writer) :
      channel_(channel), writer_(writer) {}

    virtual ~FilterEmit() {}

    virtual void run(KeyValue kv) {
      writer_->emit(channel_, kv.key(), kv.value());
    }

    std::string _str() {
      return au::str("Emit to channel %d", channel_);
    }

  private:
    int channel_;
    samson::KVWriter *writer_;
};

// ----------------------------------------------------------------------------
// FilterJSON : each line is analyzed as JSON and used as key ( value = void )
// ----------------------------------------------------------------------------

class FilterJSONLine : public Filter {
  public:
    FilterJSONLine() {
      // Value is always void at the output
      new_value_.value->SetAsVoid();

      // Prepare output key-value
      output_kv_.set_key(new_key_.value);
      output_kv_.set_value(new_value_.value);
    }

    virtual ~FilterJSONLine() {}

    virtual void run(KeyValue kv) {
      // Input key expected to be a string
      if (!kv.key()) {
        return;
      }

      if (!kv.key()->IsString()) {
        return;
      }

      // Set the new key reading json string
      new_key_.value->SetFromJSONString(kv.key()->c_str());

      // Emit the output key-value to the next element in the chain
      if (IsThereNext()) {
        next()->run(output_kv_);
      }
    }

    std::string _str() {
      return "FilterJSONLine";
    }

  private:
    system::ValueContainer new_key_;
    system::ValueContainer new_value_;

    KeyValue output_kv_;
};

// --------------------------------------------------------
// FilterXMLElement
// --------------------------------------------------------

class FilterXMLElement : public Filter {
  public:
    explicit FilterXMLElement(const std::string& element_name) :
      element_name_(element_name) {
    }

    virtual ~FilterXMLElement() {}

    static FilterXMLElement *GetFilter(au::token::TokenVector *token_vector, au::ErrorManager *error) {
      if (token_vector->eof()) {
        error->set("No name provided for xml element extraction");
        return NULL;
      }

      // Get next token
      au::token::Token *token = token_vector->popToken();

      if (!token->isNormal()) {
        error->set(au::str("Not valid element name for xml extraction (%s)", token->content.c_str()));
        return NULL;
      }

      return new FilterXMLElement(token->content);
    }

    virtual void run(KeyValue kv) {
      // Prepare output
      kv_.set_key(new_key_.value);
      kv_.set_value(kv.value());

      if (!kv.key()) {
        return;
      }

      // document to parse xml
      pugi::xml_document xml_doc;

      // Parser the xml document into "doc"
      pugi::xml_parse_result result = xml_doc.load(kv.key()->c_str());

      // Check errors in the parsing
      if (result.status != pugi::status_ok) {
        return;   // Do nothing
      }
      // Process all elements
      Process(xml_doc);
    }

    void Add(pugi::xml_node & xml_node, std::string prefix) {
      // printf("Add (%s) %s %s\n" , prefix.c_str() , xml_node.name() , xml_node.value()  );

      switch (xml_node.type()) {
        case pugi::node_element:   // Element tag, i.e. '<node/>'
        {
          for (pugi::xml_node_iterator n = xml_node.begin(); n != xml_node.end(); ++n) {
            // For each node
            pugi::xml_node node = *n;
            Add(node, prefix + xml_node.name());
          }
        }
          break;

        case pugi::node_pcdata:   // Plain character data, i.e. 'text'
        case pugi::node_cdata:    // Character data, i.e. '<![CDATA[text]]>'
        {
          if (prefix == "") {
            prefix = "content";
          }
          new_key_.value->AddValueToMap(prefix)->SetString(xml_node.value());
          return;
        }

        case pugi::node_document: {
          // Not expected here...
          return;
        }

        case pugi::node_null:
        case pugi::node_comment:      // Comment tag, i.e. '<!-- text -->'
        case pugi::node_pi:           // Processing instruction, i.e. '<?name?>'
        case pugi::node_declaration:  // Document declaration, i.e. '<?xml version="1.0"?>'
        case pugi::node_doctype:      // Document type declaration, i.e. '<!DOCTYPE doc>'
          return;
      }
    }

    void Add(pugi::xml_node & xml_node) {
      // printf("Add %s %s\n" , xml_node.name() , xml_node.value()  );

      // Init new key
      new_key_.value->SetAsMap();

      for (pugi::xml_node_iterator n = xml_node.begin(); n != xml_node.end(); ++n) {
        Add(*n, "");
      }

      if (IsThereNext()) {
        next()->run(kv_);
      }
    }

    void Process(pugi::xml_node & xml_node) {
      // printf("Process %s %s\n" , xml_node.name() , xml_node.value()  );

      switch (xml_node.type()) {
        case pugi::node_null: {
          return;
        }

        case pugi::node_document: {
          // Main document... just skip to main element
          pugi::xml_node_iterator n = xml_node.begin();
          if (n != xml_node.end()) {
            Process(*n);   // Process all elements inside
          }
          break;
        }

        case pugi::node_element:   // Element tag, i.e. '<node/>'
        {
          if (xml_node.name() == element_name_) {
            Add(xml_node);
          } else {
            for (pugi::xml_node_iterator n = xml_node.begin(); n != xml_node.end(); ++n) {
              // For each node
              pugi::xml_node node = *n;
              Process(node);
            }
          }
        }
          break;

        case pugi::node_pcdata:   // Plain character data, i.e. 'text'
        case pugi::node_cdata:    // Character data, i.e. '<![CDATA[text]]>'
          return;

        case pugi::node_comment:      // Comment tag, i.e. '<!-- text -->'
        case pugi::node_pi:           // Processing instruction, i.e. '<?name?>'
        case pugi::node_declaration:  // Document declaration, i.e. '<?xml version="1.0"?>'
        case pugi::node_doctype:      // Document type declaration, i.e. '<!DOCTYPE doc>'
          return;
      }
    }

    std::string _str() {
      return au::str("FilterXMLElement %s", element_name_.c_str());
    }

  private:
    std::string element_name_;

    system::ValueContainer new_key_;
    samson::system::KeyValue kv_;
};

// --------------------------------------------------------
// FilterEmitTxt
// --------------------------------------------------------

class FilterEmitTxt : public Filter {
  public:
    FilterEmitTxt(samson::TXTWriter *writer, au::token::TokenVector *token_vector, au::ErrorManager *error) :
      writer_(writer), fields_(), separator_("|") {
      while (!token_vector->eof()) {
        if (token_vector->getNextTokenContent() == "|") {
          break;
        }

        Source *source = GetSource(token_vector, error);
        if (error->IsActivated()) {
          return;
        }

        fields_.push_back(source);
      }

      if (fields_.size() == 0) {
        error->set("No fields specified in emit command");
      }
    }

    virtual ~FilterEmitTxt() {}

    virtual void run(KeyValue kv) {
      std::ostringstream output;

      for (size_t i = 0; i < fields_.size(); ++i) {
        samson::system::Value *value = fields_[i]->get(kv);
        if (value) {
          output << value->GetString();
        }

        if (i != (fields_.size() - 1)) {
          output << separator_;
        }
      }

      output << "\n";

      // std::string output = au::str("%s %s\n" , kv.key->str().c_str() , kv.value->str().c_str() );
      std::string output_str = output.str();
      writer_->emit(output_str.c_str(), output_str.length());
    }

    std::string _str() {
      return "emit (txt)";
    }

  private:
    samson::TXTWriter *writer_;

    // Collection of fields to be used when exporting data
    au::vector<Source> fields_;

    // Separator used at the output ( should be programmable )
    std::string separator_;
};

// --------------------------------------------------------
// parse -  parse line
// --------------------------------------------------------

class FilterParser : public Filter {
  public:
    FilterParser() : fields_()
      , separator_(' ')
      , key_container_() {
    }

    virtual ~FilterParser() {}

    // parse field0 field1 field2 -separator X

    static FilterParser *GetFilter(au::token::TokenVector *token_vector, au::ErrorManager *error) {
      if (token_vector->eof()) {
        error->set("No command provided");
        return NULL;
      }

      FilterParser *filter = new FilterParser();
      filter->separator_ = ' ';

      // Fields ( if any )
      while (!token_vector->eof()) {
        if (token_vector->popNextTokensIfTheyAre("-", "separator")) {
          // Extract separator
          au::token::Token *separator_token = token_vector->popToken();

          if (!separator_token) {
            error->set("Wrong separator in filter command");
            delete filter;
            return NULL;
          }

          if (separator_token->content.length() != 1) {
            error->set(
                       au::str("%s is a wrong separator in filter command ( only 1 char separators supported )",
                               separator_token->content.c_str()));
            delete filter;
            return NULL;
          }

          filter->separator_ = separator_token->content[0];
          continue;
        }

        au::token::Token *token = token_vector->popToken();

        if (!token->isNormal()) {
          error->set(au::str("Incorrect field definition %s", token->content.c_str()));
          delete filter;
          return NULL;
        }

        std::string field_definition = token->content;

        if ((field_definition == "number") || (field_definition == "num") || (field_definition == "n")) {
          filter->fields_.push_back(number);
        } else if ((field_definition == "string") || (field_definition == "s")) {
          filter->fields_.push_back(string);
        } else if (!token->isNormal()) {
          error->set(au::str("Incorrect field definition %s", token->content.c_str()));
          delete filter;
          return NULL;
        }
      }
      return filter;
    }

    virtual void run(KeyValue kv) {
      // Key should be string for this operation
      if (!kv.key()->IsString()) {
        LM_W(("key should be a string for this filter"));
        return;
      }

      std::string line = kv.key()->GetString();

      au::StringComponents string_components;
      string_components.ProcessLine(line.c_str(), line.length(), separator_);

      key_container_.value->SetAsVector();
      key_container_.value->clear();

      if (fields_.size() == 0) {
        for (size_t i = 0; i < string_components.components.size(); ++i) {
          key_container_.value->AddValueToVector()->SetString(string_components.components[i]);
        }
      } else {
        for (size_t i = 0; i < string_components.components.size(); ++i) {
          if (i < fields_.size()) {
            samson::system::Value *v = key_container_.value->AddValueToVector();
            v->SetString(string_components.components[i]);

            switch (fields_[i]) {
              case string:
                break;
              case number:
                v->ConvertToNumber();
                break;
              default:
                LM_E(("Wrong item in fields_[%d]: %d", i, fields_[i]));
                break;
            }
          }
        }
      }

      // Run next filter
      if (IsThereNext()) {
        KeyValue next_kv(key_container_.value, kv.value());
        next()->run(next_kv);
      }
    }

    std::string _str() {
      return au::str("parse");
    }

  private:
    typedef enum {
      string, number
    } Type;

    // Field codification
    std::vector<Type> fields_;

    // Separator
    char separator_;

    // Key key used to "emit" to the next filter
    samson::system::ValueContainer key_container_;
};

// --------------------------------------------------------
// Extract words
// --------------------------------------------------------

class FilterParserWords : public Filter {
  public:
    FilterParserWords() : key_container_() {}

    virtual ~FilterParserWords() {}

    virtual void run(KeyValue kv) {
      // Key should be string for this operation
      if (!kv.key()->IsString()) {
        return;
      }

      // Next key-value ready to be emitted
      KeyValue next_kv(key_container_.value, kv.value());

      // Line to parse
      std::string line = kv.key()->GetString();

      size_t pos = 0;
      size_t len = line.length();
      for (size_t i = 0; i < len; ++i) {
        if (line[i] == ' ') {
          if (pos < i) {
            // New word
            std::string word = line.substr(pos, i - pos);
            key_container_.value->SetString(word);

            // Run next filter
            if (IsThereNext()) {
              next()->run(next_kv);
            }
          }

          // Go to the next
          pos = i + 1;
        }
      }

      // Emit last word
      if (pos < line.length()) {
        std::string word = line.substr(pos);
        key_container_.value->SetString(word);

        // Run next filter
        if (IsThereNext()) {
          next()->run(next_kv);
        }
      }
    }

    std::string _str() {
      return au::str("parse_words");
    }

  private:
    // Key key used to "emit" to the next filter
    samson::system::ValueContainer key_container_;
};

// --------------------------------------------------------
// Extract chars
// --------------------------------------------------------

class FilterParserChars : public Filter {
  public:
    FilterParserChars() : key_container_() {}

    virtual ~FilterParserChars() {}

    virtual void run(KeyValue kv) {
      // Key should be string for this operation
      if (!kv.key()->IsString()) {
        return;
      }

      // Next key-value ready to be emitted
      KeyValue next_kv(key_container_.value, kv.value());

      // Line to parse
      std::string line = kv.key()->GetString();

      size_t len = line.length();
      for (size_t i = 0; i < len; ++i) {
        // New letter
        std::string letter = line.substr(i, 1);
        key_container_.value->SetString(letter);

        // Run next filter
        if (IsThereNext()) {
          next()->run(next_kv);
        }
      }
    }

    std::string _str() {
      return au::str("parse_chars");
    }

  private:
    // Key key used to "emit" to the next filter
    samson::system::ValueContainer key_container_;
};

// ----------------------------------------------------
// FilterCondition
// ----------------------------------------------------

class FilterCondition : public Filter {
  public:
    // filter key:2 = 4.56
    explicit FilterCondition(Source *eval_source) : eval_source_(eval_source) {}

    ~FilterCondition() {}

    bool test(KeyValue kv) {
      samson::system::Value *v = eval_source_->get(kv);

      if (!v) {
        return false;
      }

      return (v->GetDouble() != 0);
    }

    void run(KeyValue kv) {
      if (test(kv) && IsThereNext()) {
        next()->run(kv);
      }
    }

    std::string _str() {
      return au::str("filter %s", eval_source_->str().c_str());
    }

  private:
    Source *eval_source_;
};

// ----------------------------------------------------
// FilterSelect
// ----------------------------------------------------

class FilterSelect : public Filter {
  public:
    FilterSelect(Source *source_for_key, Source *source_for_value) :
      source_for_key_(source_for_key), source_for_value_(source_for_value) {}

    ~FilterSelect() {
      delete source_for_key_;
      delete source_for_value_;
    }

    std::string _str() {
      std::ostringstream output;

      output << "select " << source_for_key_->str() << " , " << source_for_value_;
      return output.str();
    }

    void run(KeyValue kv) {
      samson::system::Value *value_for_key = source_for_key_->get(kv);
      samson::system::Value *value_for_value = source_for_value_->get(kv);

      if (!value_for_key || !value_for_value) {
        return;
      }

      key_container_.value->copyFrom(value_for_key);
      value_container_.value->copyFrom(value_for_value);

      // Run next element
      if (IsThereNext()) {
        next()->run(KeyValue(key_container_.value, value_container_.value));
      }
    }

  private:
    Source *source_for_key_;
    Source *source_for_value_;

    samson::system::ValueContainer key_container_;
    samson::system::ValueContainer value_container_;
};

class SamsonTokenizer : public au::token::Tokenizer {
  public:
    SamsonTokenizer();
};

/*
 * Collections of filter-chains to be executed
 */

class FilterCollection {
  public:
    FilterCollection() : filters_() {}
    ~FilterCollection();

    // String debug
    std::string str();

    // General command to parse
    void AddFilters(std::string command, samson::KVWriter* const writer, TXTWriter *txt_writer, au::ErrorManager *error);

    // Run a particular key-value
    void run(KeyValue kv);

    size_t GetNumFilters();

  private:
    Filter *GetFilter(au::token::TokenVector *token_vector, samson::KVWriter* const writer, TXTWriter *txt_writer,
        au::ErrorManager *error);

    Filter *GetFilterChain(au::token::TokenVector *token_vector, samson::KVWriter* const writer, TXTWriter *txt_writer,
        au::ErrorManager *error);

    // Collections of filters
    au::vector<Filter> filters_;
};
}
}

#endif  // ifndef _H_SAMSON_SYSTEM_FILTER
