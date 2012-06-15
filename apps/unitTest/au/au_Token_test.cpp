#include "gtest/gtest.h"
#include "au/Tokenizer.h"


TEST(auTokenTest, simple )
{
    au::Token token("test_token", au::normal, 0);
    au::Token token_op("test_token_op", au::literal, 1);
    au::Token token_num("+3.27", au::literal, 2);
    au::Token token_num_wrong("+3.27.wrong", au::literal, 2);

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
    EXPECT_EQ( token.token_num(), true) << "Error in token.isNumber() true";
    EXPECT_EQ( token.token_num_wrong(), false) << "Error in token.isNumber() false";
}
