//: test_grammar_helper.hpp

// vb6_parser
// Copyright (c) 2022 Federico Aponte
// This code is licensed under GNU Software License (see LICENSE.txt for details)

#pragma once

#include "color_console.hpp"
#include "vb6_config.hpp"

#include <gtest/gtest.h>

#include <boost/spirit/home/x3.hpp>

#include <iostream>
#include <string>
#include <string_view>

template <class ruleType, class attrType>
void test_grammar(std::ostream& os, std::string_view testName, ruleType rule,
                  std::string_view fragment, attrType& attr)
{
  namespace x3 = boost::spirit::x3;

  os << "--------\n" << testName << "\n--------\n";

  auto it1 = cbegin(fragment);
  auto const it2 = cend(fragment);

  try
  {
    std::stringstream out;

    vb6_grammar::error_handler_type error_handler(it1, it2, out, "source.bas");

    auto const parser =
      // we pass our error handler to the parser so we can access
      // it later on in our on_error and on_success handlers
      x3::with<vb6_grammar::vb6_error_handler_tag>(std::ref(error_handler))
      [
        rule
      ];

    // NOTE:
    // see https://www.codevamping.com/2018/12/spirit-x3-file-organization/
    // "Be sure to call phrase_parse without a namespace.
    // Do not call it like x3::phrase_parse.
    // Doing so turns off Argument Dependent Lookup. Evil ensues."
    // It appears that using a namespace is not a problem here,
    // but the article is interesting.
    bool res = x3::phrase_parse(it1, it2, parser, vb6_grammar::skip, attr);

    if(res)
    {
      os << tag_ok << '\n';
      if(it1 != it2)
        os << "  but we stopped at " << std::string(it1, it2) << '\n';
    }
    else
      os << tag_fail << " - stopped at: " << std::string(it1, it2) << '\n';
  }
  catch(x3::expectation_failure<decltype(it1)>& e)
  {
    os << tag_fail << " - " << e.what() << " - " << (e.where() - it1) << " - " << e.which() << '\n';
  }
}

template <class ruleType, class attrType>
void test_grammar(std::string_view fragment, ruleType rule, attrType& attr)
{
  namespace x3 = boost::spirit::x3;

  auto it1 = cbegin(fragment);
  auto const it2 = cend(fragment);

  std::stringstream out;

  vb6_grammar::error_handler_type error_handler(it1, it2, out, "source.bas");

  auto const parser =
    // we pass our error handler to the parser so we can access
    // it later on in our on_error and on_success handlers
    x3::with<vb6_grammar::vb6_error_handler_tag>(std::ref(error_handler))
    [
      rule
    ];

  ASSERT_TRUE(x3::phrase_parse(it1, it2, parser, vb6_grammar::skip, attr))
    << "stopped at: " << std::string(it1, it2);

  EXPECT_EQ(it1, it2);
}

template <class ruleType, class attrType>
void test_grammar_false(std::string_view fragment, ruleType rule, attrType& attr)
{
  namespace x3 = boost::spirit::x3;

  auto it1 = cbegin(fragment);
  auto const it2 = cend(fragment);

  std::stringstream out;

  vb6_grammar::error_handler_type error_handler(it1, it2, out, "source.bas");

  auto const parser =
    // we pass our error handler to the parser so we can access
    // it later on in our on_error and on_success handlers
    x3::with<vb6_grammar::vb6_error_handler_tag>(std::ref(error_handler))
    [
      rule
    ];

  ASSERT_FALSE(x3::phrase_parse(it1, it2, parser, vb6_grammar::skip, attr));
}
