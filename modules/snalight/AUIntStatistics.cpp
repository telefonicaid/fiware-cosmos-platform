/* 
 * AUIntStatistics.cpp
 *
 * Class extracted from the au_lib to accumulate and compute statistics
 *
 */

#include "AUIntStatistics.h"

#include <cmath>
#include <cstdio>
#include <iostream>
#include <sstream>


/*
 *--------------------------------------------------------------------------------------
 *       Class:  AUIntStatistics
 *      Method:  Constructor
 * Description:
 *--------------------------------------------------------------------------------------
 */

AUIntStatistics::AUIntStatistics()
{
     count = 0 ;
     acc_mean = 0;
     acc_mean_square = 0;
     mean = 0;
     var = 0;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AUIntStatistics
 *      Method:  add
 * Description:
 *--------------------------------------------------------------------------------------
 */
void AUIntStatistics::add(int value)
{
     values[value]++;
     acc_mean += value;
     acc_mean_square += value*value;
     count++;
     //Continuous computation of mean and var
     mean = acc_mean / (double)count;
     var = (acc_mean_square / (double) count) - mean*mean;
}
/*
 *--------------------------------------------------------------------------------------
 *       Class:  AUIntStatistics
 *      Method:  printDistribution
 * Description:
 *--------------------------------------------------------------------------------------
 */
size_t AUIntStatistics::printDistribution( std::ostringstream& output)
{
     map<int, int>::const_iterator iter;
     output << "data = [ ";
     bool first = true;

     for (iter=values.begin(); iter != values.end(); ++iter) {
          if( first) first = false;
          else output << ",";

          output << iter->first;
     }

     output <<  " ; ";
     first = true;

     for (iter=values.begin(); iter != values.end(); ++iter) {
          if( first) first = false;
          else output << ",";

          output << iter->second ;
     }

     output << "]; ";
     return 0;
}

