#ifndef SAMSON_LIST_H
#define SAMSON_LIST_H


#include <list>                                 // std::map
#include <samson/module/DataInstance.h>         // samson::Environment
#include <sstream>


/**
 *
 *  List    Class used in SAMSON modules in combination with samsonModulesParser command
 *          It contains a collection of otherDataType elements
 *
 * make            It focused to obtimice the reuse of elements
 *
 **/

namespace samson {
template < class T >
class List {
  std::list<T *> reuse_elements;

public:

  std::list<T *> elements;

  ~List() {
    // Delete all elements in the reuse queue
    typename std::list<T *>::iterator it_elements;
    for (it_elements = reuse_elements.begin(); it_elements != reuse_elements.end(); it_elements++) {
      delete (*it_elements);
    }
    reuse_elements.clear();
  }

  // Iterate thougth the entire list
  typename std::list<T *>::iterator it;

  void clear_iterator() {
    it = elements.begin();
  }

  T *getNext() {
    if (it == elements.end()) {
      return NULL;
    } else {
      T *t = *it;
      it++;
      return t;
    }
  }

  // Add an element at both sides of the list

  T *add_back() {
    T *t = get_new_element();

    elements.push_back(t);
    return t;
  }

  T *add_front() {
    T *t = get_new_element();

    elements.push_front(t);
    return t;
  }

  T *add(typename std::list<T *>::iterator it) {
    T *t = get_new_element();

    elements.insert(it, t);
    return t;
  }

  void add_back(T *t) {
    elements.push_back(t);
  }

  void add_front(T *t) {
    elements.push_front(t);
  }

  T *get_front() {
    if (elements.size() == 0) {
      return NULL;
    }

    return *elements.front();
  }

  T *get_back() {
    if (elements.size() == 0) {
      return NULL;
    }

    return *elements.back();
  }

  T *extract_front() {
    if (elements.size() == 0) {
      return NULL;
    }

    T *t =  elements.front();
    elements.pop_front();
    return t;
  }

  T *extract_back() {
    if (elements.size() == 0) {
      return NULL;
    }

    T *t =  elements.back();
    elements.pop_back();
    return t;
  }

  void remove_front() {
    T *t = extract_front();

    if (t) {
      push_to_reuse(t);
    }
  }

  void remove_back() {
    T *t = extract_back();

    if (t) {
      push_to_reuse(t);
    }
  }

  size_t count() {
    return elements.size();
  }

  // DataType necessary operations

  int parse(char *data) {
    clear();        // Prepare this element to parse again content

    int offset = 0;

    size_t _length;     // Number of elements
    offset += samson::staticVarIntParse(data + offset, &_length);

    for (int i = 0; i < (int)_length; i++) {
      T *t = add_back();
      offset += t->parse(data + offset);
    }

    return offset;
  }

  int serialize(char *data) {
    int offset = 0;

    offset += samson::staticVarIntSerialize(data + offset, elements.size());

    typename std::list<T *>::iterator it_elements;
    for (it_elements = elements.begin(); it_elements != elements.end(); it_elements++) {
      offset += (*it_elements)->serialize(data + offset);
    }

    printf("Serializing list of %d elements with %d bytes", (int)elements.size(), offset);

    return offset;
  }

  void setFromString(const char *_value_data) {
    typename std::list<T *>::iterator it_elements;
    for (it_elements = elements.begin(); it_elements != elements.end(); it_elements++) {
      (*it_elements)->setFromString(_value_data);
    }
  }

  void clear() {
    // Put all elements to be reused
    typename std::list<T *>::iterator it_elements;
    for (it_elements = elements.begin(); it_elements != elements.end(); it_elements++) {
      push_to_reuse(*it_elements);
    }
    elements.clear();
  }

  DataInstance *getDataInstanceFromPath(const int *dataPathIntP) {
    // Not implemented
    return NULL;
  }

  void copyFrom(List<T> *other) {
    // Copy content from another list
  }

  std::string str() {
    std::ostringstream output;

    output << "[ ";

    typename std::list<T *>::iterator it_elements;
    for (it_elements = elements.begin(); it_elements != elements.end(); it_elements++) {
      output << (*it_elements)->str() << " ";
    }
    output << "]";
    return output.str();
  }

  std::string strJSON() {
    std::ostringstream output;

    output << "[ ";

    typename std::list<T *>::iterator it_elements;
    for (it_elements = elements.begin(); it_elements != elements.end(); it_elements++) {
      output << (*it_elements)->str() << " ";
    }
    output << "]";
    return output.str();
  }

  std::string strXML() {
    std::ostringstream output;

    typename std::list<T *>::iterator it_elements;
    for (it_elements = elements.begin(); it_elements != elements.end(); it_elements++) {
      output << "<item>" << (*it_elements)->strXML() << "</item>\n";
    }
    return output.str();
  }

  std::string strHTML(int level_html_heading) {
    std::ostringstream output;

    output << "<h" << level_html_heading << ">" << "List";
    level_html_heading++;
    typename std::list<T *>::iterator it_elements;
    int i;
    for (it_elements = elements.begin(), i = 0; it_elements != elements.end(); it_elements++, i++) {
      output << "<h" << level_html_heading << ">" << "List" << "[" << i << "]";
      output << (*it_elements)->strHTML(level_html_heading + 1);
      output << "</h" << level_html_heading << ">\n";
    }
    level_html_heading--;
    output << "</h" << level_html_heading << ">\n";
    return output.str();
  }

  std::string strHTMLTable(std::string name) {
    std::ostringstream output;

    output << "<table>\n";
    output << "<caption>" <<  name << "</caption>\n";

    typename std::list<T *>::iterator it_elements;
    int init_col = 0;

    output << "<tr>\n";
    int i;
    for (it_elements = elements.begin(),  i = 0; it_elements != elements.end(); it_elements++, i++) {
      output << "<th>" << "List" << "</th>";;
    }
    output << "<tr>\n";

    for (it_elements = elements.begin(),  i = 0; it_elements != elements.end(); it_elements++, i++) {
      output << "<th>" << (*it_elements)->paint_header(init_col);
      init_col += (*it_elements)->num_basic_fields();
    }

    for (it_elements = elements.begin(),  i = 0; it_elements != elements.end(); it_elements++, i++) {
      output << "<th>" << (*it_elements)->paint_header(init_col);
      init_col += (*it_elements)->num_basic_fields();
    }

    output << "<table>\n";

    return output.str();
  }

  std::string paint_header(int init_col) {
    return "TO BE DEFINED";
  }

  std::string paint_header_basic(int init_col) {
    return "TO BE DEFINED";
  }

  std::string paint_value(int index_row) {
    return "TO BE DEFINED";
  }

  int num_fields() {
    return 1;
  }

  int num_basic_fields() {
    int n_basic_fields = 0;

    typename std::list<T *>::iterator it_elements;
    for (it_elements = elements.begin(); it_elements != elements.end(); it_elements++) {
      n_basic_fields += (*it_elements)->num_basic_fields();
    }
    return n_basic_fields;
  }

  int max_depth() {
    int m_depth = 0;

    typename std::list<T *>::iterator it_elements;
    for (it_elements = elements.begin(); it_elements != elements.end(); it_elements++) {
      int depth = (*it_elements)->max_depth();
      if (depth > m_depth)
        m_depth = depth;
    }
    return m_depth + 1;
  }

  int max_num_values() {
    return elements.size();
  }

  bool is_terminal() {
    return false;
  }

  int hash(int max_num) {
    if (elements.size() == 0) {
      return 0;
    }


    return elements.front()->hash(max_num);
  }

  static size_t size(char *data) {
    size_t _length;
    size_t offset = 0;

    // Get the length of elements
    offset += samson::staticVarIntParse(data + offset, &_length);

    T _tmp;
    for (int i = 0; i < (int)_length; i++) {  // Sizing v
      offset += _tmp.parse(data + offset);
    }

    return offset;
  }

  /**
   *   Reuse mechanism
   **/

  // Get a new element ( or a reused one if possible )
  T *get_new_element() {
    if (reuse_elements.size() == 0) {
      return new T();
    }


    T *t = reuse_elements.front();
    reuse_elements.pop_front();

    return t;
  }

  // Push an element to be reused
  void push_to_reuse(T *t) {
    reuse_elements.push_back(t);
  }
};
}

#endif  // ifndef SAMSON_LIST_H
