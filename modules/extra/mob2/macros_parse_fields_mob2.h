
#ifndef _H_SAMSON_macros_parse_fields_mob2
#define _H_SAMSON_macros_parse_fields_mob2

#define CHAR_TO_INT(v) (v-48)

#define MR_PARSER_INIT(_pline)                   char *pline=_pline; int pos=0; int pos_field=0;
#define MR_PARSER_NEXT_FIELD                    pos_field=pos; while( ( pline[pos] != '|' ) && (pline[pos]!='\0')) pos++; pline[pos]='\0'; pos++;

#define MR_PARSER_NEXT_FIELD_DELIMITER_SPACE    pos_field=pos; while( ( pline[pos] != ' ' ) && (pline[pos]!='\0')) pos++; pline[pos]='\0'; pos++;

#define MR_PARSER_GET_INT( num )                num = atoi( &pline[pos_field] );

#define MR_PARSER_GET_UINT( num )               num = atoll( &pline[pos_field] );
#define MR_PARSER_GET_UINT8( num)               num = (mr_uint8) atoi( &pline[pos_field] );

#define MR_PARSER_GET_DOUBLE( num )             num = atof( &pline[pos_field] );
#define MR_PARSER_FIELD_LENGTH                  strlen(&pline[pos_field]);

#define DATE_GET_VALUE( num , offset ) num = CHAR_TO_INT( pline[pos_field+offset] ) *10 + CHAR_TO_INT( pline[pos_field+offset+1] );
#define MR_PARSER_GET_DATE( date )  DATE_GET_VALUE( date.day , 0) DATE_GET_VALUE( date.month , 3) DATE_GET_VALUE( date.year , 6);

#define GET_CLUST_INIT_MX pos=0; pos_field=0;
#define GET_CLUST_NEXT_FIELD pos_field=pos; while( pline[pos] != '|' and pline[pos] != '\0' ) pos++; pline[pos]='\0';  pos++;
#define GET_CLUST_GET_INT( _int ) _int = atoi(&pline[pos_field]);
#define GET_CLUST_GET_DOUBLE( _double  ) _double = atof(&pline[pos_field]);

//Borrar
#define GET_BTS_ID( id ) id = atoi(&pline[pos_field]);
#define GET_CLUST_GET_COORD( coord ) coord = atof(&pline[pos_field]);
#define GET_CLUST_GET_LABEL( label ) label = &pline[pos_field];


#endif //de  _H_SAMSON_macros_parse_fields_mob2
