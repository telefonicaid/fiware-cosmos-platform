

#include "Filter.h"  // Own interface


namespace samson {
namespace system {
SamsonTokenizer::SamsonTokenizer() {
  addSingleCharTokens("()[]{}<> ;&|?:,+-*/'|.");
  addToken(":[");
  addToken("<=");
  addToken(">=");
  addToken("==");
  addToken("!=");

  addToken(" -only_key ");
  addToken("select");
  addToken("parse_words");
  addToken("xml_element");
  addToken("parse");
  addToken("emit");
}

// --------------------------------------------------------
// FilterCollection
// --------------------------------------------------------


FilterCollection::~FilterCollection() {
  // Remove defined filters
  filters.clearVector();
}

std::string FilterCollection::str() {
  std::ostringstream output;

  for (size_t i = 0; i < filters.size(); i++) {
    output << filters[i]->str();
  }
  return output.str();
}

Filter *FilterCollection::getFilter(au::token::TokenVector *token_vector
                                    , samson::KVWriter *writer
                                    , TXTWriter *txt_writer
                                    , au::ErrorManager *error
                                    ) {
  // Check if there are tokens to be read
  if (token_vector->eof()) {
    error->set("Filter name not specified");
    return NULL;
  }

  // Get the next token
  au::token::Token *token = token_vector->popToken();

  if (token->content == "select") {
    Source *key_source = getSource(token_vector, error);
    if (error->IsActivated())
      return NULL;

    // Expect a ","
    if (!token_vector->popNextTokenIfItIs(",")) {
      error->set(au::str("Expected ',' to separate key and value in a select statment. Found '%s'"
                         , token_vector->getNextTokenContent().c_str()));
      return NULL;
    }

    Source *value_source = NULL;
    if (!token_vector->eof()) {
      value_source = getSource(token_vector, error);
      if (error->IsActivated())
        return NULL;
    } else {
      value_source = new SourceVoid();
    }

    return new FilterSelect(key_source, value_source);
  } else if (token->content == "parse_words") {
    return new FilterParserWords();
  } else if (token->content == "json") {
    return new FilterJSONLine();
  } else if (token->content == "xml_element") {
    // Parse kind of filter
    return FilterXMLElement::getFilter(token_vector, error);
  } else if (token->content == "parse_chars") {
    return new FilterParserChars();
  } else if (token->content == "parse") {
    // Parse kind of filter
    return FilterParser::getFilter(token_vector, error);
  } else if (token->content == "emit") {
    if (writer) {
      if (token_vector->eof())
        return new FilterEmit(0, writer);                    // Default channel "0"

      au::token::Token *number = token_vector->popToken();
      if (!number->isNumber()) {
        error->set(au::str("Channel '%s' not valid in emit command. It should be a number"
                           , number->content.c_str()));
        return NULL;
      }

      int channel = atoi(number->content.c_str());
      return new FilterEmit(channel, writer);
    } else if (txt_writer) {
      FilterEmitTxt *filter_emit = new FilterEmitTxt(txt_writer, token_vector, error);
      if (error->IsActivated()) {
        delete filter_emit;
        return NULL;
      }
      return filter_emit;
    }
  } else if (token->content == "filter") {
    Source *eval_source = getSource(token_vector, error);
    if (error->IsActivated())
      return NULL; if (!eval_source) {
      error->set("Not valid condition statment in filter command");
      return NULL;
    }

    return new FilterCondition(eval_source);
  }
  return NULL;
}

// filter key = 67 | select key:1,value | emit 0 / filter key = 56 | select key:1,value | emit 1

Filter *FilterCollection::getFilterChain(au::token::TokenVector *token_vector
                                         , samson::KVWriter *writer
                                         , TXTWriter *txt_writer
                                         ,  au::ErrorManager *error) {
  // Line of filters for this command...
  au::vector<Filter> tmp_filters;

  while (!token_vector->eof()) {
    // Get the "sub" token vector for each line
    au::token::TokenVector sub_token_vector = token_vector->getTokensUntil("|");

    // Get a filter from this token_vector
    Filter *filter = getFilter(&sub_token_vector, writer, txt_writer, error);

    // If there is an error, just return
    if (error->IsActivated()) {
      tmp_filters.clearVector();
      return NULL;
    } else {
      // Add the new filter
      tmp_filters.push_back(filter);
    }
  }


  if (tmp_filters.size() == 0)
    return NULL;

  // Link the filters
  for (size_t i = 0; i < (tmp_filters.size() - 1); i++) {
    tmp_filters[i]->next = tmp_filters[i + 1];
  }

  // Add the filter line
  return tmp_filters[0];
}

// General command to parse
void FilterCollection::addFilters(std::string command
                                  , samson::KVWriter *writer
                                  , TXTWriter *txt_writer
                                  , au::ErrorManager *error) {
  // Tokenice the entire command
  // --------------------------------------------------------------------
  SamsonTokenizer tokenizer;
  au::token::TokenVector token_vector = tokenizer.parse(command);

  while (!token_vector.eof()) {
    // Get the "sub" token vector for each line
    au::token::TokenVector sub_token_vector = token_vector.getTokensUntil(";");


    // Get a filter from this token_vector
    Filter *filter = getFilterChain(&sub_token_vector, writer, txt_writer, error);

    // If there is an error, just return
    if (error->IsActivated()) {
      filters.clearVector();
      return;
    } else {
      // Add the new filter
      filters.push_back(filter);
    }
  }
}

void FilterCollection::run(KeyValue kv) {
  for (size_t f = 0; f < filters.size(); f++) {
    filters[f]->run(kv);
  }
}

size_t FilterCollection::get_num_filters() {
  return filters.size();
}
}
}