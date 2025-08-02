// VB6 Parser
// Copyright (c) 2018-2025 Federico Aponte
// SPDX-License-Identifier:	GPL-3.0-only

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
