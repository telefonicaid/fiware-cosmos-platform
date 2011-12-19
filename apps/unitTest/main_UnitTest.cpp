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

#include "engine/MemoryManager.h"
#include "engine/Engine.h"
#include "engine/DiskManager.h"
#include "engine/ProcessManager.h"

#include "gtest/gtest.h"

//#include "main_UnitTest.h"


// Tests engine's instantiation
TEST(EngineTest, instantiation_test) {
  EXPECT_EQ(2,2);
}

