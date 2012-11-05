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
