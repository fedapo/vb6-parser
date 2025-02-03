//: vb6_parser_test_main.cpp

// vb6_parser
// Copyright (c) 2022 Federico Aponte
// This code is licensed under GNU Software License (see LICENSE.txt for details)

#include <gtest/gtest.h>

#include <iostream>

int main(int argc, char* argv[])
{
  testing::InitGoogleTest(&argc, argv);
/*
  --gtest_list_tests
  --gtest_filter=POSITIVE_PATTERNS[-NEGATIVE_PATTERNS]
  --gtest_also_run_disabled_tests
  --gtest_repeat=[COUNT]
  --gtest_shuffle
  --gtest_random_seed=[NUMBER]
  --gtest_color=(yes|no|auto)
  --gtest_brief=1
  --gtest_print_time=0
  --gtest_output=(json|xml)[:DIRECTORY_PATH/|:FILE_PATH]
  --gtest_death_test_style=(fast|threadsafe)
  --gtest_break_on_failure
  --gtest_throw_on_failure
  --gtest_catch_exceptions=0
*/
  //::test::GTEST_FLAG(list_tests) = true;
  //::testing::GTEST_FLAG(filter) = "*empty_lines*";

  return RUN_ALL_TESTS();
}