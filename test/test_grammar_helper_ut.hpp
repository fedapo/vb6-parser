//: test_grammar_ut.hpp

// vb6_parser
// Copyright (c) 2022 Federico Aponte
// This code is licensed under GNU Software License (see LICENSE.txt for details)

#pragma once

#include "color_console.hpp"
#include "vb6_config.hpp"

#include <boost/spirit/home/x3.hpp>

#include <iostream>
#include <string_view>
#include <utility>

template <class ruleType, class attrType>
std::pair<bool, std::string_view> test_grammar(std::string_view fragment, ruleType rule, attrType& attr)
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

  bool res = x3::phrase_parse(it1, it2, parser, vb6_grammar::skip, attr);

  return std::make_pair(res, std::string_view(it1, it2));
}
