//: vb6_config.hpp

// vb6_parser
// Copyright (c) 2022 Federico Aponte
// This code is licensed under GNU Software License (see LICENSE.txt for details)

#pragma once

#include "vb6_error_handler.hpp"
#include "vb6_parser.hpp" // only for having vb6_grammar::skip_type

#include <boost/spirit/home/x3.hpp>

#include <string>

namespace vb6_grammar {

namespace x3 = boost::spirit::x3;

using iterator_type = std::string_view::const_iterator;
//using iterator_type = std::string::const_iterator;

using phrase_context_type = x3::phrase_parse_context<skip_type>::type;

using error_handler_type = vb6_error_handler<iterator_type>;
//using error_handler_type = x3::unused_type;

using context_type = x3::context<vb6_error_handler_tag
                               , std::reference_wrapper<error_handler_type>
                               , phrase_context_type>;
//using context_type = x3::context<x3::skipper_tag
//                               , x3::char_class<boost::spirit::char_encoding::ascii, x3::blank_tag> const
//                               , x3::unused_type>;

}