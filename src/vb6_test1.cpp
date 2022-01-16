//: vb6_test1.cpp

// vb6_parser
// Copyright (c) 2022 Federico Aponte
// This code is licensed under GNU Software License (see LICENSE.txt for details)

//#define BOOST_SPIRIT_X3_DEBUG

#include "test_grammar_helper.hpp"
#include "vb6_parser.hpp"
#include "vb6_ast_printer.hpp"

#include <iostream>
#include <map>
#include <vector>

using namespace std;
//namespace x3 = boost::spirit::x3;

void test_vb6_unit(ostream& os, std::string_view unit)
{
  os << "---- " << __func__ << " ----\n";

#if 0
  vb6_ast::STRICT_MODULE_STRUCTURE::vb_module ast;
  auto const G = vb6_grammar::STRICT_MODULE_STRUCTURE::basModDef;
#else
  vb6_ast::vb_module ast;
  auto const G = vb6_grammar::basModDef;
#endif

  test_grammar(os, "test_vb6_unit", G, unit, ast);

  vb6_ast_printer P(os);
  P(ast);
}

void vb6_test1(ostream& os)
{
  string var;
  test_grammar(os, "var_identifier", vb6_grammar::var_identifier, "g_logger", var);
  os << var << '\n';

  string type;
  test_grammar(os, "type_identifier", vb6_grammar::type_identifier, "Long", type);
  os << type << '\n';

  vb6_ast_printer Print(os);

  vb6_ast::expression expr;
  test_grammar(os, "expression", vb6_grammar::expression, "foo1(foo2(3, M.x_coord), True)", expr);
  Print(expr);
  os << '\n';

  vb6_ast::variable P1;
  test_grammar(os, "single_var_declaration", vb6_grammar::single_var_declaration, "g_logger As Long", P1);
  Print(P1);
  os << '\n';

  vb6_ast::variable res1;
  test_grammar(os, "single_var_declaration", vb6_grammar::single_var_declaration, "name As String", res1);
  Print(res1);
  os << '\n';

  vb6_ast::record rec1;
  test_grammar(os, "record_declaration", vb6_grammar::record_declaration,
               "Type PatRecord\r\n  name As String\r\n  age As Integer\r\nEnd Type\r\n", rec1);
  Print(rec1);

  vb6_ast::vb_enum enum1;
  test_grammar(os, "enum_declaration", vb6_grammar::enum_declaration,
               "Enum PatTypes\r\n  inpatient\r\n  outpatient\r\nEnd Enum\r\n", enum1);
  Print(enum1);

  auto str = "Global g_logger As Long, v1, XRes As New Object, ptr As Module.MyRec, g_active As Boolean\r\n"s;
  vb6_ast::global_var_decls vars;
  test_grammar(os, "global_var_declaration", vb6_grammar::global_var_declaration, str, vars);
  Print(vars);

  auto cstr = "Const bActivate As Boolean = False"
              ", keyword As String = \"Module\""
              ", e As Single = 2.8"
              ", pi As Double = 3.14"
              ", u As Integer = -1\r\n"s;
  vb6_ast::const_var_stat cvars;
  test_grammar(os, "const_var_declaration", vb6_grammar::const_var_declaration, cstr, cvars);
  Print(cvars);

  vb6_ast::const_var_stat cvar;
  test_grammar(os, "const_var_declaration", vb6_grammar::const_var_declaration,
               "Private Const PI As Double = 3.1415\r\n", cvar);
  Print(cvar);

  vb6_ast::func_param res2;
  test_grammar(os, "param_decl", vb6_grammar::param_decl, "Optional ByVal name As String = \"pippo\"", res2);
  Print(res2);
  os << '\n';

  vector<vb6_ast::func_param> res3;
  test_grammar(os, "param_list_decl", -(vb6_grammar::param_decl % ','),
               "ByVal name As String, ByRef val As Integer", res3);
  for(auto& el : res3)
  {
    Print(el);
    os << ", ";
  }
  os << '\n';

  vb6_ast::eventHead event_decl;
  test_grammar(os, "event declaration", vb6_grammar::eventHead,
               "Public Event OnChange(ByVal Text As String)\r\n", event_decl);
  Print(event_decl);

  vb6_ast::functionHead func_header1;
  test_grammar(os, "function head", vb6_grammar::functionHead,
               "Private Function OneFunc(ByVal name As String, ByRef val As Integer) As Integer\r\n", func_header1);
  Print(func_header1);

  vb6_ast::functionHead func_header2;
  test_grammar(os, "function head (no params)", vb6_grammar::functionHead,
               "Private Function NoParamFunc() As Object\r\n", func_header2);
  Print(func_header2);

  vb6_ast::subHead sub_header1;
  test_grammar(os, "subroutine_head", vb6_grammar::subHead,
               "Private Sub my_sub(ByRef str As String, ByVal valid As Boolean)\r\n", sub_header1);
  Print(sub_header1);

  auto str2 = "Private Sub my_sub(ByRef str As String, ByVal valid As Boolean, Optional ByVal flag As Boolean = True)\r\n"s;
  vb6_ast::subHead sh;
  test_grammar(os, "subroutine_head2", vb6_grammar::subHead, str2, sh);
  Print(sh);

  vb6_ast::subHead sub_header2;
  test_grammar(os, "subroutine head with optional params", vb6_grammar::subHead,
               "Private Sub my_sub(ByRef str As String, Optional ByVal valid As Boolean = false)\r\n", sub_header2);
  Print(sub_header2);

  vector<vb6_ast::lonely_comment> comments;
  test_grammar(os, "lonely_comment", *vb6_grammar::lonely_comment,
               "' This is comment line 1\r\n' Comment line 2\r\n", comments);
  for(auto& el : comments)
    Print(el);

  vb6_ast::subDef sub1;
  test_grammar(os, "subroutine definition 1", vb6_grammar::subDef,
               "Private Sub my_sub(ByRef str As String, ByVal valid As Boolean)\r\nEnd Sub\r\n", sub1);
  Print(sub1);

  vb6_ast::subDef sub2;
  test_grammar(os, "subroutine definition 2", vb6_grammar::subDef,
       R"vb(Public Sub my_sub(ByRef p1 As String, ByVal p2 As Boolean, p3 As Long)
                ' this is a comment
                Dim x As Long, y As String, frm As New VB.Form
                Set x = Module1.GetField("size").Value
                y = "word"
                'On Error GoTo err_handler
                Dim z As Variant
                'ReDim arr(256, 256)
                Call routine1()
                Call routine2(34, "ciao")
                Call routine3(1.5, Module1.GetField("name1").Value)
                'Call subroutine3(1.5, Value)
                routine1
                routine2 34, "ciao"
                routine3 1.5, Module1.GetField("name1").Value
                Exit Sub
            err_handler:
                GoTo final
            final:
            End Sub
           )vb", sub2);
  Print(sub2);

  //x3::error_handler<std::string::const_iterator> err_hndlr(it1, it2, out, "dummy.cpp");

  // we pass our error handler to the parser so we can access
  // it later on in our on_error and on_sucess handlers
  //auto const parser = x3::with<x3::error_handler_tag>(std::ref(err_hndlr))[vb6_grammar::functionDef];

  vb6_ast::functionDef func1;
  test_grammar(os, "function definition", vb6_grammar::functionDef,
       R"vb(Private Function foo(ByRef p1 As String, ByVal p2 As Boolean, p3 As Long) As Long
                Static s_State As String
                Set x = "sss"
                foo = 5
            End Function
           )vb", func1);
  Print(func1);

  vb6_ast::functionDef func_def1;
  test_grammar(os, "function definition (no params)", vb6_grammar::functionDef,
    "Private Function NoParamFunc() As Object\r\nNoParamFunc = Nothing\r\nEnd Function\r\n", func_def1);
  Print(func_def1);

  vb6_ast::functionDef func2;
  test_grammar(os, "function definition (1 line)", vb6_grammar::functionDef,
               "Function foo(str As String) As String: foo = \"ciao\": End Function\r\n", func2);
  Print(func2);
}
