#include "au/tables/Table.h"
#include "gtest/gtest.h"


TEST(auTableTest, simple) {
  au::tables::TableCell cell1("val1");

  EXPECT_EQ(cell1.str(), "val1") << "Error in TableCell.str()";

  au::tables::TableCell cell2(&cell1);
  EXPECT_EQ(cell2.str(), "val1") << "Error in TableCell.str()";

  au::StringVector str_vector("str_val1", "str_val2");
  au::tables::TableCell cell3(str_vector);
  cell1.addFrom(&cell3);
  EXPECT_EQ(cell3.str(), "[ str_val1 str_val2 ]") << "Error in TableCell.str()";

  EXPECT_EQ(cell1.compare("str1", "str1", au::tables::format_string), 0) << "Error in TableCell::compare() string true";
  EXPECT_NE(cell1.compare("str1", "str2",
                          au::tables::format_string), 0) << "Error in TableCell::compare() string false";
  EXPECT_EQ(cell1.compare("1.0", "1.00", au::tables::format_double), 0) << "Error in TableCell::compare() double true";
  EXPECT_NE(cell1.compare("1.0", "2.00", au::tables::format_double), 0) << "Error in TableCell::compare() double false";

  EXPECT_NE(cell2.compare(&cell3, au::tables::format_string), 0) << "Error in TableCell::compare() string false";
  EXPECT_NE(cell2.compare(&cell3, au::tables::format_double), 0) << "Error in TableCell::compare() double false";

  au::StringVector columns("col1", "col2", "col3");
  au::StringVector values("val1", "val2", "val3");

  au::tables::TableRow empty_row(au::tables::TableRow::normal);
  au::tables::TableRow row1(columns, values);
  au::tables::TableRow row2(&row1);

  EXPECT_EQ(row1.compare(&row2, columns), 0) << "Error in TableRow::compare() true";
  EXPECT_EQ(row1.compare(&row2, values), 0) << "Error in TableRow::compare() true";

  row1.set("col4", "val4");
  row1.set("col1", "new_val1");

  au::tables::TableCell *cell_ref = row1.get("col1");
  EXPECT_EQ(cell_ref->str(), "new_val1") << "Error in TableRow.get()";
  EXPECT_EQ(row1.getValue("col4"), "val4") << "Error in TableRow.getValue()";

  au::tables::Table *table = row1.getTable();
  table->setTitle("test_table");
  EXPECT_EQ(
    table->str(),
    "/---------------------\\\n| test_table          |\n|---------------------|\n| Property |    Value | \n|----------+----------|\n|     col1 | new_val1 | \n|     col2 |     val2 | \n|     col3 |     val3 | \n|     col4 |     val4 | \n\\---------------------/\n")
  << "Error in Table.str()";
  EXPECT_EQ(
    table->str_xml(),
    "<table><title><![CDATA[test_table]]></title><item><property><name><![CDATA[Property]]></name><value><![CDATA[col1]]></value></property><property><name><![CDATA[Value]]></name><value><![CDATA[new_val1]]></value></property></item><item><property><name><![CDATA[Property]]></name><value><![CDATA[col2]]></value></property><property><name><![CDATA[Value]]></name><value><![CDATA[val2]]></value></property></item><item><property><name><![CDATA[Property]]></name><value><![CDATA[col3]]></value></property><property><name><![CDATA[Value]]></name><value><![CDATA[val3]]></value></property></item><item><property><name><![CDATA[Property]]></name><value><![CDATA[col4]]></value></property><property><name><![CDATA[Value]]></name><value><![CDATA[val4]]></value></property></item></table>")
  << "Error in Table.str_xml()";
  EXPECT_EQ(
    table->str_json(),
    "{\"title\":\"test_table\",\"items\":[{\"Property\":\"col1\",\"Value\":\"new_val1\"},{\"Property\":\"col2\",\"Value\":\"val2\"},{\"Property\":\"col3\",\"Value\":\"val3\"},{\"Property\":\"col4\",\"Value\":\"val4\"}]}")
  << "Error in Table.str_json()";
  EXPECT_EQ(
    table->str_html(),
    "<style>#table-5{font-family:\"Lucida Sans Unicode\", \"Lucida Grande\", Sans-Serif;font-size:12px;background:#fff;border-collapse:collapse;text-align:left;margin:20px;}#table-5 th{font-size:14px;font-weight:normal;color:#039;border-bottom:2px solid #6678b1;padding:10px 8px;}#table-5 td{color:#669;padding:9px 8px 0;}#table-5 tbody tr:hover td{color:#009;}</style><table id=\"table-5\"><tr><td colspan=\"2\"><h1>test_table</h1></td></tr><th>Property</th><th>Value</th><tr><td>col1</td><td>new_val1</td></tr><tr><td>col2</td><td>val2</td></tr><tr><td>col3</td><td>val3</td></tr><tr><td>col4</td><td>val4</td></tr></table>")
  << "Error in Table.str_html()";

//    std::vector<size_t> length;
//    length.push_back(10);
//    length.push_back(20);
//    length.push_back(10);
//
//    EXPECT_EQ(table->top_line(length), "/---------------------\\\n| test_table          |\n|---------------------|\n| Property |    Value | \n|----------+----------|\n|     col1 | new_val1 | \n|     col2 |     val2 | \n|     col3 |     val3 | \n|     col4 |     val4 | \n\\---------------------/\n") << "Error in Table.top_line()";
//    EXPECT_EQ(table->top_line2(&length), "/---------------------\\\n| test_table          |\n|---------------------|\n| Property |    Value | \n|----------+----------|\n|     col1 | new_val1 | \n|     col2 |     val2 | \n|     col3 |     val3 | \n|     col4 |     val4 | \n\\---------------------/\n") << "Error in Table.top_line2()";
//    EXPECT_EQ(table->bottom_line(&length), "/---------------------\\\n| test_table          |\n|---------------------|\n| Property |    Value | \n|----------+----------|\n|     col1 | new_val1 | \n|     col2 |     val2 | \n|     col3 |     val3 | \n|     col4 |     val4 | \n\\---------------------/\n") << "Error in Table.bottom_line()";
//    EXPECT_EQ(table->line(&length), "/---------------------\\\n| test_table          |\n|---------------------|\n| Property |    Value | \n|----------+----------|\n|     col1 | new_val1 | \n|     col2 |     val2 | \n|     col3 |     val3 | \n|     col4 |     val4 | \n\\---------------------/\n") << "Error in Table.bottom_line()";
//
//    EXPECT_EQ(table->string_length("test string", 10, true), "test_strin") << "Error in Table.string_length";
//    EXPECT_EQ(table->string_length("test string", 10, false), "est_string") << "Error in Table.string_length";
//    EXPECT_EQ(table->string_length("test string", 11, true), "test_string") << "Error in Table.string_length";
//    EXPECT_EQ(table->string_length("test string", 12, true), "test_string") << "Error in Table.string_length";

  au::tables::Table *description = table->getColumnDescriptionTable();
  EXPECT_EQ(
    description->str(),
    "/--------------------------------\\\n|--------------------------------|\n|   Record |    Title |   Format | \n|----------+----------+----------|\n| Property | Property | Property | \n|    Value |    Value |    Value | \n\\--------------------------------/\n")
  << "Error in Table.getColumnDescriptionTable()";

  au::StringVector all_columns = table->getColumnNames();
  table->sort(all_columns);
  EXPECT_EQ(
    table->str(),
    "/---------------------\\\n| test_table          |\n|---------------------|\n| Property |    Value | \n|----------+----------|\n|     col4 |     val4 | \n|     col3 |     val3 | \n|     col2 |     val2 | \n|     col1 | new_val1 | \n\\---------------------/\n")
  << "Error in Table.sort()";

  table->reverseRows();
  EXPECT_EQ(
    table->str(),
    "/---------------------\\\n| test_table          |\n|---------------------|\n| Property |    Value | \n|----------+----------|\n|     col1 | new_val1 | \n|     col2 |     val2 | \n|     col3 |     val3 | \n|     col4 |     val4 | \n\\---------------------/\n")
  << "Error in Table.sort()";


  au::tables::Table empty_table("Empty Table");
  EXPECT_EQ(empty_table.getValue(1, 1), "") << "Error in Table.getValue";

  au::tables::Table new_table(table);
  EXPECT_EQ(new_table.getValue(1, 1), "val2") << "Error in Table.getValue";
  EXPECT_EQ(new_table.getColumn("Value"), 1) << "Error in Table.getColumn";
  EXPECT_EQ(new_table.getColumn("Non_Existing"), static_cast<size_t>(-1)) << "Error in Table.getColumn";
  EXPECT_EQ(new_table.getColumn(1), "Value") << "Error in Table.getColumn";
  EXPECT_EQ(new_table.getColumn(10), "") << "Error in Table.getColumn";

  EXPECT_EQ(new_table.getFormatForColumn(1), "Value") << "Error in Table.getFormatForColumn";
  EXPECT_EQ(new_table.getFormatForColumn(10), "") << "Error in Table.getFormatForColumn";

  au::StringVector result_columns = new_table.getColumnNames();
  EXPECT_EQ(result_columns.str(), "Property Value") << "Error in Table.getColumnNames";

  au::StringVector result_values = new_table.getValuesFromColumn("Values");
  EXPECT_EQ(result_values.str(), "   ") << "Error in Table.getValuesFromColumn";

//    au::tables::TableColumn *column = new_table.getSelectColumn("Values");
//    au::tables::TableColumn *new_column = new_table.getSelectColumn("New Column");
//
//    EXPECT_EQ(new_column->getTitle(), "") << "Error in TableColumn.getTitle";
//    EXPECT_EQ(new_column->getName(), "New Column") << "Error in TableColumn.getName";
}
