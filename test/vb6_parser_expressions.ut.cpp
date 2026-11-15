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

ut::suite<"vb6_parser_expressions"> ___ = []
{
  ut::test("sample_expression") = []
  {
    vb6_ast::expression ast;
    auto [res, sv] = test_grammar(
      "foo1(foo2(3 + foo3(4), M.x_coord), True)", vb6_grammar::expression, ast);
    ut::expect(fatal(res)) << "stopped at: " << sv;
    ut::expect(sv.empty());
    ut::expect(boost::get<x3::forward_ast<vb6_ast::func_call>>(&ast));
    ut::expect(boost::get<x3::forward_ast<vb6_ast::func_call>>(ast).get().func_name == "foo1");
  };

  ut::test("expression_operators") = []
  {
    std::vector<std::pair<std::string, vb6_ast::operator_type>> const binary_cases{
      { "2 ^ 3", vb6_ast::operator_type::exp },
      { "2 * 3", vb6_ast::operator_type::mult },
      { "6 / 3", vb6_ast::operator_type::div },
      { "6 \\ 3", vb6_ast::operator_type::div_int },
      { "6 Mod 3", vb6_ast::operator_type::mod },
      { "1 + 2", vb6_ast::operator_type::plus },
      { "3 - 2", vb6_ast::operator_type::minus },
      { "\"a\" & \"b\"", vb6_ast::operator_type::amp },
      { "1 < 2", vb6_ast::operator_type::less },
      { "1 > 2", vb6_ast::operator_type::greater },
      { "1 <= 2", vb6_ast::operator_type::less_equal },
      { "1 >= 2", vb6_ast::operator_type::greater_equal },
      { "1 = 2", vb6_ast::operator_type::equal },
      { "1 <> 2", vb6_ast::operator_type::not_equal },
      { "\"a\" Like \"*\"", vb6_ast::operator_type::like },
      { "Nothing Is Nothing", vb6_ast::operator_type::is },
      { "True And False", vb6_ast::operator_type::and_ },
      { "True Or False", vb6_ast::operator_type::or_ },
      { "True Xor False", vb6_ast::operator_type::xor_ },
      { "True Eqv False", vb6_ast::operator_type::eqv },
      { "True Imp False", vb6_ast::operator_type::imp },
    };
    for(auto const& [source, expected_op] : binary_cases)
    {
      vb6_ast::expression ast;
      auto [res, sv] = test_grammar(source, vb6_grammar::expression, ast);
      ut::expect(fatal(res)) << "stopped at: " << sv << " while parsing " << source;
      ut::expect(sv.empty()) << "unparsed input: " << source;
      ut::expect(fatal(boost::get<x3::forward_ast<vb6_ast::binary_expression>>(&ast)));
      auto const& binary =
        boost::get<x3::forward_ast<vb6_ast::binary_expression>>(ast).get();
      ut::expect(fatal(binary.rest.size() == 1));
      ut::expect(binary.rest[0].op == expected_op);
    }

    std::vector<std::pair<std::string, vb6_ast::operator_type>> const unary_cases{
      { "+1", vb6_ast::operator_type::plus },
      { "-1", vb6_ast::operator_type::minus },
      { "Not False", vb6_ast::operator_type::not_ },
    };
    for(auto const& [source, expected_op] : unary_cases)
    {
      vb6_ast::expression ast;
      auto [res, sv] = test_grammar(source, vb6_grammar::expression, ast);
      ut::expect(fatal(res)) << "stopped at: " << sv << " while parsing " << source;
      ut::expect(sv.empty()) << "unparsed input: " << source;
      ut::expect(fatal(boost::get<x3::forward_ast<vb6_ast::unary_expression>>(&ast)));
      auto const& unary =
        boost::get<x3::forward_ast<vb6_ast::unary_expression>>(ast).get();
      ut::expect(unary.op == expected_op);
    }
  };

  ut::test("expression_operator_precedence") = []
  {
  };
};
