/*
 * AUIntStatistics.h
 *
 * Class extracted from the au_lib to accumulate and compute statistics
 *
 */

#ifndef _H_AUINTSTATISTICS
#define _H_AUINTSTATISTICS

#include <map>
#include <cstdio>
#include <iostream>

using namespace std;

class AUIntStatistics {
          map <int,int> values;

          double acc_mean;
          double acc_mean_square;

     public:

          //Continuous computation of mean / var
          double mean;
          double var;

          size_t count;

          AUIntStatistics();

          void add(int value);

          size_t printDistribution( std::ostringstream& output);
};


#endif
