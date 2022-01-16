//: vb6_ast_adapt.hpp

// vb6_parser
// Copyright (c) 2022 Federico Aponte
// This code is licensed under GNU Software License (see LICENSE.txt for details)

#pragma once

#include "vb6_ast.hpp"

#include <boost/fusion/include/adapt_struct.hpp>

// these must be in the global scope!

BOOST_FUSION_ADAPT_STRUCT(vb6_ast::integer_dec, val)
BOOST_FUSION_ADAPT_STRUCT(vb6_ast::integer_hex, val)
BOOST_FUSION_ADAPT_STRUCT(vb6_ast::integer_oct, val)
BOOST_FUSION_ADAPT_STRUCT(vb6_ast::long_dec, val)
BOOST_FUSION_ADAPT_STRUCT(vb6_ast::long_hex, val)
BOOST_FUSION_ADAPT_STRUCT(vb6_ast::long_oct, val)

// these are needed when BOOST_SPIRIT_X3_DEBUG is defined
#ifdef BOOST_SPIRIT_X3_DEBUG
BOOST_FUSION_ADAPT_STRUCT(vb6_ast::nothing)
BOOST_FUSION_ADAPT_STRUCT(vb6_ast::empty_line)
#endif

BOOST_FUSION_ADAPT_STRUCT(vb6_ast::lonely_comment,
  content
)

BOOST_FUSION_ADAPT_STRUCT(vb6_ast::identifier_context,
    leading_dot,
    elements
)

BOOST_FUSION_ADAPT_STRUCT(vb6_ast::variable,
    name,
    construct,
    //library_or_module,
    type // not sure...
)

BOOST_FUSION_ADAPT_STRUCT(vb6_ast::decorated_variable,
    ctx,
    var
)

BOOST_FUSION_ADAPT_STRUCT(vb6_ast::func_call,
    func_name,
    params
)

BOOST_FUSION_ADAPT_STRUCT(vb6_ast::global_var_decls,
    at,
    with_events,
    vars
)

BOOST_FUSION_ADAPT_STRUCT(vb6_ast::const_var,
    var,
    value
)

BOOST_FUSION_ADAPT_STRUCT(vb6_ast::record,
    at,
    name,
    members
)

BOOST_FUSION_ADAPT_STRUCT(vb6_ast::vb_enum,
    at,
    name,
    values
)

BOOST_FUSION_ADAPT_STRUCT(vb6_ast::func_param,
    isoptional,
    qualifier, // byval, byref
    var,
    defvalue
)

BOOST_FUSION_ADAPT_STRUCT(vb6_ast::statements::assignStmt,
    type,
    var,
    rhs
)

BOOST_FUSION_ADAPT_STRUCT(vb6_ast::statements::localVarDeclStmt,
    type,
    vars
)

BOOST_FUSION_ADAPT_STRUCT(vb6_ast::statements::redimStmt,
    preserve,
    var,
    newsize
)

BOOST_FUSION_ADAPT_STRUCT(vb6_ast::statements::exitStmt,
    type
)

BOOST_FUSION_ADAPT_STRUCT(vb6_ast::statements::gotoStmt,
    type,
    label
)

BOOST_FUSION_ADAPT_STRUCT(vb6_ast::statements::labelStmt,
    label
)

BOOST_FUSION_ADAPT_STRUCT(vb6_ast::statements::whileStmt,
    condition,
    block
)

BOOST_FUSION_ADAPT_STRUCT(vb6_ast::statements::doStmt,
    block
)

BOOST_FUSION_ADAPT_STRUCT(vb6_ast::statements::dowhileStmt,
    condition,
    block
)

BOOST_FUSION_ADAPT_STRUCT(vb6_ast::statements::loopwhileStmt,
    block,
    condition
)

BOOST_FUSION_ADAPT_STRUCT(vb6_ast::statements::dountilStmt,
    condition,
    block
)

BOOST_FUSION_ADAPT_STRUCT(vb6_ast::statements::loopuntilStmt,
    block,
    condition
)

BOOST_FUSION_ADAPT_STRUCT(vb6_ast::statements::forStmt,
    for_variable,
    from,
    to,
    step,
    block
)

BOOST_FUSION_ADAPT_STRUCT(vb6_ast::statements::foreachStmt,
    for_variable,
    container,
    block
)

BOOST_FUSION_ADAPT_STRUCT(vb6_ast::statements::if_branch,
    condition,
    block
)

#ifdef SIMPLE_IF_STATEMENT
BOOST_FUSION_ADAPT_STRUCT(vb6_ast::statements::ifelseStmt,
    first_branch,
    else_branch
)
#else
BOOST_FUSION_ADAPT_STRUCT(vb6_ast::statements::ifelseStmt,
    first_branch,
    if_branches,
    else_branch
)
#endif

BOOST_FUSION_ADAPT_STRUCT(vb6_ast::statements::withStmt,
    with_variable,
    block
)

BOOST_FUSION_ADAPT_STRUCT(vb6_ast::statements::case_block,
    case_expr,
    block
)

BOOST_FUSION_ADAPT_STRUCT(vb6_ast::statements::selectStmt,
    condition,
    blocks
)

BOOST_FUSION_ADAPT_STRUCT(vb6_ast::statements::onerrorStmt,
    label,
    type
)

BOOST_FUSION_ADAPT_STRUCT(vb6_ast::statements::resumeStmt,
    label_or_line_nr,
    type
)

BOOST_FUSION_ADAPT_STRUCT(vb6_ast::statements::callStmt,
    sub_name,
    params,
    explicit_call
)

BOOST_FUSION_ADAPT_STRUCT(vb6_ast::statements::raiseeventStmt,
    event_name,
    params
)

BOOST_FUSION_ADAPT_STRUCT(vb6_ast::subHead,
    at,
    name,
    params
)

BOOST_FUSION_ADAPT_STRUCT(vb6_ast::eventHead,
    at,
    name,
    params
)

BOOST_FUSION_ADAPT_STRUCT(vb6_ast::functionHead,
    at,
    name,
    params,
    return_type // not sure...
)

BOOST_FUSION_ADAPT_STRUCT(vb6_ast::propertyLetHead,
    at,
    name,
    params
)

BOOST_FUSION_ADAPT_STRUCT(vb6_ast::propertySetHead,
    at,
    name,
    params
)

BOOST_FUSION_ADAPT_STRUCT(vb6_ast::propertyGetHead,
    at,
    name,
    params,
    return_type // not sure...
)

BOOST_FUSION_ADAPT_STRUCT(vb6_ast::subDef,
    header,
    statements
)

BOOST_FUSION_ADAPT_STRUCT(vb6_ast::functionDef,
    header,
    statements
)

BOOST_FUSION_ADAPT_STRUCT(vb6_ast::externalSub,
    at,
    name,
    lib,
    alias,
    params
)

BOOST_FUSION_ADAPT_STRUCT(vb6_ast::externalFunction,
    at,
    name,
    lib,
    alias,
    params,
    return_type // not sure...
)

BOOST_FUSION_ADAPT_STRUCT(vb6_ast::STRICT_MODULE_STRUCTURE::option_block,
    items
)

BOOST_FUSION_ADAPT_STRUCT(vb6_ast::STRICT_MODULE_STRUCTURE::vb_module,
    attrs,
    opts,
    declarations,
    functions
)