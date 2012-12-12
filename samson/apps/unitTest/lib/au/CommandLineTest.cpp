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


  EXPECT_EQ(2, cmdLine.get_num_arguments()) << "Wrong number of arguments";
  EXPECT_EQ("A", cmdLine.get_argument(0)) << "Error parsing argument 0";
  EXPECT_EQ("B", cmdLine.get_argument(1)) << "Error parsing argument 1";
}

TEST(CommandLine, simple2) {
  au::CommandLine cmdLine("A B");

  EXPECT_EQ(2, cmdLine.get_num_arguments()) << "Wrong number of arguments";
  EXPECT_EQ("A", cmdLine.get_argument(0)) << "Error parsing argument 0";
  EXPECT_EQ("B", cmdLine.get_argument(1)) << "Error parsing argument 1";
  EXPECT_EQ(au::CommandLine::kNoArgument, cmdLine.get_argument(2)) << "Error parsing argument 2";
}

TEST(CommandLine, all_arguments) {
  au::CommandLine cmdLine;

  cmdLine.SetFlagString("s", "");
  cmdLine.SetFlagBoolean("b");
  cmdLine.SetFlagInt("i", 0);
  cmdLine.SetFlagUint64("ui", 0);
  cmdLine.SetFlagDouble("f", 0);

  cmdLine.Parse("command -f 12.3 -ui 56 -i 12 -s andreu -b");

  EXPECT_EQ(1, cmdLine.get_num_arguments()) << "Wrong number of arguments";
  EXPECT_EQ("command", cmdLine.get_argument(0)) << "Error parsing argument 0";


  EXPECT_TRUE(cmdLine.GetFlagBool("b")) << "Error parsing bool flag";
  EXPECT_EQ("andreu", cmdLine.GetFlagString("s")) << "Error parsing string flag";
  EXPECT_EQ(12, cmdLine.GetFlagInt("i")) << "Error parsing int flag";
  EXPECT_EQ(12.3, cmdLine.GetFlagDouble("f")) << "Error parsing double flag";
  EXPECT_EQ(56, cmdLine.GetFlagUint64("ui")) << "Error parsing uint64 flag";
  EXPECT_EQ(au::CommandLine::kUnknownFlag, cmdLine.GetFlagString("wrong")) << "Error parsing wrong flag";
  EXPECT_EQ(au::CommandLine::kWrongType, cmdLine.GetFlagString("ui")) << "Error parsing wrong type for uint64 flag";
}

TEST(CommandLine, default_values) {
  au::CommandLine cmdLine;

  cmdLine.SetFlagString("s", "default");
  cmdLine.SetFlagBoolean("b");
  cmdLine.SetFlagInt("i", 1);
  cmdLine.SetFlagUint64("ui", 1);
  cmdLine.SetFlagDouble("f", 1);

  cmdLine.Parse("command");

  EXPECT_EQ(1, cmdLine.get_num_arguments() == 1) << "Wrong number of arguments";
  EXPECT_EQ("command", cmdLine.get_argument(0)) << "Error parsing argument 0";

  EXPECT_FALSE(cmdLine.GetFlagBool("b")) << "Error parsing bool flag";
  EXPECT_EQ("default", cmdLine.GetFlagString("s")) << "Error parsing string flag";
  EXPECT_EQ(1, cmdLine.GetFlagInt("i")) << "Error parsing int flag";
  EXPECT_EQ(1.0, cmdLine.GetFlagDouble("f")) << "Error parsing double flag";
  EXPECT_EQ(1, cmdLine.GetFlagUint64("ui")) << "Error parsing uint64 flag";
}

TEST(CommandLine, literals) {
  au::CommandLine cmdLine;

  cmdLine.Parse("command \"this is the second\"");

  EXPECT_EQ(2, cmdLine.get_num_arguments()) << "Wrong number of arguments";
  EXPECT_EQ("command", cmdLine.get_argument(0)) << "Error parsing argument 0";
  EXPECT_EQ("this is the second", cmdLine.get_argument(1)) << "Error parsing argument 1";
}

TEST(CommandLine, reset) {
  au::CommandLine cmdLine;

  cmdLine.SetFlagString("s", "default");
  cmdLine.SetFlagBoolean("b");
  cmdLine.SetFlagInt("i", 1);
  cmdLine.SetFlagUint64("ui", 1);
  cmdLine.SetFlagDouble("f", 1);

  EXPECT_EQ("", cmdLine.command()) << "Error in initial command";

  cmdLine.Parse("command -s 'multiple word string' -ui 64M -u unknown");
  EXPECT_EQ("command -s 'multiple word string' -ui 64M -u unknown", cmdLine.command()) << "Error parsing command";
  EXPECT_EQ("multiple word string", cmdLine.GetFlagString("s")) << "Error parsing string flag";
  EXPECT_EQ(64*1024*1024, cmdLine.GetFlagUint64("ui")) << "Error parsing uint64 flag";

  cmdLine.ResetFlags();
  EXPECT_EQ("command -s 'multiple word string' -ui 64M -u unknown", cmdLine.command()) <<
      "Error after reset in command";
  EXPECT_EQ(au::CommandLine::kUnknownFlag, cmdLine.GetFlagString("s")) << "Error after reset string flag";
  EXPECT_EQ(0, cmdLine.GetFlagUint64("ui")) << "Error resetting uint64 flag";
}

TEST(CommandLine, collisions) {
  au::CommandLine cmdLine;

  cmdLine.SetFlagString("prefix", "");
  cmdLine.Parse("command -prefix p1 -prefix p2");
  EXPECT_EQ("p2", cmdLine.GetFlagString("prefix")) << "Error parsing string flag default";

  cmdLine.ResetFlags();
  cmdLine.SetFlagString("prefix", "", au::CommandLine::kCollisionInsertAtBegin);
  cmdLine.Parse("command -prefix p1 -prefix p2");
  EXPECT_EQ("p2.p1", cmdLine.GetFlagString("prefix")) << "Error parsing string flag with kCollisionInsertAtBegin";

  cmdLine.ResetFlags();
  cmdLine.SetFlagString("prefix", "", au::CommandLine::kCollisionConcatenateAtEnd);
  cmdLine.Parse("command -prefix p1 -prefix p2");
  EXPECT_EQ("p1.p2", cmdLine.GetFlagString("prefix")) << "Error parsing string flag with kCollisionConcatenateAtEnd";

  cmdLine.ResetFlags();
  cmdLine.SetFlagString("prefix", "", au::CommandLine::kCollisionIgnore);
  cmdLine.Parse("command -prefix p1 -prefix p2");
  EXPECT_EQ("p1", cmdLine.GetFlagString("prefix")) << "Error parsing string flag with kCollisionIgnore";

  cmdLine.ResetFlags();
  cmdLine.SetFlagString("prefix", "", au::CommandLine::kCollisionOverwrite);
  cmdLine.Parse("command -prefix p1 -prefix p2");
  EXPECT_EQ("p2", cmdLine.GetFlagString("prefix")) << "Error parsing string flag with kCollisionOverwrite";

  cmdLine.ResetFlags();
  cmdLine.SetFlagString("prefix", "", au::CommandLine::kCollisionSubtractFromPrevious);
  // Error collision parameter. No detection but in logs
  // Default behaviour (overwrite) expected
  cmdLine.Parse("command -prefix p1 -prefix p2");
  EXPECT_EQ("p2", cmdLine.GetFlagString("prefix")) << "Error parsing string flag with kCollisionSubtractFromPrevious";

  cmdLine.ResetFlags();
  cmdLine.SetFlagInt("i", 1, au::CommandLine::kCollisionAddToPrevious);
  cmdLine.SetFlagDouble("f", 1.0, au::CommandLine::kCollisionAddToPrevious);
  cmdLine.SetFlagUint64("ui", 1.0, au::CommandLine::kCollisionAddToPrevious);
  cmdLine.Parse("command -i 2 -i 3 -f 4.0 -f 5 -ui 1K -ui 1G");
  EXPECT_EQ(5, cmdLine.GetFlagInt("i")) << "Error parsing int flag with kCollisionAddToPrevious";
  EXPECT_EQ(9.0, cmdLine.GetFlagDouble("f")) << "Error parsing double flag with kCollisionAddToPrevious";
  EXPECT_EQ(1*1024 + 1*1024*1024*1024, cmdLine.GetFlagUint64("ui")) <<
      "Error parsing uint64 flag with kCollisionAddToPrevious";

  cmdLine.ResetFlags();
  cmdLine.SetFlagInt("i", 1, au::CommandLine::kCollisionSubtractFromPrevious);
  cmdLine.SetFlagDouble("f", 1.0, au::CommandLine::kCollisionSubtractFromPrevious);
  cmdLine.SetFlagUint64("ui", 1.0, au::CommandLine::kCollisionSubtractFromPrevious);
  cmdLine.Parse("command -i 2 -i 3 -f 4.0 -f 5 -ui 1K -ui 1G");
  EXPECT_EQ(-1, cmdLine.GetFlagInt("i")) << "Error parsing int flag with kCollisionSubtractFromPrevious";
  EXPECT_EQ(-1.0, cmdLine.GetFlagDouble("f")) << "Error parsing double flag with kCollisionSubtractFromPrevious";
  EXPECT_EQ(0, cmdLine.GetFlagUint64("ui")) << "Error parsing uint64 flag with kCollisionSubtractFromPrevious";
  cmdLine.Parse("command -ui 1G -ui 1M");
  EXPECT_EQ(1*1024*1024*1024 - 1*1024*1024, cmdLine.GetFlagUint64("ui")) <<
      "Error parsing uint64 flag with kCollisionSubtractFromPrevious";
}


