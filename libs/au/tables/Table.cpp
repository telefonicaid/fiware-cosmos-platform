
#include <algorithm>
#include "Select.h"

#include "au/xml.h"

#include "Table.h" // Own interterface

/*
 #define AU_TABLE_H        "-"
 #define AU_TABLE_V        "|"
 #define AU_TABLE_CORNER_LT "+"  // Left top
 #define AU_TABLE_CORNER_RT "+"  // Rigth top
 #define AU_TABLE_CORNER_LB "+"  // Left bottom
 #define AU_TABLE_CORNER_RB "+"  // Rigth bottom
 #define AU_TABLE_X_HD      "+" // Horizontal down
 #define AU_TABLE_X_HU      "+" // Horizontal down
 #define AU_TABLE_X_VR      "+" // Vertial rigth
 #define AU_TABLE_X_VL      "+" // Vertical left
 #define AU_TABLE_X         "+" // Full cross
 */

#define AU_TABLE_H         "-"
#define AU_TABLE_V         "|"
#define AU_TABLE_CORNER_LT "/"  // Left top
#define AU_TABLE_CORNER_RT "\\"  // Rigth top
#define AU_TABLE_CORNER_LB "\\"  // Left bottom
#define AU_TABLE_CORNER_RB "/"  // Rigth bottom
#define AU_TABLE_X_HD      "-" // Horizontal down
#define AU_TABLE_X_HU      "-" // Horizontal down
#define AU_TABLE_X_VR      "|" // Vertial rigths
#define AU_TABLE_X_VL      "|" // Vertical left
#define AU_TABLE_X         "+" // Full cross


/*
 #define AU_TABLE_H         "━"
 #define AU_TABLE_V         "┃"
 #define AU_TABLE_CORNER_LT "┏"  // Left top
 #define AU_TABLE_CORNER_RT "┓"  // Rigth top
 #define AU_TABLE_CORNER_LB "┗"  // Left bottom
 #define AU_TABLE_CORNER_RB "┛"  // Rigth bottom
 #define AU_TABLE_X_HD      "┳" // Horizontal down
 #define AU_TABLE_X_HU      "┻" // Horizontal down
 #define AU_TABLE_X_VR      "┣" // Vertial rigth
 #define AU_TABLE_X_VL      "┫" // Vertical left
 #define AU_TABLE_X         "╋" // Full cross
 */


namespace au 
{
    namespace tables
    {
        
        // ------------------------------------------------------------
        // TableSelectCondition
        // ------------------------------------------------------------
        
        
        bool TableSelectCondition::check( TableRow *row )
        {
            std::string row_value = row->getValue(name);
            return compare_strings( value , row_value );
        }
        
        // ------------------------------------------------------------
        // SelectTableInformation
        // ------------------------------------------------------------
        
        void SelectTableInformation::addColumn( std::string definition )
        {
            columns.push_back( definition );
        }
        
        void SelectTableInformation::addColumns( au::StringVector _columns )
        {
            for ( size_t i = 0 ; i < _columns.size() ; i++ )
                addColumn(_columns[i]);
        }
        
        bool SelectTableInformation::checkChangeDivision( TableRow* row , TableRow* row2)
        {
            return row->compare( row2 , divide_columns ) != 0;
        }
        
        void SelectTableInformation::add_conditions( std::string txt_conditions )
        {
            StringVector where_clauses = StringVector::parseFromString(txt_conditions , ',');
            
            for ( size_t w = 0 ; w < where_clauses.size() ; w++ )
            {
                StringVector where_clause_parts = StringVector::parseFromString(where_clauses[w] , '=');
                if( where_clause_parts.size() == 2)
                {
                    std::string name = where_clause_parts[0];
                    std::string value = where_clause_parts[1];
                    TableSelectCondition condition = TableSelectCondition( name , value );
                    conditions.push_back( condition );
                }
            }
        }
        
        bool SelectTableInformation::check( TableRow *row )
        {
            for ( size_t i = 0 ; i < conditions.size() ; i++ )
                if( !conditions[i].check(row) )
                    return false;
            return true;
        }

        // ------------------------------------------------------------
        // TableCell
        // ------------------------------------------------------------

        
        int TableCell::compare( std::string v1 , std::string v2 , TableColumnFormat format )
        {
            switch ( format ) 
            {
                case format_uint64:
                case format_uint:
                case format_time:
                case format_timestamp:
                case format_double:
                case format_percentadge:
                {
                    
                    
                    double _v1 = strtof( v1.c_str() , (char **)NULL ); 
                    double _v2 = strtof( v2.c_str() , (char **)NULL ); 
                    
                    if ( _v1 < _v2 )
                        return -1;
                    else if ( _v1 > _v2 )
                        return 1;
                    else
                        return 0;
                    break;
                }
                case format_string:
                {
                    
                    
                    if ( v1 < v2 )
                        return -1;
                    else if ( v1 > v2 )
                        return 1;
                    else
                        return 0;
                    break;
                }
            }                
            
            return 0;
        }
        
        
        int TableCell::compare( TableCell* cell , TableColumnFormat format )
        {
            if( values.size() !=  cell->values.size() )
                return values.size() - cell->values.size();
            
            
            for( size_t i = 0 ; i < values.size() ; i ++)
            {
                std::string v1 = values[i];
                std::string v2 = cell->values[i];
                
                int c = compare(v1, v2, format );
                
                if( c != 0 )
                    return c;
            }
            
            return 0;
        }

        
        // ------------------------------------------------------------
        // TableRow
        // ------------------------------------------------------------
        
        TableRow::TableRow( Type _type )
        {
            type = _type;
        }
        
        TableRow::TableRow( StringVector _columns , StringVector _values )
        {
            type = normal;
            for( size_t i = 0 ; i < std::min( _columns.size() , _values.size() ) ; i++ )
                cells.insertInMap(_columns[i],  new TableCell( _values[i] ) );
        }
        
        TableRow::TableRow( TableRow* row )
        {
            au::map<std::string, TableCell >::iterator it_cells;
            for( it_cells = row->cells.begin() ; it_cells !=row->cells.end() ; it_cells++)
            {
                std::string name = it_cells->first;
                cells.insertInMap( name , new TableCell( it_cells->second ) );
            }
        }
        
        int TableRow::compare( TableRow* row , std::vector<std::string> &fields )
        {
            for ( size_t i = 0 ; i < fields.size() ; i++ )
            {
                TableCell *cell1 = get( fields[i] );
                TableCell *cell2 = row->get( fields[i] );
                
                if ( cell1 && !cell2 )
                    return 1;
                if ( !cell1 && cell2 )
                    return -1;
                
                if( cell1 && cell2 )                    
                {
                    int c = cell1->compare(cell2, format_string );
                    if( c != 0 )
                        return c;
                }
            }
            return 0;
        }
        
        void TableRow::set( std::string name , std::string value )
        {
            TableCell* cell = cells.extractFromMap(name);
            if( cell )
                delete cell;
                
            cells.insertInMap( name ,  new TableCell( value ) );
        }
        
        void TableRow::set( std::string name , TableCell* cell )
        {
            TableCell* old_cell = cells.extractFromMap(name);
            if( old_cell )
                delete old_cell;
            
            cells.insertInMap( name ,  cell );
        }

        // Get a particular cell
        TableCell* TableRow::get( std::string name )
        {
            return cells.findInMap(name);
        }
        
        // Get single value
        std::string TableRow::getValue( std::string name )
        {
            TableCell* cell = cells.findInMap(name);

            if( cell )
                return cell->str();
            else
                return "";
        }
        
        TableRow::Type TableRow::getType()
        {
            return type;
        }
        
        
        Table* TableRow::getTable()
        {
            Table* table = new Table( StringVector( "Property" , "Value" ) );
            
            au::map<std::string, TableCell>::iterator it_cells;
            for ( it_cells = cells.begin() ; it_cells != cells.end() ; it_cells++ )
            {
                TableCell *cell = it_cells->second;
                table->addRow( StringVector( it_cells->first , cell->str() ) );
            }
            
            return table;
        }
        
        std::string TableRow::str()
        {
            std::ostringstream output;
            au::map<std::string,TableCell>::iterator it_cells;
            for( it_cells = cells.begin() ; it_cells != cells.end() ; it_cells ++ )
                output << it_cells->first << ":" << it_cells->second->str() << " ";
            
            return output.str();
        }
        

        
        // ------------------------------------------------------------
        // TableRow
        // ------------------------------------------------------------
        
        
        
        TableColumn::TableColumn( std::string _field_definition )
        {
            
            // Original field description
            field_definition = _field_definition;
            
            left = false;            // By default no left aligment
            format = format_string;  // By default string format
            
            group = sum;             // Default criteria
            
            std::vector<std::string> components;
            split( field_definition , ',' , components );
            
            if( field_definition.size() == 0 )
            {
                title = "error";
                return;
            }
            
            title = components[0];
            name = components[0];
            
            // Process other modifiers
            for ( size_t i = 1 ; i < components.size() ; i++ )
                processModifier( components[i] );
        }
        
        void TableColumn::processModifier( std::string modifier )
        {
            
            // Single commands
            if( ( modifier == "l" ) || ( modifier == "left") )
            {
                left = true;
                return;
            }
            
            if( ( modifier == "sum" ) || ( modifier == "s") )
            {
                group=sum;
                return;
            }
            
            if( ( modifier == "different" ) || ( modifier == "d") )
            {
                group=different;
                return;
            }
            
            if( ( modifier == "vector" ) || ( modifier == "v") )
            {
                group=vector;
                return;
            }
            
            if( ( modifier == "max" ) )
            {
                group=max;
                return;
            }
            if( ( modifier == "min" ) )
            {
                group=min;
                return;
            }
            
            std::vector<std::string> modifier_components;
            split( modifier , '=' , modifier_components );
            
            if ( modifier_components.size() != 2 )
                return;
            
            if(  ( (modifier_components[0] == "format" ) || (modifier_components[0] == "f" ) ) )
            {
                std::string str_format = modifier_components[1];
                
                if( str_format == "string" )
                    format = format_string;
                if( str_format == "uint64" )
                    format = format_uint64;
                if( str_format == "uint" )
                    format = format_uint;
                if( str_format == "double" )
                    format = format_double;
                if( str_format == "per" )
                    format = format_percentadge;
                if( str_format == "time" )
                    format = format_time;
                if( str_format == "timestamp" )
                    format = format_timestamp;
                return;
            }
            
            if(  ( (modifier_components[0] == "title" ) || (modifier_components[0] == "t" ) ) )
            {
                title = modifier_components[1];
                return;
            }
            
        }
        
        std::string TableColumn::getName()
        {
            return  name;
        }
        
        std::string TableColumn::getTitle()
        {
            return title;
        }
        
        bool TableColumn::getLeft()
        {
            return left;
        }
        
        std::string TableColumn::simple_transform( std::string value )
        {
            switch (format) 
            {
                case format_string:       return value;
                case format_uint:         return value;
                case format_uint64:       return au::str( (size_t) strtoll( value.c_str() , (char **)NULL, 10) );
                case format_double:       return au::str( strtof( value.c_str() , (char **)NULL ) );
                case format_time:         return au::str_time( strtoll( value.c_str() , (char **)NULL, 10) );
                case format_timestamp:    return au::str_timestamp( strtoll( value.c_str() , (char **)NULL, 10) );
                case format_percentadge:  return au::str_percentage( atof( value.c_str() ) );
            }
            
            return "<ERROR>";
        }
        
        std::string TableColumn::transform( StringVector& values )
        {
            
            if( values.size() == 0 )
                return "<no values>";
            
            if( values.size() == 1 )
                return simple_transform( values[0] );
            
            switch (group) 
            {
                case vector:
                {
                    std::ostringstream output;
                    output << "[ ";
                    for ( size_t i = 0 ; i < values.size() ; i++ )
                        output << simple_transform( values[i] ) << " ";
                    output << "]";
                    return output.str();
                }
                    
                case different:
                {
                    values.unique();
                    if( values.size() == 1 )
                        return values[0];
                    
                    if( values.size() > 5 )
                        return au::str("[ %lu different values ]", values.size());
                    
                    std::ostringstream output;
                    output << "[ ";
                    for ( size_t i = 0 ; i < values.size() ; i++ )
                        output << simple_transform( values[i] ) << " ";
                    output << "]";
                    return output.str();
                }
                    
                case min:
                {
                    sort(values);
                    return simple_transform( values[values.size()-1] );
                }
                    
                case max:
                {
                    sort(values);
                    return simple_transform( values[0] );
                }
                    
                case min_max:
                {
                    std::string min = simple_transform( values[values.size()-1] );
                    std::string max = simple_transform( values[0] );
                    return au::str("[ %s - %s ]" , min.c_str() , max.c_str());
                }
                    
                case sum:
                    return str_sum(values);
            }    
            
            return "<Error>";
            
        }
        
        void TableColumn::sort( StringVector& values )
        {
            if ( ( format == format_uint ) || ( format == format_uint64 ) || ( format == format_time ) )
            {
                std::vector<size_t> _values;
                for( size_t i = 0 ; i < values.size() ; i++ )
                    _values.push_back(strtoll( values[i].c_str() , (char **)NULL, 10));
                std::sort(_values.begin(), _values.end());
                
                values.clear();
                for ( size_t i = 0 ; i < _values.size() ; i++ )
                    values.push_back( au::str("%lu",_values[i] ) );
            }
            
            if ( ( format == format_double ) || ( format == format_percentadge ) )
            {
                std::vector<double> _values;
                for( size_t i = 0 ; i < values.size() ; i++ )
                    _values.push_back(strtof( values[i].c_str() , (char **)NULL));
                std::sort(_values.begin(), _values.end());
                
                values.clear();
                for ( size_t i = 0 ; i < _values.size() ; i++ )
                    values.push_back( au::str( _values[i] ) );
            }
        }
        
        std::string TableColumn::str_sum( StringVector& values )
        {
            if ( ( format == format_uint64 ) || ( format == format_time ) )
            {
                size_t total = 0;
                for( size_t i = 0 ; i < values.size() ; i++ )
                    total += strtoll( values[i].c_str() , (char **)NULL, 10);
                return simple_transform( au::str("%lu",total) );
            }
            
            if ( ( format == format_double ) || ( format == format_percentadge ) )
            {
                size_t total = 0;
                for( size_t i = 0 ; i < values.size() ; i++ )
                    total += strtof( values[i].c_str() , (char **)NULL);
                return simple_transform( au::str( total ) );
            }
            
            return "<!sum>";
        }
        
        
        // ------------------------------------------------------------
        // Table
        // ------------------------------------------------------------
        
        Table::Table( std::string description )
        {
            StringVector _columns = StringVector::parseFromString(description, '|');
            for( size_t i = 0 ; i < _columns.size() ; i++ )
                columns.push_back( new TableColumn( _columns[i] ) );
        }
        
        Table::Table( StringVector _columns )
        {
            for( size_t i = 0 ; i < _columns.size() ; i++ )
                columns.push_back( new TableColumn( _columns[i] ) );
        }
        
        Table::Table( Table* table )
        {
            title = table->title;
            
            // Copy columns
            for ( size_t i = 0 ; i < table->columns.size() ; i++ )
                columns.push_back( new TableColumn( table->columns[i]->field_definition ) );
            
            // Copy rows
            for( size_t r = 0 ; r < table->rows.size() ; r++ )
                rows.push_back( new TableRow( table->rows[r] ) );
        }
        
        Table::~Table()
        {
            rows.clearVector();
            columns.clearVector();
        }
        
        std::string Table::getValue( size_t r , size_t c )
        {
            if( ( r == (size_t)-1) || ( c == (size_t)-1) )
                return "";
            
            if ( r > rows.size() )
                return "";
            
            if ( c > columns.size() )
                return "";

            TableCell* cell = rows[r]->get( columns[c]->getName() );
            if (!cell )
                return "";

            // Return agregated version of the values
            return columns[c]->transform( cell->values );
        }
        
        std::string Table::getValue( int r , std::string column_name )
        {
            return getValue(r, getColumn(column_name) );
        }
        
        size_t Table::getColumn( std::string title )
        {            
            for( size_t i = 0 ; i < columns.size() ; i++ )
            {
                if( columns[i]->getName() == title )
                    return i;
            }
            
            return (size_t)-1;
        }
        
        std::string Table::getColumn( size_t pos )
        {
            if( pos >= columns.size() )
                return "";
            return columns[pos]->getName();
        }
        
        StringVector Table::getColumnNames()
        {
            StringVector names;
            for( size_t i = 0 ; i < columns.size() ; i++ )
                names.push_back( columns[i]->getName() );
            return names;
        }

        
        void Table::addRow( StringVector values )
        {
            rows.push_back( new TableRow( getColumnNames() , values ) );
        }
        
        void Table::addSeparator()
        {
            rows.push_back( new TableRow( TableRow::separator ) );
        }
        
        std::string Table::str( )
        {
            // Compute max widths per column
            size_t title_length = 0;
            std::vector<size_t> column_width;
            for( size_t c = 0 ; c < columns.size() ; c++)
            {
                size_t _column_width = std::max(  getMaxWidthForColumn( c ) 
                                                , columns[c]->getTitle().length() 
                                                );
                column_width.push_back( _column_width );
                title_length += _column_width;
            }
            title_length += 3*( columns.size()-1);
            
            // Check enougth space for the title length
            int pos = 0 ;
            while( title.length() > title_length )
            {
                column_width[pos++]++;
                if( pos >= (int)column_width.size() )
                    pos = 0;
                title_length++;
            }
            
            // Output string
            std::ostringstream output;
            
            // Separation line
            output << top_line( column_width );
            
            // Main title...
            if( title != "" )
            {
                output << AU_TABLE_V << " ";
                output << string_length( title , title_length, true);
                output << " " << AU_TABLE_V;
                output << "\n";
            }
            
            // Separation line
            output << top_line2( column_width );
            
            // TITLES
            output << AU_TABLE_V << " ";
            for ( size_t c = 0 ; c < columns.size() ; c++)
            {
                output << string_length( columns[c]->getTitle() , column_width[c] , columns[c]->getLeft() );
                output << " " << AU_TABLE_V << " ";
            }
            output << "\n";
            
            // Separation line
            output << line( column_width );
            
            // For each record at the input, create an output 
            for( size_t r = 0 ; r < rows.size() ; r++)
            {
                if( rows[r]->getType() == TableRow::separator )
                {
                    output << line( column_width );
                    continue;
                }
                
                output << AU_TABLE_V << " ";
                for( size_t c = 0 ; c < columns.size() ; c++ )
                {
                    std::string column = columns[c]->getName();
                    
                    // Get the formatted output
                    std::string value = getValue( r , c );
                    
                    output << string_length(  value , column_width[c] , columns[c]->getLeft() );
                    output << " " << AU_TABLE_V << " ";
                }
                output << "\n";
            }
            
            // Separation line
            output << bottom_line( column_width );
            
            return output.str();
        }
        
        std::string Table::str_xml( )
        {
            
            // Output string
            std::ostringstream output;

            au::xml_open( output , "table" );

            
            // Main title...
            if( title != "" )
                au::xml_simple(output, "title",  title );
            
            // For each record at the input, create an output 
            for( size_t r = 0 ; r < rows.size() ; r++)
            {
                // Skip separators
                if( rows[r]->getType() == TableRow::separator )
                    continue;
                
                au::xml_open(output, "item");

                for ( size_t c = 0 ; c < columns.size() ; c++)
                {
                    au::xml_open(output, "property");
                    
                    au::xml_simple(output, "name", columns[c]->getTitle() );

                    // Get the formatted output
                    std::string column = columns[c]->getName();
                    std::string value = getValue( r , c );
                    
                    au::xml_simple(output, "value", value );

                    au::xml_close(output, "property");
                    
                }
                au::xml_close(output, "item");
            }
            
            au::xml_close( output , "table" );

            return output.str();
        }
        
        std::string Table::str_json( )
        {
            // Output string
            std::ostringstream output;
            
            output << "{";
            
            // Main title...
            if( title != "" )
                output << "\"title\":\"" <<  title << "\",";
            
            // For each record at the input, create an output 
            output << "\"items\":[";
            for( size_t r = 0 ; r < rows.size() ; r++)
            {
                // Skip separators
                if( rows[r]->getType() == TableRow::separator )
                    continue;

                output << "{";
                for ( size_t c = 0 ; c < columns.size() ; c++)
                {
                    std::string _title = columns[c]->getTitle();
                    std::string column = columns[c]->getName();
                    std::string _value = getValue( r , c );
                    output << "\"" << _title << "\":\"" <<  _value << "\"";
                    
                    if ( c !=  ( columns.size() - 1 ) )
                        output << ",";
                }
                output << "}";
                
                if( r != (rows.size()-1) )
                    output << ",";
                
            }
            
            output << "]"; // End of items
            output << "}"; // End of global map

            return output.str();
        }
        
        std::string Table::str_html( )
        {
            // Output string
            std::ostringstream output;

            /*
            output << "<style type=\"text/css\"> \
            #table-5 {\
              background-color: #f5f5f5;\
              padding: 5px;\
              border-radius: 5px;\
              -moz-border-radius: 5px;\
              -webkit-border-radius: 5px;\
              border: 1px solid #ebebeb;\
              }\
              #table-5 td, #table-5 th {\
              padding: 1px 5px;\
            }\
            #table-5 thead {\
              font: normal 15px Helvetica Neue,Helvetica,sans-serif;\
              text-shadow: 0 1px 0 white;\
              color: #999;\
            }\
            #table-5 th {\
              text-align: left;\
              border-bottom: 1px solid #fff;\
            }\
            #table-5 td {\
              font-size: 14px;\
            }\
            #table-5 td:hover {\
              background-color: #fff;\
            }\
            </style>";
            */
            
            output << "<style>#table-5{font-family:\"Lucida Sans Unicode\", \"Lucida Grande\", Sans-Serif;font-size:12px;background:#fff;width:480px;border-collapse:collapse;text-align:left;margin:20px;}#table-5 th{font-size:14px;font-weight:normal;color:#039;border-bottom:2px solid #6678b1;padding:10px 8px;}#table-5 td{color:#669;padding:9px 8px 0;}#hor-minimalist-a tbody tr:hover td{color:#009;}</style>";
             
             
            output << "<table id=\"table-5\">";
            
            // Main title...
            if( title != "" )
            {
                output << "<tr>";
                output << "<td colspan=\"" << columns.size() << "\">";
                output <<  "<h1>" << title <<  "</h1>";
                output << "</td>";
                output << "</tr>";
            }
            
            // Colum titles
            for ( size_t c = 0 ; c < columns.size() ; c++)
            {
                std::string _title = columns[c]->getTitle();
                output << "<th>";
                output << _title;
                output << "</th>";
            }
            // Rows
            for( size_t r = 0 ; r < rows.size() ; r++)
            {
                // Skip separators
                if( rows[r]->getType() == TableRow::separator )
                    continue;

                output << "<tr>";

                for ( size_t c = 0 ; c < columns.size() ; c++)
                {
                    std::string column = columns[c]->getName();
                    std::string _value = getValue( r , c );
                    output << "<td>";
                    output << _value;
                    output << "</td>";

                }
                output << "</tr>";
            }
            output << "</table>";
            
            return output.str();
        }
        
        std::string Table::strFormatted( std::string format )
        {
            if( format == "json" )
                return str_json();
            
            if( format == "xml" )
                return str_xml();

            if( format == "html" )
                return str_html();

            // Default
            return str();
            
        }

        
        TableColumn* Table::getSelectColumn( std::string description )
        {
            StringVector components = StringVector::parseFromString( description , ',' );
            std::string name = components[0];
            
            for( size_t i = 0 ; i < columns.size() ; i++ )
                if( columns[i]->getName() == name )
                {
                    // Duplicate column
                    TableColumn * column = new TableColumn( columns[i]->field_definition );
                    
                    // Apply mofidiers
                    for ( size_t m = 1 ; m < components.size() ; m++ )
                        column->processModifier( components[m] );
                    
                    return column;
                }
            
            
            // Create a new one
            return new TableColumn( description );
        }
        
        Table* Table::selectTable( SelectTableInformation *select )
        {    
            
            // Create table with the correct columns
            Table *table = new Table( StringVector() );
            for( size_t c = 0 ; c < select->columns.size() ; c++ )
                table->addColumn( getSelectColumn( select->columns[c] ) );
            
            // Change title if necessary
            if( select->title != "" )
                table->title = select->title;
            else
                table->title = title;

            // Add all filtered rows
            size_t counter = 0;
            for( size_t r = 0 ; r < rows.size() ; r++ )
                if( select->check( rows[r] ) )
                {

                    if( rows[r]->getType() != TableRow::normal )
                        continue; // Skip separators and so...
                    
                    // Duplicate row for the new table
                    table->rows.push_back( new TableRow( rows[r] ) );

                    counter++;
                    if( select->limit > 0 )
                        if( counter >= select->limit )
                            break;
                }
            
            // Group by
            if( select->group_columns.size() > 0 )
            {                
                // Sort by group columns
                table->sort( select->group_columns );

                // Vector with the final aggregated rows
                au::vector<TableRow> final_rows;

                size_t row_begin = 0;
                while( row_begin < table->rows.size() )
                {
                    // Determine the end of this group of rows
                    size_t row_end = row_begin+1;
                    while (( row_end < rows.size()) 
                           && ( table->rows[row_begin]->compare( table->rows[row_end], select->group_columns ) == 0 ))
                            row_end++;
                    
                    // New normal cell is created
                    TableRow* new_row = new TableRow( TableRow::normal );
                    
                    for( size_t c = 0 ; c < table->columns.size() ; c++ )
                    {
                        // Get name of the colun
                        std::string column = table->columns[c]->getName();
                        
                        // get the vector of values
                        TableCell * new_cell = new TableCell();
                        
                        for ( size_t r = row_begin ; r < row_end ; r++ )
                        {
                            TableCell * cell = table->rows[r]->get( column );
                            if( cell ) 
                                new_cell->addFrom( cell );
                        }
                        
                        new_row->set( column , new_cell );
                    }    
                    
                    final_rows.push_back( new_row );
                    
                    // Move to the next rows
                    row_begin = row_end;
                }
                
                // Swap rows
                
                table->rows.clearVector();
                for ( size_t i = 0 ; i < final_rows.size() ; i++ )
                    table->rows.push_back( final_rows[i] );
                final_rows.clear(); // Not delete instances

            }
            
            
            // Sort
            if( select->sort_columns.size() > 0 )
                table->sort( select->sort_columns );
            
            return table;
            
        }
        
        
        std::string Table::str( SelectTableInformation *select )
        {
            // Select the fields grouped ( if required )
            Table* table = selectTable( select );
            std::string output = table->str();
            delete table;
            return output;
        }
        
        std::string Table::strSortedGroupedAndfiltered( std::string title 
                                                       , std::string group_field  
                                                       , std::string sort_field 
                                                       ,  std::string conditions 
                                                       , size_t limit )
        {
            // Select all the columns
            SelectTableInformation select;
            select.addColumns( getColumnNames() );
            
            select.title = title;
            
            if( group_field != "" )
            {
                select.group_columns.push_back(group_field);
                select.sort_columns.push_back(group_field);
            }
            
            if( sort_field != "" )
            {
                select.sort_columns.push_back(sort_field);
            }
            
            if( conditions != "" )
                select.add_conditions(conditions);
            
            select.limit = limit;
            
            return str( &select );
        }

        void Table::sort( StringVector &sort_columns )
        {
            
            // Simple double buble algorithm to sort :(
            for (size_t r = 0 ; r < rows.size() ; r++ )
                for (size_t rr = r+1 ; rr < rows.size() ; rr++ )
                {
                    int c = 0;
                    
                    int pos_sort_columns = 0;
                    while( (c==0) && (pos_sort_columns < (int) sort_columns.size() ) )
                    {
                        std::string column_name = sort_columns[pos_sort_columns];
                        size_t column_id = getColumn( column_name );
                        if( column_id != (size_t) -1 )
                        {
                            TableCell* cell1 = rows[r]->get( column_name );
                            TableCell* cell2 = rows[rr]->get( column_name );
                            
                            // Use this column to resolve comparisson
                            c = cell1->compare( cell2 , columns[column_id]->format );
                        }
                        
                        pos_sort_columns++;
                        
                    }
                    
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
            Table *tmp = new Table( StringVector("Record" , "Title" , "Format" ) );
            
            for( size_t c = 0 ; c < columns.size() ; c++ )
            {
                StringVector values;
                values.push_back(columns[c]->getName() );
                values.push_back(columns[c]->getTitle() );
                values.push_back(columns[c]->field_definition );
                tmp->addRow( values );
            }
            
            return tmp;
        }
        
        std::string Table::top_line( std::vector<size_t> &length  )
        {
            std::ostringstream output;
            
            output << AU_TABLE_CORNER_LT << AU_TABLE_H;
            // Separation line
            for ( size_t c = 0 ; c < length.size() ; c++)
            {
                for( size_t i = 0 ; i < length[c] ; i++ )
                    output << AU_TABLE_H;
                
                // Get the formatted output
                if( c != ( length.size()-1) )
                    output << AU_TABLE_H << AU_TABLE_H << AU_TABLE_H;
            }
            output << AU_TABLE_H << AU_TABLE_CORNER_RT;
            output << "\n";
            
            return output.str();
        }
        
        std::string Table::top_line2( std::vector<size_t> &length  )
        {
            std::ostringstream output;
            
            output << AU_TABLE_X_VR << AU_TABLE_H;
            // Separation line
            for ( size_t c = 0 ; c < length.size() ; c++)
            {
                for( size_t i = 0 ; i < length[c] ; i++ )
                    output << AU_TABLE_H;
                
                // Get the formatted output
                if( c != ( length.size()-1) )
                    output<< AU_TABLE_H << AU_TABLE_X_HD<< AU_TABLE_H;
            }
            output << AU_TABLE_H << AU_TABLE_X_VL;
            output << "\n";
            
            return output.str();
        }
        
        std::string Table::bottom_line( std::vector<size_t> &length  )
        {
            std::ostringstream output;
            
            output << AU_TABLE_CORNER_LB << AU_TABLE_H;
            // Separation line
            for ( size_t c = 0 ; c < length.size() ; c++)
            {
                for( size_t i = 0 ; i < length[c] ; i++ )
                    output << AU_TABLE_H;
                
                // Get the formatted output
                if( c != ( length.size()-1) )
                    output << AU_TABLE_H << AU_TABLE_X_HU << AU_TABLE_H;
            }
            output << AU_TABLE_H << AU_TABLE_CORNER_RB;
            output << "\n";
            
            return output.str();
        }
        
        
        std::string Table::line( std::vector<size_t> &length  )
        {
            std::ostringstream output;
            
            output << AU_TABLE_X_VR << AU_TABLE_H;
            // Separation line
            for ( size_t c = 0 ; c < length.size() ; c++)
            {
                for( size_t i = 0 ; i < length[c] ; i++ )
                    output << AU_TABLE_H;
                
                // Get the formatted output
                if( c != ( length.size()-1) )
                    output << AU_TABLE_H << AU_TABLE_X << AU_TABLE_H;
            }
            output << AU_TABLE_H << AU_TABLE_X_VL;
            output << "\n";
            
            return output.str();
        }
        
        std::string Table::string_length( std::string value , int width , bool left )
        {
            int len = value.length();
            
            // Adjust for excessive values...
            std::string _value;
            if( len <= width )
                _value = value;
            else
            {
                _value = value.substr( 0 , width-3 ) + "...";
                len = width;
            }
            
            std::ostringstream output;
            if( left )
            {
                output << _value;
                for( int i = 0 ; i < (width-len) ; i++ )
                    output << " ";
            }
            else
            {
                for( int i = 0 ; i < (width-len) ; i++ )
                    output << " ";
                output << _value;
            }
            
            return output.str();
        }
        
        StringVector Table::getValuesFromColumn( std::string name )
        {
            size_t c = getColumn(name);
            
            StringVector values;
            for ( size_t r = 0 ; r < rows.size() ; r++ )
                values.push_back(  getValue( r , c ) );
            return values;
        }
        
        StringVector Table::getValuesFromColumn( std::string name , TableSelectCondition* condition )
        {
            size_t c = getColumn(name);
            
            StringVector values;
            for ( size_t r = 0 ; r < rows.size() ; r++ )
                if( condition->check( rows[r] ) )
                    values.push_back(  getValue( r , c ) );
            return values;
        }
        
        void Table::setTitle( std::string _title)
        {
            title = _title;
        }
        
        size_t Table::getMaxWidthForColumn( size_t c )
        {
            size_t column_width = 0;
            
            // Search all the values to determine witdh
            for( size_t r = 0 ; r < rows.size() ; r++ )
            {
                std::string value = getValue( r , c );
                
                size_t _length = value.length();
                if(_length > column_width )
                    column_width = _length;
            }
            return column_width;
        }
        
        std::string Table::getTitle( )
        {
            return title;
        }
        
        void Table::addColumn( TableColumn * column )
        {
            columns.push_back( column );
        }

        std::string Table::getFormatForColumn( size_t c)
        {
            return columns[c]->field_definition;
        }

        void Table::reverseRows()
        {
            rows.reverse();
        }

        
    }
}

