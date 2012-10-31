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
#ifndef SAMSON_FIXED_LENGTH_DATA_INSTANCE_H
#define SAMSON_FIXED_LENGTH_DATA_INSTANCE_H
#include <string>                /* std::string                              */
#include <sstream>               /* std::ostringstream   */
#include <samson/module/var_int.h>
#include <samson/module/DataInstance.h>

#include <iostream>
#include <sstream>


namespace samson {

namespace system {

template <typename T>
class FixedLengthDataInstance : public samson::DataInstance
{
    public:
    T value;

    public:
    FixedLengthDataInstance() : samson::DataInstance(){
    }

    ~FixedLengthDataInstance() {
    }

    int parse(char *data){
        value = *((T*)data);
        return sizeof(T);
    }

    int serialize(char *data){
        *((T*)data) = value;
        return sizeof(T);
    }

    static int size(char *data){
        return sizeof(T);
    }

    inline static int compare(char * data1 , char *data2 , size_t *offset1 , size_t *offset2 ){

        T _value1,_value2;

        _value1 = *((T*)(data1+*offset1));
        _value2 = *((T*)(data2+*offset2));

        *offset1 += sizeof(T);
        *offset2 += sizeof(T);

        if( _value1 < _value2 ) return -1;
        if( _value1 > _value2 ) return  1;
        return 0;
    }

    inline static int compare( char* data1 , char* data2 )
    {
        size_t offset_1=0;
        size_t offset_2=0;
        return compare( data1 , data2 , &offset_1 , &offset_2 );
    }

    int serial_compare( char* data1 , char* data2 )
    {
        return compare( data1, data2 );
    }

    void copyFrom( FixedLengthDataInstance<T> *other ){
        value = other->value;
    };

    std::string str(){
        std::ostringstream o;
        o << value;
        return o.str();
    }

    std::string strJSON(){
        return str();
    }

    std::string strXML(){
        return str();
    }

    std::string strHTML(int level_html_heading){
        return str();
    }

    std::string strHTMLTable(std::string _varNameInternal){
        std::ostringstream o;
		o << "<style>#table-5{font-family:\"Lucida Sans Unicode\", \"Lucida Grande\", Sans-Serif;font-size:12px;background:#fff;border-collapse:collapse;text-align:left;margin:20px;}#table-5 th{text-align:center;font-size:14px;font-weight:normal;color:#039;border-bottom:2px solid #6678b1;padding:10px 8px;}#table-5 td{color:#669;padding:9px 8px 0;}#table-5 tbody tr:hover td{color:#009;}</style>";
        o << "<table id=\"table-5\">\n";
         o << "<caption>" <<  _varNameInternal << "</caption>\n";
         o << "<tr>\n";
         o << "<th>" << _varNameInternal << "</th>\n";
         o << "</tr>\n";
         o << "<tr>\n";
         o << "<th>" << getName() << "</th>\n";
         o << "</tr>\n";
         o << "<td>" << str() << "</td>\n";
         o << "</tr>\n";
         o << "</table>\n";
        return o.str();
    }

    std::string paint_header(int init_col)
    {
        return "Term";
    }

    std::string paint_header_basic(int init_col)
    {
        return "Term";
    }

    std::string paint_value(int index_row)
    {
        std::ostringstream o;
        if (index_row >= 0)
        {
            o  << "<td>" << str() << "</td>";
        }
        else
        {
            o  << "<td></td>";
        }
        return o.str();
    }

    int num_fields()
    {
        return 1;

    }

    int num_basic_fields()
    {
        return 1;
    }

    int max_depth()
    {
        return 1;
    }

    int max_num_values()
    {
        return 1;
    }

    bool is_terminal()
    {
        return true;
    }

};


}// system namespace
} // ss namespace

#endif
