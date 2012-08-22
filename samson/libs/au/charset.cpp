

#include "charset.h"  // Own interface


namespace au {
#define T true
#define F false

// Statatic table to iso_8859

const bool _iso_8859_is_letter[] =                                      \
{ F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F,     F, F, F, F, F, F, F, F, F, F,      \
  F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F,     F, F, F, F, F, F, F, F, F, F,    \
  F, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T,     T, T, T, T, T, F, F, F, F, F,    \
  F, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T,     T, T, T, T, T, F, F, F, F, F,    \
  F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F,     F, F, F, F, F, F, F, F, F, F,    \
  F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F,     F, F, F, F, F, F, F, F, F, F,    \
  T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T,     T, F, T, T, T, T, T, T, T, T,    \
  T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T,     T, F, T, T, T, T, T, T, T, T };


const bool _iso_8859_is_printable[] =                                   \
{ F, F, F, F, F, F, F, F, F, T, T, F, F, F, F, F, F, F, F, F, F, F,     F, F, F, F, F, F, F, F, F, F,      \
  T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T,     T, T, T, T, T, T, T, T, T, T,    \
  T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T,     T, T, T, T, T, T, T, T, T, T,    \
  T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T,     T, T, T, T, T, T, T, T, T, F,    \
  F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F,     F, F, F, F, F, F, F, F, F, F,    \
  F, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T,     T, T, T, T, T, T, T, T, T, T,    \
  T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T,     T, T, T, T, T, T, T, T, T, T,    \
  T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T,     T, T, T, T, T, T, T, T, T, T };


// To upper
const unsigned char _iso_8859_to_upper[] = {
  0,   1,    2,    3,    4,     5,     6,     7,     8,     9,     10,    11,    12,    13,    14,    15,    16,    17,
  18,
  19,
  20,
  21,
  22,  23,   24,   25,
  26,
  27,
  28,  29,   30,   31,     \
  32,  33,   34,   35,   36,    37,    38,    39,    40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
  50,
  51,
  52,  53,
  54,  55,   56,   57,
  58,  59,
  60,  61,   62,   63,     \
  64,  'A',  'B',  'C',  'D',   'E',   'F',   'G',   'H',   'I',   'J',   'K',   'L',   'M',   'N',   'O',   'P',   'Q',
  'R',
  'S',
  'T',
  'U', 'V',  'W',  'X',  'Y',
  'Z', 91,
  92,  93,   94,   95,     \
  96,  'A',  'B',  'C',  'D',   'E',   'F',   'G',   'H',   'I',   'J',   'K',   'L',   'M',   'N',   'O',   'P',   'Q',
  'R',
  'S',
  'T',
  'U', 'V',  'W',  'X',  'Y',
  'Z',
  123, 124,  125,  126,  127,     \
  128, 129,  130,  131,  132,   133,   134,   135,   136,   137,   138,   139,   140,   141,   142,   143,   144,   145,
  146,
  147,
  148,
  149, 150,  151,  152,  153,
  154,
  155, 156,  157,  158,  159,     \
  160, 161,  162,  163,  164,   165,   166,   167,   168,   169,   170,   171,   172,   173,   174,   175,   176,   177,
  178,
  179,
  180,
  181, 182,  183,  184,  185,
  186,
  187, 188,  189,  190,  191,     \
  'À', 'Á',  'Â',  'Ã',  'Ä',   'Å',   'Æ',   'Ç',   'È',   'É',   'Ê',   'Ë',   'Ì',   'Í',   'Î',   'Ï',   'Ð',   'Ñ',
  'Ò',
  'Ó',
  'Ô',
  'Õ', 'Ö',  215,  'Ø',  'Ù',
  'Ú',
  'Û', 'Ü',  'Ý',  'Þ',  'ß',     \
  'À', 'Á',  'Â',  'Ã',  'Ä',   'Å',   'Æ',   'Ç',   'È',   'É',   'Ê',   'Ë',   'Ì',   'Í',   'Î',   'Ï',   'ð',   'Ñ',
  'Ò',
  'Ó',
  'Ô',
  'Õ', 'Ö',  215,  'Ø',  'Ù',
  'Ú',
  'Û', 'Ü',  'Ý',  'Þ',  'ÿ',     \
};


// To lower
const unsigned char _iso_8859_to_lower[] = {
  0,   1,    2,    3,    4,     5,     6,     7,     8,     9,     10,    11,    12,    13,    14,    15,    16,    17,
  18,
  19,
  20,
  21,
  22,  23,   24,   25,
  26,
  27,
  28,  29,   30,   31,     \
  32,  33,   34,   35,   36,    37,    38,    39,    40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
  50,
  51,
  52,  53,
  54,  55,   56,   57,
  58,  59,
  60,  61,   62,   63,     \
  64,  'a',  'b',  'c',  'd',   'e',   'f',   'g',   'h',   'i',   'j',   'k',   'l',   'm',   'n',   'o',   'p',   'q',
  'r',
  's',
  't',
  'u', 'v',  'w',  'x',  'y',
  'z', 91,
  92,  93,   94,   95,     \
  96,  'a',  'b',  'c',  'd',   'e',   'f',   'g',   'h',   'i',   'j',   'k',   'l',   'm',   'n',   'o',   'p',   'q',
  'r',
  's',
  't',
  'u', 'v',  'w',  'x',  'y',
  'z',
  123, 124,  125,  126,  127,     \
  128, 129,  130,  131,  132,   133,   134,   135,   136,   137,   138,   139,   140,   141,   142,   143,   144,   145,
  146,
  147,
  148,
  149, 150,  151,  152,  153,
  154,
  155, 156,  157,  158,  159,     \
  160, 161,  162,  163,  164,   165,   166,   167,   168,   169,   170,   171,   172,   173,   174,   175,   176,   177,
  178,
  179,
  180,
  181, 182,  183,  184,  185,
  186,
  187, 188,  189,  190,  191,     \
  'à', 'á',  'â',  'ã',  'ä',   'å',   'æ',   'ç',   'è',   'é',   'ê',   'ë',   'ì',   'í',   'î',   'ï',   'Ð',   'ñ',
  'ò',
  'ó',
  'ô',
  'õ', 'ö',  247,  'ø',  'ù',
  'ú',
  'û', 'ü',  'ý',  'þ',  'ß',     \
  'à', 'á',  'â',  'ã',  'ä',   'å',   'æ',   'ç',   'è',   'é',   'ê',   'ë',   'ì',   'í',   'î',   'ï',   'ð',   'ñ',
  'ò',
  'ó',
  'ô',
  'õ', 'ö',  247,  'ø',  'ù',
  'ú',
  'û', 'ü',  'ý',  'þ',  'ÿ',     \
};

bool iso_8859_is_letter(unsigned char c) {
  return _iso_8859_is_letter[ c ];
}

bool iso_8859_is_printable(unsigned char c) {
  return _iso_8859_is_printable[ c ];
}

unsigned char iso_8859_to_lower(unsigned char c) {
  return _iso_8859_to_lower[c];
}

unsigned char iso_8859_to_upper(unsigned char c) {
  return _iso_8859_to_upper[c];
}

// Static tables for UTF8 processing

static char UTF8_2_ISO_8859_1_len[] =
{
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* erroneous */
  2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 5, 6
};

static char UTF8_2_ISO_8859_1_mask[] = { 0x3F, 0x7F, 0x1F, 0x0F, 0x07,
                                         0x03, 0x01 };



unsigned long get_utf8_symbol(const char *str, int *length) {
  int len = UTF8_2_ISO_8859_1_len[(*str >> 2) & 0x3F];
  unsigned long u   = *str & UTF8_2_ISO_8859_1_mask[len];

  if (len == 0) {
    // Error
    if (length) {
      *length = -1;
    }
    return 0;                  // What to return?
  }

  for (++str; --len > 0 && (*str != '\0'); ++str) {
    if ((*str & 0xC0) != 0x80) {
      // Error
      if (length) {
        *length = -1;
      }
      return 0;                  // What to return?
    }

    u = (u << 6) | (*str & 0x3F);
  }

  // Return length
  if (length) {
    *length = len;  // Return unicode value
  }
  return u;
}

int serialize_utf8_symbol(unsigned long s, char *str) {
  if (s <= 127) {
    // One byte
    str[0] = (char)s;
    return 1;
  }

  if (s < 2047) {
    // Two bytes
    str[0] = 0xC0 | (s >> 6);
    str[1] = 0x80 | (s & 0x3F);
    return 2;
  }

  if (s < 65535) {
    // three bytes
    str[0] = 0xE0 | (s >> 12);
    str[1] = 0x80 | (s & 0xFFF) >> 6;
    str[2] = 0x80 | (s & 0x3F);
    return 3;
  }

  if (s < 1114111) {
    // four bytes
    str[0] = 0xF0 | (s >> 18);
    str[1] = 0x80 | (s & 0xFFFF) >> 12;
    str[2] = 0x80 | (s & 0x03FF) >> 6;
    str[2] = 0x80 | (s & 0x3F);
    return 4;
  }

  // Error
  return -1;
}

size_t strlenUTF8(const char *cad_utf) {
  char *p_utf;
  size_t lengthUTF8 = 0;

  if (cad_utf == NULL) {
    return 0;
  }

  p_utf = (char *)cad_utf;
  while (*p_utf != '\0') {
    int len = UTF8_2_ISO_8859_1_len[(*p_utf >> 2) & 0x3F];
    unsigned long u   = *p_utf & UTF8_2_ISO_8859_1_mask[len];

    // Try to skip latter...
    if (len == 0) {
      len = 5;
    }
    for (++p_utf; --len > 0 && (*p_utf != '\0'); ++p_utf) {
      if ((*p_utf & 0xC0) != 0x80) {
        break;
      }
      u = (u << 6) | (*p_utf & 0x3F);
    }

    lengthUTF8++;
  }
  return(lengthUTF8);
}
}




