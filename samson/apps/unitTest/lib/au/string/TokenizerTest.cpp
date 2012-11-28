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

  EXPECT_EQ(token.IsContent("test_token"), true) << "Error in token.is() true";
  EXPECT_EQ(token.IsContent("wrong_token"), false) << "Error in token.is() false";

  EXPECT_EQ(token.IsLiteral(), false) << "Error in token.isLiteral() false";
  EXPECT_EQ(token_op.IsLiteral(), true) << "Error in token.isLiteral() true";
  EXPECT_EQ(token.IsSeparator(), false) << "Error in token.isSeparator() ";
  EXPECT_EQ(token.IsNormal(), true) << "Error in token.isNormal() ";

  EXPECT_EQ(token.IsOperation(), false) << "Error in token.isOperation() ";
  EXPECT_EQ(token_op.IsOperation(), true) << "Error in token.isOperation() true";

  EXPECT_EQ(token.IsComparator(), false) << "Error in token.isComparator() ";

  EXPECT_EQ(token.IsNumber(), false) << "Error in token.isNumber() false";
  EXPECT_EQ(token_num.IsNumber(), true) << "Error in token.isNumber() true";
  EXPECT_EQ(token_num_wrong.IsNumber(), false) << "Error in token.isNumber() false";

  EXPECT_EQ(token.str(), "test_token") << "Error in token.str() normal";
  EXPECT_EQ(token_op.str(), "\"*\"") << "Error in token.str() literal";
  EXPECT_EQ(token_separator_semicolon.str(), "<;>") << "Error in token.str() separator";

  au::token::TokenVector token_vector;

  EXPECT_EQ(token_vector.GetNextTokenContent(), "<empty>") << "Error in GetNextTokenContent empty";
  EXPECT_EQ(token_vector.PopNextTokenIfContentIs("token1"), false) << "Error in PopNextTokenIfContentIs false";
  EXPECT_EQ(token_vector.PopNextTwoTokensContentsAre("token1",
                                                     "token2"), false) << "Error in popNextTokensIfTheyAre false";
  EXPECT_EQ(token_vector.CheckNextTokenContentIs("token1"), false) << "Error in CheckNextTokenContentIs false";
  EXPECT_EQ(token_vector.CheckNextNextTokenContentIs("token1"), false) << "Error in CheckNextNextTokenContentIs false";

  au::token::Tokenizer tokenizer;

  tokenizer.AddSingleCharTokens("-_");
  tokenizer.AddToken("next");
  tokenizer.AddToken("previous");

  au::token::TokenVector vector_tokens = tokenizer.Parse(
    "Test string to tokenizer:literals: \"true\" and next:'false' , digits: 2.1, ops: +");


  EXPECT_EQ(
    vector_tokens.str(),
    "( 7 tokens ) Test string to tokenizer:literals:  \"true\"  and  <next> : \"false\"  , digits: 2.1, ops: + ") <<
  "Error in Tokenizer";


  EXPECT_EQ(token_vector.GetNextTokenContent(), "<empty>") << "Error in GetNextTokenContent non empty";
  EXPECT_EQ(token_vector.PopNextTokenIfContentIs("next"), false) << "Error in PopNextTokenIfContentIs false";
  EXPECT_EQ(token_vector.PopNextTwoTokensContentsAre("next",
                                                     "previous"), false) << "Error in popNextTokensIfTheyAre false";
  EXPECT_EQ(token_vector.CheckNextTokenContentIs("next"), false) << "Error in CheckNextTokenContentIs false";
  EXPECT_EQ(token_vector.CheckNextNextTokenContentIs("next"), false) << "Error in CheckNextNextTokenContentIs false";
}
