
#include <cstdarg>
#include <sstream>
#include <stdio.h>
#include <string>
#include <vector>

#include <sys/ioctl.h>

#include "string.h"     // Own definitions

namespace au {
std::string str_tabs(int t) {
  std::ostringstream output;

  for (int i = 0; i < t; i++) {
    output << "\t";
  }
  return output.str();
}

std::string str_percentage(double p) {
  if (p > 1) {
    return au::str(p);
  }

  char line[2000];
  sprintf(line, "%05.1f%%", p * 100);
  return std::string(line);
}

std::string str_percentage(double value, double total) {
  if (total == 0) {
    return str_percentage(0);
  } else {
    return str_percentage(value / total);
  }
}

std::string str_timestamp(time_t t) {
  struct tm timeinfo;
  char buffer_time[1024];

  localtime_r(&t, &timeinfo);
  strftime(buffer_time, 1024, "%d/%m/%Y (%X)", &timeinfo);

  return std::string(buffer_time);
}

std::string str_time_simple(size_t seconds) {
  int years = seconds / (365 * 24 * 3600);

  seconds -= (365 * 24 * 3600) * years;

  int days = seconds / (24 * 3600);
  seconds -= (24 * 3600) * days;

  int hours = seconds / 3600;
  seconds -= 3600 * hours;

  int minutes = seconds / 60;
  seconds -= 60 * minutes;


  if (years > 0) {
    return au::str("%6dy", years);
  }
  if (days > 100) {
    return au::str("%6dd", days);
  }
  if (days > 0) {
    return au::str("%2dd%2dh", days, hours);
  }
  if (hours > 0) {
    return au::str("%2dh%2dm", hours, minutes);
  }
  if (minutes > 0) {
    return au::str("%2dm%2ds", minutes, seconds);
  }

  return au::str("%6ds", seconds);
}

std::string str_time(size_t seconds) {
  if (seconds > (3600L * 24L * 265L * 100L)) {
    return "   >100y";
  }

  int days = 0;
  while (seconds > (3600 * 24)) {
    days++;
    seconds -= (3600 * 24);
  }

  int minutes = seconds / 60;
  seconds -= minutes * 60;

  int hours = minutes / 60;
  minutes -= hours * 60;

  if (days > 0) {
    return au::str("%02dd %02d:%02d", days, hours, minutes);
  }

  return au::str(" %02d:%02d:%02d", hours, minutes, (int)seconds);
}

std::string str_progress_bar(double p, int len) {
  std::ostringstream output;


  if (p < 0) {
    p = 0;
  }
  if (p > 1) {
    p = 1;
  }
  int pos = len * p;

  output << " [ ";
  for (int s = 0; s < pos; s++) {
    output << "*";
  }
  for (int s = pos; s < len; s++) {
    output << ".";
  }
  output << " ] ";

  return output.str();
}

std::string str_progress_bar(double p, int len, char c, char c2) {
  std::ostringstream output;


  if (p < 0) {
    p = 0;
  }
  if (p > 1) {
    p = 1;
  }
  int pos = len * p;

  output << " [ ";
  for (int s = 0; s < pos; s++) {
    output << c;
  }
  for (int s = pos; s < len; s++) {
    output << c2;
  }
  output << " ] ";

  return output.str();
}

std::string str_double_progress_bar(double p1, double p2, char c1, char c2, char c3, int len) {
  std::ostringstream output;


  if (p1 < 0) {
    p1 = 0;
  }
  if (p1 > 1) {
    p1 = 1;
  }
  if (p2 < 0) {
    p2 = 0;
  }
  if (p2 > 1) {
    p2 = 1;
  }
  if (p2 < p1) {
    p2 = p1;      // no sense
  }
  int pos1 = (double)len * p1;
  int pos2 = (double)len * p2;

  output << " [ ";

  for (int s = 0; s < pos1; s++) {
    output << c1;
  }

  for (int s = pos1; s < pos2; s++) {
    output << c2;
  }

  for (int s = pos2; s < len; s++) {
    output << c3;
  }

  output << " ] ";

  return output.str();
}

void find_and_replace(std::string &source, const std::string& find, const std::string& replace) {
  size_t pos = 0;

  // LM_M(("Finding string of %d bytes at position %lu of a string with length %lu" , find.length() , pos , source.length() ));
  pos = source.find(find, pos);
  // LM_M(("Position found %lu bytes" , find.length() ));

  while (pos != std::string::npos) {
    source.replace(pos, find.length(), replace);

    // Go forward in the input string
    pos += replace.length();

    // LM_M(("Finding string of %d bytes at position %lu of a string with length %lu" , find.length() , pos , source.length() ));
    pos = source.find(find, pos);
    // LM_M(("Position found %lu bytes" , find.length() ));
  }
}

std::string getRoot(std::string& path) {
  size_t pos = path.find(".", 0);

  if (pos == std::string::npos) {
    return path;
  }

  return path.substr(0, pos);
}

std::string getRest(std::string& path) {
  size_t pos = path.find(".", 0);

  if (pos == std::string::npos) {
    return "";
  }

  return path.substr(pos + 1, path.length());
}

std::string str_indent(std::string txt) {
  // Replace all "returns" by "return and tab"
  find_and_replace(txt, "\n", "\n\t");

  // Insert the first tab
  txt.insert(0, "\t");
  return txt;
}

std::string str_indent(std::string txt, int num_spaces) {
  std::string sep;

  for (int i = 0; i < num_spaces; i++) {
    sep.append(" ");
  }

  // Replace all "returns" by "return and tab"
  find_and_replace(txt, "\n", "\n" + sep);

  // Insert the first tab
  txt.insert(0, sep);
  return txt;
}

std::string str(double value, char letter) {
  char line[2000];

  if (value < 10) {
    sprintf(line, "%4.2f%c", value, letter);
  } else if (value < 100) {
    sprintf(line, "%4.1f%c", value, letter);
  } else {
    sprintf(line, "%4.0f%c", value, letter);
  } return std::string(line);
}

std::string str(const char *format, ...) {
  va_list args;
  char vmsg[2048];

  /* "Parse" the varible arguments */
  va_start(args, format);

  /* Print message to variable */
  vsnprintf(vmsg, sizeof(vmsg), format, args);
  // vmsg[2047] = 0;
  va_end(args);

  return std::string(vmsg);
}

std::string str(Color color, const char *format, ...) {
  va_list args;
  char vmsg[2048];

  /* "Parse" the varible arguments */
  va_start(args, format);

  /* Print message to variable */
  vsnprintf(vmsg, sizeof(vmsg), format, args);
  // vmsg[2047] = 0;
  va_end(args);

  switch (color) {
    case normal:
      return std::string(vmsg);

    case red:
      return std::string("\033[1;31m") + std::string(vmsg) + std::string("\033[0m");

    case purple:
      return std::string("\033[1;35m") + std::string(vmsg) + std::string("\033[0m");
  }

  // Default mode ( just in case )
  return std::string(vmsg);
}

std::string str_double(double value, char letter) {
  char line[2000];

  if (value < 10) {
    sprintf(line, " %4.2f%c", value, letter);
  } else if (value < 100) {
    sprintf(line, " %4.1f%c", value, letter);
  } else {
    sprintf(line, " %4.0f%c", value, letter);
  } return std::string(line);
}

std::string str(const std::vector<std::string>& hosts) {
  std::ostringstream output;

  for (size_t i = 0; i < hosts.size(); i++) {
    output << hosts[i];
    if (i != (hosts.size() - 2)) {
      output << " ";
    }
  }
  return output.str();
}

std::string str(double value) {
  if (value == 0) {
    return "    0 ";
  }

  if (value < 0) {
    std::string tmp = str(-value);
    size_t pos = tmp.find_first_not_of(" ");
    if ((pos == std::string::npos) || ( pos == 0 )) {
      return "ERROR";
    }

    tmp[pos - 1] = '-';
    return tmp;
  }

  if (value < 0.000000000000001) {
    return "  EPS ";
  } else if (value < 0.000000000001) {
    return au::str_double(value * 1000000000000000.0, 'f');
  } else if (value < 0.000000001) {
    return au::str_double(value * 1000000000000.0, 'p');
  } else if (value < 0.000001) {
    return au::str_double(value * 1000000000.0, 'n');
  } else if (value < 0.001) {
    return au::str_double(value * 1000000.0, 'u');
  } else if (value < 1) {
    return au::str_double(value * 1000.0, 'm');
  } else if (value < 1000) {
    return au::str_double(value, ' ');
  } else if (value < 1000000) {
    return au::str_double(value / 1000.0, 'K');
  } else if (value < 1000000000) {
    return au::str_double(value / 1000000.0, 'M');
  }

#ifdef __LP64__
  else if (value < 1000000000000) {
    return au::str_double(value / 1000000000.0, 'G');
  } else if (value < 1000000000000000) {
    return au::str_double(value / 1000000000000.0, 'T');
  } else if (value < 1000000000000000000) {
    return au::str_double(value / 1000000000000000.0, 'P');
  }

#endif  // __LP64__
  else {
    return "  INF ";
  }
}

/*
 * std::string str( size_t value )
 * {
 *
 * if (value < 1000)
 * {
 * // Special case
 * return au::str("%4d ", (int)value );
 * //return au::str( (double)value , ' ' );
 * }
 * else if( value < 1000000)
 * return au::str( (double)value/ 1000.0 , 'K');
 * else if( value < 1000000000)
 * return au::str( (double)value/ 1000000.0 , 'M');
 * #ifdef __LP64__
 * else if( value < 1000000000000)
 * return au::str( (double)value/ 1000000000.0 , 'G');
 * else if( value < 1000000000000000)
 * return au::str( (double)value/ 1000000000000.0 , 'T');
 * #endif
 * else
 * return au::str( (double)value/ 1000000000000000.0 , 'P');
 *
 * }
 */

std::string str(double value, std::string postfix) {
  return str(value) + postfix;
}

std::string str_detail(size_t value) {
  if (value < 1000) {
    return str(value);
  }

  return str("%lu (%s)", value, au::str(value).c_str());
}

std::string str_detail(size_t value, std::string postfix) {
  if (value < 1000) {
    return str(value, postfix);
  }

  return str("%lu %s (%s)", value, postfix.c_str(), au::str(value, postfix).c_str());
}

bool isOneOf(char c, std::string s) {
  for (size_t i = 0; i < s.size(); i++) {
    if (s[i] == c) {
      return true;
    }
  }

  return false;
}

std::vector<std::string> simpleTockenize(std::string txt) {
  std::string tockens = " #\r\t\r\n{};\"";  // All possible delimiters

  std::vector<std::string> items;

  // Simple parser
  size_t pos = 0;
  for (size_t i = 0; i < txt.size(); i++) {
    if (isOneOf(txt[i], tockens)) {
      if (i > pos) {
        items.push_back(txt.substr(pos, i - pos));  /*
                                                     * //Emit the literal with one letter if that is the case
                                                     * std::ostringstream o;
                                                     * o << txt[i];
                                                     * items.push_back( o.str() );
                                                     */
      }
      pos = i + 1;
    }
  }

  if (txt.size() > pos) {
    items.push_back(txt.substr(pos, txt.size() - pos));
  }
  return items;
}

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
  std::stringstream ss(s);
  std::string item;

  while (std::getline(ss, item, delim)) {
    if (item.length() > 0) {
      elems.push_back(item);
    }
  }
  return elems;
}

std::vector<std::string> split(const std::string &s, char delim) {
  std::vector<std::string> elems;
  return split(s, delim, elems);
}

int get_term_size(int fd, int *x, int *y) {
#ifdef TIOCGSIZE
  struct ttysize win;

#elif defined(TIOCGWINSZ)
  struct winsize win;

#endif

#ifdef TIOCGSIZE
  if (ioctl(fd, TIOCGSIZE, &win)) {
    return 0;
  }





  if (y) {
    *y = win.ts_lines;
  }
  if (x) {
    *x = win.ts_cols;
  }

#elif defined TIOCGWINSZ
  if (ioctl(fd, TIOCGWINSZ, &win)) {
    return 0;
  }





  if (y) {
    *y = win.ws_row;
  }
  if (x) {
    *x = win.ws_col;
  }

#else
  {
    const char *s;
    s = getenv("LINES");
    if (s) {
      *y = strtol(s, NULL, 10);
    } else {
      *y = 25;
    }
    s = getenv("COLUMNS");
    if (s) {
      *x = strtol(s, NULL, 10);
    } else {
      *x = 80;
    }
  }
#endif  // ifdef TIOCGSIZE

  return 1;
}

int getTerminalWidth() {
  int x, y;

  if (get_term_size(0, &x, &y)) {
    return x;
  } else {
    return 0;
  }
}

std::string lineInConsole(char c) {
  std::ostringstream output;

  for (int i = 0; i < getTerminalWidth(); i++) {
    output << c;
  }
  return output.str();
}

std::string strWithMaxLineLength(std::string& txt, int max_line_length) {
  std::istringstream input_stream(txt);

  std::ostringstream output;

  char line[1024];

  while (input_stream.getline(line, 1023)) {
    int line_length = (int)strlen(line);

    if (line_length > max_line_length) {
      line[max_line_length - 4] = '.';
      line[max_line_length - 3] = '.';
      line[max_line_length - 2] = '.';
      line[max_line_length - 1] = 0;

      // LM_M(("Exesive line %d / %d ", line_length , max_line_length ));
    } else {
      // LM_M(("Normal line %d / %d", line_length , max_line_length ));
    }

    output << line << "\n";
  }
  return output.str();
}

std::string strToConsole(std::string& txt) {
  return strWithMaxLineLength(txt, getTerminalWidth());
}

// Backward look up for a sequence
const char *laststrstr(const char *source, const char *target) {
  return laststrstr(source, strlen(source), target);
}

const char *laststrstr(const char *source, size_t source_length, const char *target) {
  const char *sp;

  /* Step backward through the source string - one character at a time */
  for (sp = source + source_length - strlen(target); sp >= source; sp--) {
    const char *tp, *spt;
    /* go forward through the search string and source checking characters
     * Stop the loop if the character are unequal or you reach the end of the target (or string) */
    for (tp = target, spt = sp; (*tp == *spt)  && (*spt != '\0') && (*tp != '\0'); spt++, tp++) {
      ;
    }

    /* if the loop above gets to the end of the target string then it must have matched all the characters */
    if (*tp == '\0') {
      break;
    }
  }
  /* If the outer loop finished before getting to the start of the source string then
   * it must have found a matching sub string */
  return (sp < source) ? NULL : sp;
}

// Similar to strnstr(), but limits the length of the pattern to be searched
// and assumes that text string is null terminated
const char *strnstr_limitpattern(const char *text, const char *pattern, size_t max_length) {
  char cpat, ctxt;
  size_t len;

  // Check for an empty pattern
  if ((cpat = *pattern++) != '\0') {
    len = strlen(text);
    do {
      // Locate the beginning of a possible match
      do {
        if ((len-- < max_length) || (ctxt = *text++) == '\0') {
          return ((const char *)NULL);
        }
      } while (ctxt != cpat);
    } while (strncmp(text, pattern, max_length - 1) != 0);
    text--;
  }
  return (text);
}

// match a string against a simple pattern with wildcard characters meaning
// any sequence of characters
// Could be done with regexp library, but it is too much powerful (and slow)
bool MatchPatterns(const char *inputString, const char *pattern, char wildcard) {
  const char *p_input;
  const char *p_pattern;
  char *p_wildcard;
  const char *p_match;
  bool first_wildcard = false;

  // First, lets skip trivial situations
  if (inputString == NULL) {
    return false;
  }

  if (*inputString == '\0') {
    return false;
  }

  if (pattern == NULL) {
    return false;
  }

  if (*pattern == '\0') {
    return false;
  }

  p_input = inputString;
  p_match = p_input;
  p_pattern = pattern;
  if (p_pattern[0] == wildcard) {
    first_wildcard = true;
  }

  while ((p_wildcard = strchr((char *)p_pattern, wildcard)) != NULL) {
    // First locate the next wildcard
    size_t len_pattern = p_wildcard - p_pattern;

    // and check the current portion of inputString against the fix section in the pattern
    if (first_wildcard == false) {
      if (strncmp(p_input, p_pattern, len_pattern) != 0) {
        return false;
      }
      first_wildcard = true;
    } else
    if (len_pattern > 0) {
      if ((p_match = strnstr_limitpattern(p_input, p_pattern, len_pattern)) == NULL) {
        return false;
      }
    }
    p_input = p_match + len_pattern;
    p_pattern = p_wildcard + 1;
    if (*p_pattern == '\0') {
      return true;
    }
  }

  // processing the last portion of the pattern
  // (or all of it, if there was no wildcard)
  if (first_wildcard == false) {
    if (strcmp(p_input, p_pattern) != 0) {
      return false;
    }
  } else
  if (strlen(p_pattern) > 0) {
    if (((p_match = strstr(p_input, p_pattern)) == NULL) || (*(p_match + strlen(p_pattern))) != '\0') {
      return false;
    }
  }
  return true;
}

void SplitInWords(char *line, std::vector<char *>& words, char separator) {
  size_t pos = 0;
  size_t previous = 0;

  bool finish = false;

  // Clear words vector
  words.clear();

  while (!finish) {
    if ((line[pos] == separator) || (line[pos] == '\0')) {
      if ((line[pos] == '\0') || (line[pos] == '\n')) {
        finish = true;
      }

      // Artificial termination of string
      line[pos] = '\0';

      // Add the found word
      words.push_back(&line[previous]);

      // Point to the next words
      // Jumps blank spaces
      pos++;

      // To avoid valgrind detected error when checking after the end of the buffer
      //            if (!finish)
      //            {
      //                while (line[pos] == ' ')
      //                {
      //                    pos++;
      //                }
      //            }
      previous = pos;
    } else {
      pos++;
    }
  }
}

int getCommonChars(std::string& txt, std::string& txt2) {
  size_t l = std::min(txt.length(), txt2.length());

  for (size_t i = 0; i < l; i++) {
    if (txt[i] != txt2[i]) {
      return i;
    }
  }
  return l;
}

bool isCharInRange(char c, char lower, char higher) {
  return ((c >= lower) && (c <= higher));
}

bool strings_begin_equal(std::string txt, std::string txt2) {
  size_t l = std::min(txt.length(), txt2.length());

  for (size_t i = 0; i < l; i++) {
    if (txt[i] != txt2[i]) {
      return false;
    }
  }
  return true;
}

std::string path_from_directory(std::string directory, std::string file) {
  if (directory.length() == 0) {
    return file;
  }

  if (directory[ directory.length() - 1 ] == '/') {
    return directory + file;
  } else {
    return directory + "/" + file;
  }
}

std::string string_in_color(std::string message, std::string color) {
  std::ostringstream output;

  if (( color == "red" ) || ( color == "r" )) {
    output << "\033[1;31m" << message << "\033[0m";
  } else if ((color == "purple") || (color == "p")) {
    output << "\033[1;35m" << message << "\033[0m";
  } else {
    output << message;
  } return output.str();
}

bool string_begins(std::string& str, std::string prefix) {
  return str.substr(0, prefix.length()) == prefix;
}

bool string_ends(std::string& str, std::string postfix) {
  if (postfix.length() > str.length()) {
    return false;
  }
  return str.substr(str.length() - postfix.length()) == postfix;
}

bool string_begins_and_ends(std::string& str, std::string prefix, std::string postfix) {
  if (postfix.length() > str.length()) {
    return false;
  }

  if (prefix.length() > str.length()) {
    return false;
  }

  if (str.substr(0, prefix.length()) != prefix) {
    return false;
  }
  if (str.substr(str.length() - postfix.length()) != postfix) {
    return false;
  }
  return true;
}

std::string substring_without_prefix_and_posfix(std::string& str, const std::string& prefix, const std::string& postfix) {
  return str.substr(prefix.length(), str.length() - prefix.length() - postfix.length());
}

std::string reverse_lines(std::string& txt) {
  std::istringstream input_stream(txt);


  char line[1024];

  std::vector<std::string> lines;

  while (input_stream.getline(line, 1023)) {
    lines.push_back(line);
  }

  // Generate output
  std::ostringstream output;
  size_t num = lines.size();
  for (size_t i = 0; i < num; i++) {
    output << lines[num - i - 1] << "\n";
  }
  return output.str();
}

int HashString(const std::string& str, int max_num_partitions) {
  static const size_t InitialFNV = 2166136261U;
  static const size_t FNVMultiple = 16777619;

  size_t hash = InitialFNV;

  size_t str_length = str.length();
  for (size_t i = 0; i < str_length; ++i) {
    hash ^= str[i];
    hash *= FNVMultiple;
  }
  return static_cast<int>(hash % max_num_partitions);
}
}

