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
#include "samson_system/Source.h"  // Own interface

#include "samson_system/SourceFunction.h"

namespace samson {
namespace system {
Source *GetSingleSource(au::token::TokenVector *token_vector, au::ErrorManager *error) {
  // ----------------------------------------------------------------
  // Brackets to group operations
  // ----------------------------------------------------------------

  if (token_vector->PopNextTokenIfContentIs("(")) {
    Source *source = GetSource(token_vector, error);
    if (!source || error->HasErrors()) {
      return NULL;
    }

    if (!token_vector->PopNextTokenIfContentIs(")")) {
      error->AddError("Parentheses not closed");
    }
    return source;
  }

  // ----------------------------------------------------------------
  // Vector
  // ----------------------------------------------------------------

  if (token_vector->PopNextTokenIfContentIs("[")) {
    // Vector source
    au::vector<Source> source_components;

    while (true) {
      au::token::Token *token = token_vector->GetNextToken();
      if (!token) {
        source_components.clearVector();
        error->AddError("Unfinish vector");
        return NULL;
      } else if (token->ContentMatch("]")) {
        // end of vector
        token_vector->PopToken();
        return new SourceVector(source_components);
      }

      Source *tmp = GetSource(token_vector, error);
      if (error->HasErrors()) {
        source_components.clearVector();
        return NULL;
      } else {
        source_components.push_back(tmp);
      }

      // Read the mandatory "," if it is not the end of the vector
      if (!token_vector->CheckNextTokenContentIs("]")) {
        if (!token_vector->PopNextTokenIfContentIs(",")) {
          error->AddError(
            au::str("Wrong map format (expected ',', got '%s')",
                    token_vector->GetNextTokenContent().c_str()));
          source_components.clearVector();
          return NULL;
        }
      }
    }
  }

  // ----------------------------------------------------------------
  // Map
  // ----------------------------------------------------------------

  if (token_vector->PopNextTokenIfContentIs("{")) {
    // Vector source
    au::vector<Source> source_keys;
    au::vector<Source> source_values;

    while (true) {
      au::token::Token *token = token_vector->GetNextToken();
      if (!token) {
        source_keys.clearVector();
        source_values.clearVector();
        error->AddError("Unfinish map");
        return NULL;
      } else if (token->ContentMatch("}")) {
        // end of map
        token_vector->PopToken();
        return new SourceMap(source_keys, source_values);
      }

      Source *tmp_key = GetSource(token_vector, error);
      if (error->HasErrors()) {
        source_keys.clearVector();
        source_values.clearVector();
        return NULL;
      }

      if (!token_vector->PopNextTokenIfContentIs(":")) {
        error->AddError(au::str("Wrong map format (expected ':', got '%s')",
                                token_vector->GetNextTokenContent().c_str()));
        source_keys.clearVector();
        source_values.clearVector();
        return NULL;
      }

      Source *tmp_value = GetSource(token_vector, error);
      if (error->HasErrors()) {
        source_keys.clearVector();
        source_values.clearVector();
        return NULL;
      }

      // Accumulate components for this map
      source_keys.push_back(tmp_key);
      source_values.push_back(tmp_value);

      // Read the mandatory "," if it is not the end of the map
      if (!token_vector->CheckNextTokenContentIs("}")) {
        if (!token_vector->PopNextTokenIfContentIs(",")) {
          error->AddError(
            au::str("Wrong map format (expected ',', got '%s')",
                    token_vector->GetNextTokenContent().c_str()));
          source_keys.clearVector();
          source_values.clearVector();
          return NULL;
        }
      }
    }
  }

  // ----------------------------------------------------------------
  // Functions
  // ----------------------------------------------------------------

  // Generic function interface
  if (token_vector->CheckNextNextTokenContentIs("(")) {
    // Recover the name of the function
    au::token::Token *token_function_name = token_vector->PopToken();
    if (!token_function_name->IsNormal()) {
      error->AddError("Invalid function name");
      return NULL;
    }

    token_vector->PopToken();   // Skip "("

    au::vector<Source> source_components;

    while (true) {
      au::token::Token *token = token_vector->GetNextToken();
      if (!token) {
        source_components.clearVector();
        error->AddError("Unfinish function call");
        return NULL;
      } else if (token->ContentMatch(")")) {
        // end of vector
        token_vector->PopToken();   // skip ")"
        return SourceFunction::GetSourceForFunction(token_function_name->content(), source_components, error);
      } else if ((source_components.size() == 0) || (token->ContentMatch(","))) {
        if ((source_components.size() == 0) && token->ContentMatch(",")) {
          error->AddError("Invalid first parameter for function call");
          source_components.clearVector();
          return NULL;
        }

        if (token->ContentMatch(",")) {
          token_vector->PopToken();   // Skip ","
        }
        // Another component
        Source *tmp = GetSource(token_vector, error);
        if (error->HasErrors()) {
          source_components.clearVector();
          return NULL;
        } else {
          source_components.push_back(tmp);
        }
      } else {
        error->AddError(au::str("Invalid function call. Found '%s' when expecting ')' or ','", token->content().c_str()));
        return NULL;
      }
    }
  }

  // ---------------------------------------------------------
  // Get the next element key, value , key[X]:[name]
  // ---------------------------------------------------------

  au::token::Token *token = token_vector->PopToken();
  if (!token) {
    error->AddError("Missing element");
    return NULL;
  }

  // Literal Constant
  if (token->IsLiteral()) {
    return new SourceStringConstant(token->content());
  }

  // Key-word
  Source *main = NULL;

  if (token->ContentMatch("key")) {
    main = new SourceKey();
  }
  if (token->ContentMatch("value")) {
    main = new SourceValue();
  }

  // ---------------------------------------------------------
  // key or value
  // ---------------------------------------------------------
  if (main) {
    while (true) {
      if (token_vector->PopNextTokenIfContentIs("[")) {
        Source *index = GetSource(token_vector, error);
        if (error->HasErrors()) {
          delete main;
          return NULL;
        }

        if (!token_vector->PopNextTokenIfContentIs("]")) {
          delete main;

          std::string token_content = token_vector->GetNextTokenContent();
          error->AddError(au::str("Wrong index for vector. Expeted ] and found %s", token_content.c_str()));
          return NULL;
        }

        // Cumulative source vectors
        main = new SourceVectorComponent(main, index);
      } else if (token_vector->PopNextTokenIfContentIs(".")) {
        Source *index = GetSource(token_vector, error);
        if (error->HasErrors()) {
          delete main;
          return NULL;
        }

        // Cumulative source vectors
        main = new SourceMapComponent(main, index);
      } else {
        return main;
      }
    }
  }
  // Negative numbers
  if (token->ContentMatch("-")) {
    // Get next token that has to be a number
    au::token::Token *next_token = token_vector->PopToken();
    if (!next_token) {
      error->AddError(au::str("Expected a number after '-' sign but found nothing"));
      return NULL;
    }

    if (!token->IsNumber()) {
      error->AddError(au::str("Expected a number after '-' sign but found '%s'", token->content().c_str()));
      return NULL;
    }

    return new SourceNumberConstant(-atof(next_token->content().c_str()));
  }

  // ---------------------------------------------------------
  // Constant
  // ---------------------------------------------------------
  if (token->IsNumber()) {
    return new SourceNumberConstant(atof(token->content().c_str()));
  } else {
    return new SourceStringConstant(token->content());
  }
}

Source *GetSource(au::token::TokenVector *token_vector, au::ErrorManager *error) {
  Source *source = NULL;

  while (true) {
    if (!source) {
      source = GetSingleSource(token_vector, error);
      if (!source || error->HasErrors()) {
        return NULL;
      }
    }

    // Check if there is something to continue "< > <= >= != + - * /
    au::token::Token *token = token_vector->GetNextToken();
    if (!token) {
      return source;   // No more tokens
    }
    if (token->IsComparator()) {
      // Skip the comparator
      std::string comparator = token->content();
      token_vector->PopToken();

      Source *_source = GetSingleSource(token_vector, error);
      if (!_source || error->HasErrors()) {
        delete source;
        return NULL;
      }

      // Source Comparision
      SourceCompare::Comparisson c = SourceCompare::comparition_from_string(comparator);
      source = new SourceCompare(source, _source, c);

      // Search for more operations...
      continue;
    }

    if (token->IsOperation()) {
      // Skip the comparator
      std::string operation_string = token->content();
      token_vector->PopToken();

      Source *_source = GetSingleSource(token_vector, error);
      if (!_source || error->HasErrors()) {
        delete source;
        return NULL;
      }

      // Source Comparision
      SourceOperation::Operation o = SourceOperation::operation_from_string(operation_string);
      source = new SourceOperation(source, _source, o);

      // Search for more operations...
      continue;
    }

    if (token_vector->PopNextTokenIfContentIs("?")) {
      Source *first_source = GetSingleSource(token_vector, error);
      if (!first_source || error->HasErrors()) {
        delete source;
        return NULL;
      }

      if (!token_vector->PopNextTokenIfContentIs(":")) {
        delete source;
        delete first_source;
        error->AddError(
          au::str("Statement '?' without ':'. Expected ':' but found %s",
                  token_vector->GetNextTokenContent().c_str()));
        return NULL;
      }

      Source *second_source = GetSingleSource(token_vector, error);
      if (!second_source || error->HasErrors()) {
        delete source;
        return NULL;
      }

      // Create a source X ? X : X
      source = new SourceCompareSelector(source, first_source, second_source);
    }

    return source;
  }
}
}
}
