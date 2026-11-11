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

TEST_CASE("log_compiler_info", "[info]")
{
  SKIP();
  // how to tell that AppleClang and not plain Clang has been used?
  // __APPLE_CC__ does not work as it's always defined
#if defined(__clang__)
  INFO("Compiler: Clang " << __clang_major__ << "." << __clang_minor__ << "." << __clang_patchlevel__);
#elif defined(__GNUC__)
  INFO("Compiler: GCC " << __GNUC__ << "." << __GNUC_MINOR__ << "." << __GNUC_PATCHLEVEL__);
#elif defined(__MSC_VER)
  INFO("Compiler: MSC " << __MSC_VER);
#endif
}

TEST_CASE("lonely_comment", "[vb6_parser_simple]")
{
  std::vector<vb6_ast::lonely_comment> ast;
  auto [res, sv] = test_grammar(
    "' This is comment line 1\r\n' Comment line 2\r\n",
    *vb6_grammar::lonely_comment, ast);
  INFO("stopped at: " << sv);
  REQUIRE(res);
  CHECK(sv.empty());

  REQUIRE(ast.size() == 2);

  CHECK(ast[0].content == " This is comment line 1");
  CHECK(ast[1].content == " Comment line 2");
}

TEST_CASE("empty_lines", "[vb6_parser_simple]")
{
  std::vector<
    boost::variant<vb6_ast::empty_line, vb6_ast::lonely_comment>
  > ast;
  // critical to have lonely_comment first in the parsing rule
  auto const G = *(vb6_grammar::lonely_comment | vb6_grammar::empty_line);
  auto str = "' comment1\r\n"
            "\r\n"
            "' comment2\r\n";
  auto [res, sv] = test_grammar(str, G, ast);
  INFO("stopped at: " << sv);
  REQUIRE(res);
  CHECK(sv.empty());

  REQUIRE(ast.size() == 3);

  CHECK(boost::get<vb6_ast::lonely_comment>(ast[0]).content == " comment1");
  REQUIRE_NOTHROW(boost::get<vb6_ast::empty_line>(ast[1]));
  CHECK(boost::get<vb6_ast::lonely_comment>(ast[2]).content == " comment2");
}

TEST_CASE("quoted_string", "[vb6_parser_simple]")
{
  std::string str;
  auto [res, sv] = test_grammar("\"Quoted string.\"", vb6_grammar::quoted_string, str);
  INFO("stopped at: " << sv);
  REQUIRE(res);
  CHECK(sv.empty());
  CHECK(str == "Quoted string.");
}

TEST_CASE("basic_identifier", "[vb6_parser_simple]")
{
  std::string id;
  auto [res, sv] = test_grammar("iden_tifier", vb6_grammar::basic_identifier, id);
  INFO("stopped at: " << sv);
  REQUIRE(res);
  CHECK(sv.empty());
  CHECK(id == "iden_tifier");
}

TEST_CASE("var_identifier", "[vb6_parser_simple]")
{
  std::string var;

  SECTION("1")
  {
    auto [res, sv] = test_grammar("g_logger", vb6_grammar::var_identifier, var);
    INFO("stopped at: " << sv);
    REQUIRE(res);
    CHECK(sv.empty());
    CHECK(var == "g_logger");
  }

  SECTION("2")
  {
    auto [res, sv] = test_grammar("forth ", vb6_grammar::sub_identifier, var);
    INFO("stopped at: " << sv);
    REQUIRE(res);
    CHECK(sv.empty());
    CHECK(var == "forth");
  }

  SECTION("3")
  {
    std::string var;
    auto [res, sv] = test_grammar("subroutine ", vb6_grammar::sub_identifier, var);
    INFO("stopped at: " << sv);
    REQUIRE(res);
    CHECK(sv.empty());
    CHECK(var == "subroutine");
  }

  /*
  std::string code = "sub ";
  std::string_view code_sv = code;
  auto it1 = cbegin(code_sv);
  auto const it2 = cend(code_sv);
  CHECK(!boost::spirit::x3::phrase_parse(it1, it2, vb6_grammar::sub_identifier, vb6_grammar::skip, var));
  CHECK(it1 == begin(code_sv));
  */
}

TEST_CASE("type_identifier", "[vb6_parser_simple]")
{
  vb6_ast::type_identifier type;
  auto [res, sv] = test_grammar("lOnG", vb6_grammar::type_identifier, type);
  INFO("stopped at: " << sv);
  REQUIRE(res);
  CHECK(sv.empty());
  CHECK(type.value.get().type() == typeid(vb6_ast::NativeType));
  CHECK(boost::get<vb6_ast::NativeType>(type.value.get()) == vb6_ast::NativeType::Long);

  auto [any_res, any_sv] = test_grammar("Any", vb6_grammar::type_identifier, type);
  INFO("stopped at: " << any_sv);
  REQUIRE(any_res);
  CHECK(any_sv.empty());
  CHECK(boost::get<vb6_ast::NativeType>(type.value.get()) == vb6_ast::NativeType::Any);
}

TEST_CASE("complex_type_identifier", "[vb6_parser_simple]")
{
  vb6_ast::complex_type_identifier type;
  auto [res, sv] = test_grammar("vB.fOrm", vb6_grammar::complex_type_identifier, type);
  INFO("stopped at: " << sv);
  REQUIRE(res);
  CHECK(sv.empty());
  REQUIRE(type.library_or_module.has_value());
  CHECK(*type.library_or_module == "vB");
  CHECK(type.name == "fOrm");
}

TEST_CASE("const_expression_non_numeric", "[vb6_parser_simple]")
{
  vb6_ast::const_expr ast;
  std::string_view str;

  SECTION("1")
  {
    str = "\"una stringa\"";
    auto [res, sv] = test_grammar(str, vb6_grammar::const_expression, ast);
    INFO("stopped at: " << sv);
    REQUIRE(res);
    CHECK(sv.empty());
    CHECK(boost::get<vb6_ast::quoted_string>(ast.get()) == "una stringa");
  }

  SECTION("2")
  {
    str = "True";
    auto [res, sv] = test_grammar(str, vb6_grammar::const_expression, ast);
    INFO("stopped at: " << sv);
    REQUIRE(res);
    CHECK(sv.empty());
    CHECK(boost::get<bool>(ast.get()));
  }

  SECTION("3")
  {
    str = "Nothing";
    auto [res, sv] = test_grammar(str, vb6_grammar::const_expression, ast);
    INFO("stopped at: " << sv);
    REQUIRE(res);
    CHECK(sv.empty());
    REQUIRE_NOTHROW(boost::get<vb6_ast::nothing>(ast.get()));
  }
}

TEST_CASE("const_expression_integers", "[vb6_parser_simple]")
{
  vb6_ast::const_expr ast;
  std::string str;

  SECTION("1")
  {
    str = "1234%";
    auto [res, sv] = test_grammar(str, vb6_grammar::const_expression, ast);
    INFO("stopped at: " << sv);
    REQUIRE(res);
    CHECK(sv.empty());
    CHECK(boost::get<vb6_ast::integer_dec>(ast.get()).val == 1234);
  }

  SECTION("2")
  {
    str = "1234&";
    auto [res, sv] = test_grammar(str, vb6_grammar::const_expression, ast);
    INFO("stopped at: " << sv);
    REQUIRE(res);
    CHECK(sv.empty());
    CHECK(boost::get<vb6_ast::long_dec>(ast.get()).val == 1234);
  }

  SECTION("3")
  {
    str = "&Hcafedead&";
    auto [res, sv] = test_grammar(str, vb6_grammar::const_expression, ast);
    INFO("stopped at: " << sv);
    REQUIRE(res);
    CHECK(sv.empty());
    CHECK(boost::get<vb6_ast::long_hex>(ast.get()).val == int(0xcafedead));
  }

  SECTION("4")
  {
    str = "&01234&";
    auto [res, sv] = test_grammar(str, vb6_grammar::const_expression, ast);
    INFO("stopped at: " << sv);
    REQUIRE(res);
    CHECK(sv.empty());
    CHECK(boost::get<vb6_ast::long_oct>(ast.get()).val == 01234);
  }

  SECTION("5")
  {
    str = "1234";
    auto [res, sv] = test_grammar(str, vb6_grammar::const_expression, ast);
    INFO("stopped at: " << sv);
    REQUIRE(res);
    CHECK(sv.empty());
    CHECK(boost::get<vb6_ast::integer_dec>(ast.get()).val == 1234);
  }
}

TEST_CASE("const_expression_floats", "[vb6_parser_simple]")
{
  vb6_ast::const_expr ast;
  std::string_view str;

  SECTION("1")
  {
    str = "1234!";
    auto [res, sv] = test_grammar(str, vb6_grammar::const_expression, ast);
    INFO("stopped at: " << sv);
    REQUIRE(res);
    CHECK(sv.empty());
    CHECK(boost::get<float>(ast.get()) == 1234.0f);
  }

  SECTION("2")
  {
    str = "1234#";
    auto [res, sv] = test_grammar(str, vb6_grammar::const_expression, ast);
    INFO("stopped at: " << sv);
    REQUIRE(res);
    CHECK(sv.empty());
    CHECK(boost::get<double>(ast.get()) == 1234.0);
  }

  SECTION("1")
  {
    str = "2.8";
    auto [res, sv] = test_grammar(str, vb6_grammar::const_expression, ast);
    INFO("stopped at: " << sv);
    REQUIRE(res);
    CHECK(sv.empty());
    CHECK(boost::get<float>(ast.get()) == 2.8f);
  }
}

TEST_CASE("sample_expression", "[vb6_parser_simple]")
{
  vb6_ast::expression ast;
  auto [res, sv] = test_grammar("foo1(foo2(3, M.x_coord), True)", vb6_grammar::expression, ast);
  INFO("stopped at: " << sv);
  REQUIRE(res);
  CHECK(sv.empty());
  CHECK(ast.get().type() == typeid(x3::forward_ast<vb6_ast::func_call>));
  CHECK(boost::get<x3::forward_ast<vb6_ast::func_call>>(ast.get()).get().func_name == "foo1");
}

TEST_CASE("single_var_declaration", "[vb6_parser_simple]")
{
  vb6_ast::variable P1;
  auto [res, sv] = test_grammar("g_logger As Long", vb6_grammar::single_var_declaration, P1);
  INFO("stopped at: " << sv);
  REQUIRE(res);
  CHECK(sv.empty());
  CHECK(P1.name == "g_logger");
  CHECK(!P1.construct);
  CHECK(P1.type.has_value());
  if(P1.type)
  {
    CHECK(boost::get<vb6_ast::NativeType>(P1.type->value.get()) == vb6_ast::NativeType::Long);
  }
}

TEST_CASE("single_var_declaration2", "[vb6_parser_simple]")
{
  vb6_ast::variable P2;
  auto [res, sv] = test_grammar("name As New String", vb6_grammar::single_var_declaration, P2);
  INFO("stopped at: " << sv);
  REQUIRE(res);
  CHECK(sv.empty());
  CHECK(P2.name == "name");
  CHECK(P2.construct);
  CHECK(P2.type.has_value());
  if(P2.type)
  {
    CHECK(boost::get<vb6_ast::NativeType>(P2.type->value.get()) == vb6_ast::NativeType::String);
  }
}

TEST_CASE("record_declaration", "[vb6_parser_tests]")
{
  vb6_ast::record rec;
  auto [res, sv] = test_grammar("Type PatRecord\r\n  name As String\r\n  age As Integer\r\nEnd Type\r\n",
              vb6_grammar::record_declaration, rec);
  INFO("stopped at: " << sv);
  REQUIRE(res);
  CHECK(sv.empty());

  CHECK(rec.name == "PatRecord");
  CHECK(rec.at == vb6_ast::access_type::na);
  REQUIRE(rec.members.size() == 2);

  CHECK(rec.members[0].name == "name");
  CHECK(rec.members[0].type.has_value());
  if(rec.members[0].type)
  {
    CHECK(boost::get<vb6_ast::NativeType>(rec.members[0].type->value.get()) == vb6_ast::NativeType::String);
  }
  CHECK(!rec.members[0].construct);

  CHECK(rec.members[1].name == "age");
  CHECK(rec.members[1].type.has_value());
  if(rec.members[1].type)
  {
    CHECK(boost::get<vb6_ast::NativeType>(rec.members[1].type->value.get()) == vb6_ast::NativeType::Integer);
  }
  CHECK(!rec.members[1].construct);
}

TEST_CASE("enum_declaration", "[vb6_parser_tests]")
{
  vb6_ast::vb_enum enum1;
  auto [res, sv] = test_grammar("Enum PatTypes\r\n  inpatient\r\n  outpatient\r\nEnd Enum\r\n",
              vb6_grammar::enum_declaration,  enum1);
  INFO("stopped at: " << sv);
  REQUIRE(res);
  CHECK(sv.empty());

  CHECK(enum1.name == "PatTypes");
  CHECK(enum1.at == vb6_ast::access_type::na);
  REQUIRE(enum1.values.size() == 2);

  CHECK(enum1.values[0].first == "inpatient");
  CHECK(!enum1.values[0].second);

  CHECK(enum1.values[1].first == "outpatient");
  CHECK(!enum1.values[1].second);
}

TEST_CASE("global_var_declaration", "[vb6_parser_tests]")
{
  auto str = "Global g_logger As Long, v1, XRes As New Object, ptr As Module.MyRec, g_active As Boolean\r\n";
  vb6_ast::global_var_decls vars;
  auto [res, sv] = test_grammar(str, vb6_grammar::global_var_declaration, vars);
  INFO("stopped at: " << sv);
  REQUIRE(res);
  CHECK(sv.empty());

  CHECK(vars.at == vb6_ast::access_type::global);
  CHECK(!vars.with_events);
  REQUIRE(vars.vars.size() == 5);

  CHECK(vars.vars[0].name == "g_logger");
  CHECK(!vars.vars[0].construct);
  CHECK(vars.vars[0].type.has_value());
  if(vars.vars[0].type)
  {
    CHECK(boost::get<vb6_ast::NativeType>(vars.vars[0].type->value.get()) == vb6_ast::NativeType::Long);
  }

  CHECK(vars.vars[1].name == "v1");
  CHECK(!vars.vars[1].construct);
  CHECK(!vars.vars[1].type.has_value());

  CHECK(vars.vars[2].name == "XRes");
  CHECK(vars.vars[2].construct);
  CHECK(vars.vars[2].type.has_value());
  if(vars.vars[2].type)
  {
    CHECK(boost::get<vb6_ast::NativeType>(vars.vars[2].type->value.get()) == vb6_ast::NativeType::Object);
  }

  CHECK(vars.vars[3].name == "ptr");
  CHECK(!vars.vars[3].construct);
  CHECK(vars.vars[3].type.has_value());
  if(vars.vars[3].type)
  {
    auto const& user_type =
      boost::get<vb6_ast::complex_type_identifier>(vars.vars[3].type->value.get());
    REQUIRE(user_type.library_or_module.has_value());
    CHECK(*user_type.library_or_module == "Module");
    CHECK(user_type.name == "MyRec");
  }

  CHECK(vars.vars[4].name == "g_active");
  CHECK(!vars.vars[4].construct);
  CHECK(vars.vars[4].type.has_value());
  if(vars.vars[4].type)
  {
    CHECK(boost::get<vb6_ast::NativeType>(vars.vars[4].type->value.get()) == vb6_ast::NativeType::Boolean);
  }
}

TEST_CASE("const_var_declaration1", "[vb6_parser_tests]")
{
  auto cstr = "Const e As Single = 2.8, pi As Double = 3.14, u As Integer = -1\r\n";
  vb6_ast::const_var_stat cvars;
  auto [res, sv] = test_grammar(cstr, vb6_grammar::const_var_declaration, cvars);
  INFO("stopped at: " << sv);
  REQUIRE(res);
  CHECK(sv.empty());

  REQUIRE(cvars.size() == 3);

  CHECK(cvars[0].var.name == "e");
  if(cvars[0].var.type)
  {
    CHECK(boost::get<vb6_ast::NativeType>(cvars[0].var.type->value.get()) == vb6_ast::NativeType::Single);
  }
  CHECK(!cvars[0].var.construct);
  CHECK(boost::get<float>(cvars[0].value.get()) == 2.8f);

  CHECK(cvars[1].var.name == "pi");
  if(cvars[1].var.type)
  {
    CHECK(boost::get<vb6_ast::NativeType>(cvars[1].var.type->value.get()) == vb6_ast::NativeType::Double);
  }
  CHECK(!cvars[1].var.construct);
  CHECK(boost::get<float>(cvars[1].value.get()) == 3.14f);

  CHECK(cvars[2].var.name == "u");
  if(cvars[2].var.type)
  {
    CHECK(boost::get<vb6_ast::NativeType>(cvars[2].var.type->value.get()) == vb6_ast::NativeType::Integer);
  }
  CHECK(!cvars[2].var.construct);
  CHECK(boost::get<vb6_ast::integer_dec>(cvars[2].value.get()).val == -1);
}

TEST_CASE("const_var_declaration2", "[vb6_parser_tests]")
{
  vb6_ast::const_var_stat cvars;
  auto [res, sv] = test_grammar("Private Const PI As Double = 3.1415\r\n",
              vb6_grammar::const_var_declaration,  cvars);
  INFO("stopped at: " << sv);
  REQUIRE(res);
  CHECK(sv.empty());

  REQUIRE(cvars.size() == 1);

  CHECK(cvars[0].var.name == "PI");
  if(cvars[0].var.type)
  {
    CHECK(boost::get<vb6_ast::NativeType>(cvars[0].var.type->value.get()) == vb6_ast::NativeType::Double);
  }
  CHECK(!cvars[0].var.construct);
  CHECK(boost::get<float>(cvars[0].value.get()) == 3.1415f);
}

TEST_CASE("param_decl", "[vb6_parser_tests]")
{
  vb6_ast::func_param fp;
  auto [res, sv] = test_grammar("Optional ByVal name As String = \"pippo\"", vb6_grammar::param_decl, fp);
  INFO("stopped at: " << sv);
  REQUIRE(res);
  CHECK(sv.empty());

  CHECK(fp.isoptional);
  CHECK(fp.qualifier.has_value());
  if(fp.qualifier)
  {
    CHECK(*fp.qualifier == vb6_ast::param_qualifier::byval);
  }
  CHECK(fp.var.name == "name");
  CHECK(!fp.var.construct);
  CHECK(fp.var.type.has_value());
  if(fp.var.type)
  {
    CHECK(boost::get<vb6_ast::NativeType>(fp.var.type->value.get()) == vb6_ast::NativeType::String);
  }
  CHECK(fp.defvalue.has_value());
  if(fp.defvalue)
  {
    CHECK(boost::get<vb6_ast::quoted_string>(fp.defvalue.get()) == "pippo");
  }
}

TEST_CASE("param_list_decl", "[vb6_parser_tests]")
{
  std::vector<vb6_ast::func_param> fps;
  auto [res, sv] = test_grammar("ByVal name As String, ByRef val As Integer",
              -(vb6_grammar::param_decl % ','), fps);
  INFO("stopped at: " << sv);
  REQUIRE(res);
  CHECK(sv.empty());

  REQUIRE(fps.size() == 2);

  CHECK(!fps[0].isoptional);
  CHECK(fps[0].qualifier.has_value());
  if(fps[0].qualifier)
  {
    CHECK(*fps[0].qualifier == vb6_ast::param_qualifier::byval);
  }
  CHECK(fps[0].var.name == "name");
  CHECK(!fps[0].var.construct);
  CHECK(fps[0].var.type.has_value());
  if(fps[0].var.type)
  {
    CHECK(boost::get<vb6_ast::NativeType>(fps[0].var.type->value.get()) == vb6_ast::NativeType::String);
  }
  CHECK(!fps[0].defvalue);

  CHECK(!fps[1].isoptional);
  CHECK(fps[1].qualifier.has_value());
  if(fps[1].qualifier)
  {
    CHECK(*fps[1].qualifier == vb6_ast::param_qualifier::byref);
  }
  CHECK(fps[1].var.name == "val");
  CHECK(!fps[1].var.construct);
  CHECK(fps[1].var.type.has_value());
  if(fps[1].var.type)
  {
    CHECK(boost::get<vb6_ast::NativeType>(fps[1].var.type->value.get()) == vb6_ast::NativeType::Integer);
  }
  CHECK(!fps[1].defvalue);
}

TEST_CASE("event_declaration", "[vb6_parser_tests]")
{
  vb6_ast::eventHead event_decl;
  auto [res, sv] = test_grammar("Public Event OnChange(ByVal Text As String)\r\n",
               vb6_grammar::eventHead, event_decl);
  INFO("stopped at: " << sv);
  REQUIRE(res);
  CHECK(sv.empty());

  CHECK(event_decl.at == vb6_ast::access_type::public_);
  CHECK(event_decl.name == "OnChange");
  CHECK(event_decl.params.size() == 1);
}

TEST_CASE("function_head", "[vb6_parser_tests]")
{
  vb6_ast::functionHead fh;
  auto [res, sv] = test_grammar(
    "Private Function OneFunc(ByVal name As String, ByRef val As Integer) As Integer\r\n",
    vb6_grammar::functionHead, fh);
  INFO("stopped at: " << sv);
  REQUIRE(res);
  CHECK(sv.empty());

  CHECK(fh.at == vb6_ast::access_type::private_);
  CHECK(fh.name == "OneFunc");
  CHECK(fh.params.size() == 2);
  REQUIRE(fh.return_type.has_value());
  CHECK(boost::get<vb6_ast::NativeType>(fh.return_type->value.get()) == vb6_ast::NativeType::Integer);
}

TEST_CASE("function_head_no_params", "[vb6_parser_tests]")
{
  vb6_ast::functionHead fh;
  auto [res, sv] = test_grammar(
    "Private Function NoParamFunc() As Object\r\n", vb6_grammar::functionHead, fh);
  INFO("stopped at: " << sv);
  REQUIRE(res);
  CHECK(sv.empty());

  CHECK(fh.at == vb6_ast::access_type::private_);
  CHECK(fh.name == "NoParamFunc");
  CHECK(fh.params.empty());
  REQUIRE(fh.return_type.has_value());
  CHECK(boost::get<vb6_ast::NativeType>(fh.return_type->value.get()) == vb6_ast::NativeType::Object);
}

TEST_CASE("function_head_qualified_return_type", "[vb6_parser_tests]")
{
  vb6_ast::functionHead fh;
  auto [res, sv] = test_grammar(
    "Private Function GetForm() As vB.fOrm\r\n", vb6_grammar::functionHead, fh);
  INFO("stopped at: " << sv);
  REQUIRE(res);
  CHECK(sv.empty());
  REQUIRE(fh.return_type.has_value());

  auto const& return_type =
    boost::get<vb6_ast::complex_type_identifier>(fh.return_type->value.get());
  REQUIRE(return_type.library_or_module.has_value());
  CHECK(*return_type.library_or_module == "vB");
  CHECK(return_type.name == "fOrm");
}

TEST_CASE("subroutine_head", "[vb6_parser_tests]")
{
  vb6_ast::subHead sh;
  auto [res, sv] = test_grammar(
    "Private Sub my_sub(ByRef str As String, ByVal valid As Boolean)\r\n",
    vb6_grammar::subHead, sh);
  INFO("stopped at: " << sv);
  REQUIRE(res);
  CHECK(sv.empty());

  CHECK(sh.at == vb6_ast::access_type::private_);
  CHECK(sh.name == "my_sub");
  CHECK(sh.params.size() == 2);
}

TEST_CASE("subroutine_head2", "[vb6_parser_tests]")
{
  auto str = "Private Sub my_sub(ByRef str As String, ByVal valid As Boolean, Optional ByVal flag As Boolean = True)\r\n";
  vb6_ast::subHead sh;
  auto [res, sv] = test_grammar(str, vb6_grammar::subHead, sh);
  INFO("stopped at: " << sv);
  REQUIRE(res);
  CHECK(sv.empty());

  CHECK(sh.at == vb6_ast::access_type::private_);
  CHECK(sh.name == "my_sub");
  CHECK(sh.params.size() == 3);
}

TEST_CASE("subroutine_head_with_optional_params", "[vb6_parser_tests]")
{
  vb6_ast::subHead sh;
  auto [res, sv] = test_grammar(
    "Private Sub my_sub(ByRef str As String, Optional ByVal valid As Boolean = false)\r\n",
    vb6_grammar::subHead, sh);
  INFO("stopped at: " << sv);
  REQUIRE(res);
  CHECK(sv.empty());

  CHECK(sh.at == vb6_ast::access_type::private_);
  CHECK(sh.name == "my_sub");
  REQUIRE(sh.params.size() == 2);

  CHECK(sh.params[0].qualifier.has_value());
  if (sh.params[0].qualifier)
    CHECK(sh.params[0].qualifier.get() == vb6_ast::param_qualifier::byref);
  CHECK(sh.params[0].var.name == "str");
  if(sh.params[0].var.type)
  {
    CHECK(boost::get<vb6_ast::NativeType>(sh.params[0].var.type->value.get()) == vb6_ast::NativeType::String);
  }
  CHECK(!sh.params[0].var.construct);
  CHECK(!sh.params[0].isoptional);
  CHECK(!sh.params[0].defvalue);

  CHECK(sh.params[1].qualifier.has_value());
  if(sh.params[1].qualifier)
  {
    CHECK(sh.params[1].qualifier.get() == vb6_ast::param_qualifier::byval);
  }
  CHECK(sh.params[1].var.name == "valid");
  if(sh.params[1].var.type)
  {
    CHECK(boost::get<vb6_ast::NativeType>(sh.params[1].var.type->value.get()) == vb6_ast::NativeType::Boolean);
  }
  CHECK(!sh.params[1].var.construct);
  CHECK(sh.params[1].isoptional);
  CHECK(sh.params[1].defvalue.has_value());
  if(sh.params[0].defvalue)
  {
    CHECK(boost::get<bool>(*sh.params[0].defvalue) == false);
  }
}

TEST_CASE("property_let_head", "[vb6_parser_tests]")
{
  auto str = "Public Property Let Width(ByVal w As Integer)\r\n";
  vb6_ast::propertyLetHead ast;
  auto [res, sv] = test_grammar(str, vb6_grammar::property_letHead, ast);
  INFO("stopped at: " << sv);
  REQUIRE(res);
  CHECK(sv.empty());

  CHECK(ast.at == vb6_ast::access_type::public_);
  CHECK(ast.name == "Width");
  CHECK(ast.params.size() == 1);
}

TEST_CASE("property_get_head", "[vb6_parser_tests]")
{
  auto str = "Public Property Get Width() As Integer\r\n";
  vb6_ast::propertyGetHead ast;
  auto [res, sv] = test_grammar(str, vb6_grammar::property_getHead, ast);
  INFO("stopped at: " << sv);
  REQUIRE(res);
  CHECK(sv.empty());

  CHECK(ast.at == vb6_ast::access_type::public_);
  CHECK(ast.name == "Width");
  CHECK(ast.params.size() == 0);
  REQUIRE(ast.return_type.has_value());
  CHECK(boost::get<vb6_ast::NativeType>(ast.return_type->value.get()) == vb6_ast::NativeType::Integer);
}

TEST_CASE("dll_subroutine_declaration", "[vb6_parser_tests]")
{
  vb6_ast::externalSub extsub;
  auto str = "Private Declare Sub BeepVB Lib \"kernel32.dll\" Alias \"Beep\" (ByVal time As Long, ByVal xx As Single)\r\n";
  auto [res, sv] = test_grammar(str, vb6_grammar::external_sub_decl, extsub);
  INFO("stopped at: " << sv);
  REQUIRE(res);
  CHECK(sv.empty());

  CHECK(extsub.at == vb6_ast::access_type::private_);
  CHECK(extsub.name == "BeepVB");
  CHECK(extsub.alias == "Beep");
  CHECK(extsub.params.size() == 2);
  CHECK(extsub.lib == "kernel32.dll");
}

TEST_CASE("dll_function_declaration", "[vb6_parser_tests]")
{
  vb6_ast::externalFunction extfunc;
  auto str = "Private Declare Function BeepVB Lib \"kernel32.dll\" Alias \"Beep\" (ByVal time As Long, ByVal xx As Single) As Long\r\n";
  auto [res, sv] = test_grammar(str, vb6_grammar::external_function_decl, extfunc);
  INFO("stopped at: " << sv);
  REQUIRE(res);
  CHECK(sv.empty());

  CHECK(extfunc.at == vb6_ast::access_type::private_);
  CHECK(extfunc.name == "BeepVB");
  CHECK(extfunc.return_type.has_value());
  if(extfunc.return_type)
  {
    CHECK(boost::get<vb6_ast::NativeType>(extfunc.return_type->value.get()) == vb6_ast::NativeType::Long);
  }
  CHECK(extfunc.alias == "Beep");
  CHECK(extfunc.params.size() == 2);
  CHECK(extfunc.lib == "kernel32.dll");
}

TEST_CASE("identifier_context", "[vb6_parser_tests]")
{
  vb6_ast::identifier_context ctx;
  auto [res, sv] = test_grammar("var1.func().pnt1.", vb6_grammar::identifier_context, ctx);
  INFO("stopped at: " << sv);
  REQUIRE(res);
  CHECK(sv.empty());

  CHECK(!ctx.leading_dot);
  REQUIRE(ctx.elements.size() == 3);
  CHECK(boost::get<std::string>(ctx.elements[0].get()) == "var1");
  auto& tmp = boost::get<x3::forward_ast<vb6_ast::func_call>>(ctx.elements[1].get()).get();
  CHECK(tmp.func_name == "func");
  CHECK(tmp.params.empty());
  CHECK(boost::get<std::string>(ctx.elements[2].get()) == "pnt1");
}

TEST_CASE("decorated_variable", "[vb6_parser_tests]")
{
  vb6_ast::decorated_variable dec_var;
  auto [res, sv] = test_grammar("var1.func().pnt1.X", vb6_grammar::decorated_variable, dec_var);
  INFO("stopped at: " << sv);
  REQUIRE(res);
  CHECK(sv.empty());

  CHECK(!dec_var.ctx.leading_dot);
  REQUIRE(dec_var.ctx.elements.size() == 3);
  CHECK(boost::get<std::string>(dec_var.ctx.elements[0].get()) == "var1");
  auto& tmp = boost::get<x3::forward_ast<vb6_ast::func_call>>(dec_var.ctx.elements[1].get()).get();
  CHECK(tmp.func_name == "func");
  CHECK(tmp.params.empty());
  CHECK(boost::get<std::string>(dec_var.ctx.elements[2].get()) == "pnt1");
  CHECK(dec_var.var == "X");
}

TEST_CASE("attribute_block", "[vb6_parser_tests]")
{
  vb6_ast::STRICT_MODULE_STRUCTURE::module_attributes attrs;
  auto str = "Attribute ModuleName = \"MyForm\"\r\n"
             "Attribute ProgID = \"00-00-00-00\"\r\n";
  auto [res, sv] = test_grammar(str, *vb6_grammar::attributeDef, attrs);
  INFO("stopped at: " << sv);
  REQUIRE(res);
  CHECK(sv.empty());

  CHECK(attrs.size() == 2);

  auto it1 = attrs.find("ModuleName");
  CHECK(it1 != attrs.cend());
  if(it1 != attrs.cend())
  {
    CHECK(it1->second == "MyForm");
  }

  auto it2 = attrs.find("ProgID");
  CHECK(it2 != attrs.cend());
  if(it2 != attrs.cend())
  {
    CHECK(it2->second == "00-00-00-00");
  }
}

TEST_CASE("attributes", "[vb6_parser_tests]")
{
  vb6_ast::STRICT_MODULE_STRUCTURE::module_attributes ast;
  auto str = R"vb(Attribute ModuleName = "MyForm"
                  Attribute ProgID = "00-00-00-00"
                  )vb";
  //auto [res, sv] = test_grammar(str, vb6_grammar::preamble, ast);
  auto [res, sv] = test_grammar(str, *vb6_grammar::attributeDef, ast);
  INFO("stopped at: " << sv);
  REQUIRE(res);
  CHECK(sv.empty());

  CHECK(ast.size() == 2);

  auto const it1 = ast.find("ModuleName");
  CHECK(it1 != ast.cend());
  if(it1 != ast.cend())
  {
    CHECK(it1->second == "MyForm");
  }

  auto const it2 = ast.find("ProgID");
  CHECK(it2 != ast.cend());
  if(it2 != ast.cend())
  {
    CHECK(it2->second == "00-00-00-00");
  }
}

TEST_CASE("options", "[vb6_parser_tests]")
{
  std::vector<vb6_ast::module_option> ast;
  auto str = R"vb(Option Explicit
                  Option Base 0
                  )vb";
  auto [res, sv] = test_grammar(str, *vb6_grammar::option_item, ast);
  INFO("stopped at: " << sv);
  REQUIRE(res);
  CHECK(sv.empty());

  REQUIRE(ast.size() == 2);

  CHECK(ast[0] == vb6_ast::module_option::explicit_);
  CHECK(ast[1] == vb6_ast::module_option::base_0);
}

#if 0
TEST_CASE("declaration_block")
{
  auto const declarations
    = boost::spirit::x3::rule<class _, std::vector<vb6_ast::STRICT_MODULE_STRUCTURE::declaration>>("declaration_block")
    = *vb6_grammar::STRICT_MODULE_STRUCTURE::declaration;

  std::vector<vb6_ast::STRICT_MODULE_STRUCTURE::declaration> decls;
  auto str = "Const e As Double = 2.8, pi As Double = 3.14, u As Integer = -1\r\n"
             "Global g_logger As Long, v1, XRes As Object, ptr As MyRec, g_active As Boolean\r\n"
             "Private Declare Sub PFoo Lib \"mylib.dll\" Alias \"PFoo\" (ByVal val As Long)\r\n"
             "Enum MyEnum1\r\n  c1 = 0\r\n  c2 = 1\r\nEnd Enum\r\n"
             "Public Type MyRecord1\r\n  v1 As String\r\n  v2 As Long\r\nEnd Type\r\n";
  auto [res, sv] = test_grammar(str, declarations, decls);
  INFO("stopped at: " << sv);
  REQUIRE(res);
  CHECK(sv.empty());

  REQUIRE(decls.size() == 5);

  REQUIRE_NOTHROW(boost::get<vb6_ast::const_var_stat>(decls[0].get()));
  REQUIRE_NOTHROW(boost::get<vb6_ast::global_var_decls>(decls[1].get()));
  REQUIRE_NOTHROW(boost::get<vb6_ast::externalSub>(decls[2].get()));
  REQUIRE_NOTHROW(boost::get<vb6_ast::vb_enum>(decls[3].get()));
  REQUIRE_NOTHROW(boost::get<vb6_ast::record>(decls[4].get()));
}

TEST_CASE("bas_unit_STRICT_MODULE_STRUCTURE", "[vb6_parser_tests]")
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
                  )vb";
  auto [res, sv] = test_grammar(str, vb6_grammar::STRICT_MODULE_STRUCTURE::basModDef, ast);
  INFO("stopped at: " << sv);
  REQUIRE(res);
  CHECK(sv.empty());

  vb6_ast_printer P(std::cout);
  P(ast);

  CHECK(ast.attrs.size() == 2);

  auto const it1 = ast.attrs.find("ModuleName");
  EXPECT_NE(it1, ast.attrs.cend());
  if(it1 != ast.attrs.cend())
  {
    CHECK(it1->second == "MyForm");
  }

  auto const it2 = ast.attrs.find("ProgID");
  EXPECT_NE(it2, ast.attrs.cend());
  if(it2 != ast.attrs.cend())
  {
    CHECK(it2->second == "00-00-00-00");
  }

  CHECK(ast.opts.items.size() == 4);
  if(ast.opts.items.size() == 4)
  {
#if 0
    CHECK(ast.opts.items[0] == vb6_ast::module_option::explicit_);
    CHECK(ast.opts.items[1] == vb6_ast::module_option::base_0);
#else
    CHECK(boost::get<vb6_ast::module_option>(ast.opts.items[0].get()) == vb6_ast::module_option::explicit_);
    CHECK(boost::get<vb6_ast::module_option>(ast.opts.items[1].get()) == vb6_ast::module_option::base_0);
#endif
    REQUIRE_NOTHROW(boost::get<vb6_ast::empty_line>(ast.opts.items[2].get()));
    REQUIRE_NOTHROW(boost::get<vb6_ast::lonely_comment>(ast.opts.items[3].get()));
  }

  CHECK(ast.declarations.size() == 3);
  if(ast.declarations.size() == 3)
  {
    REQUIRE_NOTHROW(boost::get<vb6_ast::const_var_stat>(ast.declarations[0].get()));
    REQUIRE_NOTHROW(boost::get<vb6_ast::global_var_decls>(ast.declarations[1].get()));
    REQUIRE_NOTHROW(boost::get<vb6_ast::vb_enum>(ast.declarations[2].get()));
  }

  CHECK(ast.functions.size() == 2);
  if(ast.functions.size() == 2)
  {
    REQUIRE_NOTHROW(boost::get<vb6_ast::subDef>(ast.functions[0].get())));
    REQUIRE_NOTHROW(boost::get<vb6_ast::functionDef>(ast.functions[1].get())));
  }
}
#endif

TEST_CASE("bas_unit", "[vb6_parser_tests]")
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
                  )vb";
  auto [res, sv] = test_grammar(str, vb6_grammar::basModDef, ast);
  INFO("stopped at: " << sv);
  REQUIRE(res);
  CHECK(sv.empty());

  vb6_ast_printer P(std::cout);
  P(ast);

  REQUIRE(ast.size() == 11);

  CHECK(boost::get<vb6_ast::module_attribute>(ast[0].get()).first == "ModuleName");
  CHECK(boost::get<vb6_ast::module_attribute>(ast[1].get()).first == "ProgID");

  CHECK(boost::get<vb6_ast::module_option>(ast[2].get()) == vb6_ast::module_option::explicit_);
  CHECK(boost::get<vb6_ast::module_option>(ast[3].get()) == vb6_ast::module_option::base_0);

  REQUIRE_NOTHROW(boost::get<vb6_ast::empty_line>(ast[4].get()));
  REQUIRE_NOTHROW(boost::get<vb6_ast::lonely_comment>(ast[5].get()));

  REQUIRE_NOTHROW(boost::get<vb6_ast::const_var_stat>(boost::get<vb6_ast::declaration>(ast[6].get())));
  REQUIRE_NOTHROW(boost::get<vb6_ast::global_var_decls>(boost::get<vb6_ast::declaration>(ast[7].get())));
  REQUIRE_NOTHROW(boost::get<vb6_ast::vb_enum>(boost::get<vb6_ast::declaration>(ast[8].get())));

  REQUIRE_NOTHROW(boost::get<vb6_ast::subDef>(ast[9].get()));
  REQUIRE_NOTHROW(boost::get<vb6_ast::functionDef>(ast[10].get()));
}

TEST_CASE("trailing_comment", "[vb6_parser_tests]")
{
  auto str = "Global g_var As Long ' how can we catch a trailing comment?\r\n";
  vb6_ast::global_var_decls vars;
  auto [res, sv] = test_grammar(str, vb6_grammar::global_var_declaration, vars);
  INFO("stopped at: " << sv);
  REQUIRE(res);
  CHECK(sv.empty());

  CHECK(vars.at == vb6_ast::access_type::global);
  CHECK(!vars.with_events);
  REQUIRE(vars.vars.size() == 1);

  CHECK(vars.vars[0].name == "g_var");
  CHECK(!vars.vars[0].construct);
  CHECK(vars.vars[0].type.has_value());
  if(vars.vars[0].type)
  {
    CHECK(boost::get<vb6_ast::NativeType>(vars.vars[0].type->value.get()) == vb6_ast::NativeType::Long);
  }
}

int main(int argc, char* argv[])
{
  Catch::Session session;

  int res = session.applyCommandLine(argc, argv);
  if(res != 0)
    return res;

  return session.run();
}
