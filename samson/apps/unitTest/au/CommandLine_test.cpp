
#include "gtest/gtest.h"

#include "au/CommandLine.h"


TEST(CommandLine, simple) {
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

  cmdLine.set_flag_string("s", "");
  cmdLine.set_flag_boolean("b");
  cmdLine.set_flag_int("i", 0);
  cmdLine.set_flag_uint64("ui", 0);
  cmdLine.set_flag_double("f", 0);

  cmdLine.parse("command -f 12.3 -ui 56 -i 12 -s andreu -b");

  ASSERT_TRUE(cmdLine.get_num_arguments() == 1) << "Wrong number of arguments";
  ASSERT_TRUE(cmdLine.get_argument(0) == "command") << "Error parsing argument 0";


  ASSERT_TRUE(cmdLine.get_flag_bool("b") == true) << "Errro parsing bool flag";
  ASSERT_TRUE(cmdLine.get_flag_string("s") == "andreu") << "Errro parsing string flag";
  ASSERT_TRUE(cmdLine.get_flag_int("i") == 12) << "Errro parsing int flag";
  ASSERT_TRUE(cmdLine.get_flag_double("f") == 12.3) << "Errro parsing double flag";
  ASSERT_TRUE(cmdLine.get_flag_uint64("ui") == 56) << "Errro parsing uint64 flag";
}

TEST(CommandLine, default_values) {
  au::CommandLine cmdLine;

  cmdLine.set_flag_string("s", "default");
  cmdLine.set_flag_boolean("b");
  cmdLine.set_flag_int("i", 1);
  cmdLine.set_flag_uint64("ui", 1);
  cmdLine.set_flag_double("f", 1);

  cmdLine.parse("command");

  ASSERT_TRUE(cmdLine.get_num_arguments() == 1) << "Wrong number of arguments";
  ASSERT_TRUE(cmdLine.get_argument(0) == "command") << "Error parsing argument 0";

  ASSERT_TRUE(cmdLine.get_flag_bool("b") == false) << "Errro parsing bool flag";
  ASSERT_TRUE(cmdLine.get_flag_string("s") == "default") << "Errro parsing string flag";
  ASSERT_TRUE(cmdLine.get_flag_int("i") == 1) << "Errro parsing int flag";
  ASSERT_TRUE(cmdLine.get_flag_double("f") == 1) << "Errro parsing double flag";
  ASSERT_TRUE(cmdLine.get_flag_uint64("ui") == 1) << "Errro parsing uint64 flag";
}


TEST(CommandLine, literals) {
  au::CommandLine cmdLine;

  cmdLine.parse("command \"this is the second\"");

  ASSERT_TRUE(cmdLine.get_num_arguments() == 2) << "Wrong number of arguments";
  ASSERT_TRUE(cmdLine.get_argument(0) == "command") << "Error parsing argument 0";
  ASSERT_TRUE(cmdLine.get_argument(1) == "this is the second") << "Error parsing argument 1";
}


