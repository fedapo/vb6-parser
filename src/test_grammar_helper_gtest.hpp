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
void test_grammar(std::string_view fragment, ruleType rule, attrType& attr, bool expected = true)
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

  ASSERT_TRUE(x3::phrase_parse(it1, it2, parser, vb6_grammar::skip, attr) == expected)
    << "stopped at: " << std::string(it1, it2);

  EXPECT_EQ(it1, it2);
}
