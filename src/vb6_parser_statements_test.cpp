//: vb6_parser_statements_test.cpp

// vb6_parser
// Copyright (c) 2022 Federico Aponte
// This code is licensed under GNU Software License (see LICENSE.txt for details)

//#define BOOST_SPIRIT_X3_DEBUG

#include "test_grammar_helper_gtest.hpp"
#include "vb6_parser.hpp"

#include <gtest/gtest.h>

#include <iostream>
#include <map>
#include <vector>

using namespace std;
namespace x3 = boost::spirit::x3;

GTEST_TEST(vb6_parser_statements, Assignment_0)
{
  vb6_ast::statements::assignStmt st;
  test_grammar("Set var1 = \"  ciao\"\r\n", vb6_grammar::statements::assignmentStmt, st);

  EXPECT_EQ(st.type, vb6_ast::assignmentType::set);
  EXPECT_EQ(st.var.var, "var1");
  EXPECT_FALSE(st.var.ctx.leading_dot);
  EXPECT_TRUE(st.var.ctx.elements.empty());
  EXPECT_EQ(boost::get<vb6_ast::quoted_string>(
              boost::get<vb6_ast::const_expr>(st.rhs.get()).get()
            ), "  ciao");
}

GTEST_TEST(vb6_parser_statements, Assignment_1)
{
  vb6_ast::statements::assignStmt st;
  test_grammar("Let var2 = 54.7\r\n", vb6_grammar::statements::assignmentStmt, st);

  EXPECT_EQ(st.type, vb6_ast::assignmentType::let);
  EXPECT_EQ(st.var.var, "var2");
  EXPECT_FALSE(st.var.ctx.leading_dot);
  EXPECT_TRUE(st.var.ctx.elements.empty());
  EXPECT_EQ(boost::get<float>(
    boost::get<vb6_ast::const_expr>(st.rhs.get()).get()
    ), 54.7f);
}

GTEST_TEST(vb6_parser_statements, Assignment_2)
{
  vb6_ast::statements::assignStmt st;
  test_grammar("var3 = Func(\"descr\", 54.7)\r\n", vb6_grammar::statements::assignmentStmt, st);

  EXPECT_EQ(st.type, vb6_ast::assignmentType::na);
  EXPECT_EQ(st.var.var, "var3");
  EXPECT_FALSE(st.var.ctx.leading_dot);
  EXPECT_TRUE(st.var.ctx.elements.empty());
  auto& func = boost::get<x3::forward_ast<vb6_ast::func_call>>(
                 st.rhs.get()
               ).get();
  EXPECT_EQ(func.func_name, "Func");
}

GTEST_TEST(vb6_parser_statements, Assignment_3)
{
  vb6_ast::statements::assignStmt st;
  test_grammar("str = CStr(i)\r\n", vb6_grammar::statements::assignmentStmt, st);

  EXPECT_EQ(st.type, vb6_ast::assignmentType::na);
  EXPECT_EQ(st.var.var, "str");
  EXPECT_FALSE(st.var.ctx.leading_dot);
  EXPECT_TRUE(st.var.ctx.elements.empty());
}

GTEST_TEST(vb6_parser_statements, LocalVarDecl_1)
{
  vb6_ast::statements::localVarDeclStmt st;
  test_grammar("Dim var As New Form\r\n", vb6_grammar::statements::localvardeclStmt, st);

  EXPECT_EQ(st.type, vb6_ast::localvardeclType::Dim);

  ASSERT_EQ(st.vars.size(), 1);

  EXPECT_TRUE(st.vars[0].construct);
  EXPECT_EQ(st.vars[0].name, "var");
  ASSERT_TRUE(st.vars[0].type);
  EXPECT_EQ(*st.vars[0].type, "Form");
}

GTEST_TEST(vb6_parser_statements, LocalVarDecl_2)
{
  vb6_ast::statements::localVarDeclStmt st;
  test_grammar("Static var1 As String, var2 As Integer\r\n", vb6_grammar::statements::localvardeclStmt, st);

  EXPECT_EQ(st.type, vb6_ast::localvardeclType::Static);

  ASSERT_EQ(st.vars.size(), 2);

  EXPECT_FALSE(st.vars[0].construct);
  EXPECT_EQ(st.vars[0].name, "var1");
  ASSERT_TRUE(st.vars[0].type);
  EXPECT_EQ(*st.vars[0].type, "String");

  EXPECT_FALSE(st.vars[1].construct);
  EXPECT_EQ(st.vars[1].name, "var2");
  ASSERT_TRUE(st.vars[1].type);
  EXPECT_EQ(*st.vars[1].type, "Integer");
}

GTEST_TEST(vb6_parser_statements, ReDim)
{
  vb6_ast::statements::redimStmt st;
  test_grammar("ReDim var1(15)\r\n", vb6_grammar::statements::redimStmt, st);
  //test_grammar("ReDim var1(2*L + 1)\r\n", vb6_grammar::statements::redimStmt, st);
  EXPECT_FALSE(st.preserve);
  EXPECT_EQ(st.var.var, "var1");
}

GTEST_TEST(vb6_parser_statements, Exit)
{
  vb6_ast::statements::exitStmt st;
  test_grammar("Exit Function\r\n", vb6_grammar::statements::exitStmt, st);

  EXPECT_EQ(st.type, vb6_ast::exit_type::function);
}

GTEST_TEST(vb6_parser_statements, GoTo)
{
  vb6_ast::statements::gotoStmt st;
  test_grammar("GoSub label1\r\n", vb6_grammar::statements::gotoStmt, st);

  EXPECT_EQ(st.type, vb6_ast::gotoType::gosub_v);
  EXPECT_EQ(st.label, "label1");
}

GTEST_TEST(vb6_parser_statements, OnError)
{
  vb6_ast::statements::onerrorStmt st;
  test_grammar("On Error Resume Next\r\n", vb6_grammar::statements::onerrorStmt, st);

  EXPECT_EQ(st.type, vb6_ast::onerror_type::resume_next);
  EXPECT_TRUE(st.label.empty());
}

GTEST_TEST(vb6_parser_statements, Resume)
{
  vb6_ast::statements::resumeStmt st;

  test_grammar("Resume\r\n", vb6_grammar::statements::resumeStmt, st);
  EXPECT_EQ(st.type, vb6_ast::resume_type::implicit);

  test_grammar("Resume Next\r\n", vb6_grammar::statements::resumeStmt, st);
  EXPECT_EQ(st.type, vb6_ast::resume_type::next);

  test_grammar("Resume 0\r\n", vb6_grammar::statements::resumeStmt, st);
  EXPECT_EQ(st.type, vb6_ast::resume_type::line_nr);
  EXPECT_EQ(boost::get<int>(st.label_or_line_nr), 0);

  test_grammar("Resume resume_point\r\n", vb6_grammar::statements::resumeStmt, st);
  EXPECT_EQ(st.type, vb6_ast::resume_type::label);
  EXPECT_EQ(boost::get<string>(st.label_or_line_nr), "resume_point");
}

GTEST_TEST(vb6_parser_statements, Label)
{
  vb6_ast::statements::labelStmt st;
  test_grammar("label1:\r\n", vb6_grammar::statements::labelStmt, st);

  EXPECT_EQ(st.label, "label1");
}

GTEST_TEST(vb6_parser_statements, Call_explicit)
{
  vb6_ast::statements::callStmt st;
  test_grammar("Call Foo(13)\r\n", vb6_grammar::statements::callexplicitStmt, st);

  EXPECT_TRUE(st.explicit_call);
  EXPECT_EQ(st.sub_name, "Foo");
  EXPECT_EQ(st.params.size(), 1);

  test_grammar("Call Foo 13\r\n", vb6_grammar::statements::callexplicitStmt, st, false);
}

GTEST_TEST(vb6_parser_statements, Call_implicit)
{
  vb6_ast::statements::callStmt st;
  test_grammar("Foo \"Sea\", Nothing, False\r\n", vb6_grammar::statements::callimplicitStmt, st);

  EXPECT_FALSE(st.explicit_call);
  EXPECT_EQ(st.sub_name, "Foo");
  EXPECT_EQ(st.params.size(), 3);
}

GTEST_TEST(vb6_parser_statements, RaiseEvent)
{
  vb6_ast::statements::raiseeventStmt st;
  test_grammar("RaiseEvent OnChange(\"hi!\")\r\n",
               vb6_grammar::statements::raiseeventStmt, st);
}

GTEST_TEST(vb6_parser_statements, statement_block)
{
  vb6_ast::statements::statement_block st;
  test_grammar(
    R"vb(Set var1 = "  ciao"
         Let var2 = 54.7
         var3 = Func("descr", 54.7)
         str = CStr(i)
         Dim var As New Form
         Static var1 As String, var2 As Integer
         ReDim var1(15)
         Exit Function
         GoSub label1
         On Error Resume Next
         Resume Next
         label1:
         Call Foo(13)
         Foo "Sea", Nothing, False
         RaiseEvent OnChange("hi!")
      )vb", vb6_grammar::statements::statement_block, st);

  EXPECT_EQ(st.size(), 15);
  EXPECT_EQ(st[0].get().type(), typeid(vb6_ast::statements::assignStmt));
  EXPECT_EQ(st[1].get().type(), typeid(vb6_ast::statements::assignStmt));
  EXPECT_EQ(st[2].get().type(), typeid(vb6_ast::statements::assignStmt));
  EXPECT_EQ(st[3].get().type(), typeid(vb6_ast::statements::assignStmt));
  EXPECT_EQ(st[4].get().type(), typeid(vb6_ast::statements::localVarDeclStmt));
  EXPECT_EQ(st[5].get().type(), typeid(vb6_ast::statements::localVarDeclStmt));
  EXPECT_EQ(st[6].get().type(), typeid(vb6_ast::statements::redimStmt));
  EXPECT_EQ(st[7].get().type(), typeid(vb6_ast::statements::exitStmt));
  EXPECT_EQ(st[8].get().type(), typeid(vb6_ast::statements::gotoStmt));
  EXPECT_EQ(st[9].get().type(), typeid(vb6_ast::statements::onerrorStmt));
  EXPECT_EQ(st[10].get().type(), typeid(vb6_ast::statements::resumeStmt));
  EXPECT_EQ(st[11].get().type(), typeid(vb6_ast::statements::labelStmt));
  EXPECT_EQ(st[12].get().type(), typeid(vb6_ast::statements::callStmt));
  EXPECT_EQ(st[13].get().type(), typeid(vb6_ast::statements::callStmt));
  EXPECT_EQ(st[14].get().type(), typeid(vb6_ast::statements::raiseeventStmt));
}

GTEST_TEST(vb6_parser_compound_statements, With)
{
  vb6_ast::statements::withStmt st;
  test_grammar(
    R"vb(With obj
               'Call Module1.foo(True, .Name)
               Call foo(True, .Name)
           End With
      )vb", vb6_grammar::statements::withStmt, st);

  EXPECT_FALSE(st.with_variable.ctx.leading_dot);
  EXPECT_TRUE(st.with_variable.ctx.elements.empty());
  EXPECT_EQ(st.with_variable.var, "obj");
  EXPECT_EQ(st.block.size(), 2);
}

GTEST_TEST(vb6_parser_compound_statements, While)
{
  vb6_ast::statements::whileStmt st;
  test_grammar(
    R"vb(While cond(34, i)
           Print str
         Wend
        )vb",
    vb6_grammar::statements::whileStmt, st);
  auto& tmp = boost::get<x3::forward_ast<vb6_ast::func_call>>(st.condition.get()).get();
  EXPECT_EQ(tmp.func_name, "cond");
  EXPECT_EQ(tmp.params.size(), 2);
  EXPECT_EQ(st.block.size(), 1);
}

GTEST_TEST(vb6_parser_compound_statements, Do)
{
  vb6_ast::statements::doStmt st;
  test_grammar(
    R"vb(Do
           Print str
         Loop
        )vb",
    vb6_grammar::statements::doStmt, st);
  EXPECT_EQ(st.block.size(), 1);
}

GTEST_TEST(vb6_parser_compound_statements, DoWhile)
{
  vb6_ast::statements::dowhileStmt st;
  test_grammar(
    R"vb(Do While cond(34, i)
           Print str
         Loop
        )vb",
    vb6_grammar::statements::dowhileStmt, st);
  auto& tmp = boost::get<x3::forward_ast<vb6_ast::func_call>>(st.condition.get()).get();
  EXPECT_EQ(tmp.func_name, "cond");
  EXPECT_EQ(tmp.params.size(), 2);
  EXPECT_EQ(st.block.size(), 1);
}

GTEST_TEST(vb6_parser_compound_statements, LoopWhile)
{
  vb6_ast::statements::loopwhileStmt st;
  test_grammar(
    R"vb(Do
           Print str
         Loop While cond(34, i)
        )vb",
    vb6_grammar::statements::loopwhileStmt, st);
  auto& tmp = boost::get<x3::forward_ast<vb6_ast::func_call>>(st.condition.get()).get();
  EXPECT_EQ(tmp.func_name, "cond");
  EXPECT_EQ(tmp.params.size(), 2);
  EXPECT_EQ(st.block.size(), 1);
}

GTEST_TEST(vb6_parser_compound_statements, DoUntil)
{
  vb6_ast::statements::dountilStmt st;
  test_grammar(
    R"vb(Do Until cond(34, i)
           Print str
         Loop
        )vb",
    vb6_grammar::statements::dountilStmt, st);
  auto& tmp = boost::get<x3::forward_ast<vb6_ast::func_call>>(st.condition.get()).get();
  EXPECT_EQ(tmp.func_name, "cond");
  EXPECT_EQ(tmp.params.size(), 2);
  EXPECT_EQ(st.block.size(), 1);
}

GTEST_TEST(vb6_parser_compound_statements, LoopUntil)
{
  vb6_ast::statements::loopuntilStmt st;
  test_grammar(
    R"vb(Do
           Print str
         Loop Until cond(34, i)
        )vb",
    vb6_grammar::statements::loopuntilStmt, st);
  auto& tmp = boost::get<x3::forward_ast<vb6_ast::func_call>>(st.condition.get()).get();
  EXPECT_EQ(tmp.func_name, "cond");
  EXPECT_EQ(tmp.params.size(), 2);
  EXPECT_EQ(st.block.size(), 1);
}

GTEST_TEST(vb6_parser_compound_statements, For)
{
  vb6_ast::statements::forStmt st;
  test_grammar(
    R"vb(For i = 1 To 100 Step 2
           Dim str As String
           str = CStr(i)
           Print str
         Next i
        )vb",
    vb6_grammar::statements::forStmt, st);
  EXPECT_EQ(st.for_variable.var, "i");
  EXPECT_FALSE(st.for_variable.ctx.leading_dot);
  EXPECT_TRUE(st.for_variable.ctx.elements.empty());

  auto& tmp1 = boost::get<vb6_ast::const_expr>(st.from.get()).get();
  auto& tmp2 = boost::get<vb6_ast::const_expr>(st.to.get()).get();
  auto& tmp3 = boost::get<vb6_ast::const_expr>(st.step.get()).get();

  EXPECT_EQ(boost::get<vb6_ast::integer_dec>(tmp1).val, 1);
  EXPECT_EQ(boost::get<vb6_ast::integer_dec>(tmp2).val, 100);
  EXPECT_EQ(boost::get<vb6_ast::integer_dec>(tmp3).val, 2);

  EXPECT_EQ(st.block.size(), 3);
}

GTEST_TEST(vb6_parser_compound_statements, ForEach)
{
  vb6_ast::statements::foreachStmt st;
  test_grammar(
    R"vb(For Each el In Items
           Call Print(el)
         Next el
        )vb",
    vb6_grammar::statements::foreachStmt, st);
  EXPECT_EQ(st.for_variable.var, "el");
  EXPECT_FALSE(st.for_variable.ctx.leading_dot);
  EXPECT_TRUE(st.for_variable.ctx.elements.empty());

  auto& tmp1 = boost::get<vb6_ast::decorated_variable>(st.container.get());

  EXPECT_TRUE(tmp1.ctx.elements.empty());
  EXPECT_EQ(tmp1.var, "Items");

  EXPECT_EQ(st.block.size(), 1);
}

GTEST_TEST(vb6_parser_compound_statements, IfElse_substatements)
{
  vb6_ast::statements::if_branch ast1;
  test_grammar(
    R"vb(If cond1 Then
           Dim str As String
           str = CStr(i)
           Print str
        )vb",
    vb6_grammar::statements::ifBranch, ast1);

  auto& tmp1 = boost::get<vb6_ast::decorated_variable>(ast1.condition.get());

  EXPECT_EQ(tmp1.var, "cond1");
  EXPECT_EQ(ast1.block.size(), 3);

  vb6_ast::statements::if_branch ast2;
  test_grammar(
    R"vb(ElseIf cond2 Then
           Dim str As String
           str = CStr(i)
           Print str
        )vb",
    vb6_grammar::statements::elsifBranch, ast2);

  auto& tmp2 = boost::get<vb6_ast::decorated_variable>(ast2.condition.get());

  EXPECT_EQ(tmp2.var, "cond2");
  EXPECT_EQ(ast2.block.size(), 3);

  vb6_ast::statements::statement_block ast3;
  test_grammar(
    R"vb(Else
           Print str
         'End If
        )vb",
    vb6_grammar::statements::elseBranch, ast3);

  EXPECT_EQ(ast3.size(), 2);
}

GTEST_TEST(vb6_parser_compound_statements, IfElse)
{
  vb6_ast::statements::ifelseStmt ast;
  test_grammar(
    R"vb(If cond1 Then
           Print "1"
         ElseIf cond2 Then
           Print "2"
         ElseIf cond3 Then
           Print "3"
         Else
           Print "4"
         End If
        )vb",
    vb6_grammar::statements::ifelseStmt, ast);

#ifdef SIMPLE_IF_STATEMENT
  EXPECT_EQ(ast.first_branch.block.size(), 5);
  auto& tmp1 = boost::get<vb6_ast::decorated_variable>(ast.first_branch.condition.get());
  EXPECT_EQ(tmp1.var, "cond1");
#else
  EXPECT_EQ(ast.first_branch.block.size(), 1);
  auto& tmp0 = boost::get<vb6_ast::decorated_variable>(ast.first_branch.condition.get());
  EXPECT_EQ(tmp0.var, "cond1");

  ASSERT_EQ(ast.if_branches.size(), 2);

  EXPECT_EQ(ast.if_branches[0].block.size(), 1);
  auto& tmp1 = boost::get<vb6_ast::decorated_variable>(ast.if_branches[0].condition.get());
  EXPECT_EQ(tmp1.var, "cond2");

  EXPECT_EQ(ast.if_branches[1].block.size(), 1);
  auto& tmp2 = boost::get<vb6_ast::decorated_variable>(ast.if_branches[1].condition.get());
  EXPECT_EQ(tmp2.var, "cond3");
#endif

  ASSERT_TRUE(ast.else_branch.has_value());
  EXPECT_EQ(ast.else_branch.get().size(), 1);
}

GTEST_TEST(vb6_parser_compound_statements, Select)
{
  vb6_ast::statements::selectStmt st;
  test_grammar(
    R"vb(Select Case frm.Type
           Case 0
             Dim str As String
             Call Print(0)
           Case 1
             ' case 1
             Dim str As String
           'Case 1 To 4, 7 To 9, 11, 13, Is > MaxNumber
           '  Print "Difficult"
           'Case Else
           '  Print "Default"
         End Select
      )vb", vb6_grammar::statements::selectStmt, st);

  ASSERT_EQ(st.condition.get().type(), typeid(vb6_ast::decorated_variable));
  EXPECT_EQ(boost::get<vb6_ast::decorated_variable>(st.condition.get()).var, "Type");
  EXPECT_EQ(st.blocks.size(), 2);

  ASSERT_EQ(st.blocks[0].case_expr.get().type(), typeid(vb6_ast::const_expr));
  EXPECT_EQ(boost::get<vb6_ast::integer_dec>(
              boost::get<vb6_ast::const_expr>(st.blocks[0].case_expr.get()).get()
            ).val, 0);
  ASSERT_EQ(st.blocks[0].block.size(), 2);
  EXPECT_EQ(st.blocks[0].block[0].get().type(), typeid(vb6_ast::statements::localVarDeclStmt));
  EXPECT_EQ(st.blocks[0].block[1].get().type(), typeid(vb6_ast::statements::callStmt));

  ASSERT_EQ(st.blocks[1].case_expr.get().type(), typeid(vb6_ast::const_expr));
  EXPECT_EQ(boost::get<vb6_ast::integer_dec>(
              boost::get<vb6_ast::const_expr>(st.blocks[1].case_expr.get()).get()
            ).val, 1);
  ASSERT_EQ(st.blocks[1].block.size(), 6);
  EXPECT_EQ(st.blocks[1].block[0].get().type(), typeid(vb6_ast::lonely_comment));
  EXPECT_EQ(st.blocks[1].block[1].get().type(), typeid(vb6_ast::statements::localVarDeclStmt));
}
