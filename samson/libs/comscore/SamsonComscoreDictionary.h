
#ifndef _H_SAMSON_COMSCORE
#define _H_SAMSON_COMSCORE

#include "logMsg/logMsg.h"

#include <fnmatch.h>
#include <string.h>

#include "au/StringCollection.h"
#include "au/StructCollection.h"
#include "au/string.h"

#include "comscore/MapId2Id.h"  // MapId2Id
#include "comscore/common.h"    // common definitions

bool match(const char *s1, const char *s2);

namespace samson {
namespace comscore {
// -----------------------------------------------------------------
// URL
//
// Utility class to split url in parts
//
// -----------------------------------------------------------------

class URL {
public:

  std::string domain;
  std::string pre_domain;
  std::string core_domain;
  std::string path;

  URL(std::string url);
};

// -----------------------------------------------------------------
// DictionaryEntry
//
// Structure for main dictionary element
//
// -----------------------------------------------------------------


typedef struct {
  uint id;
  uint domain;
  uint pre_domain_pattern;
  uint path_pattern;
} DictionaryEntry;


// -----------------------------------------------------------------
// Header
//
// HEader used for read or write dictionary to disk
//
// -----------------------------------------------------------------

typedef struct {
  // StringCollection
  // StructCollection<SamsonDictionaryEntry>

  size_t size_string_collection;
  size_t size_struct_collection_dictionary_entries;
  size_t size_struct_collection_pattern_to_category;
  size_t size_struct_collection_categories;
} Header;


// -----------------------------------------------------------------
// SamsonComscoreDictionary
//
// Main class for TID implementation of comscore dictionary
//
// -----------------------------------------------------------------

class SamsonComscoreDictionary {
  Header header;

protected:
  au::StringCollection string_collection;
  au::StructCollection<DictionaryEntry> dictionary_entries;
  MapId2Id pattern_to_category;
  MapId2Id categories;

public:

  SamsonComscoreDictionary();

  // Get categories for a provided url
  std::vector<uint> getCategories(const char *url);

  // Get description name for a particular category
  const char *getCategoryName(uint id);

  // Methods to add elements during construction of SamsonComscoreDictionary
  void push(OriginalDictionaryEntry& original_dictionary_entry);
  void push_pattern_to_category(Id2Id entry);
  void push_category(uint id, std::string category);

  // Read and write content to file
  void write(const char *file_name);
  void read(const char *file_name);


  // Method to access content directly
  size_t getNumEntries();
  size_t getPatternIdForEnty(size_t i);
  const char *getDomainForEntry(size_t i);
  const char *getPreDomainPatternForEntry(size_t i);
  const char *getPathPatternForEntry(size_t i);

private:

  // Methods to find pattern of a provided URL
  bool find_pattern_range(const char *core_domain, uint *begin, uint *end);
  uint find_one_pattern(const char *core_domain);
  uint find_one_pattern(const char *core_domain, uint begin, uint end);
  bool findURLPattern(const char *_url, uint *pattern);

  // Handy function to access pattern of a pattern
  const char *get_domain_for_pattern(uint pos);
  const char *get_pre_domain_for_pattern(uint pos);
  const char *get_path_for_pattern(uint pos);
};
}
}

#endif // ifndef _H_SAMSON_COMSCORE
