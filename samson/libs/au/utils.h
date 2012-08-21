#ifndef _H_AU_UTILS
#define _H_AU_UTILS

#include <cstring>
#include <string>


namespace au {
int getColumns();
void clear_line();

// Random 64 bit number
size_t code64_rand();
bool code64_is_valid(size_t v);
std::string code64_str(size_t);

// Remove '\n's at the end of the line
void remove_return_chars(char *line);
}

#endif
