//: vb6_parser_test_main.cpp

// vb6_parser
// Copyright (c) 2022 Federico Aponte
// This code is licensed under GNU Software License (see LICENSE.txt for details)

#include <gtest/gtest.h>

#include <iostream>

int main(int argc, char* argv[])
{
  testing::InitGoogleTest(&argc, argv);

  // --gtest_filter = Test_Cases1*
  // --gtest_repeat = 1000
  // --gtest_break_on_failure
  // --gtest_shuffle
  // --gtest_random_seed=SEED
  // --gtest_color=no
  // --gtest_print_time=0
  // --gtest_print_utf8=0
  // --gtest_output=xml:path_to_output_file
  // --gtest_output=json:path_to_output_file
  //::test::GTEST_FLAG(list_tests) = true;
  //::testing::GTEST_FLAG(filter) = "*empty_lines*";

  return RUN_ALL_TESTS();
}