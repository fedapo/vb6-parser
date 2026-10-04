// VB6 Parser
// Copyright (c) 2018-2026 Federico Aponte
// SPDX-License-Identifier:	GPL-3.0-only

#pragma once

#include "color_console.hpp"
#include "vb6_config.hpp"

#include <boost/spirit/home/x3.hpp>

#include <iostream>
#include <print>
#include <sstream>
#include <string_view>

template <class ruleType, class attrType>
void test_grammar(std::ostream& os, std::string_view testName, ruleType rule,
                  std::string_view fragment, attrType& attr)
{
  namespace x3 = boost::spirit::x3;

  std::println("\x1b[34;1m================================================================================");
  std::println("TEST:\x1b[0m {}", testName);

  auto it1 = cbegin(fragment);
  auto const it2 = cend(fragment);

  try
  {
    std::ostringstream out;

    vb6_grammar::error_handler_type error_handler(it1, it2, out, "source.bas");

    auto const parser =
      // we pass our error handler to the parser so we can access
      // it later on in our on_error and on_success handlers
      x3::with<vb6_grammar::vb6_error_handler_tag>(std::ref(error_handler))
      [
        rule
      ];

    // NOTE: see https://www.codevamping.com/2018/12/spirit-x3-file-organization/
    //
    // > Be sure to call phrase_parse without a namespace.
    // > Do not call it like x3::phrase_parse.
    // > Doing so turns off Argument Dependent Lookup. Evil ensues.
    //
    // It appears that using a namespace is not a problem here, but the article is interesting.
    bool res = x3::phrase_parse(it1, it2, parser, vb6_grammar::skip, attr);

    if(res)
    {
      if(it1 != it2)
      {
        error_handler(it1, it2, "Error - did not parse all input:");
        os << out.str();
        std::println("[\x1b[31;1mpartial fail\x1b[0m]");
      }
      else
      {
        std::println("[\x1b[32;1mok\x1b[0m]");
      }
    }
    else
    {
      error_handler(it1, it2, "Error parsing:");
      os << out.str();

      std::println("[\x1b[31;1mfail\x1b[0m]");
    }
    std::println("\x1b[34;1m--------------------------------\x1b[0m");
  }
  catch(x3::expectation_failure<decltype(it1)>& e)
  {
    os << tag_fail << " - " << e.what() << " - " << (e.where() - it1) << " - " << e.which() << '\n';
  }
}
