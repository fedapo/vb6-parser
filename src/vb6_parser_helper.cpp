// VB6 Parser
// Copyright (c) 2018-2026 Federico Aponte
// SPDX-License-Identifier:	GPL-3.0-only

#include <boost/spirit/home/x3/version.hpp>

#include <format>

namespace vb6_grammar {

std::string getParserInfo()
{
  return std::format("SPIRIT_X3_VERSION: {:0x}\n"
                     "VB6_PARSER_VERSION: {}\n",
                     SPIRIT_X3_VERSION, VB6_PARSER_VERSION);
}

}
