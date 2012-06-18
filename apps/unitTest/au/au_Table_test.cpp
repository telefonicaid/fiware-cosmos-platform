#include "gtest/gtest.h"
#include "au/tables/Table.h"


TEST(auTableTest, simple )
{
    au::tables::TableCell cell("val1");

    EXPECT_EQ( cell.str(), "val1") << "Error in TableCell.str()";



}
