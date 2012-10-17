#include "samson/common/KVRange.h"  // Own interface

#include "au/string.h"
#include "au/xml.h"

#include "logMsg/logMsg.h"

#include "samson/common/FullKVInfo.h"
#include "samson/common/KVInfo.h"

namespace samson {
#pragma mark KVRange

void KVRange::set(int hg_begin, int hg_end) {
  hg_begin_ = hg_begin;
  hg_end_ = hg_end;
}

void KVRange::setFrom(KVInfo *info) {
  int i = 0;

  while ((info[i].size == 0) && (i < KVFILE_NUM_HASHGROUPS)) {
    ++i;
  }

  if (i == KVFILE_NUM_HASHGROUPS) {
    LM_W(("No content for generated block"));
    set(0, 1); // Smaller set
    return;
  }

  hg_begin_ = i;

  int j = KVFILE_NUM_HASHGROUPS;
  while ((info[j - 1].size == 0) && (j > (i + 1))) {
    --j;
  }

  hg_end_ = j;

  // LM_M(("KVRange for output block %s" , str().c_str()));
}

void KVRange::setFrom(FullKVInfo *info) {
  int i = 0;

  while ((info[i].size == 0) && (i < KVFILE_NUM_HASHGROUPS)) {
    ++i;
  }

  if (i == KVFILE_NUM_HASHGROUPS) {
    LM_W(("No content for generated block"));
    set(0, 1); // Smaller set
    return;
  }

  hg_begin_ = i;

  int j = KVFILE_NUM_HASHGROUPS;
  while ((info[j - 1].size == 0) && (j > (i + 1))) {
    --j;
  }

  hg_end_ = j;

  // LM_M(("KVRange for output block %s" , str().c_str()));
}

void KVRange::add(KVRange range) {
  if (!range.isValid()) {
    return;
  }

  if (range.hg_begin_ < hg_begin_) {
    hg_begin_ = range.hg_begin_;
  }
  if (range.hg_end_ > hg_end_) {
    hg_end_ = range.hg_end_;
  }
}

int KVRange::size() const {
  return hg_end_ - hg_begin_;
}

bool KVRange::isValid() const {
  if ((hg_begin_ < 0) || (hg_begin_ > (KVFILE_NUM_HASHGROUPS))) {
    return false;
  }
  if ((hg_end_ < 0) || (hg_end_ > KVFILE_NUM_HASHGROUPS)) {
    return false;
  }

  if (hg_begin_ >= hg_end_) {
    return false;
  }

  return true;
}

void KVRange::getInfo(std::ostringstream& output) const {
  au::xml_open(output, "kv_range");
  au::xml_simple(output, "hg_begin", hg_begin_);
  au::xml_simple(output, "hg_end", hg_end_);
  au::xml_close(output, "kv_range");
}

std::string KVRange::str() const {
  return au::str("[%05d %05d)", hg_begin_, hg_end_);
}

bool KVRange::IsOverlapped(const KVRange& range) const {
  if (range.hg_end_ <= hg_begin_) {
    return false;
  }

  if (range.hg_begin_ >= hg_end_) {
    return false;
  }

  return true;
}

int KVRange::getNumHashGroups() const {
  return hg_end_ - hg_begin_;
}

bool KVRange::includes(KVRange range) const {
  if (range.hg_begin_ < hg_begin_) {
    return false;
  }
  if (range.hg_end_ > hg_end_) {
    return false;
  }

  return true;
}

bool KVRange::contains(int hg) const {
  if (hg < hg_begin_) {
    return false;
  }
  if (hg >= hg_end_) {
    return false;
  }

  return true;
}

bool KVRange::contains(KVRange range) const {
  if (range.hg_begin_ < hg_begin_) {
    return false;
  }
  if (range.hg_end_ > hg_end_) {
    return false;
  }

  return true;
}

// Get the maximum division pattern for this range

bool KVRange::isValidForNumDivisions(int num_divisions) const {
  int size_per_division = KVFILE_NUM_HASHGROUPS / num_divisions;

  if ((hg_end_ - hg_begin_) > size_per_division) {
    return false;
  }

  int max_hg_end = (hg_begin_ / size_per_division) * size_per_division + size_per_division;

  if (hg_end_ > max_hg_end) {
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
  for (int i = 0; i < KVFILE_NUM_HASHGROUPS; ++i) {
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
  for (int i = 0; i < num_division; ++i) {
    KVRange range = rangeForDivision(i, num_division);
    if (range.contains(hg)) {
      return i;
    }
  }
  LM_X(1, ("Internal error"));
  return -1;
}

// Get the aggregation of
KVInfo selectRange(KVInfo *info, KVRange range) {
  KVInfo tmp;

  tmp.clear();

  for (int i = 0; i < KVFILE_NUM_HASHGROUPS; ++i) {
    if (range.contains(i)) {
      tmp.append(info[i]);
    }
  }

  return tmp;
}

bool operator<(const KVRange & left, const KVRange & right) {
  // What does exactly "<" mean? Previous version seemed to be copy-paste wrong
  if (left.hg_end_ < right.hg_begin_) {
    return true;
  }
  if (left.hg_begin_ > right.hg_end_) {
    return false;
  }
  if (left.hg_end_ < right.hg_end_) {
    return true;
  }
  return false;
}

bool operator!=(const KVRange & left, const KVRange & right) {
  if (left.hg_begin_ != right.hg_begin_) {
    return true;
  }

  if (left.hg_end_ != right.hg_end_) {
    return true;
  }

  return false;
}

bool operator==(const KVRange & left, const KVRange & right) {
  if (left.hg_begin_ != right.hg_begin_) {
    return false;
  }

  if (left.hg_end_ != right.hg_end_) {
    return false;
  }

  return true;
}

KVRange rangeForIntersection(const KVRange & left, const KVRange & right) {
  if (left.hg_end_ >= right.hg_begin_) {
    return KVRange(0, 0); // No intersection
  }
  if (right.hg_end_ >= left.hg_begin_) {
    return KVRange(0, 0); // No intersection
  }
  return KVRange(std::max(left.hg_begin_, right.hg_begin_), std::min(left.hg_end_, left.hg_end_));
}

std::string str(const std::vector<KVRange>& ranges) {
  std::ostringstream output;
  size_t ranges_size = ranges.size();
  for (size_t i = 0; i < ranges_size; ++i) {
    output << ranges[i].str() << " ";
  }
  return output.str();
}

bool CheckCompleteKVRanges(const std::vector<KVRange>& ranges) {
  for (int i = 0; i < KVFILE_NUM_HASHGROUPS; i++) {
    int total = 0;
    size_t ranges_size = ranges.size();
    for (size_t r = 0; r < ranges_size; ++r) {
      if (ranges[r].contains(i)) {
        ++total;
      }
    }

    if (total != 1) {
      return false;
    }

  }
  return true;
}
}
