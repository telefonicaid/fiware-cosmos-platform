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

#include "gtest/gtest.h"

#include "au/CommandLine.h"


TEST(au_CommandLine, simple) {
  int argc = 2;
  const char *argv[] = { "A", "B" };

  au::CommandLine cmdLine(argc, argv);


  ASSERT_TRUE(cmdLine.get_num_arguments() == 2) << "Wrong number of arguments";
  ASSERT_TRUE(cmdLine.get_argument(0) == "A") << "Error parsing argument 0";
  ASSERT_TRUE(cmdLine.get_argument(1) == "B") << "Error parsing argument 1";
}

TEST(CommandLine, simple2) {
  au::CommandLine cmdLine("A B");

  ASSERT_TRUE(cmdLine.get_num_arguments() == 2) << "Wrong number of arguments";
  ASSERT_TRUE(cmdLine.get_argument(0) == "A") << "Error parsing argument 0";
  ASSERT_TRUE(cmdLine.get_argument(1) == "B") << "Error parsing argument 1";
}

TEST(CommandLine, all_arguments) {
  au::CommandLine cmdLine;

  cmdLine.SetFlagString("s", "");
  cmdLine.SetFlagBoolean("b");
  cmdLine.SetFlagInt("i", 0);
  cmdLine.SetFlagUint64("ui", 0);
  cmdLine.SetFlagDouble("f", 0);

  cmdLine.Parse("command -f 12.3 -ui 56 -i 12 -s andreu -b");

  ASSERT_TRUE(cmdLine.get_num_arguments() == 1) << "Wrong number of arguments";
  ASSERT_TRUE(cmdLine.get_argument(0) == "command") << "Error parsing argument 0";


  ASSERT_TRUE(cmdLine.GetFlagBool("b") == true) << "Errro parsing bool flag";
  ASSERT_TRUE(cmdLine.GetFlagString("s") == "andreu") << "Errro parsing string flag";
  ASSERT_TRUE(cmdLine.GetFlagInt("i") == 12) << "Errro parsing int flag";
  ASSERT_TRUE(cmdLine.GetFlagDouble("f") == 12.3) << "Errro parsing double flag";
  ASSERT_TRUE(cmdLine.GetFlagUint64("ui") == 56) << "Errro parsing uint64 flag";
}

TEST(CommandLine, default_values) {
  au::CommandLine cmdLine;

  cmdLine.SetFlagString("s", "default");
  cmdLine.SetFlagBoolean("b");
  cmdLine.SetFlagInt("i", 1);
  cmdLine.SetFlagUint64("ui", 1);
  cmdLine.SetFlagDouble("f", 1);

  cmdLine.Parse("command");

  ASSERT_TRUE(cmdLine.get_num_arguments() == 1) << "Wrong number of arguments";
  ASSERT_TRUE(cmdLine.get_argument(0) == "command") << "Error parsing argument 0";

  ASSERT_TRUE(cmdLine.GetFlagBool("b") == false) << "Errro parsing bool flag";
  ASSERT_TRUE(cmdLine.GetFlagString("s") == "default") << "Errro parsing string flag";
  ASSERT_TRUE(cmdLine.GetFlagInt("i") == 1) << "Errro parsing int flag";
  ASSERT_TRUE(cmdLine.GetFlagDouble("f") == 1) << "Errro parsing double flag";
  ASSERT_TRUE(cmdLine.GetFlagUint64("ui") == 1) << "Errro parsing uint64 flag";
}


TEST(CommandLine, literals) {
  au::CommandLine cmdLine;

  cmdLine.Parse("command \"this is the second\"");

  ASSERT_TRUE(cmdLine.get_num_arguments() == 2) << "Wrong number of arguments";
  ASSERT_TRUE(cmdLine.get_argument(0) == "command") << "Error parsing argument 0";
  ASSERT_TRUE(cmdLine.get_argument(1) == "this is the second") << "Error parsing argument 1";
}

TEST(CommandLine, collisions) {
  au::CommandLine cmdLine1;

  cmdLine1.SetFlagString("prefix", "");
  cmdLine1.Parse("command -prefix p1 -prefix p2");
  EXPECT_EQ("p2", cmdLine1.GetFlagString("prefix")) << "Error parsing string flag default";

  au::CommandLine cmdLine2;
  cmdLine2.SetFlagString("prefix", "", au::CommandLine::kCollisionBegin);
  cmdLine2.Parse("command -prefix p1 -prefix p2");
  EXPECT_EQ("p2.p1", cmdLine2.GetFlagString("prefix")) << "Error parsing string flag with kCollisionBegin";

  au::CommandLine cmdLine3;
  cmdLine3.SetFlagString("prefix", "", au::CommandLine::kCollisionEnd);
  cmdLine3.Parse("command -prefix p1 -prefix p2");
  EXPECT_EQ("p1.p2", cmdLine3.GetFlagString("prefix")) << "Error parsing string flag with kCollisionEnd";

  au::CommandLine cmdLine4;
  cmdLine4.SetFlagString("prefix", "", au::CommandLine::kCollisionIgnore);
  cmdLine4.Parse("command -prefix p1 -prefix p2");
  EXPECT_EQ("p1", cmdLine4.GetFlagString("prefix")) << "Error parsing string flag with kCollisionIgnore";

  au::CommandLine cmdLine5;
  cmdLine5.SetFlagString("prefix", "", au::CommandLine::kCollisionOverwrite);
  cmdLine5.Parse("command -prefix p1 -prefix p2");
  EXPECT_EQ("p2", cmdLine5.GetFlagString("prefix")) << "Error parsing string flag with kCollisionOverwrite";

  au::CommandLine cmdLine6;
  // Not way to check the detection of the wrong parameter, but in logs
  // We check that the Parse() behaviour is the same as the default
  cmdLine6.SetFlagString("prefix", "", "unsupported");
  cmdLine6.Parse("command -prefix p1 -prefix p2");
  EXPECT_EQ("p2", cmdLine6.GetFlagString("prefix")) << "Error parsing string flag with unsupported parameter";
}


