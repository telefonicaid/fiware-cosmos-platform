#include "samson/common/KVRange.h"  // Own interface

#include "au/string.h"
#include "au/xml.h"

#include "logMsg/logMsg.h"

#include "samson/common/FullKVInfo.h"
#include "samson/common/KVInfo.h"


namespace samson {
#pragma mark KVRange

KVRange::KVRange() {
  hg_begin = 0;
  hg_end = KVFILE_NUM_HASHGROUPS;
}

KVRange::KVRange(int _hg_begin, int _hg_end) {
  hg_begin = _hg_begin;
  hg_end = _hg_end;
}

void KVRange::set(int _hg_begin, int _hg_end) {
  hg_begin = _hg_begin;
  hg_end = _hg_end;
}

void KVRange::setFrom(KVInfo *info) {
  int i = 0;

  while ((info[i].size == 0) && ( i < KVFILE_NUM_HASHGROUPS )) {
    i++;
  }

  if (i == KVFILE_NUM_HASHGROUPS) {
    LM_W(("No content for generated block"));
    set(0, 1);    // Smaller set
    return;
  }

  hg_begin = i;

  int j = KVFILE_NUM_HASHGROUPS;
  while ((info[j - 1].size == 0) && ( j > (i + 1))) {
    j--;
  }

  hg_end = j;

  // LM_M(("KVRange for output block %s" , str().c_str()));
}

void KVRange::setFrom(FullKVInfo *info) {
  int i = 0;

  while ((info[i].size == 0) && ( i < KVFILE_NUM_HASHGROUPS )) {
    i++;
  }

  if (i == KVFILE_NUM_HASHGROUPS) {
    LM_W(("No content for generated block"));
    set(0, 1);    // Smaller set
    return;
  }

  hg_begin = i;

  int j = KVFILE_NUM_HASHGROUPS;
  while ((info[j - 1].size == 0) && ( j > (i + 1))) {
    j--;
  }

  hg_end = j;

  // LM_M(("KVRange for output block %s" , str().c_str()));
}

void KVRange::add(KVRange range) {
  if (!range.isValid()) {
    return;
  }

  if (range.hg_begin < hg_begin) {
    hg_begin = range.hg_begin;
  }
  if (range.hg_end > hg_end) {
    hg_end = range.hg_end;
  }
}

int KVRange::size() const {
  return hg_end - hg_begin;
}

bool KVRange::isValid() const {
  if (( hg_begin < 0 ) || (hg_begin > (KVFILE_NUM_HASHGROUPS))) {
    return false;
  }
  if (( hg_end < 0 ) || (hg_end > KVFILE_NUM_HASHGROUPS )) {
    return false;
  }

  if (hg_begin >= hg_end) {
    return false;
  }

  return true;
}

void KVRange::getInfo(std::ostringstream& output) const {
  au::xml_open(output, "kv_range");
  au::xml_simple(output, "hg_begin", hg_begin);
  au::xml_simple(output, "hg_end", hg_end);
  au::xml_close(output, "kv_range");
}

std::string KVRange::str() const {
  return au::str("[%d %d]", hg_begin, hg_end);
}

bool KVRange::IsOverlapped(const KVRange& range) const {
  if (range.hg_end <= hg_begin) {
    return false;
  }

  if (range.hg_begin >= hg_end) {
    return false;
  }

  return true;
}

int KVRange::getNumHashGroups() const {
  return hg_end - hg_begin;
}

bool KVRange::includes(KVRange range) const {
  if (range.hg_begin < hg_begin) {
    return false;
  }
  if (range.hg_end > hg_end) {
    return false;
  }

  return true;
}

bool KVRange::contains(int hg) const {
  if (hg < hg_begin) {
    return false;
  }
  if (hg >= hg_end) {
    return false;
  }

  return true;
}

bool KVRange::contains(KVRange range) const {
  if (range.hg_begin < hg_begin) {
    return false;
  }
  if (range.hg_end > hg_end) {
    return false;
  }

  return true;
}

// Get the maximum division pattern for this range

bool KVRange::isValidForNumDivisions(int num_divisions) const {
  int size_per_division = KVFILE_NUM_HASHGROUPS / num_divisions;

  if (( hg_end - hg_begin ) > size_per_division) {
    return false;
  }

  int max_hg_end = ( hg_begin / size_per_division ) * size_per_division + size_per_division;

  if (hg_end > max_hg_end) {
    return false;
  }

  return true;
}

int KVRange::getMaxNumDivisions() const {
  int num_divisions = 1;

  while (true) {
    if (isValidForNumDivisions(num_divisions * 2)) {
      num_divisions *= 2;
    } else {
      return num_divisions;
    }
  }
}

bool KVRange::check(KVInfo *info) const {
  for (int i = 0; i < KVFILE_NUM_HASHGROUPS; i++) {
    if (!contains(i)) {
      if (info[i].size != 0) {
        return false;
      }
    }
  }
  return true;
}

KVRange rangeForDivision(int pos, int num_divisions) {
  int size_per_division = KVFILE_NUM_HASHGROUPS / num_divisions;
  int hg_begin = pos * size_per_division;
  int hg_end = (pos + 1) * size_per_division;

  if (pos == (num_divisions - 1)) {
    hg_end = KVFILE_NUM_HASHGROUPS;
  }
  return KVRange(hg_begin, hg_end);
}

int divisionForHashGroup(int hg, int num_division) {
  for (int i = 0; i < num_division; i++) {
    KVRange range = rangeForDivision(i, num_division);
    if (range.contains(hg)) {
      return i;
    }
  }
  LM_X(1, ("Interal error"));
  return -1;
}

// Get the agregation of
KVInfo selectRange(KVInfo *info, KVRange range) {
  KVInfo tmp;

  tmp.clear();

  for (int i = 0; i < KVFILE_NUM_HASHGROUPS; i++) {
    if (range.contains(i)) {
      tmp.append(info[i]);
    }
  }

  return tmp;
}

bool operator<(const KVRange & left, const KVRange & right) {
  // What does exactly "<" mean? Previous version seemed to be copy-paste wrong
  if (left.hg_end < right.hg_begin) {
    return true;
  }
  if (left.hg_begin > right.hg_end) {
    return false;
  }
  if (left.hg_end < right.hg_end) {
    return true;
  }
  return false;
}

bool operator!=(const KVRange & left, const KVRange & right) {
  if (left.hg_begin != right.hg_begin) {
    return true;
  }

  if (left.hg_end != right.hg_end) {
    return true;
  }

  return false;
}

bool operator==(const KVRange & left, const KVRange & right) {
  if (left.hg_begin != right.hg_begin) {
    return false;
  }

  if (left.hg_end != right.hg_end) {
    return false;
  }

  return true;
}

KVRange rangeForIntersection(const KVRange & left, const KVRange & right) {
  if (left.hg_end >= right.hg_begin) {
    return KVRange(0, 0);  // No intersection
  }
  if (right.hg_end >= left.hg_begin) {
    return KVRange(0, 0);  // No intersection
  }
  return KVRange(std::max(left.hg_begin, right.hg_begin), std::min(left.hg_end, left.hg_end));
}
  std::string str( const std::vector<KVRange>& ranges )
  {
    std::ostringstream output;
    for ( size_t i = 0 ; i < ranges.size() ; i++)
      output << ranges[i].str() << " ";
    return output.str();
  }
  bool CheckCompleteKVRanges( const std::vector<KVRange>& ranges )
  {
    for ( int i = 0 ; i < KVFILE_NUM_HASHGROUPS ; i++ )
    {
      int total = 0;
      for ( size_t r = 0 ; r < ranges.size() ; r++ )
        if( ranges[r].contains(i))
          total++;
      
      if( total != 1)
        return false;
      
    }
    return true;
  }

  
}
