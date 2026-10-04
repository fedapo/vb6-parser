// VB6 Parser
// Copyright (c) 2018-2026 Federico Aponte
// SPDX-License-Identifier:	GPL-3.0-only

#include "color_console.hpp"
#include "vb6_parser.hpp" // only for vb6_grammar::getParserInfo()

#include <fstream>
#include <iostream>
#include <iterator>
#include <print>
#include <string_view>

void vb6_test1(std::ostream&);
void vb6_test2(std::ostream&);
void vb6_test_statements(std::ostream&);

void test_vb6_unit(std::ostream&, std::string_view unit);
void test_gosub(std::ostream&);

void test_vbasic(std::ostream& os, std::string const& fname)
{
  std::ifstream is(fname, std::ios::binary);

  if(!is)
  {
    std::println(std::cerr, "Could not open input file: {}", fname);
    return;
  }

  // no whitespace skipping on the stream
  std::noskipws(is);

  std::string unit;
  std::copy(std::istream_iterator<char>(is), std::istream_iterator<char>(),
            std::back_inserter(unit));

  test_vb6_unit(os, unit);
}

void test_vbasic(std::ostream& os)
{
  std::string_view unit = R"vb(Option Explicit
Dim str As String

Sub foo(a As Integer, b As Long)
)vb";

  test_vb6_unit(os, unit);
}

int main()
{
  std::println("{}", vb6_grammar::getParserInfo());

  vb6_test1(std::cout);
  vb6_test_statements(std::cout);
  vb6_test2(std::cout);

  test_vbasic(std::cout, "data/test.bas");
  test_vbasic(std::cout, "data/long_source.bas");
  test_vbasic(std::cout);

  //test_gosub(std::cout);
}
