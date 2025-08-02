// VB6 Parser
// Copyright (c) 2018-2025 Federico Aponte
// SPDX-License-Identifier:	GPL-3.0-only

#include "color_console.hpp"
#include "vb6_parser.hpp" // only for vb6_grammar::getParserInfo()

#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <string_view>

void vb6_test1(std::ostream&);
void vb6_test2(std::ostream&);
void vb6_test_statements(std::ostream&);

void test_vb6_unit(std::ostream&, std::string_view unit);
void test_gosub(std::ostream&);

using namespace std;

void test_vbasic(ostream& os, string const& fname)
{
  ifstream is(fname, ios::binary);

  if(!is)
  {
    cerr << "Could not open input file: " << fname << '\n';
    return;
  }

  // no whitespace skipping on the stream
  noskipws(is); //is.unsetf(ios::skipws);

  string unit;
  copy(istream_iterator<char>(is), istream_iterator<char>(),
       back_inserter(unit));

  test_vb6_unit(os, unit);
}

void test_vbasic(ostream& os)
{
  string unit = R"vb(Option Explicit
Dim str As String

Sub foo(a As Integer, b As Long)
)vb";

  test_vb6_unit(os, unit);
}

int main()
{
  cout << vb6_grammar::getParserInfo() << '\n';

  vb6_test1(cout);
  vb6_test_statements(cout);
  vb6_test2(cout);

  test_vbasic(cout, "data/test.bas");
  test_vbasic(cout, "data/long_source.bas");
  test_vbasic(cout);

  //test_gosub(cout);
}
