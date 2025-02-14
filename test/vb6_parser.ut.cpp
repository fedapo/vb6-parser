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
#include <boost/ut.hpp>

#include <iostream>
#include <map>
#include <vector>

namespace x3 = boost::spirit::x3;
namespace ut = boost::ut;

using namespace std;
// need for ut's user-defined literals and overloaded operators
using namespace boost::ut;

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

ut::suite<"vb6_parser_simple"> _ = []
{ 
  ut::test("lonely_comment") = []
  {
    vector<vb6_ast::lonely_comment> ast;
    auto [res, sv] = test_grammar(
      "' This is comment line 1\r\n' Comment line 2\r\n",
      *vb6_grammar::lonely_comment, ast);
    ut::expect((res) >> fatal) << "stopped at: " << sv;
    ut::expect(sv.empty());

    //cout << ast.size() << "\n";
    ut::expect(ut::eq(ast.size(), 2));

    ut::expect(ast[0].content == " This is comment line 1");
    ut::expect(ast[1].content == " Comment line 2");
  };

  ut::test("empty_lines") = []
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
    ut::expect((res) >> fatal) << "stopped at: " << sv;
    ut::expect(sv.empty());

    ut::expect(ast.size() == 3);

    ut::expect(boost::get<vb6_ast::lonely_comment>(ast[0]).content == " comment1");
    ut::expect(ut::nothrow([ast](){boost::get<vb6_ast::empty_line>(ast[1]);}));
    ut::expect(boost::get<vb6_ast::lonely_comment>(ast[2]).content == " comment2");
  };

  ut::test("quoted_string") = []
  {
    string str;
    auto [res, sv] = test_grammar("\"Quoted string.\"", vb6_grammar::quoted_string, str);
    ut::expect((res) >> fatal) << "stopped at: " << sv;
    ut::expect(sv.empty());
    ut::expect(str == "Quoted string.");
  };

  ut::test("basic_identifier") = []
  {
    string id;
    auto [res, sv] = test_grammar("iden_tifier", vb6_grammar::basic_identifier, id);
    ut::expect((res) >> fatal) << "stopped at: " << sv;
    ut::expect(sv.empty());
    ut::expect(id == "iden_tifier");
  };

  ut::test("var_identifier") = []
  {
    string var;

    ut::test("1") = [&var]
    {
      auto [res, sv] = test_grammar("g_logger", vb6_grammar::var_identifier, var);
      ut::expect((res) >> fatal) << "stopped at: " << sv;
      ut::expect(sv.empty());
      ut::expect(var == "g_logger");
    };

    ut::test("2") = [&var]
    {
      auto [res, sv] = test_grammar("forth ", vb6_grammar::sub_identifier, var);
      ut::expect((res) >> fatal) << "stopped at: " << sv;
      ut::expect(sv.empty());
      ut::expect(var == "forth");
    };

    ut::test("3") = [&var]
    {
      auto [res, sv] = test_grammar("subroutine ", vb6_grammar::sub_identifier, var);
      ut::expect((res) >> fatal) << "stopped at: " << sv;
      ut::expect(sv.empty());
      ut::expect(var == "subroutine");
    };

    /*
    string code = "sub ";
    string_view code_sv = code;
    auto it1 = cbegin(code_sv);
    auto const it2 = cend(code_sv);
    ut::expect(!boost::spirit::x3::phrase_parse(it1, it2, vb6_grammar::sub_identifier, vb6_grammar::skip, var));
    ut::expect(it1 == begin(code_sv));
    */
  };

  ut::test("type_identifier") = []
  {
    string type;
    auto [res, sv] = test_grammar("Long", vb6_grammar::type_identifier, type);
    ut::expect((res) >> fatal) << "stopped at: " << sv;
    ut::expect(sv.empty());
    ut::expect(that % type == "Long"s);
  };

  ut::test("complex_type_identifier") = []
  {
    string type;
    auto [res, sv] = test_grammar("VB.Form", vb6_grammar::complex_type_identifier, type);
    ut::expect((res) >> fatal) << "stopped at: " << sv;
    ut::expect(sv.empty());
    ut::expect(that % type == "VB.Form"s);
  };

  ut::test("const_expression_non_numeric") = []
  {
    vb6_ast::const_expr ast;

    ut::test("1") = [&ast]
    {
      string str = "\"una stringa\""s;
      auto [res, sv] = test_grammar(str, vb6_grammar::const_expression, ast);
      ut::expect((res) >> fatal) << "stopped at: " << sv;
      ut::expect(sv.empty());
      ut::expect(that % boost::get<vb6_ast::quoted_string>(ast.get()) == "una stringa"s);
    };

    ut::test("2") = [&ast]
    {
      string str = "True"s;
      auto [res, sv] = test_grammar(str, vb6_grammar::const_expression, ast);
      ut::expect((res) >> fatal) << "stopped at: " << sv;
      ut::expect(sv.empty());
      ut::expect(boost::get<bool>(ast.get()));
    };

    ut::test("3") = [&ast]
    {
      string str = "Nothing"s;
      auto [res, sv] = test_grammar(str, vb6_grammar::const_expression, ast);
      ut::expect((res) >> fatal) << "stopped at: " << sv;
      ut::expect(sv.empty());
      ut::expect(ut::nothrow([ast](){boost::get<vb6_ast::nothing>(ast.get());}));
    };
  };

  ut::test("const_expression_integers") = []
  {
    vb6_ast::const_expr ast;

    ut::test("1") = [&ast]
    {
      string str = "1234%"s;
      auto [res, sv] = test_grammar(str, vb6_grammar::const_expression, ast);
      ut::expect((res) >> fatal) << "stopped at: " << sv;
      ut::expect(sv.empty());
      ut::expect(that % boost::get<vb6_ast::integer_dec>(ast.get()).val == 1234);
    };

    ut::test("2") = [&ast]
    {
      string str = "1234&"s;
      auto [res, sv] = test_grammar(str, vb6_grammar::const_expression, ast);
      ut::expect((res) >> fatal) << "stopped at: " << sv;
      ut::expect(sv.empty());
      ut::expect(that % boost::get<vb6_ast::long_dec>(ast.get()).val == 1234);
    };

    ut::test("3") = [&ast]
    {
      string str = "&Hcafedead&"s;
      auto [res, sv] = test_grammar(str, vb6_grammar::const_expression, ast);
      ut::expect((res) >> fatal) << "stopped at: " << sv;
      ut::expect(sv.empty());
      ut::expect(that % boost::get<vb6_ast::long_hex>(ast.get()).val == 0xcafedead);
    };

    ut::test("4") = [&ast]
    {
      string str = "&01234&"s;
      auto [res, sv] = test_grammar(str, vb6_grammar::const_expression, ast);
      ut::expect((res) >> fatal) << "stopped at: " << sv;
      ut::expect(sv.empty());
      ut::expect(that % boost::get<vb6_ast::long_oct>(ast.get()).val == 01234);
    };

    ut::test("5") = [&ast]
    {
      string str = "1234"s;
      auto [res, sv] = test_grammar(str, vb6_grammar::const_expression, ast);
      ut::expect((res) >> fatal) << "stopped at: " << sv;
      ut::expect(sv.empty());
      ut::expect(that % boost::get<vb6_ast::integer_dec>(ast.get()).val == 1234);
    };
  };

  ut::test("const_expression_floats") = []
  {
    vb6_ast::const_expr ast;

    ut::test("1") = [&ast]
    {
      string str = "1234!"s;
      auto [res, sv] = test_grammar(str, vb6_grammar::const_expression, ast);
      ut::expect((res) >> fatal) << "stopped at: " << sv;
      ut::expect(sv.empty());
      ut::expect(boost::get<float>(ast.get()) == 1234.0f);
    };

    ut::test("2") = [&ast]
    {
      string str = "1234#"s;
      auto [res, sv] = test_grammar(str, vb6_grammar::const_expression, ast);
      ut::expect((res) >> fatal) << "stopped at: " << sv;
      ut::expect(sv.empty());
      ut::expect(boost::get<double>(ast.get()) == 1234.0);
    };

    ut::test("3") = [&ast]
    {
      string str = "2.8"s;
      auto [res, sv] = test_grammar(str, vb6_grammar::const_expression, ast);
      ut::expect((res) >> fatal) << "stopped at: " << sv;
      ut::expect(sv.empty());
      ut::expect(boost::get<float>(ast.get()) == 2.8f);
    };
  };

  ut::test("sample_expression") = []
  {
    vb6_ast::expression ast;
    auto [res, sv] = test_grammar("foo1(foo2(3, M.x_coord), True)", vb6_grammar::expression, ast);
    ut::expect((res) >> fatal) << "stopped at: " << sv;
    ut::expect(sv.empty());
    ut::expect(ast.get().type() == typeid(x3::forward_ast<vb6_ast::func_call>));
    ut::expect(boost::get<x3::forward_ast<vb6_ast::func_call>>(ast.get()).get().func_name == "foo1");
  };

  ut::test("single_var_declaration") = []
  {
    ut::test("1") = []
    {
      vb6_ast::variable P1;
      auto [res, sv] = test_grammar("g_logger As Long", vb6_grammar::single_var_declaration, P1);
      ut::expect((res) >> fatal) << "stopped at: " << sv;
      ut::expect(sv.empty());
      ut::expect(P1.name == "g_logger");
      ut::expect(!P1.construct);
      ut::expect(P1.type.has_value());
      if(P1.type)
      {
        ut::expect(*P1.type == "Long");
      }
    };

    ut::test("2") = []
    {
      vb6_ast::variable P2;
      auto [res, sv] = test_grammar("name As New String", vb6_grammar::single_var_declaration, P2);
      ut::expect((res) >> fatal) << "stopped at: " << sv;
      ut::expect(sv.empty());
      ut::expect(P2.name == "name");
      ut::expect(P2.construct);
      ut::expect(P2.type.has_value());
      if(P2.type)
      {
        ut::expect(*P2.type == "String");
      }
    };
  };
};

ut::suite<"vb6_parser_tests"> __ = []
{
  ut::test("record_declaration") = []
  {
    vb6_ast::record rec;
    auto [res, sv] = test_grammar("Type PatRecord\r\n  name As String\r\n  age As Integer\r\nEnd Type\r\n",
                 vb6_grammar::record_declaration, rec);
    ut::expect((res) >> fatal) << "stopped at: " << sv;
    ut::expect(sv.empty());

    ut::expect(rec.name == "PatRecord");
    ut::expect(rec.at == vb6_ast::access_type::na);
    ut::expect(rec.members.size() == 2);

    ut::expect(rec.members[0].name == "name");
    ut::expect(rec.members[0].type.has_value());
    if(rec.members[0].type)
    {
      ut::expect(*rec.members[0].type == "String");
    }
    ut::expect(!rec.members[0].construct);

    ut::expect(rec.members[1].name == "age");
    ut::expect(rec.members[1].type.has_value());
    if(rec.members[1].type)
    {
      ut::expect(*rec.members[1].type == "Integer");
    }
    ut::expect(!rec.members[1].construct);
  };

  ut::test("enum_declaration") = []
  {
    vb6_ast::vb_enum enum1;
    auto [res, sv] = test_grammar("Enum PatTypes\r\n  inpatient\r\n  outpatient\r\nEnd Enum\r\n",
                 vb6_grammar::enum_declaration,  enum1);
    ut::expect((res) >> fatal) << "stopped at: " << sv;
    ut::expect(sv.empty());

    ut::expect(enum1.name == "PatTypes");
    ut::expect(enum1.at == vb6_ast::access_type::na);
    ut::expect((enum1.values.size() == 2) >> ut::fatal);

    ut::expect(enum1.values[0].first == "inpatient");
    ut::expect(!enum1.values[0].second);

    ut::expect(enum1.values[1].first == "outpatient");
    ut::expect(!enum1.values[1].second);
  };

  ut::test("global_var_declaration") = []
  {
    auto str = "Global g_logger As Long, v1, XRes As New Object, ptr As Module.MyRec, g_active As Boolean\r\n"s;
    vb6_ast::global_var_decls vars;
    auto [res, sv] = test_grammar(str, vb6_grammar::global_var_declaration, vars);
    ut::expect((res) >> fatal) << "stopped at: " << sv;
    ut::expect(sv.empty());

    ut::expect(vars.at == vb6_ast::access_type::global);
    ut::expect(!vars.with_events);
    ut::expect((vars.vars.size() == 5) >> ut::fatal);

    //cout << "size: " << vars.vars.size() << "\n";

    ut::expect(vars.vars[0].name == "g_logger");
    ut::expect(!vars.vars[0].construct);
    ut::expect(vars.vars[0].type.has_value());
    if(vars.vars[0].type)
    {
      ut::expect(*vars.vars[0].type == "Long");
    }

    ut::expect(vars.vars[1].name == "v1");
    ut::expect(!vars.vars[1].construct);
    ut::expect(!vars.vars[1].type.has_value());

    ut::expect(vars.vars[2].name == "XRes");
    ut::expect(vars.vars[2].construct);
    ut::expect(vars.vars[2].type.has_value());
    if(vars.vars[2].type)
    {
      ut::expect(*vars.vars[2].type == "Object");
    }

    ut::expect(vars.vars[3].name == "ptr");
    ut::expect(!vars.vars[3].construct);
    ut::expect(vars.vars[3].type.has_value());
    if(vars.vars[3].type)
    {
      ut::expect(*vars.vars[3].type == "MyRec");
    }

    ut::expect(vars.vars[4].name == "g_active");
    ut::expect(!vars.vars[4].construct);
    ut::expect(vars.vars[4].type.has_value());
    if(vars.vars[4].type)
    {
      ut::expect(*vars.vars[4].type == "Boolean");
    }
  };

  ut::test("const_var_declaration1") = []
  {
    auto cstr = "Const e As Single = 2.8, pi As Double = 3.14, u As Integer = -1\r\n"s;
    vb6_ast::const_var_stat cvars;
    auto [res, sv] = test_grammar(cstr, vb6_grammar::const_var_declaration, cvars);
    ut::expect((res) >> fatal) << "stopped at: " << sv;
    ut::expect(sv.empty());

    ut::expect((cvars.size() == 3) >> ut::fatal);

    //cout << "size: " << cvars.size() << "\n";

    ut::expect(cvars[0].var.name == "e");
    if(cvars[0].var.type)
    {
      ut::expect(*cvars[0].var.type == "Single");
    }
    ut::expect(!cvars[0].var.construct);
    ut::expect(boost::get<float>(cvars[0].value.get()) == 2.8f);

    ut::expect(cvars[1].var.name == "pi");
    if(cvars[1].var.type)
    {
      ut::expect(*cvars[1].var.type == "Double");
    }
    ut::expect(!cvars[1].var.construct);
    ut::expect(boost::get<float>(cvars[1].value.get()) == 3.14f);

    ut::expect(cvars[2].var.name == "u");
    if(cvars[2].var.type)
    {
      ut::expect(*cvars[2].var.type == "Integer");
    }
    ut::expect(!cvars[2].var.construct);
    ut::expect(boost::get<vb6_ast::integer_dec>(cvars[2].value.get()).val == -1);
  };

  ut::test("const_var_declaration2") = []
  {
    vb6_ast::const_var_stat cvars;
    auto [res, sv] = test_grammar("Private Const PI As Double = 3.1415\r\n",
                vb6_grammar::const_var_declaration,  cvars);
    ut::expect((res) >> fatal) << "stopped at: " << sv;
    ut::expect(sv.empty());

    ut::expect((cvars.size() == 1) >> ut::fatal);

    //cout << "size: " << cvars.size() << "\n";

    ut::expect(cvars[0].var.name == "PI");
    if(cvars[0].var.type)
    {
      ut::expect(*cvars[0].var.type == "Double");
    }
    ut::expect(!cvars[0].var.construct);
    ut::expect(boost::get<float>(cvars[0].value.get()) == 3.1415f);
  };

  ut::test("param_decl") = []
  {
    vb6_ast::func_param fp;
    auto [res, sv] = test_grammar("Optional ByVal name As String = \"pippo\"", vb6_grammar::param_decl, fp);
    ut::expect((res) >> fatal) << "stopped at: " << sv;
    ut::expect(sv.empty());

    ut::expect(fp.isoptional);
    ut::expect(fp.qualifier.has_value());
    if(fp.qualifier)
    {
      ut::expect(*fp.qualifier == vb6_ast::param_qualifier::byval);
    }
    ut::expect(fp.var.name == "name");
    ut::expect(!fp.var.construct);
    ut::expect(fp.var.type.has_value());
    if(fp.var.type)
    {
      ut::expect(*fp.var.type == "String");
    }
    ut::expect(fp.defvalue.has_value());
    if(fp.defvalue)
    {
      ut::expect(boost::get<vb6_ast::quoted_string>(fp.defvalue.get()) == "pippo");
    }
  };

  ut::test("param_list_decl") = []
  {
    vector<vb6_ast::func_param> fps;
    auto [res, sv] = test_grammar("ByVal name As String, ByRef val As Integer",
                -(vb6_grammar::param_decl % ','), fps);
    ut::expect((res) >> fatal) << "stopped at: " << sv;
    ut::expect(sv.empty());

    //cout << "size: " << fps.size() << "\n";

    ut::expect((fps.size() == 2) >> ut::fatal);

    ut::expect(!fps[0].isoptional);
    ut::expect(fps[0].qualifier.has_value());
    if(fps[0].qualifier)
    {
      ut::expect(*fps[0].qualifier == vb6_ast::param_qualifier::byval);
    }
    ut::expect(fps[0].var.name == "name");
    ut::expect(!fps[0].var.construct);
    ut::expect(fps[0].var.type.has_value());
    if(fps[0].var.type)
    {
      ut::expect(*fps[0].var.type == "String");
    }
    ut::expect(!fps[0].defvalue);

    ut::expect(!fps[1].isoptional);
    ut::expect(fps[1].qualifier.has_value());
    if(fps[1].qualifier)
    {
      ut::expect(*fps[1].qualifier == vb6_ast::param_qualifier::byref);
    }
    ut::expect(fps[1].var.name == "val");
    ut::expect(!fps[1].var.construct);
    ut::expect(fps[1].var.type.has_value());
    if(fps[1].var.type)
    {
      ut::expect(*fps[1].var.type == "Integer");
    }
    ut::expect(!fps[1].defvalue);
  };

  ut::test("event_declaration") = []
  {
    vb6_ast::eventHead event_decl;
    auto [res, sv] = test_grammar("Public Event OnChange(ByVal Text As String)\r\n",
                vb6_grammar::eventHead, event_decl);
    ut::expect((res) >> fatal) << "stopped at: " << sv;
    ut::expect(sv.empty());

    ut::expect(event_decl.at == vb6_ast::access_type::public_);
    ut::expect(event_decl.name == "OnChange");
    ut::expect(event_decl.params.size() == 1);
  };

  ut::test("function_head") = []
  {
    vb6_ast::functionHead fh;
    auto [res, sv] = test_grammar(
      "Private Function OneFunc(ByVal name As String, ByRef val As Integer) As Integer\r\n",
      vb6_grammar::functionHead, fh);
    ut::expect((res) >> fatal) << "stopped at: " << sv;
    ut::expect(sv.empty());

    ut::expect(fh.at == vb6_ast::access_type::private_);
    ut::expect(fh.name == "OneFunc");
    ut::expect(fh.params.size() == 2);
    ut::expect(fh.return_type.has_value());
    ut::expect(*fh.return_type == "Integer");
  };

  ut::test("function_head_no_params") = []
  {
    vb6_ast::functionHead fh;
    auto [res, sv] = test_grammar(
      "Private Function NoParamFunc() As Object\r\n", vb6_grammar::functionHead, fh);
    ut::expect((res) >> fatal) << "stopped at: " << sv;
    ut::expect(sv.empty());

    ut::expect(fh.at == vb6_ast::access_type::private_);
    ut::expect(fh.name == "NoParamFunc");
    ut::expect(fh.params.empty());
    ut::expect((fh.return_type.has_value()) >> ut::fatal);
    ut::expect(*fh.return_type == "Object");
  };

  ut::test("subroutine_head") = []
  {
    vb6_ast::subHead sh;
    auto [res, sv] = test_grammar(
      "Private Sub my_sub(ByRef str As String, ByVal valid As Boolean)\r\n",
      vb6_grammar::subHead, sh);
    ut::expect((res) >> fatal) << "stopped at: " << sv;
    ut::expect(sv.empty());

    ut::expect(sh.at == vb6_ast::access_type::private_);
    ut::expect(sh.name == "my_sub");
    ut::expect(sh.params.size() == 2);
  };

  ut::test("subroutine_head2") = []
  {
    auto str = "Private Sub my_sub(ByRef str As String, ByVal valid As Boolean, Optional ByVal flag As Boolean = True)\r\n"s;
    vb6_ast::subHead sh;
    auto [res, sv] = test_grammar(str, vb6_grammar::subHead, sh);
    ut::expect((res) >> fatal) << "stopped at: " << sv;
    ut::expect(sv.empty());

    ut::expect(sh.at == vb6_ast::access_type::private_);
    ut::expect(sh.name == "my_sub");
    ut::expect(sh.params.size() == 3);
  };

  ut::test("subroutine_head_with_optional_params") = []
  {
    vb6_ast::subHead sh;
    auto [res, sv] = test_grammar(
      "Private Sub my_sub(ByRef str As String, Optional ByVal valid As Boolean = false)\r\n",
      vb6_grammar::subHead, sh);
    ut::expect((res) >> fatal) << "stopped at: " << sv;
    ut::expect(sv.empty());

    ut::expect(sh.at == vb6_ast::access_type::private_);
    ut::expect(sh.name == "my_sub");
    ut::expect((sh.params.size() == 2) >> ut::fatal);

    ut::expect(sh.params[0].qualifier.has_value());
    if (sh.params[0].qualifier)
      ut::expect(sh.params[0].qualifier.get() == vb6_ast::param_qualifier::byref);
    ut::expect(sh.params[0].var.name == "str");
    if(sh.params[0].var.type)
    {
      ut::expect(*sh.params[0].var.type == "String");
    }
    ut::expect(!sh.params[0].var.construct);
    ut::expect(!sh.params[0].isoptional);
    ut::expect(!sh.params[0].defvalue);

    ut::expect(sh.params[1].qualifier.has_value());
    if(sh.params[1].qualifier)
    {
      ut::expect(sh.params[1].qualifier.get() == vb6_ast::param_qualifier::byval);
    }
    ut::expect(sh.params[1].var.name == "valid");
    if(sh.params[1].var.type)
    {
      ut::expect(*sh.params[1].var.type == "Boolean");
    }
    ut::expect(!sh.params[1].var.construct);
    ut::expect(sh.params[1].isoptional);
    ut::expect(sh.params[1].defvalue.has_value());
    if(sh.params[0].defvalue)
    {
      ut::expect(boost::get<bool>(*sh.params[0].defvalue) == false);
    }
  };

  ut::test("property_let_head") = []
  {
    auto str = "Public Property Let Width(ByVal w As Integer)\r\n"s;
    vb6_ast::propertyLetHead ast;
    auto [res, sv] = test_grammar(str, vb6_grammar::property_letHead, ast);
    ut::expect((res) >> fatal) << "stopped at: " << sv;
    ut::expect(sv.empty());

    ut::expect(ast.at == vb6_ast::access_type::public_);
    ut::expect(ast.name == "Width");
    ut::expect(ast.params.size() == 1);
  };

  ut::test("property_get_head") = []
  {
    auto str = "Public Property Get Width() As Integer\r\n"s;
    vb6_ast::propertyGetHead ast;
    auto [res, sv] = test_grammar(str, vb6_grammar::property_getHead, ast);
    ut::expect((res) >> fatal) << "stopped at: " << sv;
    ut::expect(sv.empty());

    ut::expect(ast.at == vb6_ast::access_type::public_);
    ut::expect(ast.name == "Width");
    ut::expect(ast.params.size() == 0);
    ut::expect(ast.return_type >> fatal);
    ut::expect(*ast.return_type == "Integer");
  };

  ut::test("dll_subroutine_declaration") = []
  {
    vb6_ast::externalSub extsub;
    auto str = "Private Declare Sub BeepVB Lib \"kernel32.dll\" Alias \"Beep\" (ByVal time As Long, ByVal xx As Single)\r\n"s;
    auto [res, sv] = test_grammar(str, vb6_grammar::external_sub_decl, extsub);
    ut::expect((res) >> fatal) << "stopped at: " << sv;
    ut::expect(sv.empty());

    ut::expect(extsub.at == vb6_ast::access_type::private_);
    ut::expect(extsub.name == "BeepVB");
    ut::expect(extsub.alias == "Beep");
    ut::expect(extsub.params.size() == 2);
    ut::expect(extsub.lib == "kernel32.dll");
  };

  ut::test("dll_function_declaration") = []
  {
    vb6_ast::externalFunction extfunc;
    auto str = "Private Declare Function BeepVB Lib \"kernel32.dll\" Alias \"Beep\" (ByVal time As Long, ByVal xx As Single) As Long\r\n"s;
    auto [res, sv] = test_grammar(str, vb6_grammar::external_function_decl, extfunc);
    ut::expect((res) >> fatal) << "stopped at: " << sv;
    ut::expect(sv.empty());

    ut::expect(extfunc.at == vb6_ast::access_type::private_);
    ut::expect(extfunc.name == "BeepVB");
    ut::expect(extfunc.return_type.has_value());
    if(extfunc.return_type)
    {
      ut::expect(*extfunc.return_type == "Long");
    }
    ut::expect(extfunc.alias == "Beep");
    ut::expect(extfunc.params.size() == 2);
    ut::expect(extfunc.lib == "kernel32.dll");
  };

  ut::test("identifier_context") = []
  {
    vb6_ast::identifier_context ctx;
    auto [res, sv] = test_grammar("var1.func().pnt1.", vb6_grammar::identifier_context, ctx);
    ut::expect((res) >> fatal) << "stopped at: " << sv;
    ut::expect(sv.empty());

    ut::expect(!ctx.leading_dot);
    ut::expect((ctx.elements.size() == 3) >> ut::fatal);
    ut::expect(boost::get<string>(ctx.elements[0].get()) == "var1");
    auto& tmp = boost::get<x3::forward_ast<vb6_ast::func_call>>(ctx.elements[1].get()).get();
    ut::expect(tmp.func_name == "func");
    ut::expect(tmp.params.empty());
    ut::expect(boost::get<string>(ctx.elements[2].get()) == "pnt1");
  };

  ut::test("decorated_variable") = []
  {
    vb6_ast::decorated_variable dec_var;
    auto [res, sv] = test_grammar("var1.func().pnt1.X", vb6_grammar::decorated_variable, dec_var);
    ut::expect((res) >> fatal) << "stopped at: " << sv;
    ut::expect(sv.empty());

    ut::expect(!dec_var.ctx.leading_dot);
    ut::expect((dec_var.ctx.elements.size() == 3) >> ut::fatal);
    ut::expect(boost::get<string>(dec_var.ctx.elements[0].get()) == "var1");
    auto& tmp = boost::get<x3::forward_ast<vb6_ast::func_call>>(dec_var.ctx.elements[1].get()).get();
    ut::expect(tmp.func_name == "func");
    ut::expect(tmp.params.empty());
    ut::expect(boost::get<string>(dec_var.ctx.elements[2].get()) == "pnt1");
    ut::expect(dec_var.var == "X");
  };

  ut::test("attribute_block") = []
  {
    vb6_ast::STRICT_MODULE_STRUCTURE::module_attributes attrs;
    auto str = "Attribute ModuleName = \"MyForm\"\r\n"
              "Attribute ProgID = \"00-00-00-00\"\r\n"s;
    auto [res, sv] = test_grammar(str, *vb6_grammar::attributeDef, attrs);
    ut::expect((res) >> fatal) << "stopped at: " << sv;
    ut::expect(sv.empty());

    ut::expect(attrs.size() == 2);

    auto it1 = attrs.find("ModuleName");
    ut::expect(it1 != attrs.cend());
    if(it1 != attrs.cend())
    {
      ut::expect(it1->second == "MyForm");
    }

    auto it2 = attrs.find("ProgID");
    ut::expect(it2 != attrs.cend());
    if(it2 != attrs.cend())
    {
      ut::expect(it2->second == "00-00-00-00");
    }
  };

  ut::test("attributes") = []
  {
    vb6_ast::STRICT_MODULE_STRUCTURE::module_attributes ast;
    auto str = R"vb(Attribute ModuleName = "MyForm"
                    Attribute ProgID = "00-00-00-00"
                    )vb"s;
    //auto [res, sv] = test_grammar(str, vb6_grammar::preamble, ast);
    auto [res, sv] = test_grammar(str, *vb6_grammar::attributeDef, ast);
    ut::expect((res) >> fatal) << "stopped at: " << sv;
    ut::expect(sv.empty());

    ut::expect(ast.size() == 2);

    auto const it1 = ast.find("ModuleName");
    ut::expect(it1 != ast.cend());
    if(it1 != ast.cend())
    {
      ut::expect(it1->second == "MyForm");
    }

    auto const it2 = ast.find("ProgID");
    ut::expect(it2 != ast.cend());
    if(it2 != ast.cend())
    {
      ut::expect(it2->second == "00-00-00-00");
    }
  };

  ut::test("options") = []
  {
    vector<vb6_ast::module_option> ast;
    auto str = R"vb(Option Explicit
                    Option Base 0
                    )vb"s;
    auto [res, sv] = test_grammar(str, *vb6_grammar::option_item, ast);
    ut::expect((res) >> fatal) << "stopped at: " << sv;
    ut::expect(sv.empty());

    ut::expect((ast.size() == 2) >> ut::fatal);

    ut::expect(ast[0] == vb6_ast::module_option::explicit_);
    ut::expect(ast[1] == vb6_ast::module_option::base_0);
  };

#if 0
  ut::test("declaration_block") = []
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
    ut::expect((res) >> fatal) << "stopped at: " << sv;
    ut::expect(sv.empty());

    ut::expect((decls.size() == 5) >> ut::fatal);

    ut::expect(ut::nothrow([ast](){boost::get<vb6_ast::const_var_stat>(decls[0].get();})));
    ut::expect(ut::nothrow([ast](){boost::get<vb6_ast::global_var_decls>(decls[1].get();})));
    ut::expect(ut::nothrow([ast](){boost::get<vb6_ast::externalSub>(decls[2].get();})));
    ut::expect(ut::nothrow([ast](){boost::get<vb6_ast::vb_enum>(decls[3].get();})));
    ut::expect(ut::nothrow([ast](){boost::get<vb6_ast::record>(decls[4].get();})));
  };

  ut::test("bas_unit_STRICT_MODULE_STRUCTURE") = []
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
    ut::expect((res) >> fatal) << "stopped at: " << sv;
    ut::expect(sv.empty());

    vb6_ast_printer P(cout);
    P(ast);

    ut::expect(ast.attrs.size() == 2);

    auto const it1 = ast.attrs.find("ModuleName");
    EXPECT_NE(it1, ast.attrs.cend());
    if(it1 != ast.attrs.cend())
    {
      ut::expect(it1->second == "MyForm");
    }

    auto const it2 = ast.attrs.find("ProgID");
    EXPECT_NE(it2, ast.attrs.cend());
    if(it2 != ast.attrs.cend())
    {
      ut::expect(it2->second == "00-00-00-00");
    }

    ut::expect(ast.opts.items.size() == 4);
    if(ast.opts.items.size() == 4)
    {
  #if 0
      ut::expect(ast.opts.items[0] == vb6_ast::module_option::explicit_);
      ut::expect(ast.opts.items[1] == vb6_ast::module_option::base_0);
  #else
      ut::expect(boost::get<vb6_ast::module_option>(ast.opts.items[0].get()) == vb6_ast::module_option::explicit_);
      ut::expect(boost::get<vb6_ast::module_option>(ast.opts.items[1].get()) == vb6_ast::module_option::base_0);
  #endif
      ut::expect(ut::nothrow([ast](){boost::get<vb6_ast::empty_line>(ast.opts.items[2].get();})));
      ut::expect(ut::nothrow([ast](){boost::get<vb6_ast::lonely_comment>(ast.opts.items[3].get();})));
    }

    ut::expect(ast.declarations.size() == 3);
    if(ast.declarations.size() == 3)
    {
      ut::expect(ut::nothrow([ast](){boost::get<vb6_ast::const_var_stat>(ast.declarations[0].get();})));
      ut::expect(ut::nothrow([ast](){boost::get<vb6_ast::global_var_decls>(ast.declarations[1].get();})));
      ut::expect(ut::nothrow([ast](){boost::get<vb6_ast::vb_enum>(ast.declarations[2].get();})));
    }

    ut::expect(ast.functions.size() == 2);
    if(ast.functions.size() == 2)
    {
      ut::expect(ut::nothrow([ast](){boost::get<vb6_ast::subDef>(ast.functions[0].get();})));
      ut::expect(ut::nothrow([ast](){boost::get<vb6_ast::functionDef>(ast.functions[1].get();})));
    }
  };
  #endif

  ut::test("bas_unit") = []
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
    ut::expect((res) >> fatal) << "stopped at: " << sv;
    ut::expect(sv.empty());

    vb6_ast_printer P(cout);
    P(ast);

    ut::expect(ut::eq(ast.size(), 11) >> ut::fatal);

    ut::expect(boost::get<vb6_ast::module_attribute>(ast[0].get()).first == "ModuleName");
    ut::expect(boost::get<vb6_ast::module_attribute>(ast[1].get()).first == "ProgID");

    ut::expect(boost::get<vb6_ast::module_option>(ast[2].get()) == vb6_ast::module_option::explicit_);
    ut::expect(boost::get<vb6_ast::module_option>(ast[3].get()) == vb6_ast::module_option::base_0);

    ut::expect(ut::nothrow([ast](){boost::get<vb6_ast::empty_line>(ast[4].get());}));
    ut::expect(ut::nothrow([ast](){boost::get<vb6_ast::lonely_comment>(ast[5].get());}));

    ut::expect(ut::nothrow([ast](){boost::get<vb6_ast::const_var_stat>(boost::get<vb6_ast::declaration>(ast[6].get()));}));
    ut::expect(ut::nothrow([ast](){boost::get<vb6_ast::global_var_decls>(boost::get<vb6_ast::declaration>(ast[7].get()));}));
    ut::expect(ut::nothrow([ast](){boost::get<vb6_ast::vb_enum>(boost::get<vb6_ast::declaration>(ast[8].get()));}));

    ut::expect(ut::nothrow([ast](){boost::get<vb6_ast::subDef>(ast[9].get());}));
    ut::expect(ut::nothrow([ast](){boost::get<vb6_ast::functionDef>(ast[10].get());}));
  };

  ut::test("trailing_comment") = []
  {
    auto str = "Global g_var As Long ' how can we catch a trailing comment?\r\n"s;
    vb6_ast::global_var_decls vars;
    auto [res, sv] = test_grammar(str, vb6_grammar::global_var_declaration, vars);
    ut::expect((res) >> fatal) << "stopped at: " << sv;
    ut::expect(sv.empty());

    ut::expect(vars.at == vb6_ast::access_type::global);
    ut::expect(!vars.with_events);
    ut::expect((vars.vars.size() == 1) >> ut::fatal);

    ut::expect(vars.vars[0].name == "g_var");
    ut::expect(!vars.vars[0].construct);
    ut::expect(vars.vars[0].type.has_value());
    if(vars.vars[0].type)
    {
      ut::expect(*vars.vars[0].type == "Long");
    }
  };
};

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
  log_compiler_info(std::cout);

  bool res = boost::ut::cfg<boost::ut::override>.run();
  return res ? 0 : -1;
}
