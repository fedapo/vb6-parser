//: vb6_parser_helper.cpp

// vb6_parser
// Copyright (c) 2022 Federico Aponte
// This code is licensed under GNU Software License (see LICENSE.txt for details)

#include <boost/spirit/home/x3/version.hpp>

#include <sstream>

namespace vb6_grammar {

std::string getParserInfo()
{
  using namespace std;
  ostringstream os;
  os << "SPIRIT_X3_VERSION: " << showbase << hex << SPIRIT_X3_VERSION
     << noshowbase << dec << '\n'
     << "VB6_PARSER_VERSION: 0.1" << '\n';
  return os.str();
}

}