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

  EXPECT_TRUE(token.ContentMatch("test_token")) << "Error in token.is() true";
  EXPECT_FALSE(token.ContentMatch("wrong_token")) << "Error in token.is() false";

  EXPECT_FALSE(token.IsLiteral()) << "Error in token.isLiteral() false";
  EXPECT_TRUE(token_op.IsLiteral()) << "Error in token.isLiteral() true";
  EXPECT_FALSE(token.IsSeparator()) << "Error in token.isSeparator() ";
  EXPECT_TRUE(token.IsNormal()) << "Error in token.isNormal() ";

  EXPECT_FALSE(token.IsOperation()) << "Error in token.isOperation() ";
  EXPECT_TRUE(token_op.IsOperation()) << "Error in token.isOperation() true";

  EXPECT_FALSE(token.IsComparator()) << "Error in token.isComparator() ";

  EXPECT_FALSE(token.IsNumber()) << "Error in token.isNumber() false";
  EXPECT_TRUE(token_num.IsNumber()) << "Error in token.isNumber() true";
  EXPECT_FALSE(token_num_wrong.IsNumber()) << "Error in token.isNumber() false";

  EXPECT_EQ(token.str(), "test_token") << "Error in token.str() normal";
  EXPECT_EQ(token_op.str(), "\"*\"") << "Error in token.str() literal";
  EXPECT_EQ(token_separator_semicolon.str(), "<;>") << "Error in token.str() separator";

  au::token::TokenVector token_vector;

  EXPECT_EQ(token_vector.GetNextTokenContent(), "<empty>") << "Error in GetNextTokenContent empty";
  EXPECT_FALSE(token_vector.PopNextTokenIfContentIs("token1")) << "Error in PopNextTokenIfContentIs false";
  EXPECT_FALSE(token_vector.PopNextTwoTokensIfContentsAre("token1",
                                                          "token2")) << "Error in popNextTokensIfTheyAre false";
  EXPECT_FALSE(token_vector.CheckNextTokenContentIs("token1")) << "Error in CheckNextTokenContentIs false";
  EXPECT_FALSE(token_vector.CheckNextNextTokenContentIs("token1")) << "Error in CheckNextNextTokenContentIs false";

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
  EXPECT_FALSE(token_vector.PopNextTokenIfContentIs("next")) << "Error in PopNextTokenIfContentIs false";
  EXPECT_FALSE(token_vector.PopNextTwoTokensIfContentsAre("next",
                                                          "previous")) << "Error in popNextTokensIfTheyAre false";
  EXPECT_FALSE(token_vector.CheckNextTokenContentIs("next")) << "Error in CheckNextTokenContentIs false";
  EXPECT_FALSE(token_vector.CheckNextNextTokenContentIs("next")) << "Error in CheckNextNextTokenContentIs false";
}
