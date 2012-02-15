
#include "Select.h"

#include "Table.h" // Own interterface


NAMESPACE_BEGIN(au)
NAMESPACE_BEGIN(tables)

TableRow::TableRow( StringVector _columns , StringVector _values )
{
    for( size_t i = 0 ; i < std::min( _columns.size() , _values.size() ) ; i++ )
        values.insertInMap(_columns[i], _values[i]);
}

TableRow::TableRow( TableRow* row )
{
    au::simple_map<std::string,std::string>::iterator it_values;
    for( it_values = row->values.begin() ; it_values !=row->values.end() ; it_values++)
        set( it_values->first , it_values->second );
}

void TableRow::set( std::string name , std::string value )
{
    values.insertInMap(name, value);
}

std::string TableRow::get( std::string name )
{
    if( values.isInMap( name ) )
        return values.findInMap( name );
    else
        return "";
}

// Function to compare two rows
int TableRow::compare( TableRow* row , StringVector &sort_columns )
{
    for ( size_t i = 0 ; i < sort_columns.size() ; i++ )
    {
        std::string column = sort_columns[i];
        
        std::string value = get( column );
        std::string value2 = row->get( column );
        
        if( value < value2 )
            return -1;
        if ( value > value2 )
            return 1;
    }
    return 0;
}

Table* TableRow::getTable()
{
    Table* table = new Table( StringVector( "Property" , "Value" ) );
    
    au::simple_map<std::string,std::string>::iterator it_values;
    for ( it_values = values.begin() ; it_values != values.end() ; it_values++ )
        table->addRow( StringVector( it_values->first , it_values->second ) );

    return table;
}



#pragma mark Table

Table::Table( StringVector _columns , StringVector _formats )
{
    columns.copyFrom( _columns );
    formats.copyFrom( _formats );
}


Table::Table( StringVector _columns )
{
    columns.copyFrom( _columns );
    for( size_t i = 0 ; i < columns.size() ; i++ )
        formats.push_back(""); // No format
}

Table::Table( Table* table )
{
    // Copy columns
    columns.copyFrom( table->columns );

    // Copy records
    for( size_t r = 0 ; r < table->rows.size() ; r++ )
        rows.push_back( new TableRow( table->rows[r] ) );
}

Table::~Table()
{
    // Delete rows
    for ( size_t r = 0 ; r < rows.size() ; r++ )
        delete rows[r];
    rows.clear();
}

std::string Table::getValue( size_t r , size_t c )
{
    if( ( r == (size_t)-1) || ( c == (size_t)-1) )
        return "";
    
    if ( r > rows.size() )
        return "";
    
    return rows[r]->get( columns[c] );
}

std::string Table::getValue( int r , std::string column_name )
{
    return getValue(r, getColumn(column_name) );
}

size_t Table::getColumn( std::string title )
{
    LM_M(("SEARCH %s in %lu" , title.c_str() , columns.size() ));
    
    for( size_t i = 0 ; i < columns.size() ; i++ )
    {
        LM_M(("Comparing %s %s" , columns[i].c_str() , title.c_str() ));
        
        if( columns[i] == title )
            return i;
    }
    
    return (size_t)-1;
}

std::string Table::getColumn( size_t pos )
{
    if( pos >= columns.size() )
        return "";
    
    return columns[pos];
}

std::string Table::getFormatForColumn( size_t pos )
{
    if( pos >= formats.size() )
        return "";
    
    return formats[pos];
}

void Table::addRow( StringVector string_vector )
{
    addRow( new TableRow( columns , string_vector ) );
}

void Table::addRow( TableRow * row )
{
    rows.push_back(row);
}

std::string Table::str( )
{
    SelectTableInformation* print_table_information = new SelectTableInformation( this );
    std::string result =  str( print_table_information );
    delete print_table_information;
    
    return result;
}

std::string Table::str( std::string title )
{
    // Select all the columns
    SelectTableInformation* select = new SelectTableInformation( this );
    select->title = title;
    
    std::string result =  str( select );
    delete select;
    
    return result;
}

std::string Table::strSortedAndGrouped( std::string title , std::string field)
{
    // Select all the columns
    SelectTableInformation* select = new SelectTableInformation( this );
    select->title = title;
    
    select->group_columns.push_back(field);
    select->sort_columns.push_back(field);
    
    std::string result =  str( select );
    delete select;
    
    return result;
    
}


Table* Table::selectTable( SelectTableInformation *select_table_information )
{    
    
    // Sort by given criteria
    sort( select_table_information->group_columns );

    // Vector with all pre-selected rows
    std::vector<TableRow*> selected_rows;
    for( size_t r = 0 ; r < rows.size() ; r++ )
        if( select_table_information->check( rows[r] ) )
            selected_rows.push_back(rows[r]);

    
    StringVector columns = select_table_information->getColumns();
    Table *table = new Table( columns );
    
    size_t row_begin = 0;
    
    while( row_begin < selected_rows.size() )
    {
        // Determine the end of this group of rows
        size_t row_end = row_begin+1;
        
        if( select_table_information->isGroupRequired() )
            while (( row_end < selected_rows.size()) 
                   && ( selected_rows[row_begin]->compare(selected_rows[row_end], select_table_information->group_columns ) == 0 ))
                    row_end++;
        
        // Create a record for this group of rows....
        // ---------------------------------------------------------------
        StringVector values;
        
        for( size_t c = 0 ; c < select_table_information->columns.size() ; c++)
        {
            // Get position of this column
            std::string column = select_table_information->columns[c].getName();
            
            // Collection of values ( one per row involved in this group )
            StringVector tmp_values;
            for( size_t r = row_begin ; r < row_end ; r++ )
                tmp_values.push_back( selected_rows[r]->get( column ) );
            
            // Final value combining all values in the group
            std::string value = select_table_information->columns[c].transform( tmp_values );

            // Accumulate this value
            values.push_back(value);
        }    

        // Create the row and add to the table
        TableRow *new_row = new TableRow( columns , values );
        table->addRow( new_row );
        
        
        // Move to the next rows
        row_begin = row_end;
    }
    
    return table;
    
}

std::string Table::str( SelectTableInformation *select )
{
    // Select the fields grouped ( if required )
    Table* table = selectTable( select );

    if( select->isDivideRequired() )
        table->sort( select->divide_columns );
    else if ( select->isSortRequired() )
        table->sort( select->sort_columns );
    
    // Compute max widths per column
    size_t title_length = 0;
    std::vector<size_t> column_width;
    for( size_t c = 0 ; c < select->columns.size() ; c++)
    {
        size_t _column_width = std::max( table->getMaxWidthForColumn( c ) , select->columns[c].getTitle().length() );
        column_width.push_back( _column_width );
        title_length += _column_width;
    }
    title_length += 3*( select->columns.size()-1);

    
    // Check enougth space for the title length
    int pos = 0 ;
    while( select->title.length() > title_length )
    {
        column_width[pos++]++;
        if( pos >= (int)column_width.size() )
            pos = 0;
        title_length++;
    }
    
    // Output string
    std::ostringstream output;

    // Separation line
    output << line( column_width );
    
    // Main title...
    if( select->title != "" )
    {
        output << "| ";
        output << string_length( select->title , title_length, true);
        output << " |";
        output << "\n";
    }
    
    // Separation line
    output << line( column_width );
    
    // TITLES
    output << "| ";
    for ( size_t c = 0 ; c < select->columns.size() ; c++)
        output << string_length( select->columns[c].getTitle() , column_width[c] , select->columns[c].getLeft() ) << " | ";
    output << "\n";
        
    
    // Separation line
    output << line( column_width );
    
    // For each record at the input, create an output 
    for( size_t r = 0 ; r < table->rows.size() ; r++)
    {
        // Skip lines if there is a limit...
        if( ( select->limit > 0 ) && ( r > select->limit ) )
            continue;
        
        output << "| ";
        for( size_t c = 0 ; c < select->columns.size() ; c++ )
        {
            std::string column = select->columns[c].getName();
            // Get the formatted output
            output << string_length( table->rows[r]->get(column) , column_width[c] , select->columns[c].getLeft() ) << " | ";
        }
        output << "\n";
        
        // Separation line for divisions
        if ( select->isDivideRequired() )
            if( r != (table->rows.size()-1) )
                if ( select->checkChangeDivision( table->rows[r] , table->rows[r+1] ) )
                    output << line( column_width );
        
    }
    
    // Separation line
    output << line( column_width );
    
    if( select->limit > table->rows.size() )
        output << au::str("%lu rows not displayed ( limit %lu )\n" , table->rows.size() - select->limit , select->limit );
    
    
    return output.str();
}

void Table::sort( StringVector &sort_columns )
{

    // Simple double buble algorithm to sort :(
    for (size_t r = 0 ; r < rows.size() ; r++ )
        for (size_t rr = r+1 ; rr < rows.size() ; rr++ )
        {
            int c = rows[r]->compare(rows[rr], sort_columns );
            if( c < 0 )
            {
                // Exchange r and rr
                TableRow* tmp_row = rows[r];
                rows[r] = rows[rr];
                rows[rr] = tmp_row;
            }
        }    
}



size_t Table::getNumRows()
{
    return rows.size();
}

size_t Table::getNumColumns()
{
    return columns.size();
}

Table* Table::getColumnDescriptionTable()
{
    // Example to play with
    Table *tmp = new Table( StringVector("Record" , "Format" ) );
    
    for( size_t c = 0 ; c < columns.size() ; c++ )
    {
        std::string title = columns[c];
        std::string format = formats[c];
        tmp->addRow( StringVector( title , format ) );
    }
    
    return tmp;
}

std::string Table::line( std::vector<size_t> &length )
{
    std::ostringstream output;
    
    output << "+-";
    // Separation line
    for ( size_t c = 0 ; c < length.size() ; c++)
    {
        for( size_t i = 0 ; i < length[c] ; i++ )
            output << "-";
        
        // Get the formatted output
        if( c != ( length.size()-1) )
            output << "-+-";
    }
    output << "-+";
    output << "\n";
    
    return output.str();
}

std::string Table::string_length( std::string value , int width , bool left )
{
    std::string _value;
    if( (int)value.length() <= width )
        _value = value;
    else
        _value = value.substr( 0 , width-3 ) + "...";
    
    if( left )
        return au::str( au::str("%%-%ds",width).c_str() , _value.c_str() );
    else
        return au::str( au::str("%%%ds",width).c_str() , _value.c_str() );
}

StringVector Table::getValuesFromColumn( std::string name )
{
    StringVector values;
    for ( size_t r = 0 ; r < rows.size() ; r++ )
        values.push_back(  rows[r]->get( name ) );
    
    return values;
}

StringVector Table::getValuesFromColumn( std::string name , SelectCondition* condition )
{
    StringVector values;
    for ( size_t r = 0 ; r < rows.size() ; r++ )
        if( condition->check( rows[r] ) )
            values.push_back(  rows[r]->get( name ) );
    return values;
}

NAMESPACE_END
NAMESPACE_END

