/* ****************************************************************************
*
* FILE            main_UnitTest.cpp
*
* AUTHOR          Javier Lois
*
* DATE            December 2011
*
* DESCRIPTION
*
* Main file for the automatic unit testing application
*
*/

#include "gtest/gtest.h"




/* ****************************************************************************
*
* main - 
*/

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
