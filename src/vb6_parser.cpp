//: vb6_parser.cpp

// vb6_parser
// Copyright (c) 2022 Federico Aponte
// This code is licensed under GNU Software License (see LICENSE.txt for details)

#include "vb6_config.hpp"
#include "vb6_parser_def.hpp"

namespace vb6_grammar {

std::string getParserInfo();

/*
template bool parse_rule<iterator_type, context_type>(
    ????_type rule_
  , iterator_type& first, iterator_type const& last
  , context_type const& context, ????_type::attribute_type&);
*/
BOOST_SPIRIT_INSTANTIATE(empty_line_type,              iterator_type, context_type)
BOOST_SPIRIT_INSTANTIATE(lonely_comment_type,          iterator_type, context_type)
BOOST_SPIRIT_INSTANTIATE(quoted_string_type,           iterator_type, context_type)
BOOST_SPIRIT_INSTANTIATE(basic_identifier_type,        iterator_type, context_type)
BOOST_SPIRIT_INSTANTIATE(identifier_context_type,      iterator_type, context_type)
BOOST_SPIRIT_INSTANTIATE(decorated_variable_type,      iterator_type, context_type)
BOOST_SPIRIT_INSTANTIATE(simple_type_identifier_type,  iterator_type, context_type)
BOOST_SPIRIT_INSTANTIATE(complex_type_identifier_type, iterator_type, context_type)
BOOST_SPIRIT_INSTANTIATE(type_identifier_type,         iterator_type, context_type)
BOOST_SPIRIT_INSTANTIATE(single_var_declaration_type,  iterator_type, context_type)
BOOST_SPIRIT_INSTANTIATE(global_var_declaration_type,  iterator_type, context_type)
BOOST_SPIRIT_INSTANTIATE(record_declaration_type,      iterator_type, context_type)
BOOST_SPIRIT_INSTANTIATE(const_expression_type,        iterator_type, context_type)
BOOST_SPIRIT_INSTANTIATE(expression_type,              iterator_type, context_type)
BOOST_SPIRIT_INSTANTIATE(enum_declaration_type,        iterator_type, context_type)
BOOST_SPIRIT_INSTANTIATE(const_var_declaration_type,   iterator_type, context_type)
BOOST_SPIRIT_INSTANTIATE(param_decl_type,              iterator_type, context_type)

}