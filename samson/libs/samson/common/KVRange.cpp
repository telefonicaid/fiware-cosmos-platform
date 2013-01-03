/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */
#include "samson/common/KVRange.h"  // Own interface

#include "au/string/StringUtilities.h"
#include "au/string/xml.h"

#include "logMsg/logMsg.h"

#include "samson/common/FullKVInfo.h"
#include "samson/common/KVInfo.h"

namespace samson {
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
    LOG_SW(("No content for generated block"));
    set(0, 1);  // Smaller set
    return;
  }

  hg_begin_ = i;

  int j = KVFILE_NUM_HASHGROUPS;
  while ((info[j - 1].size == 0) && (j > (i + 1))) {
    --j;
  }

  hg_end_ = j;

  // LOG_SM(("KVRange for output block %s" , str().c_str()));
}

void KVRange::setFrom(FullKVInfo *info) {
  int i = 0;

  while ((info[i].size == 0) && (i < KVFILE_NUM_HASHGROUPS)) {
    ++i;
  }

  if (i == KVFILE_NUM_HASHGROUPS) {
    LOG_SW(("No content for generated block"));
    set(0, 1);  // Smaller set
    return;
  }

  hg_begin_ = i;

  int j = KVFILE_NUM_HASHGROUPS;
  while ((info[j - 1].size == 0) && (j > (i + 1))) {
    --j;
  }

  hg_end_ = j;

  // LOG_SM(("KVRange for output block %s" , str().c_str()));
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

  if (hg_begin_ > hg_end_) {
    return false;
  }

  return true;
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

double KVRange::GetOverlapFactor(const KVRange& range) const {
  return (double)Intersection(range).size() / (double)size();
}

bool KVRange::IsOverlapped(const std::vector<KVRange>& ranges) {
  size_t ranges_size = ranges.size();

  for (size_t i = 0; i < ranges_size; ++i) {
    if (IsOverlapped(ranges[i])) {
      return true;
    }
  }
  return false;
}

bool KVRange::Includes(KVRange range) const {
  if (range.hg_begin_ < hg_begin_) {
    return false;
  }
  if (range.hg_end_ > hg_end_) {
    return false;
  }

  return true;
}

bool KVRange::Contains(int hg) const {
  if (hg < hg_begin_) {
    return false;
  }
  if (hg >= hg_end_) {
    return false;
  }

  return true;
}

bool KVRange::Contains(KVRange range) const {
  if (range.hg_begin_ < hg_begin_) {
    return false;
  }
  if (range.hg_end_ > hg_end_) {
    return false;
  }

  return true;
}

/*
 * bool KVRange::check(KVInfo *info) const {
 * for (int i = 0; i < KVFILE_NUM_HASHGROUPS; ++i) {
 *  if (!contains(i)) {
 *    if (info[i].size != 0) {
 *      return false;
 *    }
 *  }
 * }
 * return true;
 * }
 */

KVRange GetKVRangeForDivision(int pos, int num_divisions) {
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
    KVRange range = GetKVRangeForDivision(i, num_division);
    if (range.Contains(hg)) {
      return i;
    }
  }
  LM_X(1, ("Internal error"));
  return -1;
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
      if (ranges[r].Contains(i)) {
        ++total;
      }
    }

    if (total != 1) {
      return false;
    }
  }
  return true;
}

KVRange KVRange::Intersection(KVRange range) const {
  // Disjoint ranges
  if (range.hg_end_ <= hg_begin_) {
    return KVRange(0, 0);
  }

  if (range.hg_begin_ >= hg_end_) {
    return KVRange(0, 0);
  }

  return KVRange(std::max(hg_begin_, range.hg_begin_), std::min(hg_end_, range.hg_end_));
}

std::vector<KVRange> KVRange::divide(int factor) const {
  // Max factor
  if (factor > (hg_end_ - hg_begin_)) {
    factor = hg_end_ - hg_begin_;
  }

  // Divide range in a number of sub ranges
  int length = (hg_end_ - hg_begin_) / factor;

  std::vector<KVRange> ranges;
  for (int i = 0; i < (factor - 1); ++i) {
    ranges.push_back(KVRange(hg_begin_ + i * length, hg_begin_ + (i + 1) * length));
  }
  ranges.push_back(KVRange(hg_begin_ + (factor - 1) * length, hg_end_));
  return ranges;
}
}
