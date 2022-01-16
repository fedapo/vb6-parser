//: vb6_parser_statements.cpp

// vb6_parser
// Copyright (c) 2022 Federico Aponte
// This code is licensed under GNU Software License (see LICENSE.txt for details)

#include "vb6_config.hpp"
#include "vb6_parser_statements_def.hpp"

namespace vb6_grammar {

/*
template bool parse_rule<iterator_type, context_type>(
    ????_type rule_
  , iterator_type& first, iterator_type const& last
  , context_type const& context, ????_type::attribute_type&);
*/
BOOST_SPIRIT_INSTANTIATE(statements::singleStmt_type,       iterator_type, context_type)
BOOST_SPIRIT_INSTANTIATE(statements::statement_block_type,  iterator_type, context_type)
BOOST_SPIRIT_INSTANTIATE(statements::assignmentStmt_type,   iterator_type, context_type)
BOOST_SPIRIT_INSTANTIATE(statements::localvardeclStmt_type, iterator_type, context_type)
BOOST_SPIRIT_INSTANTIATE(statements::redimStmt_type,        iterator_type, context_type)
BOOST_SPIRIT_INSTANTIATE(statements::exitStmt_type,         iterator_type, context_type)
BOOST_SPIRIT_INSTANTIATE(statements::gotoStmt_type,         iterator_type, context_type)
BOOST_SPIRIT_INSTANTIATE(statements::onerrorStmt_type,      iterator_type, context_type)
BOOST_SPIRIT_INSTANTIATE(statements::resumeStmt_type,       iterator_type, context_type)
BOOST_SPIRIT_INSTANTIATE(statements::labelStmt_type,        iterator_type, context_type)
BOOST_SPIRIT_INSTANTIATE(statements::callimplicitStmt_type, iterator_type, context_type)
BOOST_SPIRIT_INSTANTIATE(statements::callexplicitStmt_type, iterator_type, context_type)
BOOST_SPIRIT_INSTANTIATE(statements::raiseeventStmt_type,   iterator_type, context_type)

// compound statements
BOOST_SPIRIT_INSTANTIATE(statements::whileStmt_type,        iterator_type, context_type)
BOOST_SPIRIT_INSTANTIATE(statements::doStmt_type,           iterator_type, context_type)
BOOST_SPIRIT_INSTANTIATE(statements::dowhileStmt_type,      iterator_type, context_type)
BOOST_SPIRIT_INSTANTIATE(statements::loopwhileStmt_type,    iterator_type, context_type)
BOOST_SPIRIT_INSTANTIATE(statements::dountilStmt_type,      iterator_type, context_type)
BOOST_SPIRIT_INSTANTIATE(statements::loopuntilStmt_type,    iterator_type, context_type)
BOOST_SPIRIT_INSTANTIATE(statements::forStmt_type,          iterator_type, context_type)
BOOST_SPIRIT_INSTANTIATE(statements::foreachStmt_type,      iterator_type, context_type)
BOOST_SPIRIT_INSTANTIATE(statements::ifelseStmt_type,       iterator_type, context_type)
BOOST_SPIRIT_INSTANTIATE(statements::withStmt_type,         iterator_type, context_type)
BOOST_SPIRIT_INSTANTIATE(statements::selectStmt_type,       iterator_type, context_type)

BOOST_SPIRIT_INSTANTIATE(statements::ifBranch_type,    iterator_type, context_type)
BOOST_SPIRIT_INSTANTIATE(statements::elsifBranch_type, iterator_type, context_type)
BOOST_SPIRIT_INSTANTIATE(statements::elseBranch_type,  iterator_type, context_type)
BOOST_SPIRIT_INSTANTIATE(statements::case_block_type,  iterator_type, context_type)

}