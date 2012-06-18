#include "gtest/gtest.h"
#include "au/Tokenizer.h"


TEST(auTokenTest, simple )
{
    au::token::Token token("test_token", au::token::Token::normal, 0);
    au::token::Token token_op("*", au::token::Token::literal, 1);
    au::token::Token token_num("+3.27", au::token::Token::literal, 2);
    au::token::Token token_num_wrong("+3.27.wrong", au::token::Token::literal, 2);
    au::token::Token token_separator_semicolon(";", au::token::Token::separator, 2);

    EXPECT_EQ( token.is("test_token"), true) << "Error in token.is() true";
    EXPECT_EQ( token.is("wrong_token"), false) << "Error in token.is() false";
    
    EXPECT_EQ( token.isLiteral(), false) << "Error in token.isLiteral() false";
    EXPECT_EQ( token_op.isLiteral(), true) << "Error in token.isLiteral() true";
    EXPECT_EQ( token.isSeparator(), false) << "Error in token.isSeparator() ";
    EXPECT_EQ( token.isNormal(), true) << "Error in token.isNormal() ";
    
    EXPECT_EQ( token.isOperation(), false) << "Error in token.isOperation() ";
    EXPECT_EQ( token_op.isOperation(), true) << "Error in token.isOperation() true";
    
    EXPECT_EQ( token.isComparator(), false) << "Error in token.isComparator() ";
    
    EXPECT_EQ( token.isNumber(), false) << "Error in token.isNumber() false";
    EXPECT_EQ( token_num.isNumber(), true) << "Error in token.isNumber() true";
    EXPECT_EQ( token_num_wrong.isNumber(), false) << "Error in token.isNumber() false";

    EXPECT_EQ( token.str(), "test_token")  << "Error in token.str() normal";
    EXPECT_EQ( token_op.str(), "\"*\"")  << "Error in token.str() literal";
    EXPECT_EQ( token_separator_semicolon.str(), "<;>")  << "Error in token.str() separator";

    au::token::TokenVector token_vector;

    EXPECT_EQ( token_vector.getNextTokenContent(), "<empty>") << "Error in getNextTokenContent empty";

    au::token::Tokenizer tokenizer;

    tokenizer.addSingleCharTokens ("-_");
    tokenizer.addToken("next");

    au::token::TokenVector vector_tokens = tokenizer.parse ("Test string to tokenizer:literals: \"true\", digits: 2.1, ops: +");


    EXPECT_EQ(vector_tokens.str(), "( 3 tokens ) Test string to tokenizer:literals:  \\\"true\\\" , digits: 2.1, ops: + ") << "Error in Tokenizer";






}
