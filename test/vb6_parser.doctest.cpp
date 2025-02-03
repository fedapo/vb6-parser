//: vb6_parser.ut.cpp

// vb6_parser
// Copyright (c) 2022 Federico Aponte
// This code is licensed under GNU Software License (see LICENSE.txt for details)

//#define BOOST_SPIRIT_X3_DEBUG
#pragma GCC diagnostic ignored "-Wunused-variable"

#include "test_grammar_helper_ut.hpp"
#include "vb6_parser.hpp"
#include "vb6_ast_printer.hpp"

#include <boost/optional/optional_io.hpp>
#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>

#include <iostream>
#include <map>
#include <sstream>
#include <vector>

using namespace std;
namespace x3 = boost::spirit::x3;

void log_compiler_info(std::ostream& os)
{
  // how to tell that AppleClang and not plain Clang has been used?
  // __APPLE_CC__ does not work as it's always defined
#if defined(__clang__)
  os << "Compiler: Clang " << __clang_major__ << "." << __clang_minor__ << "." << __clang_patchlevel__<< '\n';
#elif defined(__GNUC__)
  os << "Compiler: GCC " << __GNUC__ << "." << __GNUC_MINOR__ << "." << __GNUC_PATCHLEVEL__<< '\n';
#elif defined(__MSC_VER)
  os << "Compiler: MSC " << __MSC_VER << '\n';
#endif
}

DOCTEST_TEST_SUITE_BEGIN("vb6_parser_simple");

DOCTEST_TEST_CASE("lonely_comment")
{
  vector<vb6_ast::lonely_comment> ast;
  auto [res, sv] = test_grammar(
    "' This is comment line 1\r\n' Comment line 2\r\n",
    *vb6_grammar::lonely_comment, ast);
  REQUIRE_MESSAGE(res, "stopped at: " << sv);
  CHECK(sv.empty());

  REQUIRE_EQ(ast.size(), 2);

  CHECK_EQ(ast[0].content, " This is comment line 1");
  CHECK_EQ(ast[1].content, " Comment line 2");
}

DOCTEST_TEST_CASE("empty_lines")
{
  vector<
    boost::variant<vb6_ast::empty_line, vb6_ast::lonely_comment>
  > ast;
  // critical to have lonely_comment first in the parsing rule
  auto const G = *(vb6_grammar::lonely_comment | vb6_grammar::empty_line);
  auto str = "' comment1\r\n"
            "\r\n"
            "' comment2\r\n";
  auto [res, sv] = test_grammar(str, G, ast);
  REQUIRE_MESSAGE(res, "stopped at: " << sv);
  CHECK(sv.empty());

  REQUIRE_EQ(ast.size(), 3);

  CHECK_EQ(boost::get<vb6_ast::lonely_comment>(ast[0]).content, " comment1");
  REQUIRE_NOTHROW([ast](){boost::get<vb6_ast::empty_line>(ast[1]);});
  CHECK_EQ(boost::get<vb6_ast::lonely_comment>(ast[2]).content, " comment2");
}

DOCTEST_TEST_CASE("quoted_string")
{
  string str;
  auto [res, sv] = test_grammar("\"Quoted string.\"", vb6_grammar::quoted_string, str);
  REQUIRE_MESSAGE(res, "stopped at: " << sv);
  CHECK(sv.empty());
  CHECK_EQ(str, "Quoted string.");
}

DOCTEST_TEST_CASE("basic_identifier")
{
  string id;
  auto [res, sv] = test_grammar("iden_tifier", vb6_grammar::basic_identifier, id);
  REQUIRE_MESSAGE(res, "stopped at: " << sv);
  CHECK(sv.empty());
  CHECK_EQ(id, "iden_tifier");
}

DOCTEST_TEST_CASE("var_identifier")
{
  string var;

  DOCTEST_SUBCASE("1")
  {
    auto [res, sv] = test_grammar("g_logger", vb6_grammar::var_identifier, var);
    REQUIRE_MESSAGE(res, "stopped at: " << sv);
    CHECK(sv.empty());
    CHECK_EQ(var, "g_logger");
  }

  DOCTEST_SUBCASE("2")
  {
    auto [res, sv] = test_grammar("forth ", vb6_grammar::sub_identifier, var);
    REQUIRE_MESSAGE(res, "stopped at: " << sv);
    CHECK(sv.empty());
    CHECK_EQ(var, "forth");
  }

  DOCTEST_SUBCASE("3")
  {
    string var;
    auto [res, sv] = test_grammar("subroutine ", vb6_grammar::sub_identifier, var);
    REQUIRE_MESSAGE(res, "stopped at: " << sv);
    CHECK(sv.empty());
    CHECK_EQ(var, "subroutine");
  }

  /*
  string code = "sub ";
  string_view code_sv = code;
  auto it1 = cbegin(code_sv);
  auto const it2 = cend(code_sv);
  CHECK(!boost::spirit::x3::phrase_parse(it1, it2, vb6_grammar::sub_identifier, vb6_grammar::skip, var));
  CHECK_EQ(it1, begin(code_sv));
  */
}

DOCTEST_TEST_CASE("type_identifier")
{
  string type;
  auto [res, sv] = test_grammar("Long", vb6_grammar::type_identifier, type);
  REQUIRE_MESSAGE(res, "stopped at: " << sv);
  CHECK(sv.empty());
  CHECK_EQ(type, "Long"s);
}

DOCTEST_TEST_CASE("complex_type_identifier")
{
  string type;
  auto [res, sv] = test_grammar("VB.Form", vb6_grammar::complex_type_identifier, type);
  REQUIRE_MESSAGE(res, "stopped at: " << sv);
  CHECK(sv.empty());
  CHECK_EQ(type, "VB.Form"s);
}

DOCTEST_TEST_CASE("const_expression_non_numeric")
{
  vb6_ast::const_expr ast;
  string str;

  DOCTEST_SUBCASE("1")
  {
    str = "\"una stringa\""s;
    auto [res, sv] = test_grammar(str, vb6_grammar::const_expression, ast);
    REQUIRE_MESSAGE(res, "stopped at: " << sv);
    CHECK(sv.empty());
    CHECK_EQ(boost::get<vb6_ast::quoted_string>(ast.get()), "una stringa"s);
  }

  DOCTEST_SUBCASE("2")
  {
    str = "True"s;
    auto [res, sv] = test_grammar(str, vb6_grammar::const_expression, ast);
    REQUIRE_MESSAGE(res, "stopped at: " << sv);
    CHECK(sv.empty());
    CHECK(boost::get<bool>(ast.get()));
  }

  DOCTEST_SUBCASE("3")
  {
    str = "Nothing"s;
    auto [res, sv] = test_grammar(str, vb6_grammar::const_expression, ast);
    REQUIRE_MESSAGE(res, "stopped at: " << sv);
    CHECK(sv.empty());
    REQUIRE_NOTHROW([ast](){boost::get<vb6_ast::nothing>(ast.get());});
  }
}

DOCTEST_TEST_CASE("const_expression_integers")
{
  vb6_ast::const_expr ast;
  string str;

  DOCTEST_SUBCASE("1")
  {
    str = "1234%"s;
    auto [res, sv] = test_grammar(str, vb6_grammar::const_expression, ast);
    REQUIRE_MESSAGE(res, "stopped at: " << sv);
    CHECK(sv.empty());
    CHECK_EQ(boost::get<vb6_ast::integer_dec>(ast.get()).val, 1234);
  }

  DOCTEST_SUBCASE("2")
  {
    str = "1234&"s;
    auto [res, sv] = test_grammar(str, vb6_grammar::const_expression, ast);
    REQUIRE_MESSAGE(res, "stopped at: " << sv);
    CHECK(sv.empty());
    CHECK_EQ(boost::get<vb6_ast::long_dec>(ast.get()).val, 1234);
  }

  DOCTEST_SUBCASE("3")
  {
    str = "&Hcafedead&"s;
    auto [res, sv] = test_grammar(str, vb6_grammar::const_expression, ast);
    REQUIRE_MESSAGE(res, "stopped at: " << sv);
    CHECK(sv.empty());
    CHECK_EQ(boost::get<vb6_ast::long_hex>(ast.get()).val, 0xcafedead);
  }

  DOCTEST_SUBCASE("4")
  {
    str = "&01234&"s;
    auto [res, sv] = test_grammar(str, vb6_grammar::const_expression, ast);
    REQUIRE_MESSAGE(res, "stopped at: " << sv);
    CHECK(sv.empty());
    CHECK_EQ(boost::get<vb6_ast::long_oct>(ast.get()).val, 01234);
  }

  DOCTEST_SUBCASE("5")
  {
    str = "1234"s;
    auto [res, sv] = test_grammar(str, vb6_grammar::const_expression, ast);
    REQUIRE_MESSAGE(res, "stopped at: " << sv);
    CHECK(sv.empty());
    CHECK_EQ(boost::get<vb6_ast::integer_dec>(ast.get()).val, 1234);
  }
}

DOCTEST_TEST_CASE("const_expression_floats")
{
  vb6_ast::const_expr ast;
  string str;

  DOCTEST_SUBCASE("1")
  {
    str = "1234!"s;
    auto [res, sv] = test_grammar(str, vb6_grammar::const_expression, ast);
    REQUIRE_MESSAGE(res, "stopped at: " << sv);
    CHECK(sv.empty());
    CHECK_EQ(boost::get<float>(ast.get()), 1234.0f);
  }

  DOCTEST_SUBCASE("2")
  {
    str = "1234#"s;
    auto [res, sv] = test_grammar(str, vb6_grammar::const_expression, ast);
    REQUIRE_MESSAGE(res, "stopped at: " << sv);
    CHECK(sv.empty());
    CHECK_EQ(boost::get<double>(ast.get()), 1234.0);
  }

  DOCTEST_SUBCASE("1")
  {
    str = "2.8"s;
    auto [res, sv] = test_grammar(str, vb6_grammar::const_expression, ast);
    REQUIRE_MESSAGE(res, "stopped at: " << sv);
    CHECK(sv.empty());
    CHECK_EQ(boost::get<float>(ast.get()), 2.8f);
  }
}

DOCTEST_TEST_CASE("sample_expression")
{
  vb6_ast::expression ast;
  auto [res, sv] = test_grammar("foo1(foo2(3, M.x_coord), True)", vb6_grammar::expression, ast);
  REQUIRE_MESSAGE(res, "stopped at: " << sv);
  CHECK(sv.empty());
  CHECK_EQ(ast.get().type(), typeid(x3::forward_ast<vb6_ast::func_call>));
  CHECK_EQ(boost::get<x3::forward_ast<vb6_ast::func_call>>(ast.get()).get().func_name, "foo1");
}

DOCTEST_TEST_CASE("single_var_declaration")
{
  vb6_ast::variable P1;
  auto [res, sv] = test_grammar("g_logger As Long", vb6_grammar::single_var_declaration, P1);
  REQUIRE_MESSAGE(res, "stopped at: " << sv);
  CHECK(sv.empty());
  CHECK_EQ(P1.name, "g_logger");
  CHECK(!P1.construct);
  CHECK(P1.type.has_value());
  if(P1.type)
  {
    CHECK_EQ(*P1.type, "Long");
  }
}

DOCTEST_TEST_CASE("single_var_declaration2")
{
  vb6_ast::variable P2;
  auto [res, sv] = test_grammar("name As New String", vb6_grammar::single_var_declaration, P2);
  REQUIRE_MESSAGE(res, "stopped at: " << sv);
  CHECK(sv.empty());
  CHECK_EQ(P2.name, "name");
  CHECK(P2.construct);
  CHECK(P2.type.has_value());
  if(P2.type)
  {
    CHECK_EQ(*P2.type, "String");
  }
}

DOCTEST_TEST_SUITE_END();

DOCTEST_TEST_SUITE_BEGIN("vb6_parser_tests");

DOCTEST_TEST_CASE("record_declaration")
{
  vb6_ast::record rec;
  auto [res, sv] = test_grammar("Type PatRecord\r\n  name As String\r\n  age As Integer\r\nEnd Type\r\n",
              vb6_grammar::record_declaration, rec);
  REQUIRE_MESSAGE(res, "stopped at: " << sv);
  CHECK(sv.empty());

  CHECK_EQ(rec.name, "PatRecord");
  CHECK_EQ(rec.at, vb6_ast::access_type::na);
  REQUIRE_EQ(rec.members.size(), 2);

  CHECK_EQ(rec.members[0].name, "name");
  CHECK(rec.members[0].type.has_value());
  if(rec.members[0].type)
  {
    CHECK_EQ(*rec.members[0].type, "String");
  }
  CHECK(!rec.members[0].construct);

  CHECK_EQ(rec.members[1].name, "age");
  CHECK(rec.members[1].type.has_value());
  if(rec.members[1].type)
  {
    CHECK_EQ(*rec.members[1].type, "Integer");
  }
  CHECK(!rec.members[1].construct);
}

DOCTEST_TEST_CASE("enum_declaration")
{
  vb6_ast::vb_enum enum1;
  auto [res, sv] = test_grammar("Enum PatTypes\r\n  inpatient\r\n  outpatient\r\nEnd Enum\r\n",
              vb6_grammar::enum_declaration,  enum1);
  REQUIRE_MESSAGE(res, "stopped at: " << sv);
  CHECK(sv.empty());

  CHECK_EQ(enum1.name, "PatTypes");
  CHECK_EQ(enum1.at, vb6_ast::access_type::na);
  REQUIRE_EQ(enum1.values.size(), 2);

  CHECK_EQ(enum1.values[0].first, "inpatient");
  CHECK(!enum1.values[0].second);

  CHECK_EQ(enum1.values[1].first, "outpatient");
  CHECK(!enum1.values[1].second);
}

DOCTEST_TEST_CASE("global_var_declaration")
{
  auto str = "Global g_logger As Long, v1, XRes As New Object, ptr As Module.MyRec, g_active As Boolean\r\n"s;
  vb6_ast::global_var_decls vars;
  auto [res, sv] = test_grammar(str, vb6_grammar::global_var_declaration, vars);
  REQUIRE_MESSAGE(res, "stopped at: " << sv);
  CHECK(sv.empty());

  CHECK_EQ(vars.at, vb6_ast::access_type::global);
  CHECK(!vars.with_events);
  REQUIRE_EQ(vars.vars.size(), 5);

  cout << "size: " << vars.vars.size() << "\n";

  CHECK_EQ(vars.vars[0].name, "g_logger");
  CHECK(!vars.vars[0].construct);
  CHECK(vars.vars[0].type.has_value());
  if(vars.vars[0].type)
  {
    CHECK_EQ(*vars.vars[0].type, "Long");
  }

  CHECK_EQ(vars.vars[1].name, "v1");
  CHECK(!vars.vars[1].construct);
  CHECK(!vars.vars[1].type.has_value());

  CHECK_EQ(vars.vars[2].name, "XRes");
  CHECK(vars.vars[2].construct);
  CHECK(vars.vars[2].type.has_value());
  if(vars.vars[2].type)
  {
    CHECK_EQ(*vars.vars[2].type, "Object");
  }

  CHECK_EQ(vars.vars[3].name, "ptr");
  CHECK(!vars.vars[3].construct);
  CHECK(vars.vars[3].type.has_value());
  if(vars.vars[3].type)
  {
    CHECK_EQ(*vars.vars[3].type, "MyRec");
  }

  CHECK_EQ(vars.vars[4].name, "g_active");
  CHECK(!vars.vars[4].construct);
  CHECK(vars.vars[4].type.has_value());
  if(vars.vars[4].type)
  {
    CHECK_EQ(*vars.vars[4].type, "Boolean");
  }
}

DOCTEST_TEST_CASE("const_var_declaration1")
{
  auto cstr = "Const e As Single = 2.8, pi As Double = 3.14, u As Integer = -1\r\n"s;
  vb6_ast::const_var_stat cvars;
  auto [res, sv] = test_grammar(cstr, vb6_grammar::const_var_declaration, cvars);
  REQUIRE_MESSAGE(res, "stopped at: " << sv);
  CHECK(sv.empty());

  REQUIRE_EQ(cvars.size(), 3);

  cout << "size: " << cvars.size() << "\n";

  CHECK_EQ(cvars[0].var.name, "e");
  if(cvars[0].var.type)
  {
    CHECK_EQ(*cvars[0].var.type, "Single");
  }
  CHECK(!cvars[0].var.construct);
  CHECK_EQ(boost::get<float>(cvars[0].value.get()), 2.8f);

  CHECK_EQ(cvars[1].var.name, "pi");
  if(cvars[1].var.type)
  {
    CHECK_EQ(*cvars[1].var.type, "Double");
  }
  CHECK(!cvars[1].var.construct);
  CHECK_EQ(boost::get<float>(cvars[1].value.get()), 3.14f);

  CHECK_EQ(cvars[2].var.name, "u");
  if(cvars[2].var.type)
  {
    CHECK_EQ(*cvars[2].var.type, "Integer");
  }
  CHECK(!cvars[2].var.construct);
  CHECK_EQ(boost::get<vb6_ast::integer_dec>(cvars[2].value.get()).val, -1);
}

DOCTEST_TEST_CASE("const_var_declaration2")
{
  vb6_ast::const_var_stat cvars;
  auto [res, sv] = test_grammar("Private Const PI As Double = 3.1415\r\n",
              vb6_grammar::const_var_declaration,  cvars);
  REQUIRE_MESSAGE(res, "stopped at: " << sv);
  CHECK(sv.empty());

  REQUIRE_EQ(cvars.size(), 1);

  cout << "size: " << cvars.size() << "\n";

  CHECK_EQ(cvars[0].var.name, "PI");
  if(cvars[0].var.type)
  {
    CHECK_EQ(*cvars[0].var.type, "Double");
  }
  CHECK(!cvars[0].var.construct);
  CHECK_EQ(boost::get<float>(cvars[0].value.get()), 3.1415f);
}

DOCTEST_TEST_CASE("param_decl")
{
  vb6_ast::func_param fp;
  auto [res, sv] = test_grammar("Optional ByVal name As String = \"pippo\"", vb6_grammar::param_decl, fp);
  REQUIRE_MESSAGE(res, "stopped at: " << sv);
  CHECK(sv.empty());

  CHECK(fp.isoptional);
  CHECK(fp.qualifier.has_value());
  if(fp.qualifier)
  {
    CHECK_EQ(*fp.qualifier, vb6_ast::param_qualifier::byval);
  }
  CHECK_EQ(fp.var.name, "name");
  CHECK(!fp.var.construct);
  CHECK(fp.var.type.has_value());
  if(fp.var.type)
  {
    CHECK_EQ(*fp.var.type, "String");
  }
  CHECK(fp.defvalue.has_value());
  if(fp.defvalue)
  {
    CHECK_EQ(boost::get<vb6_ast::quoted_string>(fp.defvalue.get()), "pippo");
  }
}

DOCTEST_TEST_CASE("param_list_decl")
{
  vector<vb6_ast::func_param> fps;
  auto [res, sv] = test_grammar("ByVal name As String, ByRef val As Integer",
              -(vb6_grammar::param_decl % ','), fps);
  REQUIRE_MESSAGE(res, "stopped at: " << sv);
  CHECK(sv.empty());

  cout << "size: " << fps.size() << "\n";

  REQUIRE_EQ(fps.size(), 2);

  CHECK(!fps[0].isoptional);
  CHECK(fps[0].qualifier.has_value());
  if(fps[0].qualifier)
  {
    CHECK_EQ(*fps[0].qualifier, vb6_ast::param_qualifier::byval);
  }
  CHECK_EQ(fps[0].var.name, "name");
  CHECK(!fps[0].var.construct);
  CHECK(fps[0].var.type.has_value());
  if(fps[0].var.type)
  {
    CHECK_EQ(*fps[0].var.type, "String");
  }
  CHECK(!fps[0].defvalue);

  CHECK(!fps[1].isoptional);
  CHECK(fps[1].qualifier.has_value());
  if(fps[1].qualifier)
  {
    CHECK_EQ(*fps[1].qualifier, vb6_ast::param_qualifier::byref);
  }
  CHECK_EQ(fps[1].var.name, "val");
  CHECK(!fps[1].var.construct);
  CHECK(fps[1].var.type.has_value());
  if(fps[1].var.type)
  {
    CHECK_EQ(*fps[1].var.type, "Integer");
  }
  CHECK(!fps[1].defvalue);
}

DOCTEST_TEST_CASE("event_declaration")
{
  vb6_ast::eventHead event_decl;
  auto [res, sv] = test_grammar("Public Event OnChange(ByVal Text As String)\r\n",
               vb6_grammar::eventHead, event_decl);
  REQUIRE_MESSAGE(res, "stopped at: " << sv);
  CHECK(sv.empty());

  CHECK_EQ(event_decl.at, vb6_ast::access_type::public_);
  CHECK_EQ(event_decl.name, "OnChange");
  CHECK_EQ(event_decl.params.size(), 1);
}

DOCTEST_TEST_CASE("function_head")
{
  vb6_ast::functionHead fh;
  auto [res, sv] = test_grammar(
    "Private Function OneFunc(ByVal name As String, ByRef val As Integer) As Integer\r\n",
    vb6_grammar::functionHead, fh);
  REQUIRE_MESSAGE(res, "stopped at: " << sv);
  CHECK(sv.empty());

  CHECK_EQ(fh.at, vb6_ast::access_type::private_);
  CHECK_EQ(fh.name, "OneFunc");
  CHECK_EQ(fh.params.size(), 2);
  REQUIRE(fh.return_type.has_value());
  CHECK_EQ(*fh.return_type, "Integer");
}

DOCTEST_TEST_CASE("function_head_no_params")
{
  vb6_ast::functionHead fh;
  auto [res, sv] = test_grammar(
    "Private Function NoParamFunc() As Object\r\n", vb6_grammar::functionHead, fh);
  REQUIRE_MESSAGE(res, "stopped at: " << sv);
  CHECK(sv.empty());

  CHECK_EQ(fh.at, vb6_ast::access_type::private_);
  CHECK_EQ(fh.name, "NoParamFunc");
  CHECK(fh.params.empty());
  REQUIRE(fh.return_type.has_value());
  CHECK_EQ(*fh.return_type, "Object");
}

DOCTEST_TEST_CASE("subroutine_head")
{
  vb6_ast::subHead sh;
  auto [res, sv] = test_grammar(
    "Private Sub my_sub(ByRef str As String, ByVal valid As Boolean)\r\n",
    vb6_grammar::subHead, sh);
  REQUIRE_MESSAGE(res, "stopped at: " << sv);
  CHECK(sv.empty());

  CHECK_EQ(sh.at, vb6_ast::access_type::private_);
  CHECK_EQ(sh.name, "my_sub");
  CHECK_EQ(sh.params.size(), 2);
}

DOCTEST_TEST_CASE("subroutine_head2")
{
  auto str = "Private Sub my_sub(ByRef str As String, ByVal valid As Boolean, Optional ByVal flag As Boolean = True)\r\n"s;
  vb6_ast::subHead sh;
  auto [res, sv] = test_grammar(str, vb6_grammar::subHead, sh);
  REQUIRE_MESSAGE(res, "stopped at: " << sv);
  CHECK(sv.empty());

  CHECK_EQ(sh.at, vb6_ast::access_type::private_);
  CHECK_EQ(sh.name, "my_sub");
  CHECK_EQ(sh.params.size(), 3);
}

DOCTEST_TEST_CASE("subroutine_head_with_optional_params")
{
  vb6_ast::subHead sh;
  auto [res, sv] = test_grammar(
    "Private Sub my_sub(ByRef str As String, Optional ByVal valid As Boolean = false)\r\n",
    vb6_grammar::subHead, sh);
  REQUIRE_MESSAGE(res, "stopped at: " << sv);
  CHECK(sv.empty());

  CHECK_EQ(sh.at, vb6_ast::access_type::private_);
  CHECK_EQ(sh.name, "my_sub");
  REQUIRE_EQ(sh.params.size(), 2);

  CHECK(sh.params[0].qualifier.has_value());
  if (sh.params[0].qualifier)
    CHECK_EQ(sh.params[0].qualifier.get(), vb6_ast::param_qualifier::byref);
  CHECK_EQ(sh.params[0].var.name, "str");
  if(sh.params[0].var.type)
  {
    CHECK_EQ(*sh.params[0].var.type, "String");
  }
  CHECK(!sh.params[0].var.construct);
  CHECK(!sh.params[0].isoptional);
  CHECK(!sh.params[0].defvalue);

  CHECK(sh.params[1].qualifier.has_value());
  if(sh.params[1].qualifier)
  {
    CHECK_EQ(sh.params[1].qualifier.get(), vb6_ast::param_qualifier::byval);
  }
  CHECK_EQ(sh.params[1].var.name, "valid");
  if(sh.params[1].var.type)
  {
    CHECK_EQ(*sh.params[1].var.type, "Boolean");
  }
  CHECK(!sh.params[1].var.construct);
  CHECK(sh.params[1].isoptional);
  CHECK(sh.params[1].defvalue.has_value());
  if(sh.params[0].defvalue)
  {
    CHECK_EQ(boost::get<bool>(*sh.params[0].defvalue), false);
  }
}

DOCTEST_TEST_CASE("property_let_head")
{
  auto str = "Public Property Let Width(ByVal w As Integer)\r\n"s;
  vb6_ast::propertyLetHead ast;
  auto [res, sv] = test_grammar(str, vb6_grammar::property_letHead, ast);
  REQUIRE_MESSAGE(res, "stopped at: " << sv);
  CHECK(sv.empty());

  CHECK_EQ(ast.at, vb6_ast::access_type::public_);
  CHECK_EQ(ast.name, "Width");
  CHECK_EQ(ast.params.size(), 1);
}

DOCTEST_TEST_CASE("property_get_head")
{
  auto str = "Public Property Get Width() As Integer\r\n"s;
  vb6_ast::propertyGetHead ast;
  auto [res, sv] = test_grammar(str, vb6_grammar::property_getHead, ast);
  REQUIRE_MESSAGE(res, "stopped at: " << sv);
  CHECK(sv.empty());

  CHECK_EQ(ast.at, vb6_ast::access_type::public_);
  CHECK_EQ(ast.name, "Width");
  CHECK_EQ(ast.params.size(), 0);
  REQUIRE(ast.return_type);
  CHECK_EQ(*ast.return_type, "Integer");
}

DOCTEST_TEST_CASE("dll_subroutine_declaration")
{
  vb6_ast::externalSub extsub;
  auto str = "Private Declare Sub BeepVB Lib \"kernel32.dll\" Alias \"Beep\" (ByVal time As Long, ByVal xx As Single)\r\n"s;
  auto [res, sv] = test_grammar(str, vb6_grammar::external_sub_decl, extsub);
  REQUIRE_MESSAGE(res, "stopped at: " << sv);
  CHECK(sv.empty());

  CHECK_EQ(extsub.at, vb6_ast::access_type::private_);
  CHECK_EQ(extsub.name, "BeepVB");
  CHECK_EQ(extsub.alias, "Beep");
  CHECK_EQ(extsub.params.size(), 2);
  CHECK_EQ(extsub.lib, "kernel32.dll");
}

DOCTEST_TEST_CASE("dll_function_declaration")
{
  vb6_ast::externalFunction extfunc;
  auto str = "Private Declare Function BeepVB Lib \"kernel32.dll\" Alias \"Beep\" (ByVal time As Long, ByVal xx As Single) As Long\r\n"s;
  auto [res, sv] = test_grammar(str, vb6_grammar::external_function_decl, extfunc);
  REQUIRE_MESSAGE(res, "stopped at: " << sv);
  CHECK(sv.empty());

  CHECK_EQ(extfunc.at, vb6_ast::access_type::private_);
  CHECK_EQ(extfunc.name, "BeepVB");
  CHECK(extfunc.return_type.has_value());
  if(extfunc.return_type)
  {
    CHECK_EQ(*extfunc.return_type, "Long");
  }
  CHECK_EQ(extfunc.alias, "Beep");
  CHECK_EQ(extfunc.params.size(), 2);
  CHECK_EQ(extfunc.lib, "kernel32.dll");
}

DOCTEST_TEST_CASE("identifier_context")
{
  vb6_ast::identifier_context ctx;
  auto [res, sv] = test_grammar("var1.func().pnt1.", vb6_grammar::identifier_context, ctx);
  REQUIRE_MESSAGE(res, "stopped at: " << sv);
  CHECK(sv.empty());

  CHECK(!ctx.leading_dot);
  REQUIRE_EQ(ctx.elements.size(), 3);
  CHECK_EQ(boost::get<string>(ctx.elements[0].get()), "var1");
  auto& tmp = boost::get<x3::forward_ast<vb6_ast::func_call>>(ctx.elements[1].get()).get();
  CHECK_EQ(tmp.func_name, "func");
  CHECK(tmp.params.empty());
  CHECK_EQ(boost::get<string>(ctx.elements[2].get()), "pnt1");
}

DOCTEST_TEST_CASE("decorated_variable")
{
  vb6_ast::decorated_variable dec_var;
  auto [res, sv] = test_grammar("var1.func().pnt1.X", vb6_grammar::decorated_variable, dec_var);
  REQUIRE_MESSAGE(res, "stopped at: " << sv);
  CHECK(sv.empty());

  CHECK(!dec_var.ctx.leading_dot);
  REQUIRE_EQ(dec_var.ctx.elements.size(), 3);
  CHECK_EQ(boost::get<string>(dec_var.ctx.elements[0].get()), "var1");
  auto& tmp = boost::get<x3::forward_ast<vb6_ast::func_call>>(dec_var.ctx.elements[1].get()).get();
  CHECK_EQ(tmp.func_name, "func");
  CHECK(tmp.params.empty());
  CHECK_EQ(boost::get<string>(dec_var.ctx.elements[2].get()), "pnt1");
  CHECK_EQ(dec_var.var, "X");
}

DOCTEST_TEST_CASE("attribute_block")
{
  vb6_ast::STRICT_MODULE_STRUCTURE::module_attributes attrs;
  auto str = "Attribute ModuleName = \"MyForm\"\r\n"
            "Attribute ProgID = \"00-00-00-00\"\r\n"s;
  auto [res, sv] = test_grammar(str, *vb6_grammar::attributeDef, attrs);
  REQUIRE_MESSAGE(res, "stopped at: " << sv);
  CHECK(sv.empty());

  CHECK_EQ(attrs.size(), 2);

  auto it1 = attrs.find("ModuleName");
  CHECK(it1 != attrs.cend());
  if(it1 != attrs.cend())
  {
    CHECK_EQ(it1->second, "MyForm");
  }

  auto it2 = attrs.find("ProgID");
  CHECK(it2 != attrs.cend());
  if(it2 != attrs.cend())
  {
    CHECK_EQ(it2->second, "00-00-00-00");
  }
}

DOCTEST_TEST_CASE("attributes")
{
  vb6_ast::STRICT_MODULE_STRUCTURE::module_attributes ast;
  auto str = R"vb(Attribute ModuleName = "MyForm"
                  Attribute ProgID = "00-00-00-00"
                  )vb"s;
  //auto [res, sv] = test_grammar(str, vb6_grammar::preamble, ast);
  auto [res, sv] = test_grammar(str, *vb6_grammar::attributeDef, ast);
  REQUIRE_MESSAGE(res, "stopped at: " << sv);
  CHECK(sv.empty());

  CHECK_EQ(ast.size(), 2);

  auto const it1 = ast.find("ModuleName");
  CHECK(it1 != ast.cend());
  if(it1 != ast.cend())
  {
    CHECK_EQ(it1->second, "MyForm");
  }

  auto const it2 = ast.find("ProgID");
  CHECK(it2 != ast.cend());
  if(it2 != ast.cend())
  {
    CHECK_EQ(it2->second, "00-00-00-00");
  }
}

DOCTEST_TEST_CASE("options")
{
  vector<vb6_ast::module_option> ast;
  auto str = R"vb(Option Explicit
                  Option Base 0
                  )vb"s;
  auto [res, sv] = test_grammar(str, *vb6_grammar::option_item, ast);
  REQUIRE_MESSAGE(res, "stopped at: " << sv);
  CHECK(sv.empty());

  REQUIRE_EQ(ast.size(), 2);

  CHECK_EQ(ast[0], vb6_ast::module_option::explicit_);
  CHECK_EQ(ast[1], vb6_ast::module_option::base_0);
}

#if 0
DOCTEST_TEST_CASE("declaration_block")
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
  auto [res, sv] = test_grammar(str, declarations, decls);
  REQUIRE_MESSAGE(res, "stopped at: " << sv);
  CHECK(sv.empty());

  REQUIRE_EQ(decls.size(), 5);

  REQUIRE_NOTHROW([ast](){boost::get<vb6_ast::const_var_stat>(decls[0].get();}));
  REQUIRE_NOTHROW([ast](){boost::get<vb6_ast::global_var_decls>(decls[1].get();}));
  REQUIRE_NOTHROW([ast](){boost::get<vb6_ast::externalSub>(decls[2].get();}));
  REQUIRE_NOTHROW([ast](){boost::get<vb6_ast::vb_enum>(decls[3].get();}));
  REQUIRE_NOTHROW([ast](){boost::get<vb6_ast::record>(decls[4].get();})));
}

DOCTEST_TEST_CASE("bas_unit_STRICT_MODULE_STRUCTURE")
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
  auto [res, sv] = test_grammar(str, vb6_grammar::STRICT_MODULE_STRUCTURE::basModDef, ast);
  REQUIRE_MESSAGE(res, "stopped at: " << sv);
  CHECK(sv.empty());

  vb6_ast_printer P(cout);
  P(ast);

  CHECK_EQ(ast.attrs.size(), 2);

  auto const it1 = ast.attrs.find("ModuleName");
  EXPECT_NE(it1, ast.attrs.cend());
  if(it1 != ast.attrs.cend())
  {
    CHECK_EQ(it1->second, "MyForm");
  }

  auto const it2 = ast.attrs.find("ProgID");
  EXPECT_NE(it2, ast.attrs.cend());
  if(it2 != ast.attrs.cend())
  {
    CHECK_EQ(it2->second, "00-00-00-00");
  }

  CHECK_EQ(ast.opts.items.size(), 4);
  if(ast.opts.items.size() == 4)
  {
#if 0
    CHECK_EQ(ast.opts.items[0], vb6_ast::module_option::explicit_);
    CHECK_EQ(ast.opts.items[1], vb6_ast::module_option::base_0);
#else
    CHECK_EQ(boost::get<vb6_ast::module_option>(ast.opts.items[0].get()), vb6_ast::module_option::explicit_);
    CHECK_EQ(boost::get<vb6_ast::module_option>(ast.opts.items[1].get()), vb6_ast::module_option::base_0);
#endif
    REQUIRE_NOTHROW([ast](){boost::get<vb6_ast::empty_line>(ast.opts.items[2].get();}));
    REQUIRE_NOTHROW([ast](){boost::get<vb6_ast::lonely_comment>(ast.opts.items[3].get();}));
  }

  CHECK_EQ(ast.declarations.size(), 3);
  if(ast.declarations.size() == 3)
  {
    REQUIRE_NOTHROW([ast](){boost::get<vb6_ast::const_var_stat>(ast.declarations[0].get();}));
    REQUIRE_NOTHROW([ast](){boost::get<vb6_ast::global_var_decls>(ast.declarations[1].get();}));
    REQUIRE_NOTHROW([ast](){boost::get<vb6_ast::vb_enum>(ast.declarations[2].get();}));
  }

  CHECK_EQ(ast.functions.size(), 2);
  if(ast.functions.size() == 2)
  {
    REQUIRE_NOTHROW([ast](){boost::get<vb6_ast::subDef>(ast.functions[0].get();})));
    REQUIRE_NOTHROW([ast](){boost::get<vb6_ast::functionDef>(ast.functions[1].get();})));
  }
}
#endif

DOCTEST_TEST_CASE("bas_unit")
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
  auto [res, sv] = test_grammar(str, vb6_grammar::basModDef, ast);
  REQUIRE_MESSAGE(res, "stopped at: " << sv);
  CHECK(sv.empty());

  vb6_ast_printer P(cout);
  P(ast);

  REQUIRE_EQ(ast.size(), 11);

  CHECK_EQ(boost::get<vb6_ast::module_attribute>(ast[0].get()).first, "ModuleName");
  CHECK_EQ(boost::get<vb6_ast::module_attribute>(ast[1].get()).first, "ProgID");

  CHECK_EQ(boost::get<vb6_ast::module_option>(ast[2].get()), vb6_ast::module_option::explicit_);
  CHECK_EQ(boost::get<vb6_ast::module_option>(ast[3].get()), vb6_ast::module_option::base_0);

  REQUIRE_NOTHROW([ast](){boost::get<vb6_ast::empty_line>(ast[4].get());});
  REQUIRE_NOTHROW([ast](){boost::get<vb6_ast::lonely_comment>(ast[5].get());});

  REQUIRE_NOTHROW([ast](){boost::get<vb6_ast::const_var_stat>(boost::get<vb6_ast::declaration>(ast[6].get()));});
  REQUIRE_NOTHROW([ast](){boost::get<vb6_ast::global_var_decls>(boost::get<vb6_ast::declaration>(ast[7].get()));});
  REQUIRE_NOTHROW([ast](){boost::get<vb6_ast::vb_enum>(boost::get<vb6_ast::declaration>(ast[8].get()));});

  REQUIRE_NOTHROW([ast](){boost::get<vb6_ast::subDef>(ast[9].get());});
  REQUIRE_NOTHROW([ast](){boost::get<vb6_ast::functionDef>(ast[10].get());});
}

DOCTEST_TEST_CASE("trailing_comment")
{
  auto str = "Global g_var As Long ' how can we catch a trailing comment?\r\n"s;
  vb6_ast::global_var_decls vars;
  auto [res, sv] = test_grammar(str, vb6_grammar::global_var_declaration, vars);
  REQUIRE_MESSAGE(res, "stopped at: " << sv);
  CHECK(sv.empty());

  CHECK_EQ(vars.at, vb6_ast::access_type::global);
  CHECK(!vars.with_events);
  REQUIRE_EQ(vars.vars.size(), 1);

  CHECK_EQ(vars.vars[0].name, "g_var");
  CHECK(!vars.vars[0].construct);
  CHECK(vars.vars[0].type.has_value());
  if(vars.vars[0].type)
  {
    CHECK_EQ(*vars.vars[0].type, "Long");
  }
}

DOCTEST_TEST_SUITE_END();

int main(int argc, char* argv[])
{
  log_compiler_info(cout);

  doctest::Context context;
  context.applyCommandLine(argc, argv);

  // overrides
  context.setOption("no-breaks", true); // don't break in the debugger when assertions fail

  return context.run();
}
