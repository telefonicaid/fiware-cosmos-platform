#include "logMsg/logMsg.h"
#include "logMsg/traceLevels.h"

#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <iostream>

#include "comscore/common.h"
#include "comscore/SamsonComscoreDictionary.h"

#define MAX_BUF 10000
#define NOTIFY_EVERY 100000

using namespace std;

extern char *progName; // Bug workaround

// Vector of entries from original dictionary
std::vector<samson::comscore::OriginalDictionaryEntry>
    original_dictionary_entries;

// Vector of relations pattern to category
std::vector<samson::comscore::Id2Id> original_pattern_to_category;

typedef  samson::comscore::uint uint;

// Map of category names
std::map< uint , std::string > categories;

void read_original_categories_file(const char* file_name) {
    cout << "Reading original categories file" << endl;

    FILE *file = fopen(file_name, "r");

    if(!file) {
        cerr << "Error reading file " << file_name <<
	    " to load original categories file" << endl;
	exit(1);
    }

    char line[MAX_BUF];
    char* fields[10];
    size_t num = 0;

    while(fgets(line, MAX_BUF, file)) {
        fields[0] = line;
        for (int f = 1; f < 3; f++) {
            char * pos = strstr(fields[f-1], "\t");
            *pos = '\0';
            fields[f] = pos+1;
        }

        uint id = atoll(fields[0]);
        std::string description = fields[1];

        categories.insert(pair<uint, string>(id, description));

        if ((++num % NOTIFY_EVERY) == 0) {
            cout << "Readed " << num << " records" << endl;
	}
    }
}

void read_original_pattern_to_category_file(const char* file_name) {
    cout << "Reading original pattern to cageory file" << endl;

    FILE *file = fopen(file_name, "r");

    if(!file) {
	cout << "Error reading file " << file_name <<
	    " to load original pattern to category file" << endl;
	exit(1);
    }

    char line[MAX_BUF];
    char* fields[10];
    size_t num = 0;

    while(fgets(line, MAX_BUF , file))
    {
        fields[0] = line;
        for (int f = 1 ; f < 3 ; f++)
        {
            char * pos = strstr(fields[f-1], "\t");
            *pos = '\0';

            fields[f] = pos+1;
        }

        // Fill element
        samson::comscore::Id2Id entry;

        entry.first  = atoll(fields[0]);
        entry.second = atoll(fields[1]);


        // Push back this entry
        original_pattern_to_category.push_back(entry);

        if ((++num % NOTIFY_EVERY)==0) {
            cout << "Readed " << num << " records" << endl;
	}
    }

    // Sorting original records
    cout << "Sorting entries..." << endl;

    std::sort(original_pattern_to_category.begin(), original_pattern_to_category.end() , samson::comscore::compareId2Id);

}


void read_original_dictionary_file(const char * file_name)
{
    cout << "Reading original dictionary file" << endl;

    FILE *file = fopen(file_name , "r");

    if(!file) {
        cout << "Error reading file " << file_name 
	     << " to load original dictionary file" << endl;
	exit(1);
    }

    char line[10000];
    char* fields[10];
    size_t num = 0;

    while(fgets(line, 10000 , file))
    {
        fields[0] = line;
        for (int f = 1 ; f < 10 ; f++)
        {
            char * pos = strstr(fields[f-1], "\t");
            *pos = '\0';

            fields[f] = pos+1;
        }

        // Fill element
        samson::comscore::OriginalDictionaryEntry dictionary_entry;

        dictionary_entry.id = atoll(fields[0]);
        dictionary_entry.pre_domain_pattern = fields[1];
        dictionary_entry.domain = fields[2];
        dictionary_entry.path_pattern = fields[5];
        dictionary_entry.order_1 = atoll(fields[6]);
        dictionary_entry.order_2 = atoll(fields[7]);


        if(
           (strstr(dictionary_entry.domain.c_str(), "/") != NULL) ||
           (strstr(dictionary_entry.domain.c_str(), "%") != NULL) ||
           (strcmp(dictionary_entry.domain.c_str(), "") == 0)
) {
            cerr << "Discarting domain '" << dictionary_entry.domain << "' ["
		 << dictionary_entry.pre_domain_pattern << "]["
                 << dictionary_entry.path_pattern << "]" << endl;
            continue;
        }

        // Replace % by * in pre_domain_pattern and path_pattern
        replace(dictionary_entry.pre_domain_pattern.begin(), 
		dictionary_entry.pre_domain_pattern.end(), '%', '*');
        replace(dictionary_entry.path_pattern.begin(), 
		dictionary_entry.path_pattern.end(), '%', '*');

        // Remove first "/" and last "/" in path if exist
        size_t l = dictionary_entry.path_pattern.length();

        if (l > 0) {
            if (dictionary_entry.path_pattern.substr(0,1) == "/")
                dictionary_entry.path_pattern.erase(0,1);
            if (dictionary_entry.path_pattern.substr(l-1,1) == "/")
                dictionary_entry.path_pattern.erase(l-1,1);
        }

        // Push back this entry
        original_dictionary_entries.push_back(dictionary_entry);

        if ((++num % NOTIFY_EVERY)==0) {
            cout << "Readed " << num << " records" << endl;
	}
    }

    // Sorting original records
    cout << "Sorting entries" << endl;
    std::sort(original_dictionary_entries.begin(),
	      original_dictionary_entries.end(),
	      samson::comscore::compareOriginalDictionaryEntry);
}

int main(int argC, const char *argV[]) {
    lmInitX((char*)argV[0], NULL, NULL, NULL);
    progName = strdup(progName); // Bug workaround

    if (argC != 2) {
	cerr << "Usage: " << argV[0] << " dictionary_path" << endl;
	exit(1);
    }

    string base = argV[1];
    string comsore_dictionary_file_name  = base + "/cs_mmxi.bcp";
    string pattern_to_category_file_name = base + "/pattern_category_mapping.txt";
    string comscore_categories_files     = base + "/cat_subcat_lookup.txt";

    // Reading original files
    read_original_dictionary_file(comsore_dictionary_file_name.c_str());
    read_original_pattern_to_category_file(pattern_to_category_file_name.c_str());
    read_original_categories_file(comscore_categories_files.c_str());

    // ------------------------------------
    // Creating the file
    // ------------------------------------
    cout << "Creating samson comscore dictionary" << endl;
    samson::comscore::SamsonComscoreDictionary samson_comscore_dictionary;

    cout << "Adding main dictionary entries" << endl;
    for (size_t i = 0; i < original_dictionary_entries.size(); i++) {
        if ((++i % NOTIFY_EVERY)==0) {
            cout << "Progress " << i << " records" << endl;
	}
        samson_comscore_dictionary.push(original_dictionary_entries[i]);
    }

    cout << "Adding pattern to dictionary mapping entries" << endl;
    for (size_t i = 0; i < original_pattern_to_category.size(); i++) {
        if ((++i % NOTIFY_EVERY)==0) {
            cout << "Progress " << i << " records" << endl;
	}
        samson_comscore_dictionary.push_pattern_to_category(original_pattern_to_category[i]);
    }

    cout << "Adding category description" << endl;
    std::map< uint , std::string >::iterator it_categories;
    for(it_categories = categories.begin() ; it_categories != categories.end() ; it_categories++)
        samson_comscore_dictionary.push_category(it_categories->first , it_categories->second);

    const char* output_file = "dictionary.bin";
    cout << "Writing comscore dictionary to file " << output_file << endl;
    samson_comscore_dictionary.write(output_file);
}
