// VB6 Parser
// Copyright (c) 2018-2025 Federico Aponte
// SPDX-License-Identifier:	GPL-3.0-only

#include "vb6_config.hpp"
#include "vb6_parser_def.hpp"

namespace vb6_grammar {

/*
template bool parse_rule<iterator_type, context_type>(
    ????_type rule_
  , iterator_type& first, iterator_type const& last
  , context_type const& context, ????_type::attribute_type&);
*/
BOOST_SPIRIT_INSTANTIATE(external_sub_decl_type,       iterator_type, context_type)
BOOST_SPIRIT_INSTANTIATE(external_function_decl_type,  iterator_type, context_type)
BOOST_SPIRIT_INSTANTIATE(subHead_type,                 iterator_type, context_type)
BOOST_SPIRIT_INSTANTIATE(eventHead_type,               iterator_type, context_type)
BOOST_SPIRIT_INSTANTIATE(functionHead_type,            iterator_type, context_type)
BOOST_SPIRIT_INSTANTIATE(property_letHead_type,        iterator_type, context_type)
BOOST_SPIRIT_INSTANTIATE(property_setHead_type,        iterator_type, context_type)
BOOST_SPIRIT_INSTANTIATE(property_getHead_type,        iterator_type, context_type)
BOOST_SPIRIT_INSTANTIATE(functionCall_type,            iterator_type, context_type)
BOOST_SPIRIT_INSTANTIATE(declaration_type,             iterator_type, context_type)
BOOST_SPIRIT_INSTANTIATE(attributeDef_type,            iterator_type, context_type)
BOOST_SPIRIT_INSTANTIATE(option_item_type,             iterator_type, context_type)
BOOST_SPIRIT_INSTANTIATE(subDef_type,                  iterator_type, context_type)
BOOST_SPIRIT_INSTANTIATE(functionDef_type,             iterator_type, context_type)
//BOOST_SPIRIT_INSTANTIATE(propertyDef_type,             iterator_type, context_type)
BOOST_SPIRIT_INSTANTIATE(basModDef_type,               iterator_type, context_type)

}
