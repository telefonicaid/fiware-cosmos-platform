#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <json/json.h>

extern "C" void exit(int);



// ----------------------------------------------------------------------------
//
// print_json_value - print value corresponding to boolean, double, integer and strings
//
void print_json_value(json_object *jobj)
{
    enum json_type type;

    printf("type: ",type);
    type = json_object_get_type(jobj); /*Getting the type of the json object*/
    switch (type)
    {
    case json_type_boolean: printf("json_type_boolean\n");
        printf("value: %s\n", json_object_get_boolean(jobj)? "true": "false");
        break;

    case json_type_double: printf("json_type_double\n");
        printf("          value: %lf\n", json_object_get_double(jobj));
        break;

    case json_type_int: printf("json_type_int\n");
        printf("          value: %d\n", json_object_get_int(jobj));
        break;

    case json_type_string: printf("json_type_string\n");
        printf("          value: %s\n", json_object_get_string(jobj));
        break;
    }
}
 


void json_parse_array( json_object *jobj, char *key)
{
    void json_parse(json_object * jobj); /*Forward Declaration*/
    enum json_type  type;
    json_object*    jarray = jobj; /*Simply get the array*/

    if (key)
    {
        jarray = json_object_object_get(jobj, key); /*Getting the array if it is a key value pair*/
    }
 
    int arraylen = json_object_array_length(jarray); /*Getting the length of the array*/
    printf("Array Length: %d\n",arraylen);
    int i;
    json_object * jvalue;
 
    for (i=0; i< arraylen; i++)
    {
        jvalue = json_object_array_get_idx(jarray, i); /*Getting the array element at position i*/
        type = json_object_get_type(jvalue);
        if (type == json_type_array)
        {
            json_parse_array(jvalue, NULL);
        }
        else if (type != json_type_object)
        {
            printf("value[%d]: ",i);
            print_json_value(jvalue);
        }
        else
        {
            json_parse(jvalue);
        }
    }
}
 

// ----------------------------------------------------------------------------
//
// json_parse - parse the json object
//
void json_parse(json_object * jobj)
{
    enum json_type type;

    /* Passing through every array element*/
    json_object_object_foreach(jobj, key, val)
    {
        printf("type: ",type);
        type = json_object_get_type(val);

        switch (type)
        {
        case json_type_boolean:
        case json_type_double:
        case json_type_int:
        case json_type_string: print_json_value(val);
            break;

        case json_type_object: printf("json_type_object\n");
            jobj = json_object_object_get(jobj, key);
            json_parse(jobj);
            break;

        case json_type_array: printf("type: json_type_array, ");
            json_parse_array(jobj, key);
            break;
        }
    }
}
 


// ----------------------------------------------------------------------------
//
// main - 
//
int main(int argC, char* argV[])
{
    char         buffer[2048];
    char*        buf    = argV[1];
    const char*  string = "{\"sitename\" : \"joys of programming\",\
                             \"categories\" : [ \"c\" , [\"c++\" , \"c\" ], \"java\", \"PHP\" ], \
                             \"author-details\": { \"admin\": false, \"name\" : \"Joys of Programming\", \"Number of Posts\" : 10 } \
                           }";

    if (access(argV[1], R_OK) == 0)
    {
        printf("opening file %s\n", argV[1]);
        int fd = open(argV[1], O_RDONLY);
        int nb;

        nb = read(fd, buffer, sizeof(buffer));
        if (nb == -1)
        {
            printf("error reading from '%s': %s\n", argV[1], strerror(errno));
            exit(1);
        }
        else if (nb == 0)
        {
            printf("read 0 bytes from '%s'\n", argV[1]);
            exit(2);
        }

        buf = buffer;
        buf[nb] = 0;
    }

    printf("JSON string: %s\n----------------------------------------\n", buf);
    
    json_object * jobj = json_tokener_parse(buf);
    json_parse(jobj);
    
    return 0;
}
