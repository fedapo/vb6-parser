// VB6 Parser
// Copyright (c) 2018-2026 Federico Aponte
// SPDX-License-Identifier:	GPL-3.0-only

#include "test_grammar_helper.hpp"
#include "vb6_parser.hpp"
#include "vb6_ast_printer.hpp"

#include <boost/optional/optional_io.hpp>

#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>

#include <iostream>
#include <vector>

namespace x3 = boost::spirit::x3;

TEST_CASE("expression_operators", "[vb6_parser_expressions]")
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
    INFO("source: " << source << ", stopped at: " << sv);
    REQUIRE(res);
    CHECK(sv.empty());
    REQUIRE(ast.get().type() == typeid(x3::forward_ast<vb6_ast::binary_expression>));
    auto const& binary =
      boost::get<x3::forward_ast<vb6_ast::binary_expression>>(ast.get()).get();
    REQUIRE(binary.rest.size() == 1);
    CHECK(binary.rest[0].op == expected_op);
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
    INFO("source: " << source << ", stopped at: " << sv);
    REQUIRE(res);
    CHECK(sv.empty());
    REQUIRE(ast.get().type() == typeid(x3::forward_ast<vb6_ast::unary_expression>));
    auto const& unary =
      boost::get<x3::forward_ast<vb6_ast::unary_expression>>(ast.get()).get();
    CHECK(unary.op == expected_op);
  }
}

TEST_CASE("expression_operator_precedence", "[vb6_parser_expressions]")
{
  vb6_ast::expression ast;
  auto [res, sv] = test_grammar("1 + 2 * 3", vb6_grammar::expression, ast);
  INFO("stopped at: " << sv);
  REQUIRE(res);
  CHECK(sv.empty());
  auto const& addition =
    boost::get<x3::forward_ast<vb6_ast::binary_expression>>(ast.get()).get();
  REQUIRE(addition.rest.size() == 1);
  CHECK(addition.rest[0].op == vb6_ast::operator_type::plus);
  auto const& multiplication =
    boost::get<x3::forward_ast<vb6_ast::binary_expression>>(
      addition.rest[0].operand.get()).get();
  REQUIRE(multiplication.rest.size() == 1);
  CHECK(multiplication.rest[0].op == vb6_ast::operator_type::mult);

  auto [parenthesized_res, parenthesized_sv] =
    test_grammar("(1 + 2) * 3", vb6_grammar::expression, ast);
  INFO("stopped at: " << parenthesized_sv);
  REQUIRE(parenthesized_res);
  CHECK(parenthesized_sv.empty());
  auto const& parenthesized_multiplication =
    boost::get<x3::forward_ast<vb6_ast::binary_expression>>(ast.get()).get();
  REQUIRE(parenthesized_multiplication.rest.size() == 1);
  CHECK(parenthesized_multiplication.rest[0].op == vb6_ast::operator_type::mult);
  auto const& parenthesized_addition =
    boost::get<x3::forward_ast<vb6_ast::binary_expression>>(
      parenthesized_multiplication.lhs.get()).get();
  CHECK(parenthesized_addition.rest[0].op == vb6_ast::operator_type::plus);

  auto [power_res, power_sv] = test_grammar("2 ^ 3 ^ 2", vb6_grammar::expression, ast);
  INFO("stopped at: " << power_sv);
  REQUIRE(power_res);
  CHECK(power_sv.empty());
  auto const& power =
    boost::get<x3::forward_ast<vb6_ast::binary_expression>>(ast.get()).get();
  REQUIRE(power.rest.size() == 1);
  CHECK(power.rest[0].op == vb6_ast::operator_type::exp);
  auto const& right_power =
    boost::get<x3::forward_ast<vb6_ast::binary_expression>>(
      power.rest[0].operand.get()).get();
  CHECK(right_power.rest[0].op == vb6_ast::operator_type::exp);

  auto [unary_res, unary_sv] = test_grammar("-2 ^ 2", vb6_grammar::expression, ast);
  INFO("stopped at: " << unary_sv);
  REQUIRE(unary_res);
  CHECK(unary_sv.empty());
  auto const& negation =
    boost::get<x3::forward_ast<vb6_ast::unary_expression>>(ast.get()).get();
  CHECK(negation.op == vb6_ast::operator_type::minus);
  auto const& negated_power =
    boost::get<x3::forward_ast<vb6_ast::binary_expression>>(negation.operand.get()).get();
  CHECK(negated_power.rest[0].op == vb6_ast::operator_type::exp);

  auto [not_res, not_sv] =
    test_grammar("Not False = True And 1 < 2", vb6_grammar::expression, ast);
  INFO("stopped at: " << not_sv);
  REQUIRE(not_res);
  CHECK(not_sv.empty());
  auto const& conjunction =
    boost::get<x3::forward_ast<vb6_ast::binary_expression>>(ast.get()).get();
  CHECK(conjunction.rest[0].op == vb6_ast::operator_type::and_);
  auto const& negated_comparison =
    boost::get<x3::forward_ast<vb6_ast::unary_expression>>(conjunction.lhs.get()).get();
  CHECK(negated_comparison.op == vb6_ast::operator_type::not_);
  auto const& comparison =
    boost::get<x3::forward_ast<vb6_ast::binary_expression>>(
      negated_comparison.operand.get()).get();
  CHECK(comparison.rest[0].op == vb6_ast::operator_type::equal);

  auto [modulo_res, modulo_sv] =
    test_grammar("8 \\ 4 Mod 3", vb6_grammar::expression, ast);
  INFO("stopped at: " << modulo_sv);
  REQUIRE(modulo_res);
  CHECK(modulo_sv.empty());
  auto const& modulo =
    boost::get<x3::forward_ast<vb6_ast::binary_expression>>(ast.get()).get();
  CHECK(modulo.rest[0].op == vb6_ast::operator_type::mod);
  auto const& integer_division =
    boost::get<x3::forward_ast<vb6_ast::binary_expression>>(modulo.lhs.get()).get();
  CHECK(integer_division.rest[0].op == vb6_ast::operator_type::div_int);

  auto [concat_res, concat_sv] =
    test_grammar("1 + 2 & 3", vb6_grammar::expression, ast);
  INFO("stopped at: " << concat_sv);
  REQUIRE(concat_res);
  CHECK(concat_sv.empty());
  auto const& concatenation =
    boost::get<x3::forward_ast<vb6_ast::binary_expression>>(ast.get()).get();
  CHECK(concatenation.rest[0].op == vb6_ast::operator_type::amp);
  auto const& concatenation_addition =
    boost::get<x3::forward_ast<vb6_ast::binary_expression>>(concatenation.lhs.get()).get();
  CHECK(concatenation_addition.rest[0].op == vb6_ast::operator_type::plus);

  auto [logical_res, logical_sv] =
    test_grammar("True Or False And True", vb6_grammar::expression, ast);
  INFO("stopped at: " << logical_sv);
  REQUIRE(logical_res);
  CHECK(logical_sv.empty());
  auto const& disjunction =
    boost::get<x3::forward_ast<vb6_ast::binary_expression>>(ast.get()).get();
  CHECK(disjunction.rest[0].op == vb6_ast::operator_type::or_);
  auto const& conjunction_rhs =
    boost::get<x3::forward_ast<vb6_ast::binary_expression>>(
      disjunction.rest[0].operand.get()).get();
  CHECK(conjunction_rhs.rest[0].op == vb6_ast::operator_type::and_);

  auto [levels_res, levels_sv] =
    test_grammar("True Xor False Eqv True Imp False", vb6_grammar::expression, ast);
  INFO("stopped at: " << levels_sv);
  REQUIRE(levels_res);
  CHECK(levels_sv.empty());
  auto const& implication =
    boost::get<x3::forward_ast<vb6_ast::binary_expression>>(ast.get()).get();
  CHECK(implication.rest[0].op == vb6_ast::operator_type::imp);
  auto const& equivalence =
    boost::get<x3::forward_ast<vb6_ast::binary_expression>>(implication.lhs.get()).get();
  CHECK(equivalence.rest[0].op == vb6_ast::operator_type::eqv);
  auto const& exclusive_or =
    boost::get<x3::forward_ast<vb6_ast::binary_expression>>(equivalence.lhs.get()).get();
  CHECK(exclusive_or.rest[0].op == vb6_ast::operator_type::xor_);
}
