// VB6 Parser
// Copyright (c) 2018-2026 Federico Aponte
// SPDX-License-Identifier:	GPL-3.0-only

#include "test_grammar_helper.hpp"
#include "vb6_parser.hpp"

#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>

#include <iostream>
#include <vector>

namespace x3 = boost::spirit::x3;

TEST_CASE("Assignment_0", "[vb6_parser_statements]")
{
  vb6_ast::statements::assignStmt st;
  auto [res, sv] = test_grammar("Set var1 = \"  ciao\"\r\n", vb6_grammar::statements::assignmentStmt, st);
  INFO("stopped at: " << sv);
  REQUIRE(res);
  CHECK(sv.empty());

  CHECK(st.type == vb6_ast::assignmentType::set);
  CHECK(st.var.var == "var1");
  CHECK(!st.var.ctx.leading_dot);
  CHECK(st.var.ctx.elements.empty());
  CHECK(boost::get<vb6_ast::quoted_string>(
              boost::get<vb6_ast::const_expr>(st.rhs.get()).get()
            ) == "  ciao");
}

TEST_CASE("Assignment_1", "[vb6_parser_statements]")
{
  vb6_ast::statements::assignStmt st;
  auto [res, sv] = test_grammar("Let var2 = 54.7\r\n", vb6_grammar::statements::assignmentStmt, st);
  INFO("stopped at: " << sv);
  REQUIRE(res);
  CHECK(sv.empty());

  CHECK(st.type == vb6_ast::assignmentType::let);
  CHECK(st.var.var == "var2");
  CHECK(!st.var.ctx.leading_dot);
  CHECK(st.var.ctx.elements.empty());
  CHECK(boost::get<float>(boost::get<vb6_ast::const_expr>(st.rhs.get()).get()) == 54.7f);
}

TEST_CASE("Assignment_2", "[vb6_parser_statements]")
{
  vb6_ast::statements::assignStmt st;
  auto [res, sv] = test_grammar("var3 = Func(\"descr\", 54.7)\r\n", vb6_grammar::statements::assignmentStmt, st);
  INFO("stopped at: " << sv);
  REQUIRE(res);
  CHECK(sv.empty());

  CHECK(st.type == vb6_ast::assignmentType::na);
  CHECK(st.var.var == "var3");
  CHECK(!st.var.ctx.leading_dot);
  CHECK(st.var.ctx.elements.empty());
  auto& func = boost::get<x3::forward_ast<vb6_ast::func_call>>(st.rhs.get()).get();
  CHECK(func.func_name == "Func");
}

TEST_CASE("Assignment_3", "[vb6_parser_statements]")
{
  vb6_ast::statements::assignStmt st;
  auto [res, sv] = test_grammar("str = CStr(i)\r\n", vb6_grammar::statements::assignmentStmt, st);
  INFO("stopped at: " << sv);
  REQUIRE(res);
  CHECK(sv.empty());

  CHECK(st.type == vb6_ast::assignmentType::na);
  CHECK(st.var.var == "str");
  CHECK(!st.var.ctx.leading_dot);
  CHECK(st.var.ctx.elements.empty());
}

TEST_CASE("LocalVarDecl_1", "[vb6_parser_statements]")
{
  vb6_ast::statements::localVarDeclStmt st;
  auto [res, sv] = test_grammar("Dim var As New Form\r\n", vb6_grammar::statements::localvardeclStmt, st);
  INFO("stopped at: " << sv);
  REQUIRE(res);
  CHECK(sv.empty());

  CHECK(st.type == vb6_ast::localvardeclType::Dim);

  REQUIRE(st.vars.size() == 1);

  CHECK(st.vars[0].construct);
  CHECK(st.vars[0].name == "var");
  REQUIRE(st.vars[0].type.has_value());
  auto const& form_type =
    boost::get<vb6_ast::complex_type_identifier>(st.vars[0].type->value.get());
  CHECK(!form_type.library_or_module.has_value());
  CHECK(form_type.name == "Form");
}

TEST_CASE("LocalVarDecl_2", "[vb6_parser_statements]")
{
  vb6_ast::statements::localVarDeclStmt st;
  auto [res, sv] = test_grammar("Static var1 As String, var2 As Integer\r\n", vb6_grammar::statements::localvardeclStmt, st);
  INFO("stopped at: " << sv);
  REQUIRE(res);
  CHECK(sv.empty());

  CHECK(st.type == vb6_ast::localvardeclType::Static);

  REQUIRE(st.vars.size() == 2);

  CHECK(!st.vars[0].construct);
  CHECK(st.vars[0].name == "var1");
  REQUIRE(st.vars[0].type.has_value());
  CHECK(boost::get<vb6_ast::NativeType>(st.vars[0].type->value.get()) == vb6_ast::NativeType::String);

  CHECK(!st.vars[1].construct);
  CHECK(st.vars[1].name == "var2");
  REQUIRE(st.vars[1].type.has_value());
  CHECK(boost::get<vb6_ast::NativeType>(st.vars[1].type->value.get()) == vb6_ast::NativeType::Integer);
}

TEST_CASE("ReDim", "[vb6_parser_statements]")
{
  vb6_ast::statements::redimStmt st;
  auto [res, sv] = test_grammar("ReDim var1(15)\r\n", vb6_grammar::statements::redimStmt, st);
  //auto [res, sv] = test_grammar("ReDim var1(2*L + 1)\r\n", vb6_grammar::statements::redimStmt, st);
  INFO("stopped at: " << sv);
  REQUIRE(res);
  CHECK(sv.empty());

  CHECK(!st.preserve);
  CHECK(st.var.var == "var1");
}

TEST_CASE("Exit", "[vb6_parser_statements]")
{
  vb6_ast::statements::exitStmt st;
  auto [res, sv] = test_grammar("Exit Function\r\n", vb6_grammar::statements::exitStmt, st);
  INFO("stopped at: " << sv);
  REQUIRE(res);
  CHECK(sv.empty());

  CHECK(st.type == vb6_ast::exit_type::function);
}

TEST_CASE("GoTo", "[vb6_parser_statements]")
{
  vb6_ast::statements::gotoStmt st;
  auto [res, sv] = test_grammar("GoSub label1\r\n", vb6_grammar::statements::gotoStmt, st);
  INFO("stopped at: " << sv);
  REQUIRE(res);
  CHECK(sv.empty());

  CHECK(st.type == vb6_ast::gotoType::gosub_v);
  CHECK(st.label == "label1");
}

TEST_CASE("OnError", "[vb6_parser_statements]")
{
  vb6_ast::statements::onerrorStmt st;
  auto [res, sv] = test_grammar("On Error Resume Next\r\n", vb6_grammar::statements::onerrorStmt, st);
  INFO("stopped at: " << sv);
  REQUIRE(res);
  CHECK(sv.empty());

  CHECK(st.type == vb6_ast::onerror_type::resume_next);
  CHECK(st.label.empty());
}

TEST_CASE("Resume", "[vb6_parser_statements]")
{
  vb6_ast::statements::resumeStmt st;

  {
    auto [res, sv] = test_grammar("Resume\r\n", vb6_grammar::statements::resumeStmt, st);
    INFO("stopped at: " << sv);
  REQUIRE(res);
    CHECK(sv.empty());
    CHECK(st.type == vb6_ast::resume_type::implicit);
  }
  {
    auto [res, sv] = test_grammar("Resume Next\r\n", vb6_grammar::statements::resumeStmt, st);
    INFO("stopped at: " << sv);
  REQUIRE(res);
    CHECK(sv.empty());
    CHECK(st.type == vb6_ast::resume_type::next);
  }
  {
    auto [res, sv] = test_grammar("Resume 0\r\n", vb6_grammar::statements::resumeStmt, st);
    INFO("stopped at: " << sv);
  REQUIRE(res);
    CHECK(sv.empty());
    CHECK(st.type == vb6_ast::resume_type::line_nr);
    CHECK(boost::get<int>(st.label_or_line_nr) == 0);
  }
  {
    auto [res, sv] = test_grammar("Resume resume_point\r\n", vb6_grammar::statements::resumeStmt, st);
    INFO("stopped at: " << sv);
  REQUIRE(res);
    CHECK(sv.empty());
    CHECK(st.type == vb6_ast::resume_type::label);
    CHECK(boost::get<std::string>(st.label_or_line_nr) == "resume_point");
  }
}

TEST_CASE("Label", "[vb6_parser_statements]")
{
  vb6_ast::statements::labelStmt st;
  auto [res, sv] = test_grammar("label1:\r\n", vb6_grammar::statements::labelStmt, st);
  INFO("stopped at: " << sv);
  REQUIRE(res);
  CHECK(sv.empty());

  CHECK(st.label == "label1");
}

TEST_CASE("Call_explicit", "[vb6_parser_statements]")
{
  vb6_ast::statements::callStmt st;
  auto [res, sv] = test_grammar("Call Foo(13)\r\n", vb6_grammar::statements::callexplicitStmt, st);
  INFO("stopped at: " << sv);
  REQUIRE(res);
  CHECK(sv.empty());

  CHECK(st.explicit_call);
  CHECK(st.sub_name == "Foo");
  CHECK(st.params.size() == 1);

  {
    auto [res, sv] = test_grammar("Call Foo 13\r\n", vb6_grammar::statements::callexplicitStmt, st);
    INFO("stopped at: " << sv);
    REQUIRE(!res);
    //CHECK(sv.empty());
  }
}

TEST_CASE("Call_implicit", "[vb6_parser_statements]")
{
  vb6_ast::statements::callStmt st;
  auto [res, sv] = test_grammar("Foo \"Sea\", Nothing, False\r\n", vb6_grammar::statements::callimplicitStmt, st);
  INFO("stopped at: " << sv);
  REQUIRE(res);
  CHECK(sv.empty());

  CHECK(!st.explicit_call);
  CHECK(st.sub_name == "Foo");
  CHECK(st.params.size() == 3);
}

TEST_CASE("RaiseEvent", "[vb6_parser_statements]")
{
  vb6_ast::statements::raiseeventStmt st;
  auto [res, sv] = test_grammar("RaiseEvent OnChange(\"hi!\")\r\n",
               vb6_grammar::statements::raiseeventStmt, st);
  INFO("stopped at: " << sv);
  REQUIRE(res);
  CHECK(sv.empty());
}

TEST_CASE("statement_block", "[vb6_parser_statements]")
{
  vb6_ast::statements::statement_block st;
  auto [res, sv] = test_grammar(
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
  INFO("stopped at: " << sv);
  REQUIRE(res);
  CHECK(sv.empty());

  REQUIRE(st.size() == 15);
  CHECK(st[0].get().type() == typeid(vb6_ast::statements::assignStmt));
  CHECK(st[1].get().type() == typeid(vb6_ast::statements::assignStmt));
  CHECK(st[2].get().type() == typeid(vb6_ast::statements::assignStmt));
  CHECK(st[3].get().type() == typeid(vb6_ast::statements::assignStmt));
  CHECK(st[4].get().type() == typeid(vb6_ast::statements::localVarDeclStmt));
  CHECK(st[5].get().type() == typeid(vb6_ast::statements::localVarDeclStmt));
  CHECK(st[6].get().type() == typeid(vb6_ast::statements::redimStmt));
  CHECK(st[7].get().type() == typeid(vb6_ast::statements::exitStmt));
  CHECK(st[8].get().type() == typeid(vb6_ast::statements::gotoStmt));
  CHECK(st[9].get().type() == typeid(vb6_ast::statements::onerrorStmt));
  CHECK(st[10].get().type() == typeid(vb6_ast::statements::resumeStmt));
  CHECK(st[11].get().type() == typeid(vb6_ast::statements::labelStmt));
  CHECK(st[12].get().type() == typeid(vb6_ast::statements::callStmt));
  CHECK(st[13].get().type() == typeid(vb6_ast::statements::callStmt));
  CHECK(st[14].get().type() == typeid(vb6_ast::statements::raiseeventStmt));
}

TEST_CASE("With", "[vb6_parser_compound_statements]")
{
  vb6_ast::statements::withStmt st;
  auto [res, sv] = test_grammar(
    R"vb(With obj
               'Call Module1.foo(True, .Name)
               Call foo(True, .Name)
           End With
      )vb", vb6_grammar::statements::withStmt, st);
  INFO("stopped at: " << sv);
  REQUIRE(res);
  CHECK(sv.empty());

  CHECK(!st.with_variable.ctx.leading_dot);
  CHECK(st.with_variable.ctx.elements.empty());
  CHECK(st.with_variable.var == "obj");
  CHECK(st.block.size() == 2);
}

TEST_CASE("While", "[vb6_parser_compound_statements]")
{
  vb6_ast::statements::whileStmt st;
  auto [res, sv] = test_grammar(
    R"vb(While cond(34, i)
           Print str
         Wend
        )vb",
    vb6_grammar::statements::whileStmt, st);
  INFO("stopped at: " << sv);
  REQUIRE(res);
  CHECK(sv.empty());

  auto& tmp = boost::get<x3::forward_ast<vb6_ast::func_call>>(st.condition.get()).get();
  CHECK(tmp.func_name == "cond");
  CHECK(tmp.params.size() == 2);
  CHECK(st.block.size() == 1);
}

TEST_CASE("Do", "[vb6_parser_compound_statements]")
{
  vb6_ast::statements::doStmt st;
  auto [res, sv] = test_grammar(
    R"vb(Do
           Print str
         Loop
        )vb",
    vb6_grammar::statements::doStmt, st);
  INFO("stopped at: " << sv);
  REQUIRE(res);
  CHECK(sv.empty());

  CHECK(st.block.size() == 1);
}

TEST_CASE("DoWhile", "[vb6_parser_compound_statements]")
{
  vb6_ast::statements::dowhileStmt st;
  auto [res, sv] = test_grammar(
    R"vb(Do While cond(34, i)
           Print str
         Loop
        )vb",
    vb6_grammar::statements::dowhileStmt, st);
  INFO("stopped at: " << sv);
  REQUIRE(res);
  CHECK(sv.empty());

  auto& tmp = boost::get<x3::forward_ast<vb6_ast::func_call>>(st.condition.get()).get();
  CHECK(tmp.func_name == "cond");
  CHECK(tmp.params.size() == 2);
  CHECK(st.block.size() == 1);
}

TEST_CASE("LoopWhile", "[vb6_parser_compound_statements]")
{
  vb6_ast::statements::loopwhileStmt st;
  auto [res, sv] = test_grammar(
    R"vb(Do
           Print str
         Loop While cond(34, i)
        )vb",
    vb6_grammar::statements::loopwhileStmt, st);
  INFO("stopped at: " << sv);
  REQUIRE(res);
  CHECK(sv.empty());

  auto& tmp = boost::get<x3::forward_ast<vb6_ast::func_call>>(st.condition.get()).get();
  CHECK(tmp.func_name == "cond");
  CHECK(tmp.params.size() == 2);
  CHECK(st.block.size() == 1);
}

TEST_CASE("DoUntil", "[vb6_parser_compound_statements]")
{
  vb6_ast::statements::dountilStmt st;
  auto [res, sv] = test_grammar(
    R"vb(Do Until cond(34, i)
           Print str
         Loop
        )vb",
    vb6_grammar::statements::dountilStmt, st);
  INFO("stopped at: " << sv);
  REQUIRE(res);
  CHECK(sv.empty());

  auto& tmp = boost::get<x3::forward_ast<vb6_ast::func_call>>(st.condition.get()).get();
  CHECK(tmp.func_name == "cond");
  CHECK(tmp.params.size() == 2);
  CHECK(st.block.size() == 1);
}

TEST_CASE("LoopUntil", "[vb6_parser_compound_statements]")
{
  vb6_ast::statements::loopuntilStmt st;
  auto [res, sv] = test_grammar(
    R"vb(Do
           Print str
         Loop Until cond(34, i)
        )vb",
    vb6_grammar::statements::loopuntilStmt, st);
  INFO("stopped at: " << sv);
  REQUIRE(res);
  CHECK(sv.empty());

  auto& tmp = boost::get<x3::forward_ast<vb6_ast::func_call>>(st.condition.get()).get();
  CHECK(tmp.func_name == "cond");
  CHECK(tmp.params.size() == 2);
  CHECK(st.block.size() == 1);
}

TEST_CASE("For", "[vb6_parser_compound_statements]")
{
  vb6_ast::statements::forStmt st;
  auto [res, sv] = test_grammar(
    R"vb(For i = 1 To 100 Step 2
           Dim str As String
           str = CStr(i)
           Print str
         Next i
        )vb",
    vb6_grammar::statements::forStmt, st);
  INFO("stopped at: " << sv);
  REQUIRE(res);
  CHECK(sv.empty());

  CHECK(st.for_variable.var == "i");
  CHECK(!st.for_variable.ctx.leading_dot);
  CHECK(st.for_variable.ctx.elements.empty());

  auto& tmp1 = boost::get<vb6_ast::const_expr>(st.from.get()).get();
  auto& tmp2 = boost::get<vb6_ast::const_expr>(st.to.get()).get();
  auto& tmp3 = boost::get<vb6_ast::const_expr>(st.step.get()).get();

  CHECK(boost::get<vb6_ast::integer_dec>(tmp1).val == 1);
  CHECK(boost::get<vb6_ast::integer_dec>(tmp2).val == 100);
  CHECK(boost::get<vb6_ast::integer_dec>(tmp3).val == 2);

  CHECK(st.block.size() == 3);
}

TEST_CASE("ForEach", "[vb6_parser_compound_statements]")
{
  vb6_ast::statements::foreachStmt st;
  auto [res, sv] = test_grammar(
    R"vb(For Each el In Items
           Call Print(el)
         Next el
        )vb",
    vb6_grammar::statements::foreachStmt, st);
  INFO("stopped at: " << sv);
  REQUIRE(res);
  CHECK(sv.empty());

  CHECK(st.for_variable.var == "el");
  CHECK(!st.for_variable.ctx.leading_dot);
  CHECK(st.for_variable.ctx.elements.empty());

  auto& tmp1 = boost::get<vb6_ast::decorated_variable>(st.container.get());

  CHECK(tmp1.ctx.elements.empty());
  CHECK(tmp1.var == "Items");

  CHECK(st.block.size() == 1);
}

TEST_CASE("IfElse_substatements", "[vb6_parser_compound_statements]")
{
  {
    vb6_ast::statements::if_branch ast1;
    auto [res, sv] = test_grammar(
      R"vb(If cond1 Then
            Dim str As String
            str = CStr(i)
            Print str
          )vb",
      vb6_grammar::statements::ifBranch, ast1);
    INFO("stopped at: " << sv);
  REQUIRE(res);
    CHECK(sv.empty());

    auto& tmp1 = boost::get<vb6_ast::decorated_variable>(ast1.condition.get());

    CHECK(tmp1.var == "cond1");
    CHECK(ast1.block.size() == 3);
  }
  {
    vb6_ast::statements::if_branch ast2;
    auto [res, sv] = test_grammar(
      R"vb(ElseIf cond2 Then
            Dim str As String
            str = CStr(i)
            Print str
          )vb",
      vb6_grammar::statements::elsifBranch, ast2);
    INFO("stopped at: " << sv);
  REQUIRE(res);
    CHECK(sv.empty());

    auto& tmp2 = boost::get<vb6_ast::decorated_variable>(ast2.condition.get());

    CHECK(tmp2.var == "cond2");
    CHECK(ast2.block.size() == 3);
  }
  {
    vb6_ast::statements::statement_block ast3;
    auto [res, sv] = test_grammar(
      R"vb(Else
            Print str
          'End If
          )vb",
      vb6_grammar::statements::elseBranch, ast3);
    INFO("stopped at: " << sv);
  REQUIRE(res);
    CHECK(sv.empty());

    CHECK(ast3.size() == 2);
  }
}

TEST_CASE("IfElse", "[vb6_parser_compound_statements]")
{
  vb6_ast::statements::ifelseStmt ast;
  auto [res, sv] = test_grammar(
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
  INFO("stopped at: " << sv);
  REQUIRE(res);
  CHECK(sv.empty());

#ifdef SIMPLE_IF_STATEMENT
  CHECK(ast.first_branch.block.size() == 5);
  auto& tmp1 = boost::get<vb6_ast::decorated_variable>(ast.first_branch.condition.get());
  CHECK(tmp1.var == "cond1");
#else
  CHECK(ast.first_branch.block.size() == 1);
  auto& tmp0 = boost::get<vb6_ast::decorated_variable>(ast.first_branch.condition.get());
  CHECK(tmp0.var == "cond1");

  REQUIRE(ast.if_branches.size() == 2);

  CHECK(ast.if_branches[0].block.size() == 1);
  auto& tmp1 = boost::get<vb6_ast::decorated_variable>(ast.if_branches[0].condition.get());
  CHECK(tmp1.var == "cond2");

  CHECK(ast.if_branches[1].block.size() == 1);
  auto& tmp2 = boost::get<vb6_ast::decorated_variable>(ast.if_branches[1].condition.get());
  CHECK(tmp2.var == "cond3");
#endif

  REQUIRE(ast.else_branch.has_value());
  CHECK(ast.else_branch.get().size() == 1);
}

TEST_CASE("Select", "[vb6_parser_compound_statements]")
{
  vb6_ast::statements::selectStmt st;
  auto [res, sv] = test_grammar(
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
  INFO("stopped at: " << sv);
  REQUIRE(res);
  CHECK(sv.empty());

  REQUIRE(st.condition.get().type() == typeid(vb6_ast::decorated_variable));
  CHECK(boost::get<vb6_ast::decorated_variable>(st.condition.get()).var == "Type");
  CHECK(st.blocks.size() == 2);

  REQUIRE(st.blocks[0].case_expr.get().type() == typeid(vb6_ast::const_expr));
  CHECK(boost::get<vb6_ast::integer_dec>(
              boost::get<vb6_ast::const_expr>(st.blocks[0].case_expr.get()).get()
            ).val == 0);
  REQUIRE(st.blocks[0].block.size() == 2);
  CHECK(st.blocks[0].block[0].get().type() == typeid(vb6_ast::statements::localVarDeclStmt));
  CHECK(st.blocks[0].block[1].get().type() == typeid(vb6_ast::statements::callStmt));

  REQUIRE(st.blocks[1].case_expr.get().type() == typeid(vb6_ast::const_expr));
  CHECK(boost::get<vb6_ast::integer_dec>(
              boost::get<vb6_ast::const_expr>(st.blocks[1].case_expr.get()).get()
            ).val == 1);
  REQUIRE(st.blocks[1].block.size() == 6);
  CHECK(st.blocks[1].block[0].get().type() == typeid(vb6_ast::lonely_comment));
  CHECK(st.blocks[1].block[1].get().type() == typeid(vb6_ast::statements::localVarDeclStmt));
}
