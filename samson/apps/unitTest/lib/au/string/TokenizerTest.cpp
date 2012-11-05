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
#include "au/string/Tokenizer.h"
#include "gtest/gtest.h"


TEST(au_Tokenizer, simple) {
  au::token::Token token("test_token", au::token::Token::normal, 0);
  au::token::Token token_op("*", au::token::Token::literal, 1);
  au::token::Token token_num("+3.27", au::token::Token::literal, 2);
  au::token::Token token_num_wrong("+3.27.wrong", au::token::Token::literal, 2);
  au::token::Token token_separator_semicolon(";", au::token::Token::separator, 2);

  EXPECT_EQ(token.is("test_token"), true) << "Error in token.is() true";
  EXPECT_EQ(token.is("wrong_token"), false) << "Error in token.is() false";

  EXPECT_EQ(token.isLiteral(), false) << "Error in token.isLiteral() false";
  EXPECT_EQ(token_op.isLiteral(), true) << "Error in token.isLiteral() true";
  EXPECT_EQ(token.isSeparator(), false) << "Error in token.isSeparator() ";
  EXPECT_EQ(token.isNormal(), true) << "Error in token.isNormal() ";

  EXPECT_EQ(token.isOperation(), false) << "Error in token.isOperation() ";
  EXPECT_EQ(token_op.isOperation(), true) << "Error in token.isOperation() true";

  EXPECT_EQ(token.isComparator(), false) << "Error in token.isComparator() ";

  EXPECT_EQ(token.isNumber(), false) << "Error in token.isNumber() false";
  EXPECT_EQ(token_num.isNumber(), true) << "Error in token.isNumber() true";
  EXPECT_EQ(token_num_wrong.isNumber(), false) << "Error in token.isNumber() false";

  EXPECT_EQ(token.str(), "test_token") << "Error in token.str() normal";
  EXPECT_EQ(token_op.str(), "\"*\"") << "Error in token.str() literal";
  EXPECT_EQ(token_separator_semicolon.str(), "<;>") << "Error in token.str() separator";

  au::token::TokenVector token_vector;

  EXPECT_EQ(token_vector.getNextTokenContent(), "<empty>") << "Error in getNextTokenContent empty";
  EXPECT_EQ(token_vector.popNextTokenIfItIs("token1"), false) << "Error in popNextTokenIfItIs false";
  EXPECT_EQ(token_vector.popNextTokensIfTheyAre("token1", "token2"), false) << "Error in popNextTokensIfTheyAre false";
  EXPECT_EQ(token_vector.checkNextTokenIs("token1"), false) << "Error in checkNextTokenIs false";
  EXPECT_EQ(token_vector.checkNextNextTokenIs("token1"), false) << "Error in checkNextNextTokenIs false";

  au::token::Tokenizer tokenizer;

  tokenizer.addSingleCharTokens("-_");
  tokenizer.addToken("next");
  tokenizer.addToken("previous");

  au::token::TokenVector vector_tokens = tokenizer.parse(
    "Test string to tokenizer:literals: \"true\" and next:'false' , digits: 2.1, ops: +");


  EXPECT_EQ(
    vector_tokens.str(),
    "( 7 tokens ) Test string to tokenizer:literals:  \"true\"  and  <next> : \"false\"  , digits: 2.1, ops: + ") <<
  "Error in Tokenizer";


  EXPECT_EQ(token_vector.getNextTokenContent(), "<empty>") << "Error in getNextTokenContent non empty";
  EXPECT_EQ(token_vector.popNextTokenIfItIs("next"), false) << "Error in popNextTokenIfItIs false";
  EXPECT_EQ(token_vector.popNextTokensIfTheyAre("next", "previous"), false) << "Error in popNextTokensIfTheyAre false";
  EXPECT_EQ(token_vector.checkNextTokenIs("next"), false) << "Error in checkNextTokenIs false";
  EXPECT_EQ(token_vector.checkNextNextTokenIs("next"), false) << "Error in checkNextNextTokenIs false";
}
