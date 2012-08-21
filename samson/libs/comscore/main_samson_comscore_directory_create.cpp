/* ****************************************************************************
 *
 * FILE                     main_samson_comscore_directory_create.cpp
 *
 * AUTHOR                   Andreu Urrueka
 *
 * CREATION DATE            2012
 *
 */

#include "logMsg/logMsg.h"
#include "logMsg/traceLevels.h"
#include "parseArgs/parseArgs.h"

#include <algorithm>
#include <signal.h>

#include "au/ThreadManager.h"
#include "au/mutex/LockDebugger.h"            // au::LockDebugger

#include "comscore/SamsonComscoreDictionary.h"
#include "comscore/common.h"


/* ****************************************************************************
 *
 * Option variables
 */

int max_num_entries;



char comsore_dictionary_file_name[1024];
char pattern_to_category_file_name[1024];
char comscore_categories_files[1024];
char samson_comsore_dictionary_file_name[1024];

#define DEF_DICTIONARY  "/var/comscore/cs_mmxi.bcp"
#define DEF_PATTERN_CAT "/var/comscore/pattern_category_mapping.txt"
#define DEF_CAT         "/var/comscore/cat_subcat_lookup.txt"
#define DEF_OUTPUT      "samson_comscore_dictionary.bin"

/* ****************************************************************************
 *
 * parse arguments
 */

PaArgument paArgs[] =
{
  { "-source",                      comsore_dictionary_file_name,                             "",
    PaString,                    PaOpt,
    _i DEF_DICTIONARY,  PaNL,        PaNL,          "Original Comscore dictionary (.bcp)"              },
  { "-pattern_to_catergories_file", pattern_to_category_file_name,                            "",
    PaString,                     PaOpt,
    _i DEF_PATTERN_CAT, PaNL,        PaNL,          "Pattern to categories file"                       },
  { "-categories_file",             comscore_categories_files,                                "",
    PaString,                     PaOpt,
    _i DEF_CAT,         PaNL,        PaNL,          "Categories file"                                  },
  { "-output",                      samson_comsore_dictionary_file_name,                      "",
    PaString,                     PaOpt,
    _i DEF_OUTPUT,      PaNL,        PaNL,          "Output binary diccionary"                         },
  { "-max_num_entries",             &max_num_entries,                                         "",
    PaInt,                        PaOpt,   0,
    0,           1000000000,    "Maximum number of entries from original file"     },
  PA_END_OF_ARGS
};


/* ****************************************************************************
 *
 * global variables
 */

int logFd             = -1;


/* ****************************************************************************
 *
 * main -
 */

// Vector of entries from original dictionary
std::vector<samson::comscore::OriginalDictionaryEntry> original_dictionary_entries;

// Vector of relations pattern to category
std::vector<samson::comscore::Id2Id> original_pattern_to_category;

typedef  samson::comscore::uint   uint;

// Map of category names
std::map< uint, std::string > categories;


void read_original_categories_file(const char *file_name) {
  LM_M(("Reading original cageories file"));

  FILE *file = fopen(file_name, "r");

  if (!file)
    LM_X(1, ("Error reading file %s to load original categories file", file_name));
  char line[10000];
  char *fields[10];
  size_t num = 0;

  while (fgets(line, 10000, file)) {
    fields[0] = line;
    for (int f = 1; f < 3; f++) {
      char *pos = strstr(fields[f - 1], "\t");
      *pos = '\0';

      fields[f] = pos + 1;
    }

    uint id = atoll(fields[0]);
    std::string description = fields[1];

    categories.insert(std::pair< uint, std::string >(id, description));

    if ((++num % 100000) == 0)
      LM_M(("Readed %lu records", num));
  }
}

bool isPatternIdUsed(size_t id) {
  for (size_t i = 0; i < original_dictionary_entries.size(); i++) {
    if (original_dictionary_entries[i].id == id)
      return true;
  }
  return false;
}

void read_original_pattern_to_category_file(const char *file_name) {
  LM_M(("Reading original pattern to cageory file"));

  FILE *file = fopen(file_name, "r");

  if (!file)
    LM_X(1, ("Error reading file %s to load original pattern to category file", file_name));
  char line[10000];
  char *fields[10];
  size_t num = 0;

  while (fgets(line, 10000, file)) {
    fields[0] = line;
    for (int f = 1; f < 3; f++) {
      char *pos = strstr(fields[f - 1], "\t");
      *pos = '\0';

      fields[f] = pos + 1;
    }

    // Fill element
    samson::comscore::Id2Id entry;

    entry.first  = atoll(fields[0]);
    entry.second = atoll(fields[1]);

    if ((++num % 100000) == 0)
      LM_M(("Readed %lu records", num)); if (( max_num_entries > 0 ) && ( max_num_entries < 1000 ))
      if (!isPatternIdUsed(entry.first))
        // LM_M(("Pattern %lu not used...", entry.first));
        continue;
    original_pattern_to_category.push_back(entry);
  }

  // Sorting original records
  LM_M(("Sorting entries..."));

  std::sort(original_pattern_to_category.begin(), original_pattern_to_category.end(), samson::comscore::compareId2Id);
}

void read_original_dictionary_file(const char *file_name, size_t max_num_records = 0) {
  LM_M(("Reading original dictionary file '%s'", file_name ));

  if (max_num_records > 0)
    LM_M(("Max number records %d", max_num_records )); FILE *file = fopen(file_name, "r");

  if (!file)
    LM_X(1, ("Error reading file %s to load original dictionary file", file_name));
  char line[10000];
  char *fields[11];
  size_t num = 0;

  while (fgets(line, 10000, file)) {
    fields[0] = line;
    for (int f = 1; f < (int)(sizeof(fields) / sizeof(char *)); f++) {
      char *pos = strstr(fields[f - 1], "\t");
      *pos = '\0';

      fields[f] = pos + 1;
    }

    if (atoi(fields[10]) == -1) {
      LM_W(("Discarted element"));
      continue;
    }

    // Fill element
    samson::comscore::OriginalDictionaryEntry dictionary_entry;

    dictionary_entry.id = atoll(fields[0]);
    dictionary_entry.pre_domain_pattern = fields[1];
    dictionary_entry.domain = fields[2];
    dictionary_entry.path_pattern = fields[5];
    dictionary_entry.order_1 = atoll(fields[6]);
    dictionary_entry.order_2 = atoll(fields[7]);


    if (
      ( strstr(dictionary_entry.domain.c_str(), "/") != NULL ) ||
      ( strstr(dictionary_entry.domain.c_str(), "%") != NULL ) ||
      ( strcmp(dictionary_entry.domain.c_str(), "") == 0 )
      )
    {
      LM_W(("Discarting domain '%s' [%s][%s]"
            , dictionary_entry.domain.c_str()
            , dictionary_entry.pre_domain_pattern.c_str()
            , dictionary_entry.path_pattern.c_str()
            ));
      continue;
    }

    // Replace % by * in pre_domain_pattern and path_pattern
    replace(dictionary_entry.pre_domain_pattern.begin(), dictionary_entry.pre_domain_pattern.end(), '%', '*');
    replace(dictionary_entry.path_pattern.begin(), dictionary_entry.path_pattern.end(), '%', '*');


    // Remove first "/" and last "/" in path if exist
    size_t l = dictionary_entry.path_pattern.length();

    if (l > 0) {
      if (dictionary_entry.path_pattern.substr(0, 1) == "/")
        dictionary_entry.path_pattern.erase(0, 1); if (dictionary_entry.path_pattern.substr(l - 1, 1) == "/")
        dictionary_entry.path_pattern.erase(l - 1, 1);
    }

    // Push back this entry
    original_dictionary_entries.push_back(dictionary_entry);

    // Increase counter of records
    num++;

    // Max number of records limit
    if (max_num_records > 0)
      if (num >= max_num_records)
        break; if ((++num % 100000) == 0)
      LM_M(("Readed %lu records", num));
  }

  // Close the file
  fclose(file);

  // Sorting original records
  LM_M(("Sorting entries"));
  std::sort(original_dictionary_entries.begin(), original_dictionary_entries.end(), samson::comscore::compareOriginalDictionaryEntry);
}

int main(int argC, const char *argV[]) {
  paConfig("builtin prefix",                (void *)"SS_WORKER_");
  paConfig("usage and exit on any warning", (void *)true);

  paConfig("log to screen",                 (void *)true);

  paConfig("log file line format",          (void *)"TYPE:DATE:EXEC-AUX/FILE[LINE](p.PID)(t.TID) FUNC: TEXT");
  paConfig("screen line format",            (void *)"TYPE@TIME  EXEC: TEXT");
  paConfig("log to file",                   (void *)true);

  paParse(paArgs, argC, (char **)argV, 1, false);


  // Reading original files
  read_original_dictionary_file(comsore_dictionary_file_name, max_num_entries);
  read_original_pattern_to_category_file(pattern_to_category_file_name);
  read_original_categories_file(comscore_categories_files);


  // Creating the file
  LM_M(("Creating samson comscore dictionary"));
  samson::comscore::SamsonComscoreDictionary samson_comscore_dictionary;

  LM_M(("Adding main dictionary entries"));
  for (size_t i = 0; i < original_dictionary_entries.size(); i++) {
    if ((i % 100000) == 0)
      LM_M(("Progress %lu records", i)); samson_comscore_dictionary.push(original_dictionary_entries[i]);
  }

  LM_M(("Adding pattern to dictionary mapping entries"));
  for (size_t i = 0; i < original_pattern_to_category.size(); i++) {
    if ((i % 100000) == 0)
      LM_M(("Progress %lu records", i)); samson_comscore_dictionary.push_pattern_to_category(original_pattern_to_category[i]);
  }

  LM_M(("Adding category description"));
  std::map< uint, std::string >::iterator it_categories;
  for (it_categories = categories.begin(); it_categories != categories.end(); it_categories++) {
    samson_comscore_dictionary.push_category(it_categories->first, it_categories->second);
  }

  LM_M(("Writing comscore dictionary to file %s", samson_comsore_dictionary_file_name));
  samson_comscore_dictionary.write(samson_comsore_dictionary_file_name);
}

