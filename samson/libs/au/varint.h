
/* ****************************************************************************
*
* FILE            varint.h
*
* AUTHOR          Andreu Urruela
*
* PROJECT         au library
*
* DATE            Septembre 2011
*
* DESCRIPTION
*
*  Serialization and Deserializarion methods for variable-length integer codificiation
*  It can codify a 64 bits unsigned integer into a 1,2,3,4,5,6,8 bytes length number
*
* COPYRIGTH       Copyright 2011 Andreu Urruela. All rights reserved.
*
* ****************************************************************************/

#ifdef AU_VAR_INT_H_
#define AU_VAR_INT_H_

/**
 * Variable length integer definition
 */

inline int staticVarIntParse(char *data, size_t *value) {
  unsigned char *p = (unsigned char *)data;
  size_t base = 1;

  int offset = 0;
  size_t tmp_value = 0;

  do {
    tmp_value += (size_t)(p[offset] & 127) * base;
    base = base * 128;
  } while (p[offset++] & 128);

  *value = tmp_value;
  return offset;
}

inline int staticVarIntSerialize(char *data, size_t value) {
  unsigned char *p = (unsigned char *)data;

  size_t __value = value;

  // Special case
  if (__value == 0) {
    p[0] = 0;
    return 1;
  }

  int offset = 0;
  unsigned char tmp;

  while (__value > 0) {
    // cerr << "Parsign value "<< __value << "\n";

    tmp  = __value - (__value / 128) * 128;
    __value = __value / 128;

    if (__value > 0) {
      tmp |= 128;      // Add flag
    }
    // Push to the buffer
    p[offset++] = tmp;
  }
  return offset;
}

#endif  // ifdef AU_VAR_INT_H_
