//: vb6_parser_test.cpp

// vb6_parser
// Copyright (c) 2022 Federico Aponte
// This code is licensed under GNU Software License (see LICENSE.txt for details)

//#define BOOST_SPIRIT_X3_DEBUG

#include "test_grammar_helper.hpp"
#include "vb6_parser.hpp"
#include "vb6_ast_printer.hpp"

#include <boost/optional/optional_io.hpp>
#include <gtest/gtest.h>

#include <iostream>
#include <map>
#include <vector>

using namespace std;
namespace x3 = boost::spirit::x3;

GTEST_TEST(vb6_parser_simple, lonely_comment)
{
  vector<vb6_ast::lonely_comment> ast;
  test_grammar(
    "' This is comment line 1\r\n' Comment line 2\r\n",
    *vb6_grammar::lonely_comment, ast);

  ASSERT_EQ(ast.size(), 2);

  EXPECT_EQ(ast[0].content, " This is comment line 1");
  EXPECT_EQ(ast[1].content, " Comment line 2");
}

GTEST_TEST(vb6_parser_simple, empty_lines)
{
  vector<
    boost::variant<vb6_ast::empty_line, vb6_ast::lonely_comment>
  > ast;
  // critical to have lonely_comment first in the parsing rule
  auto const G = *(vb6_grammar::lonely_comment | vb6_grammar::empty_line);
  auto str = "' comment1\r\n"
             "\r\n"
             "' comment2\r\n";
  test_grammar(str, G, ast);

  ASSERT_EQ(ast.size(), 3);

  EXPECT_EQ(boost::get<vb6_ast::lonely_comment>(ast[0]).content, " comment1");
  EXPECT_NO_THROW(boost::get<vb6_ast::empty_line>(ast[1]));
  EXPECT_EQ(boost::get<vb6_ast::lonely_comment>(ast[2]).content, " comment2");
}

GTEST_TEST(vb6_parser_simple, quoted_string)
{
  string str;
  test_grammar("\"Quoted string.\"", vb6_grammar::quoted_string, str);
  EXPECT_EQ(str, "Quoted string.");
}

GTEST_TEST(vb6_parser_simple, basic_identifier)
{
  string id;
  test_grammar("iden_tifier", vb6_grammar::basic_identifier, id);
  EXPECT_EQ(id, "iden_tifier");
}

GTEST_TEST(vb6_parser_simple, var_identifier)
{
  string var;
  test_grammar("g_logger", vb6_grammar::var_identifier, var);
  EXPECT_EQ(var, "g_logger");

  var.clear();
  test_grammar("forth ", vb6_grammar::sub_identifier, var);
  EXPECT_EQ(var, "forth");

  var.clear();
  test_grammar("subroutine ", vb6_grammar::sub_identifier, var);
  EXPECT_EQ(var, "subroutine");

  /*
  string code = "sub ";
  string_view code_sv = code;
  auto it1 = cbegin(code_sv);
  auto const it2 = cend(code_sv);
  ASSERT_FALSE(boost::spirit::x3::phrase_parse(it1, it2, vb6_grammar::sub_identifier, vb6_grammar::skip, var));
  EXPECT_EQ(it1, begin(code_sv));
  */
}

GTEST_TEST(vb6_parser_simple, type_identifier)
{
  string type;
  test_grammar("Long", vb6_grammar::type_identifier, type);
  EXPECT_EQ(type, "Long");
}

GTEST_TEST(vb6_parser_simple, complex_type_identifier)
{
  string type;
  test_grammar("VB.Form", vb6_grammar::complex_type_identifier, type);
  EXPECT_EQ(type, "VB.Form");
}

GTEST_TEST(vb6_parser_simple, const_expression_non_numeric)
{
  vb6_ast::const_expr ast;
  string str;

  str = "\"una stringa\""s;
  test_grammar(str, vb6_grammar::const_expression, ast);
  EXPECT_EQ(boost::get<vb6_ast::quoted_string>(ast.get()), "una stringa");

  str = "True"s;
  test_grammar(str, vb6_grammar::const_expression, ast);
  EXPECT_EQ(boost::get<bool>(ast.get()), true);

  str = "Nothing"s;
  test_grammar(str, vb6_grammar::const_expression, ast);
  EXPECT_NO_THROW(boost::get<vb6_ast::nothing>(ast.get()));
}

GTEST_TEST(vb6_parser_simple, const_expression_integers)
{
  vb6_ast::const_expr ast;
  string str;

  str = "1234%"s;
  test_grammar(str, vb6_grammar::const_expression, ast);
  EXPECT_EQ(boost::get<vb6_ast::integer_dec>(ast.get()).val, 1234);

  str = "1234&"s;
  test_grammar(str, vb6_grammar::const_expression, ast);
  EXPECT_EQ(boost::get<vb6_ast::long_dec>(ast.get()).val, 1234);

  str = "&Hcafedead&"s;
  test_grammar(str, vb6_grammar::const_expression, ast);
  EXPECT_EQ(boost::get<vb6_ast::long_hex>(ast.get()).val, 0xcafedead);

  str = "&01234&"s;
  test_grammar(str, vb6_grammar::const_expression, ast);
  EXPECT_EQ(boost::get<vb6_ast::long_oct>(ast.get()).val, 01234);

  str = "1234"s;
  test_grammar(str, vb6_grammar::const_expression, ast);
  EXPECT_EQ(boost::get<vb6_ast::integer_dec>(ast.get()).val, 1234);
}

GTEST_TEST(vb6_parser_simple, const_expression_floats)
{
  vb6_ast::const_expr ast;
  string str;

  str = "1234!"s;
  test_grammar(str, vb6_grammar::const_expression, ast);
  EXPECT_EQ(boost::get<float>(ast.get()), 1234.0f);

  str = "1234#"s;
  test_grammar(str, vb6_grammar::const_expression, ast);
  EXPECT_EQ(boost::get<double>(ast.get()), 1234.0);

  str = "2.8"s;
  test_grammar(str, vb6_grammar::const_expression, ast);
  EXPECT_EQ(boost::get<float>(ast.get()), 2.8f);
}

GTEST_TEST(vb6_parser_simple, sample_expression)
{
  vb6_ast::expression ast;
  test_grammar("foo1(foo2(3, M.x_coord), True)", vb6_grammar::expression, ast);
  EXPECT_EQ(ast.get().type(), typeid(x3::forward_ast<vb6_ast::func_call>));
  EXPECT_EQ(boost::get<x3::forward_ast<vb6_ast::func_call>>(ast.get()).get().func_name, "foo1");
}

GTEST_TEST(vb6_parser_simple, single_var_declaration)
{
  vb6_ast::variable P1;
  test_grammar("g_logger As Long", vb6_grammar::single_var_declaration, P1);
  EXPECT_EQ(P1.name, "g_logger");
  EXPECT_FALSE(P1.construct);
  EXPECT_TRUE(P1.type);
  if(P1.type)
  {
    EXPECT_EQ(*P1.type, "Long");
  }

  vb6_ast::variable P2;
  test_grammar("name As New String", vb6_grammar::single_var_declaration, P2);
  EXPECT_EQ(P2.name, "name");
  EXPECT_TRUE(P2.construct);
  EXPECT_TRUE(P2.type);
  if(P2.type)
  {
    EXPECT_EQ(*P2.type, "String");
  }
}

GTEST_TEST(vb6_parser_tests, record_declaration)
{
  vb6_ast::record rec;
  test_grammar("Type PatRecord\r\n  name As String\r\n  age As Integer\r\nEnd Type\r\n",
               vb6_grammar::record_declaration, rec);

  EXPECT_EQ(rec.name, "PatRecord");
  EXPECT_EQ(rec.at, vb6_ast::access_type::na);
  ASSERT_EQ(rec.members.size(), 2);

  EXPECT_EQ(rec.members[0].name, "name");
  EXPECT_TRUE(rec.members[0].type);
  if(rec.members[0].type)
  {
    EXPECT_EQ(*rec.members[0].type, "String");
  }
  EXPECT_FALSE(rec.members[0].construct);

  EXPECT_EQ(rec.members[1].name, "age");
  EXPECT_TRUE(rec.members[1].type);
  if(rec.members[1].type)
  {
    EXPECT_EQ(*rec.members[1].type, "Integer");
  }
  EXPECT_FALSE(rec.members[1].construct);
}

GTEST_TEST(vb6_parser_tests, enum_declaration)
{
  vb6_ast::vb_enum enum1;
  test_grammar("Enum PatTypes\r\n  inpatient\r\n  outpatient\r\nEnd Enum\r\n",
               vb6_grammar::enum_declaration,  enum1);

  EXPECT_EQ(enum1.name, "PatTypes");
  EXPECT_EQ(enum1.at, vb6_ast::access_type::na);
  ASSERT_EQ(enum1.values.size(), 2);

  EXPECT_EQ(enum1.values[0].first, "inpatient");
  EXPECT_FALSE(enum1.values[0].second);

  EXPECT_EQ(enum1.values[1].first, "outpatient");
  EXPECT_FALSE(enum1.values[1].second);
}

GTEST_TEST(vb6_parser_tests, global_var_declaration)
{
  auto str = "Global g_logger As Long, v1, XRes As New Object, ptr As Module.MyRec, g_active As Boolean\r\n"s;
  vb6_ast::global_var_decls vars;
  test_grammar(str, vb6_grammar::global_var_declaration, vars);

  EXPECT_EQ(vars.at, vb6_ast::access_type::global);
  EXPECT_FALSE(vars.with_events);
  ASSERT_EQ(vars.vars.size(), 5);

  EXPECT_EQ(vars.vars[0].name, "g_logger");
  EXPECT_FALSE(vars.vars[0].construct);
  EXPECT_TRUE(vars.vars[0].type);
  if(vars.vars[0].type)
  {
    EXPECT_EQ(*vars.vars[0].type, "Long");
  }

  EXPECT_EQ(vars.vars[1].name, "v1");
  EXPECT_FALSE(vars.vars[1].construct);
  EXPECT_FALSE(vars.vars[1].type);

  EXPECT_EQ(vars.vars[2].name, "XRes");
  EXPECT_TRUE(vars.vars[2].construct);
  EXPECT_TRUE(vars.vars[2].type);
  if(vars.vars[2].type)
  {
    EXPECT_EQ(*vars.vars[2].type, "Object");
  }

  EXPECT_EQ(vars.vars[3].name, "ptr");
  EXPECT_FALSE(vars.vars[3].construct);
  EXPECT_TRUE(vars.vars[3].type);
  if(vars.vars[3].type)
  {
    EXPECT_EQ(*vars.vars[3].type, "MyRec");
  }

  EXPECT_EQ(vars.vars[4].name, "g_active");
  EXPECT_FALSE(vars.vars[4].construct);
  EXPECT_TRUE(vars.vars[4].type);
  if(vars.vars[4].type)
  {
    EXPECT_EQ(*vars.vars[4].type, "Boolean");
  }
}

GTEST_TEST(vb6_parser_tests, const_var_declaration1)
{
  auto cstr = "Const e As Single = 2.8, pi As Double = 3.14, u As Integer = -1\r\n"s;
  vb6_ast::const_var_stat cvars;
  test_grammar(cstr, vb6_grammar::const_var_declaration, cvars);

  ASSERT_EQ(cvars.size(), 3);

  EXPECT_EQ(cvars[0].var.name, "e");
  if(cvars[0].var.type)
  {
    EXPECT_EQ(*cvars[0].var.type, "Single");
  }
  EXPECT_FALSE(cvars[0].var.construct);
  EXPECT_EQ(boost::get<float>(cvars[0].value.get()), 2.8f);

  EXPECT_EQ(cvars[1].var.name, "pi");
  if(cvars[1].var.type)
  {
    EXPECT_EQ(*cvars[1].var.type, "Double");
  }
  EXPECT_FALSE(cvars[1].var.construct);
  EXPECT_EQ(boost::get<float>(cvars[1].value.get()), 3.14f);

  EXPECT_EQ(cvars[2].var.name, "u");
  if(cvars[2].var.type)
  {
    EXPECT_EQ(*cvars[2].var.type, "Integer");
  }
  EXPECT_FALSE(cvars[2].var.construct);
  EXPECT_EQ(boost::get<vb6_ast::integer_dec>(cvars[2].value.get()).val, -1);
}

GTEST_TEST(vb6_parser_tests, const_var_declaration2)
{
  vb6_ast::const_var_stat cvars;
  test_grammar("Private Const PI As Double = 3.1415\r\n",
               vb6_grammar::const_var_declaration,  cvars);

  ASSERT_EQ(cvars.size(), 1);

  EXPECT_EQ(cvars[0].var.name, "PI");
  if(cvars[0].var.type)
  {
    EXPECT_EQ(*cvars[0].var.type, "Double");
  }
  EXPECT_FALSE(cvars[0].var.construct);
  EXPECT_EQ(boost::get<float>(cvars[0].value.get()), 3.1415f);
}

GTEST_TEST(vb6_parser_tests, param_decl)
{
  vb6_ast::func_param fp;
  test_grammar("Optional ByVal name As String = \"pippo\"", vb6_grammar::param_decl, fp);

  EXPECT_TRUE(fp.isoptional);
  EXPECT_TRUE(fp.qualifier);
  if(fp.qualifier)
  {
    EXPECT_EQ(*fp.qualifier, vb6_ast::param_qualifier::byval);
  }
  EXPECT_EQ(fp.var.name, "name");
  EXPECT_FALSE(fp.var.construct);
  EXPECT_TRUE(fp.var.type);
  if(fp.var.type)
  {
    EXPECT_EQ(*fp.var.type, "String");
  }
  EXPECT_TRUE(fp.defvalue);
  if(fp.defvalue)
  {
    EXPECT_EQ(boost::get<vb6_ast::quoted_string>(fp.defvalue.get()), "pippo");
  }
}

GTEST_TEST(vb6_parser_tests, param_list_decl)
{
  vector<vb6_ast::func_param> fps;
  test_grammar("ByVal name As String, ByRef val As Integer",
               -(vb6_grammar::param_decl % ','), fps);

  ASSERT_EQ(fps.size(), 2);

  EXPECT_FALSE(fps[0].isoptional);
  EXPECT_TRUE(fps[0].qualifier);
  if(fps[0].qualifier)
  {
    EXPECT_EQ(*fps[0].qualifier, vb6_ast::param_qualifier::byval);
  }
  EXPECT_EQ(fps[0].var.name, "name");
  EXPECT_FALSE(fps[0].var.construct);
  EXPECT_TRUE(fps[0].var.type);
  if(fps[0].var.type)
  {
    EXPECT_EQ(*fps[0].var.type, "String");
  }
  EXPECT_FALSE(fps[0].defvalue);

  EXPECT_FALSE(fps[1].isoptional);
  EXPECT_TRUE(fps[1].qualifier);
  if(fps[1].qualifier)
  {
    EXPECT_EQ(*fps[1].qualifier, vb6_ast::param_qualifier::byref);
  }
  EXPECT_EQ(fps[1].var.name, "val");
  EXPECT_FALSE(fps[1].var.construct);
  EXPECT_TRUE(fps[1].var.type);
  if(fps[1].var.type)
  {
    EXPECT_EQ(*fps[1].var.type, "Integer");
  }
  EXPECT_FALSE(fps[1].defvalue);
}

GTEST_TEST(vb6_parser_tests, event_declaration)
{
  vb6_ast::eventHead event_decl;
  test_grammar("Public Event OnChange(ByVal Text As String)\r\n",
               vb6_grammar::eventHead, event_decl);

  EXPECT_EQ(event_decl.at, vb6_ast::access_type::public_);
  EXPECT_EQ(event_decl.name, "OnChange");
  EXPECT_EQ(event_decl.params.size(), 1);
}

GTEST_TEST(vb6_parser_tests, function_head)
{
  vb6_ast::functionHead fh;
  test_grammar(
    "Private Function OneFunc(ByVal name As String, ByRef val As Integer) As Integer\r\n",
    vb6_grammar::functionHead, fh);

  EXPECT_EQ(fh.at, vb6_ast::access_type::private_);
  EXPECT_EQ(fh.name, "OneFunc");
  EXPECT_EQ(fh.params.size(), 2);
  ASSERT_TRUE(fh.return_type);
  EXPECT_EQ(*fh.return_type, "Integer");
}

GTEST_TEST(vb6_parser_tests, function_head_no_params)
{
  vb6_ast::functionHead fh;
  test_grammar(
    "Private Function NoParamFunc() As Object\r\n", vb6_grammar::functionHead, fh);

  EXPECT_EQ(fh.at, vb6_ast::access_type::private_);
  EXPECT_EQ(fh.name, "NoParamFunc");
  EXPECT_TRUE(fh.params.empty());
  ASSERT_TRUE(fh.return_type);
  EXPECT_EQ(*fh.return_type, "Object");
}

GTEST_TEST(vb6_parser_tests, subroutine_head)
{
  vb6_ast::subHead sh;
  test_grammar(
    "Private Sub my_sub(ByRef str As String, ByVal valid As Boolean)\r\n",
    vb6_grammar::subHead, sh);

  EXPECT_EQ(sh.at, vb6_ast::access_type::private_);
  EXPECT_EQ(sh.name, "my_sub");
  EXPECT_EQ(sh.params.size(), 2);
}

GTEST_TEST(vb6_parser_tests, subroutine_head2)
{
  auto str = "Private Sub my_sub(ByRef str As String, ByVal valid As Boolean, Optional ByVal flag As Boolean = True)\r\n"s;
  vb6_ast::subHead sh;
  test_grammar(str, vb6_grammar::subHead, sh);

  EXPECT_EQ(sh.at, vb6_ast::access_type::private_);
  EXPECT_EQ(sh.name, "my_sub");
  EXPECT_EQ(sh.params.size(), 3);
}

GTEST_TEST(vb6_parser_tests, subroutine_head_with_optional_params)
{
  vb6_ast::subHead sh;
  test_grammar(
    "Private Sub my_sub(ByRef str As String, Optional ByVal valid As Boolean = false)\r\n",
    vb6_grammar::subHead, sh);

  EXPECT_EQ(sh.at, vb6_ast::access_type::private_);
  EXPECT_EQ(sh.name, "my_sub");
  ASSERT_EQ(sh.params.size(), 2);

  EXPECT_TRUE(sh.params[0].qualifier.has_value());
  if (sh.params[0].qualifier)
    EXPECT_EQ(sh.params[0].qualifier.get(), vb6_ast::param_qualifier::byref);
  EXPECT_EQ(sh.params[0].var.name, "str");
  if(sh.params[0].var.type)
  {
    EXPECT_EQ(*sh.params[0].var.type, "String");
  }
  EXPECT_FALSE(sh.params[0].var.construct);
  EXPECT_FALSE(sh.params[0].isoptional);
  EXPECT_FALSE(sh.params[0].defvalue);

  EXPECT_TRUE(sh.params[1].qualifier);
  if(sh.params[1].qualifier)
  {
    EXPECT_EQ(sh.params[1].qualifier.get(), vb6_ast::param_qualifier::byval);
  }
  EXPECT_EQ(sh.params[1].var.name, "valid");
  if(sh.params[1].var.type)
  {
    EXPECT_EQ(*sh.params[1].var.type, "Boolean");
  }
  EXPECT_FALSE(sh.params[1].var.construct);
  EXPECT_TRUE(sh.params[1].isoptional);
  EXPECT_TRUE(sh.params[1].defvalue);
  if(sh.params[0].defvalue)
  {
    EXPECT_EQ(boost::get<bool>(*sh.params[0].defvalue), false);
  }
}

GTEST_TEST(vb6_parser_tests, property_let_head)
{
  auto str = "Public Property Let Width(ByVal w As Integer)\r\n"s;
  vb6_ast::propertyLetHead ast;
  test_grammar(str, vb6_grammar::property_letHead, ast);

  EXPECT_EQ(ast.at, vb6_ast::access_type::public_);
  EXPECT_EQ(ast.name, "Width");
  EXPECT_EQ(ast.params.size(), 1);
}

GTEST_TEST(vb6_parser_tests, property_get_head)
{
  auto str = "Public Property Get Width() As Integer\r\n"s;
  vb6_ast::propertyGetHead ast;
  test_grammar(str, vb6_grammar::property_getHead, ast);

  EXPECT_EQ(ast.at, vb6_ast::access_type::public_);
  EXPECT_EQ(ast.name, "Width");
  EXPECT_EQ(ast.params.size(), 0);
  ASSERT_TRUE(ast.return_type);
  EXPECT_EQ(*ast.return_type, "Integer");
}

GTEST_TEST(vb6_parser_tests, dll_subroutine_declaration)
{
  vb6_ast::externalSub extsub;
  auto str = "Private Declare Sub BeepVB Lib \"kernel32.dll\" Alias \"Beep\" (ByVal time As Long, ByVal xx As Single)\r\n"s;
  test_grammar(str, vb6_grammar::external_sub_decl, extsub);

  EXPECT_EQ(extsub.at, vb6_ast::access_type::private_);
  EXPECT_EQ(extsub.name, "BeepVB");
  EXPECT_EQ(extsub.alias, "Beep");
  EXPECT_EQ(extsub.params.size(), 2);
  EXPECT_EQ(extsub.lib, "kernel32.dll");
}

GTEST_TEST(vb6_parser_tests, dll_function_declaration)
{
  vb6_ast::externalFunction extfunc;
  auto str = "Private Declare Function BeepVB Lib \"kernel32.dll\" Alias \"Beep\" (ByVal time As Long, ByVal xx As Single) As Long\r\n"s;
  test_grammar(str, vb6_grammar::external_function_decl, extfunc);

  EXPECT_EQ(extfunc.at, vb6_ast::access_type::private_);
  EXPECT_EQ(extfunc.name, "BeepVB");
  EXPECT_TRUE(extfunc.return_type);
  if(extfunc.return_type)
  {
    EXPECT_EQ(*extfunc.return_type, "Long");
  }
  EXPECT_EQ(extfunc.alias, "Beep");
  EXPECT_EQ(extfunc.params.size(), 2);
  EXPECT_EQ(extfunc.lib, "kernel32.dll");
}

GTEST_TEST(vb6_parser_tests, identifier_context)
{
  vb6_ast::identifier_context ctx;
  test_grammar("var1.func().pnt1.", vb6_grammar::identifier_context, ctx);

  EXPECT_FALSE(ctx.leading_dot);
  ASSERT_EQ(ctx.elements.size(), 3);
  EXPECT_EQ(boost::get<string>(ctx.elements[0].get()), "var1");
  auto& tmp = boost::get<x3::forward_ast<vb6_ast::func_call>>(ctx.elements[1].get()).get();
  EXPECT_EQ(tmp.func_name, "func");
  EXPECT_TRUE(tmp.params.empty());
  EXPECT_EQ(boost::get<string>(ctx.elements[2].get()), "pnt1");
}

GTEST_TEST(vb6_parser_tests, decorated_variable)
{
  vb6_ast::decorated_variable dec_var;
  test_grammar("var1.func().pnt1.X", vb6_grammar::decorated_variable, dec_var);

  EXPECT_FALSE(dec_var.ctx.leading_dot);
  ASSERT_EQ(dec_var.ctx.elements.size(), 3);
  EXPECT_EQ(boost::get<string>(dec_var.ctx.elements[0].get()), "var1");
  auto& tmp = boost::get<x3::forward_ast<vb6_ast::func_call>>(dec_var.ctx.elements[1].get()).get();
  EXPECT_EQ(tmp.func_name, "func");
  EXPECT_TRUE(tmp.params.empty());
  EXPECT_EQ(boost::get<string>(dec_var.ctx.elements[2].get()), "pnt1");
  EXPECT_EQ(dec_var.var, "X");
}

GTEST_TEST(vb6_parser_tests, attribute_block)
{
  vb6_ast::STRICT_MODULE_STRUCTURE::module_attributes attrs;
  auto str = "Attribute ModuleName = \"MyForm\"\r\n"
             "Attribute ProgID = \"00-00-00-00\"\r\n"s;
  test_grammar(str, *vb6_grammar::attributeDef, attrs);

  EXPECT_EQ(attrs.size(), 2);

  auto it1 = attrs.find("ModuleName");
  EXPECT_NE(it1, attrs.cend());
  if(it1 != attrs.cend())
  {
    EXPECT_EQ(it1->second, "MyForm");
  }

  auto it2 = attrs.find("ProgID");
  EXPECT_NE(it2, attrs.cend());
  if(it2 != attrs.cend())
  {
    EXPECT_EQ(it2->second, "00-00-00-00");
  }
}

GTEST_TEST(vb6_parser_tests, attributes)
{
  vb6_ast::STRICT_MODULE_STRUCTURE::module_attributes ast;
  auto str = R"vb(Attribute ModuleName = "MyForm"
                  Attribute ProgID = "00-00-00-00"
                  )vb"s;
  //test_grammar(str, vb6_grammar::preamble, ast);
  test_grammar(str, *vb6_grammar::attributeDef, ast);

  EXPECT_EQ(ast.size(), 2);

  auto const it1 = ast.find("ModuleName");
  EXPECT_NE(it1, ast.cend());
  if(it1 != ast.cend())
  {
    EXPECT_EQ(it1->second, "MyForm");
  }

  auto const it2 = ast.find("ProgID");
  EXPECT_NE(it2, ast.cend());
  if(it2 != ast.cend())
  {
    EXPECT_EQ(it2->second, "00-00-00-00");
  }
}

GTEST_TEST(vb6_parser_tests, options)
{
  vector<vb6_ast::module_option> ast;
  auto str = R"vb(Option Explicit
                  Option Base 0
                  )vb"s;
  test_grammar(str, *vb6_grammar::option_item, ast);

  ASSERT_EQ(ast.size(), 2);

  EXPECT_EQ(ast[0], vb6_ast::module_option::explicit_);
  EXPECT_EQ(ast[1], vb6_ast::module_option::base_0);
}

#if 0
GTEST_TEST(vb6_parser_tests, declaration_block)
{
  auto const declarations
    = boost::spirit::x3::rule<class _, vector<vb6_ast::STRICT_MODULE_STRUCTURE::declaration>>("declaration_block")
    = *vb6_grammar::STRICT_MODULE_STRUCTURE::declaration;

  vector<vb6_ast::STRICT_MODULE_STRUCTURE::declaration> decls;
  auto str = "Const e As Double = 2.8, pi As Double = 3.14, u As Integer = -1\r\n"
             "Global g_logger As Long, v1, XRes As Object, ptr As MyRec, g_active As Boolean\r\n"
             "Private Declare Sub PFoo Lib \"mylib.dll\" Alias \"PFoo\" (ByVal val As Long)\r\n"
             "Enum MyEnum1\r\n  c1 = 0\r\n  c2 = 1\r\nEnd Enum\r\n"
             "Public Type MyRecord1\r\n  v1 As String\r\n  v2 As Long\r\nEnd Type\r\n"s;
  test_grammar(str, declarations, decls);

  ASSERT_EQ(decls.size(), 5);

  EXPECT_NO_THROW(boost::get<vb6_ast::const_var_stat>(decls[0].get()));
  EXPECT_NO_THROW(boost::get<vb6_ast::global_var_decls>(decls[1].get()));
  EXPECT_NO_THROW(boost::get<vb6_ast::externalSub>(decls[2].get()));
  EXPECT_NO_THROW(boost::get<vb6_ast::vb_enum>(decls[3].get()));
  EXPECT_NO_THROW(boost::get<vb6_ast::record>(decls[4].get()));
}

GTEST_TEST(vb6_parser_tests, bas_unit_STRICT_MODULE_STRUCTURE)
{
  vb6_ast::STRICT_MODULE_STRUCTURE::vb_module ast;
  auto str = R"vb(Attribute ModuleName = "MyForm"
                  Attribute ProgID = "00-00-00-00"
                  Option Explicit
                  Option Base 0

                  ' declarations
                  Const u As Integer = 1234
                  Global g_logger As Long
                  Enum MyEnum1
                    c1 = 0
                    c2 = 1
                  End Enum
                  Sub my_sub(ByRef str As String)
                  End Sub
                  Function my_fun(ByRef str As String) As Long
                  End Function
                  )vb"s;
  test_grammar(str, vb6_grammar::STRICT_MODULE_STRUCTURE::basModDef, ast);

  vb6_ast_printer P(cout);
  P(ast);

  EXPECT_EQ(ast.attrs.size(), 2);

  auto const it1 = ast.attrs.find("ModuleName");
  EXPECT_NE(it1, ast.attrs.cend());
  if(it1 != ast.attrs.cend())
  {
    EXPECT_EQ(it1->second, "MyForm");
  }

  auto const it2 = ast.attrs.find("ProgID");
  EXPECT_NE(it2, ast.attrs.cend());
  if(it2 != ast.attrs.cend())
  {
    EXPECT_EQ(it2->second, "00-00-00-00");
  }

  EXPECT_EQ(ast.opts.items.size(), 4);
  if(ast.opts.items.size() == 4)
  {
#if 0
    EXPECT_EQ(ast.opts.items[0], vb6_ast::module_option::explicit_);
    EXPECT_EQ(ast.opts.items[1], vb6_ast::module_option::base_0);
#else
    EXPECT_EQ(boost::get<vb6_ast::module_option>(ast.opts.items[0].get()), vb6_ast::module_option::explicit_);
    EXPECT_EQ(boost::get<vb6_ast::module_option>(ast.opts.items[1].get()), vb6_ast::module_option::base_0);
#endif
    EXPECT_NO_THROW(boost::get<vb6_ast::empty_line>(ast.opts.items[2].get()));
    EXPECT_NO_THROW(boost::get<vb6_ast::lonely_comment>(ast.opts.items[3].get()));
  }

  EXPECT_EQ(ast.declarations.size(), 3);
  if(ast.declarations.size() == 3)
  {
    EXPECT_NO_THROW(boost::get<vb6_ast::const_var_stat>(ast.declarations[0].get()));
    EXPECT_NO_THROW(boost::get<vb6_ast::global_var_decls>(ast.declarations[1].get()));
    EXPECT_NO_THROW(boost::get<vb6_ast::vb_enum>(ast.declarations[2].get()));
  }

  EXPECT_EQ(ast.functions.size(), 2);
  if(ast.functions.size() == 2)
  {
    EXPECT_NO_THROW(boost::get<vb6_ast::subDef>(ast.functions[0].get()));
    EXPECT_NO_THROW(boost::get<vb6_ast::functionDef>(ast.functions[1].get()));
  }
}
#endif

GTEST_TEST(vb6_parser_tests, bas_unit)
{
  vb6_ast::vb_module ast;
  auto str = R"vb(Attribute ModuleName = "MyForm"
                  Attribute ProgID = "00-00-00-00"
                  Option Explicit
                  Option Base 0

                  ' declarations
                  Const u As Integer = 1234
                  Global g_logger As Long
                  Enum MyEnum1
                    c1 = 0
                    c2 = 1
                  End Enum
                  Sub my_sub(ByRef str As String)
                  End Sub
                  Function my_fun(ByRef str As String) As Long
                  End Function
                  )vb"s;
  test_grammar(str, vb6_grammar::basModDef, ast);

  vb6_ast_printer P(cout);
  P(ast);

  ASSERT_EQ(ast.size(), 11);

  EXPECT_EQ(boost::get<vb6_ast::module_attribute>(ast[0].get()).first, "ModuleName");
  EXPECT_EQ(boost::get<vb6_ast::module_attribute>(ast[1].get()).first, "ProgID");

  EXPECT_EQ(boost::get<vb6_ast::module_option>(ast[2].get()), vb6_ast::module_option::explicit_);
  EXPECT_EQ(boost::get<vb6_ast::module_option>(ast[3].get()), vb6_ast::module_option::base_0);

  EXPECT_NO_THROW(boost::get<vb6_ast::empty_line>(ast[4].get()));
  EXPECT_NO_THROW(boost::get<vb6_ast::lonely_comment>(ast[5].get()));

  EXPECT_NO_THROW(boost::get<vb6_ast::const_var_stat>(boost::get<vb6_ast::declaration>(ast[6].get())));
  EXPECT_NO_THROW(boost::get<vb6_ast::global_var_decls>(boost::get<vb6_ast::declaration>(ast[7].get())));
  EXPECT_NO_THROW(boost::get<vb6_ast::vb_enum>(boost::get<vb6_ast::declaration>(ast[8].get())));

  EXPECT_NO_THROW(boost::get<vb6_ast::subDef>(ast[9].get()));
  EXPECT_NO_THROW(boost::get<vb6_ast::functionDef>(ast[10].get()));
}

GTEST_TEST(vb6_parser_tests, trailing_comment)
{
  auto str = "Global g_var As Long ' how can we catch a trailing comment?\r\n"s;
  vb6_ast::global_var_decls vars;
  test_grammar(str, vb6_grammar::global_var_declaration, vars);

  EXPECT_EQ(vars.at, vb6_ast::access_type::global);
  EXPECT_FALSE(vars.with_events);
  ASSERT_EQ(vars.vars.size(), 1);

  EXPECT_EQ(vars.vars[0].name, "g_var");
  EXPECT_FALSE(vars.vars[0].construct);
  EXPECT_TRUE(vars.vars[0].type);
  if(vars.vars[0].type)
  {
    EXPECT_EQ(*vars.vars[0].type, "Long");
  }
}
