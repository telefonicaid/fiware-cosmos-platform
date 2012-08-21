
/* ****************************************************************************
 *
 * FILE                     ModuleContainer.cpp
 *
 * DESCRIPTION				Information about a module (module definition)
 *
 * ***************************************************************************/

#include "AUTockenizer.h"                       // AUTockenizer
#include "DataContainer.h"
#include "DataCreator.h"
#include "ModuleContainer.h"
#include <cstdio>
#include <map>

namespace samson {
ModuleContainer::ModuleContainer() {
}

#pragma mark DEFINE NAME

std::string ModuleContainer::getDefineUniqueName() {
  std::ostringstream o;

  o << "_H_SS_";

  std::vector<std::string> tockens = tockenizeWithDots(name.c_str());
  for (size_t i = 0; i < tockens.size(); i++) {
    o << tockens[i] << "_";
  }

  o << "Module";
  return o.str();
}

#pragma mark Begin and End namepsace definitions

std::string ModuleContainer::getClassName() {
  return "Module";
}

std::string ModuleContainer::getFullClassName() {
  std::ostringstream o;

  o << "samson::";

  std::vector<std::string> tockens = tockenizeWithDots(name.c_str());
  for (size_t i = 0; i < tockens.size(); i++) {
    o << tockens[i] << "::";
  }

  o << "Module";
  return o.str();
}

#pragma mark Begin and End namepsace definitions


std::vector<std::string> ModuleContainer::tockenizeWithDots(std::string myString) {
  if (myString.length() > 1000) {
    fprintf(stderr, "samsonModuleParser: Error tokenizing a string with more than 1000 characters");
    exit(1);
  }

  char tmp[1000];
  strcpy(tmp, myString.c_str());
  std::vector<std::string> tockens;
  char *p = strtok(tmp, ".");
  while (p) {
    tockens.push_back(std::string(p));
    p = strtok(NULL, " ");
  }
  return tockens;
}

void ModuleContainer::parse(AUTockenizer *module_creator,  int begin, int end) {
  int pos = begin;

  while (pos < end) {
    if (module_creator->isSpecial(pos)) {
      fprintf(stderr, "samsonModuleParser: Error parsing module definition at line:%d\n", module_creator->items[pos].line);
      exit(1);
    }

    std::string mainCommand = module_creator->itemAtPos(pos++).str;

    if (mainCommand == "title") {
      title = module_creator->getLiteral(&pos);
    } else if (mainCommand == "author") {
      author = module_creator->getLiteral(&pos);
    } else if (mainCommand == "version") {
      version = module_creator->getLiteral(&pos);
    } else if (mainCommand == "include") {
      std::string fileName = module_creator->itemAtPos(pos++).str;
      includes.push_back(fileName);
    } else if (mainCommand == "set") {
      std::string setName             = module_creator->itemAtPos(pos++).str;
      std::string setValue    = module_creator->itemAtPos(pos++).str;
      sets.insert(std::pair<std::string, std::string>(setName, setValue));
    } else if (mainCommand == "help") {
      int begin, end;

      module_creator->getScopeLimits(&pos, &begin, &end);
      while (begin < end) {
        help.push_back(module_creator->getLiteral(&begin));
      }
    } else {
      fprintf(stderr, "samsonModuleParser: Error: Unknown command inside module section (%s), at line:%d\n",
              mainCommand.c_str(), module_creator->items[pos].line);
      exit(1);
    }
  }

  if (pos != (end + 1)) {
    fprintf(stderr, "samsonModuleParser: Error parsing module definition. Invalid number of items in the definition at line:%d\n",
            module_creator->items[pos].line);
    exit(1);
  }
}
}
