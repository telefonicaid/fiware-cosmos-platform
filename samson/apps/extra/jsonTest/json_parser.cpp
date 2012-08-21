#include <errno.h>
#include <fcntl.h>
#include <json/json.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

extern "C" void exit(int);



/* ****************************************************************************
 *
 * Debug macros
 */
#define M(s)                                                      \
  do {                                                              \
    printf("%s[%d]: %s: ", __FILE__, __LINE__, __FUNCTION__);     \
    printf s;                                                     \
    printf("\n");                                                 \
  } while (0)

#define V0(level, s)                                              \
  do {                                                              \
    if (verbose >= level) {                                         \
      M(s); }                                                     \
  } while (0)

#define V1(s) V0(1, s)
#define V2(s) V0(2, s)
#define V3(s) V0(3, s)
#define V4(s) V0(4, s)
#define V5(s) V0(5, s)
#define E(s)  M(s)

#define X(code, s)                                                \
  do {                                                              \
    M(s);                                                         \
    exit(code);                                                   \
  } while (0)

#define R(r, s)                                                   \
  do {                                                              \
    M(s);                                                         \
    return r;                                                     \
  } while (0)



// ----------------------------------------------------------------------------
//
// print_json_value - print value corresponding to boolean, double, integer and strings
//
void print_json_value(json_object *jobj) {
  enum json_type type;

  type = json_object_get_type(jobj);   /*Getting the type of the json object*/
  M(("type: %d", type));
  switch (type) {
    case json_type_boolean: printf("json_type_boolean\n");
      printf("value: %s\n", json_object_get_boolean(jobj) ? "true" : "false");
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

void json_parse_array(json_object *jobj, char *key) {
  void json_parse(json_object * jobj);   /*Forward Declaration*/
  enum json_type type;
  json_object *jarray = jobj;      /*Simply get the array*/

  if (key)
    jarray = json_object_object_get(jobj, key);     /*Getting the array if it is a key value pair*/

  int arraylen = json_object_array_length(jarray);   /*Getting the length of the array*/
  printf("Array Length: %d\n", arraylen);
  int i;
  json_object *jvalue;

  for (i = 0; i < arraylen; i++) {
    jvalue = json_object_array_get_idx(jarray, i);     /*Getting the array element at position i*/
    type = json_object_get_type(jvalue);
    if (type == json_type_array) {
      json_parse_array(jvalue, NULL);
    } else if (type != json_type_object) {
      printf("value[%d]: ", i);
      print_json_value(jvalue);
    } else {
      json_parse(jvalue);
    }
  }
}

// ----------------------------------------------------------------------------
//
// json_parse - parse the json object
//
void json_parse(json_object *jobj) {
  enum json_type type;

  M(("In"));
  /* Passing through every array element*/
  json_object_object_foreach(jobj, key, val) {
    M(("type: %d", type));
    type = json_object_get_type(val);

    switch (type) {
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
int main(int argC, char *argV[]) {
  char buffer[2048];
  char *buf    = argV[1];

//  const char*  string = "{\"sitename\" : \"joys of programming\",   \
//                              \"categories\" : [ \"c\" , [\"c++\" , \"c\" ], \"java\", \"PHP\" ], \
//                              \"author-details\": { \"admin\": false, \"name\" : \"Joys of Programming\", \"Number of Posts\" : 10 } \
//                            }";

  printf("argV[1] == '%s'\n", argV[1]);
  if (access(argV[1], R_OK) == 0) {
    printf("opening file %s\n", argV[1]);
    int fd = open(argV[1], O_RDONLY);
    int nb;

    nb = read(fd, buffer, sizeof(buffer));
    if (nb == -1) {
      printf("error reading from '%s': %s\n", argV[1], strerror(errno));
      exit(1);
    } else if (nb == 0) {
      printf("read 0 bytes from '%s'\n", argV[1]);
      exit(2);
    }

    buf = buffer;
    buf[nb] = 0;
  }

  printf("JSON string: %s\n----------------------------------------\n", buf);

  json_object *jobj = json_tokener_parse(buf);
  json_parse(jobj);

  return 0;
}

