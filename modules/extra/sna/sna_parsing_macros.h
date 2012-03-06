/*
 * sna_parsing_macros.h
 *
 *  Created on: 04/03/2011
 *      Author: jges
 */

#ifndef SNA_PARSING_MACROS_H_
#define SNA_PARSING_MACROS_H_

#include <stdlib.h>

//Macros to parte CDRs
#define GET_CDRS_INIT int pos=0; int pos_celd=0; int length_celd=0; int pos_field=0; int cont=0;
#define GET_CDRS_NEXT_FIELD pos_field=pos; while( line[pos] != '|' && line[pos] != '\0' ) pos++; line[pos]='\0';  pos++;
#define GET_CDRS_LAST_FIELD pos_field=pos; while( line[pos] != '|' ) pos++;
#define GET_CDRS_GET_NUMBER( num )    num = atoll( &line[pos_field] );
#define GET_CDRS_GET_DURATION( num )  num = atoll( &line[pos_field] );
#define GET_CDRS_GET_DIR( num )  num = atoll( &line[pos_field] );

#define LENGTH_MAX_CLIENT 10
#define GET_CDRS_GET_PHONE_MX( num ) num=0; length_celd = pos-pos_field-1; pos_celd=pos_field; if(length_celd>LENGTH_MAX_CLIENT) pos_celd=pos-LENGTH_MAX_CLIENT-1; num = atoll( line+pos_celd ); cont=0;


#define CHAR_TO_INT(v) (v-48)

#define SNA_PARSER_INIT(_line)			char *line=_line; int pos=0; int pos_field=0;
#define SNA_PARSER_NEXT_FIELD			pos_field=pos; while( ( line[pos] != '|' ) && (line[pos]!='\0')) pos++; line[pos]='\0'; pos++;

#define SNA_PARSER_NEXT_FIELD_DELIMITER_SPACE	pos_field=pos; while( ( line[pos] != ' ' ) && (line[pos]!='\0')) pos++; line[pos]='\0'; pos++;

#define SNA_PARSER_GET_INT( num )		num = atoi( &line[pos_field] );

#define SNA_PARSER_GET_UINT( num )		num = atoll( &line[pos_field] );
#define SNA_PARSER_GET_UINT8( num)		num = (unsigned char) atoi( &line[pos_field] )

#define SNA_PARSER_GET_DOUBLE( num )		num = atof( &line[pos_field] );
#define SNA_PARSER_FIELD_LENGTH			strlen(&line[pos_field])

#define CHAR_TO_INT(v) (v-48)
#define DATE_GET_VALUE( num , offset ) num = CHAR_TO_INT( line[pos_field+offset] ) *10 + CHAR_TO_INT( line[pos_field+offset+1] );
#define SNA_PARSER_GET_DATE( date )  DATE_GET_VALUE( date.day , 0) DATE_GET_VALUE( date.month , 3) DATE_GET_VALUE( date.year , 6)


#endif /* SNA_PARSING_MACROS_H_ */
