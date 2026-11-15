// VB6 Parser
// Copyright (c) 2018-2026 Federico Aponte
// SPDX-License-Identifier:	GPL-3.0-only

#include "test_grammar_helper.hpp"
#include "vb6_parser.hpp"
#include "vb6_ast_printer.hpp"

#include <boost/optional/optional_io.hpp>
#include <gtest/gtest.h>

#include <iostream>
#include <vector>

namespace x3 = boost::spirit::x3;

GTEST_TEST(vb6_parser_expressions, sample_expression)
{
  vb6_ast::expression ast;
  auto [res, sv] =
    test_grammar("foo1(foo2(3 + foo3(4), M.x_coord), True)", vb6_grammar::expression, ast);
  ASSERT_TRUE(res) << "stopped at: " << sv;
  EXPECT_TRUE(sv.empty());

  EXPECT_TRUE(boost::get<x3::forward_ast<vb6_ast::func_call>>(&ast));
  auto const& outer = boost::get<x3::forward_ast<vb6_ast::func_call>>(ast).get();
  EXPECT_EQ(outer.func_name, "foo1");
  ASSERT_EQ(outer.params.size(), 2);

  ASSERT_TRUE(boost::get<x3::forward_ast<vb6_ast::func_call>>(&outer.params[0]));
  auto const& inner =
    boost::get<x3::forward_ast<vb6_ast::func_call>>(outer.params[0]).get();
  EXPECT_EQ(inner.func_name, "foo2");
  ASSERT_EQ(inner.params.size(), 2);

  ASSERT_TRUE(boost::get<x3::forward_ast<vb6_ast::binary_expression>>(&inner.params[0]));
  auto const& sum =
    boost::get<x3::forward_ast<vb6_ast::binary_expression>>(inner.params[0]).get();
  ASSERT_EQ(sum.rest.size(), 1);
  EXPECT_EQ(sum.rest[0].op, vb6_ast::operator_type::plus);
  auto const& integer = boost::get<vb6_ast::const_expr>(sum.lhs);
  EXPECT_EQ(boost::get<vb6_ast::integer_dec>(integer).val, 3);
  ASSERT_TRUE(boost::get<x3::forward_ast<vb6_ast::func_call>>(&sum.rest[0].operand));
  auto const& operand_call =
    boost::get<x3::forward_ast<vb6_ast::func_call>>(sum.rest[0].operand).get();
  EXPECT_EQ(operand_call.func_name, "foo3");
  ASSERT_EQ(operand_call.params.size(), 1);
  auto const& operand_arg = boost::get<vb6_ast::const_expr>(operand_call.params[0]);
  EXPECT_EQ(boost::get<vb6_ast::integer_dec>(operand_arg).val, 4);
  auto const& member = boost::get<vb6_ast::decorated_variable>(inner.params[1]);
  EXPECT_EQ(member.ctx.elements.size(), 1);
  EXPECT_EQ(member.var, "x_coord");
}

GTEST_TEST(vb6_parser_expressions, expression_operators)
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
    ASSERT_TRUE(res) << "stopped at: " << sv << " while parsing " << source;
    EXPECT_TRUE(sv.empty()) << "unparsed input: " << source;
    ASSERT_TRUE(boost::get<x3::forward_ast<vb6_ast::binary_expression>>(&ast))
      << source;

    auto const& binary =
      boost::get<x3::forward_ast<vb6_ast::binary_expression>>(ast).get();
    ASSERT_EQ(binary.rest.size(), 1) << source;
    EXPECT_EQ(binary.rest[0].op, expected_op) << source;
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
    ASSERT_TRUE(res) << "stopped at: " << sv << " while parsing " << source;
    EXPECT_TRUE(sv.empty()) << "unparsed input: " << source;
    ASSERT_TRUE(boost::get<x3::forward_ast<vb6_ast::unary_expression>>(&ast))
      << source;

    auto const& unary =
      boost::get<x3::forward_ast<vb6_ast::unary_expression>>(ast).get();
    EXPECT_EQ(unary.op, expected_op) << source;
  }
}

GTEST_TEST(vb6_parser_expressions, expression_operator_precedence)
{
  vb6_ast::expression ast;
  auto [res, sv] = test_grammar("1 + 2 * 3", vb6_grammar::expression, ast);
  ASSERT_TRUE(res) << "stopped at: " << sv;
  EXPECT_TRUE(sv.empty());

  auto const& addition =
    boost::get<x3::forward_ast<vb6_ast::binary_expression>>(ast).get();
  ASSERT_EQ(addition.rest.size(), 1);
  EXPECT_EQ(addition.rest[0].op, vb6_ast::operator_type::plus);
  auto const& multiplication =
    boost::get<x3::forward_ast<vb6_ast::binary_expression>>(addition.rest[0].operand).get();
  ASSERT_EQ(multiplication.rest.size(), 1);
  EXPECT_EQ(multiplication.rest[0].op, vb6_ast::operator_type::mult);

  auto [parenthesized_res, parenthesized_sv] =
    test_grammar("(1 + 2) * 3", vb6_grammar::expression, ast);
  ASSERT_TRUE(parenthesized_res) << "stopped at: " << parenthesized_sv;
  EXPECT_TRUE(parenthesized_sv.empty());
  auto const& parenthesized_multiplication =
    boost::get<x3::forward_ast<vb6_ast::binary_expression>>(ast).get();
  EXPECT_EQ(parenthesized_multiplication.rest[0].op, vb6_ast::operator_type::mult);
  auto const& parenthesized_addition =
    boost::get<x3::forward_ast<vb6_ast::binary_expression>>(parenthesized_multiplication.lhs).get();
  EXPECT_EQ(parenthesized_addition.rest[0].op, vb6_ast::operator_type::plus);

  auto [power_res, power_sv] =
    test_grammar("2 ^ 3 ^ 2", vb6_grammar::expression, ast);
  ASSERT_TRUE(power_res) << "stopped at: " << power_sv;
  EXPECT_TRUE(power_sv.empty());
  auto const& power =
    boost::get<x3::forward_ast<vb6_ast::binary_expression>>(ast).get();
  ASSERT_EQ(power.rest.size(), 1);
  EXPECT_EQ(power.rest[0].op, vb6_ast::operator_type::exp);
  auto const& right_associated_power =
    boost::get<x3::forward_ast<vb6_ast::binary_expression>>(power.rest[0].operand).get();
  EXPECT_EQ(right_associated_power.rest[0].op, vb6_ast::operator_type::exp);

  auto [unary_res, unary_sv] =
    test_grammar("-2 ^ 2", vb6_grammar::expression, ast);
  ASSERT_TRUE(unary_res) << "stopped at: " << unary_sv;
  EXPECT_TRUE(unary_sv.empty());
  auto const& negation =
    boost::get<x3::forward_ast<vb6_ast::unary_expression>>(ast).get();
  EXPECT_EQ(negation.op, vb6_ast::operator_type::minus);
  auto const& negated_power =
    boost::get<x3::forward_ast<vb6_ast::binary_expression>>(negation.operand).get();
  EXPECT_EQ(negated_power.rest[0].op, vb6_ast::operator_type::exp);

  auto [logical_res, logical_sv] =
    test_grammar("Not False = True And 1 < 2", vb6_grammar::expression, ast);
  ASSERT_TRUE(logical_res) << "stopped at: " << logical_sv;
  EXPECT_TRUE(logical_sv.empty());
  auto const& conjunction =
    boost::get<x3::forward_ast<vb6_ast::binary_expression>>(ast).get();
  ASSERT_EQ(conjunction.rest.size(), 1);
  EXPECT_EQ(conjunction.rest[0].op, vb6_ast::operator_type::and_);
  auto const& logical_negation =
    boost::get<x3::forward_ast<vb6_ast::unary_expression>>(conjunction.lhs).get();
  EXPECT_EQ(logical_negation.op, vb6_ast::operator_type::not_);
  auto const& comparison =
    boost::get<x3::forward_ast<vb6_ast::binary_expression>>(logical_negation.operand).get();
  EXPECT_EQ(comparison.rest[0].op, vb6_ast::operator_type::equal);

  auto [modulo_res, modulo_sv] =
    test_grammar("8 \\ 4 Mod 3", vb6_grammar::expression, ast);
  ASSERT_TRUE(modulo_res) << "stopped at: " << modulo_sv;
  EXPECT_TRUE(modulo_sv.empty());
  auto const& modulo =
    boost::get<x3::forward_ast<vb6_ast::binary_expression>>(ast).get();
  EXPECT_EQ(modulo.rest[0].op, vb6_ast::operator_type::mod);
  auto const& integer_division =
    boost::get<x3::forward_ast<vb6_ast::binary_expression>>(modulo.lhs).get();
  EXPECT_EQ(integer_division.rest[0].op, vb6_ast::operator_type::div_int);

  auto [concatenation_res, concatenation_sv] =
    test_grammar("1 + 2 & 3", vb6_grammar::expression, ast);
  ASSERT_TRUE(concatenation_res) << "stopped at: " << concatenation_sv;
  EXPECT_TRUE(concatenation_sv.empty());
  auto const& concatenation =
    boost::get<x3::forward_ast<vb6_ast::binary_expression>>(ast).get();
  EXPECT_EQ(concatenation.rest[0].op, vb6_ast::operator_type::amp);
  auto const& concatenation_addition =
    boost::get<x3::forward_ast<vb6_ast::binary_expression>>(concatenation.lhs).get();
  EXPECT_EQ(concatenation_addition.rest[0].op, vb6_ast::operator_type::plus);

  auto [logical_order_res, logical_order_sv] =
    test_grammar("True Or False And True", vb6_grammar::expression, ast);
  ASSERT_TRUE(logical_order_res) << "stopped at: " << logical_order_sv;
  EXPECT_TRUE(logical_order_sv.empty());
  auto const& disjunction =
    boost::get<x3::forward_ast<vb6_ast::binary_expression>>(ast).get();
  EXPECT_EQ(disjunction.rest[0].op, vb6_ast::operator_type::or_);
  auto const& conjunction_rhs =
    boost::get<x3::forward_ast<vb6_ast::binary_expression>>(disjunction.rest[0].operand).get();
  EXPECT_EQ(conjunction_rhs.rest[0].op, vb6_ast::operator_type::and_);

  auto [logical_levels_res, logical_levels_sv] =
    test_grammar("True Xor False Eqv True Imp False",
                 vb6_grammar::expression, ast);
  ASSERT_TRUE(logical_levels_res) << "stopped at: " << logical_levels_sv;
  EXPECT_TRUE(logical_levels_sv.empty());
  auto const& implication =
    boost::get<x3::forward_ast<vb6_ast::binary_expression>>(ast).get();
  EXPECT_EQ(implication.rest[0].op, vb6_ast::operator_type::imp);
  auto const& equivalence =
    boost::get<x3::forward_ast<vb6_ast::binary_expression>>(implication.lhs).get();
  EXPECT_EQ(equivalence.rest[0].op, vb6_ast::operator_type::eqv);
  auto const& exclusive_or =
    boost::get<x3::forward_ast<vb6_ast::binary_expression>>(equivalence.lhs).get();
  EXPECT_EQ(exclusive_or.rest[0].op, vb6_ast::operator_type::xor_);
}
