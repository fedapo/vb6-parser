// VB6 Parser
// Copyright (c) 2018-2026 Federico Aponte
// SPDX-License-Identifier:	GPL-3.0-only

#include "test_grammar_helper.hpp"
#include "vb6_parser.hpp"
#include "vb6_ast_printer.hpp"

#include <boost/optional/optional_io.hpp>
#include <boost/ut.hpp>

#include <iostream>
#include <vector>

namespace x3 = boost::spirit::x3;
namespace ut = boost::ut;

// need for ut's user-defined literals and overloaded operators
using namespace boost::ut;

ut::suite<"vb6_parser_simple"> ___ = []
{
  ut::test("sample_expression") = []
  {
    vb6_ast::expression ast;
    auto [res, sv] = test_grammar(
      "foo1(foo2(3 + foo3(4), M.x_coord), True)", vb6_grammar::expression, ast);
    ut::expect(fatal(res)) << "stopped at: " << sv;
    ut::expect(sv.empty());
    ut::expect(ast.get().type() == typeid(x3::forward_ast<vb6_ast::func_call>));
    ut::expect(boost::get<x3::forward_ast<vb6_ast::func_call>>(ast.get()).get().func_name == "foo1");
  };
};
