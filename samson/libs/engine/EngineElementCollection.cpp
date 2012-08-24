
#include "au/tables/Table.h"

#include "engine/EngineElement.h"
#include "engine/EngineElementCollection.h"  // Own interface


namespace engine {
EngineElementCollection::EngineElementCollection() : token_("EngineElementCollection") {
}

EngineElementCollection::~EngineElementCollection() {
  au::TokenTaker tt(&token_);

  // Remove pending elements in Engine
  repeated_elements_.clearList();
  normal_elements_.clearList();
  extra_elements_.clearVector();
}

void EngineElementCollection::Add(EngineElement *element) {
  au::TokenTaker tt(&token_);

  // Insert an element in the rigth queue
  LM_T(LmtEngine, ("Adding Engineelement: %s", element->str().c_str()));

  if (element->IsRepeated()) {
    repeated_elements_.insert(FindPositionForRepeatedEngineElement(element),  element);
  } else if (element->IsExtra()) {
    extra_elements_.push_back(element);
  } else {
    normal_elements_.push_back(element);
  }
}

bool EngineElementCollection::IsEmpty() {
  au::TokenTaker tt(&token_);

  if (repeated_elements_.size() > 0) {
    return false;
  }
  if (normal_elements_.size() > 0) {
    return false;
  }
  if (extra_elements_.size() > 0) {
    return false;
  }

  return true;
}

EngineElement *EngineElementCollection::NextRepeatedEngineElement() {
  au::TokenTaker tt(&token_);

  // Check first repeated elements
  if (repeated_elements_.size() == 0) {
    return NULL;
  }

  double t_sleep = repeated_elements_.front()->GetTimeToTrigger();

  // If ready to be executed....
  if (t_sleep < 0.01) {
    EngineElement *element = repeated_elements_.front();
    repeated_elements_.pop_front();
    return element;
  } else {
    return NULL;
  }
}

EngineElement *EngineElementCollection::NextNormalEngineElement() {
  au::TokenTaker tt(&token_);

  if (normal_elements_.size() == 0) {
    return NULL;
  }

  EngineElement *element = normal_elements_.front();
  normal_elements_.pop_front();

  return element;
}

double EngineElementCollection::TimeForNextRepeatedEngineElement() {
  au::TokenTaker tt(&token_);

  // Check first repeated elements
  if (repeated_elements_.size() == 0) {
    return 0;
  }

  double t_sleep = repeated_elements_.front()->GetTimeToTrigger();

  // If ready to be executed....
  if (t_sleep < 0) {
    return 0;
  }

  return t_sleep;
}

size_t EngineElementCollection::GetNumEngineElements() {
  size_t total = 0;

  total += repeated_elements_.size();
  total += normal_elements_.size();
  total += extra_elements_.size();

  return total;
}

size_t EngineElementCollection::GetNumNormalEngineElements() {
  return normal_elements_.size();
}

std::string EngineElementCollection::GetTableOfEngineElements() {
  au::TokenTaker tt(&token_);

  au::tables::Table table("Type|Waiting,f=double|Period,f=double|Description,left");

  au::list<EngineElement>::iterator it_elements;
  for (it_elements = repeated_elements_.begin(); it_elements != repeated_elements_.end(); it_elements++) {
    EngineElement *element = *it_elements;

    au::StringVector values;
    values.push_back("Repeat");
    values.push_back(au::str("%.12f", element->GetWaitingTime()));
    values.push_back(au::str("%.12f", element->period()));
    // values.push_back(element->getName());
    values.push_back(element->description());
    table.addRow(values);
  }

  for (it_elements = normal_elements_.begin(); it_elements != normal_elements_.end(); it_elements++) {
    EngineElement *element = *it_elements;

    au::StringVector values;
    values.push_back("Normal");
    values.push_back(au::str("%.12f", element->GetWaitingTime()));
    values.push_back("X");
    // values.push_back(element->getName());
    values.push_back(element->description());
    table.addRow(values);
  }

  for (size_t i = 0; i < extra_elements_.size(); i++) {
    EngineElement *element = extra_elements_[i];

    au::StringVector values;
    values.push_back("Extra");
    values.push_back(au::str("%.12f", element->GetWaitingTime()));
    values.push_back("X");
    // values.push_back(element->getName());
    values.push_back(element->description());
    table.addRow(values);
  }

  return table.str();
}

void EngineElementCollection::PrintElements() {
  au::TokenTaker tt(&token_);

  // Print entire engine items...

  au::list<EngineElement>::iterator it_elements;
  for (it_elements = repeated_elements_.begin(); it_elements != repeated_elements_.end(); it_elements++) {
    EngineElement *element = *it_elements;
    LM_M(("ENGINE REPEATED ELEMENT: %s", element->str().c_str()));
  }

  for (it_elements = normal_elements_.begin(); it_elements != normal_elements_.end(); it_elements++) {
    EngineElement *element = *it_elements;
    LM_M(("ENGINE NORMAL ELEMENT: %s", element->str().c_str()));
  }

  for (size_t i = 0; i < extra_elements_.size(); i++) {
    EngineElement *element = extra_elements_[i];
    LM_M(("ENGINE EXTRA ELEMENT: %s", element->str().c_str()));
  }
}

std::list<EngineElement *>::iterator EngineElementCollection::FindPositionForRepeatedEngineElement(EngineElement *e) {
  for (std::list<EngineElement *>::iterator i = repeated_elements_.begin(); i != repeated_elements_.end(); i++) {
    if ((*i)->GetTimeToTrigger() > e->GetTimeToTrigger()) {
      return i;
    }
  }
  return repeated_elements_.end();
}

std::vector<EngineElement *> EngineElementCollection::ExtraElements() {
  au::TokenTaker tt(&token_);

  std::vector<EngineElement *> tmp;
  for (size_t i = 0; i < extra_elements_.size(); i++) {
    EngineElement *element = extra_elements_[i];
    tmp.push_back(element);
  }
  return tmp;
}

size_t EngineElementCollection::GetMaxWaitingTimeInEngineStack() {
  au::TokenTaker tt(&token_);

  if (normal_elements_.size() == 0) {
    return 0;
  }

  EngineElement *last_element =  normal_elements_.back();
  return last_element->GetWaitingTime();
}
}
