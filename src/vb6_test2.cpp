//: vb6_test2.cpp

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

void vb6_test_statements(ostream& os)
{
  vb6_ast_printer Print(os);

  vb6_ast::statements::assignStmt P2;
  test_grammar(os, "assignmentStmt 0", vb6_grammar::statements::assignmentStmt,
               "Set var1 = \"  ciao\"\r\n", P2);
  Print(P2);

  vb6_ast::statements::assignStmt P3;
  test_grammar(os, "assignmentStmt 1", vb6_grammar::statements::assignmentStmt,
               "Let var2 = 54.7\r\n", P3);
  Print(P3);

  vb6_ast::statements::assignStmt P4;
  test_grammar(os, "assignmentStmt 2", vb6_grammar::statements::assignmentStmt,
               "var3 = Func(\"descr\", 54.7)\r\n", P4);
  Print(P4);

  vb6_ast::statements::assignStmt P5;
  test_grammar(os, "assignmentStmt 3", vb6_grammar::statements::assignmentStmt,
               "str = CStr(i)\r\n", P5);
  Print(P5);

  vb6_ast::statements::localVarDeclStmt localVarDecl;
  test_grammar(os, "localvardeclStmt", vb6_grammar::statements::localvardeclStmt,
               "Dim var1 As String, var2 As Integer\r\n", localVarDecl);
  Print(localVarDecl);

  vb6_ast::statements::redimStmt redim;
  test_grammar(os, "redimStmt", vb6_grammar::statements::redimStmt,
               "ReDim var1(15)\r\n", redim);
  Print(redim);

  vb6_ast::statements::exitStmt exitSt;
  test_grammar(os, "exitStmt", vb6_grammar::statements::exitStmt,
               "Exit Function\r\n", exitSt);
  Print(exitSt);

  vb6_ast::statements::gotoStmt gotoSt;
  test_grammar(os, "gotoStmt", vb6_grammar::statements::gotoStmt,
               "GoSub label1\r\n", gotoSt);
  Print(gotoSt);

  vb6_ast::statements::onerrorStmt onerrorSt;
  test_grammar(os, "onerrorStmt", vb6_grammar::statements::onerrorStmt,
               "On Error Resume Next\r\n", onerrorSt);
  Print(onerrorSt);

  vb6_ast::statements::labelStmt labelSt;
  test_grammar(os, "labelStmt", vb6_grammar::statements::labelStmt,
               "label1:\r\n", labelSt);
  Print(labelSt);

  vb6_ast::statements::callStmt callSt1;
  test_grammar(os, "callexplicitStmt", vb6_grammar::statements::callexplicitStmt,
               "Call Foo(13)\r\n", callSt1);
  Print(callSt1);

  vb6_ast::statements::callStmt callSt2;
  test_grammar(os, "callimplicitStmt", vb6_grammar::statements::callimplicitStmt,
               "Foo \"Sea\", Nothing, False\r\n", callSt2);
  Print(callSt2);

  vb6_ast::statements::raiseeventStmt raise_ev;
  test_grammar(os, "raise_event_statement", vb6_grammar::statements::raiseeventStmt,
               "RaiseEvent OnChange(\"hi!\")\r\n", raise_ev);

  vb6_ast::statements::withStmt with_stat;
  test_grammar(os, "with_statement", vb6_grammar::statements::withStmt,
    R"vb(With obj
             'Call Module1.foo(True, .Name)
             Call foo(True, .Name)
         End With
    )vb", with_stat);
  Print(with_stat);

  vb6_ast::statements::whileStmt while_stat;
  test_grammar(os, "while_statement", vb6_grammar::statements::whileStmt,
    R"vb(While cond(34, i)
           Print str
         Wend
        )vb", while_stat);
  Print(while_stat);

  vb6_ast::statements::dowhileStmt dowhile_stat;
  test_grammar(os, "dowhile_statement", vb6_grammar::statements::dowhileStmt,
    R"vb(Do While cond(34, i)
           Print str
         Loop
        )vb", dowhile_stat);
  Print(dowhile_stat);

  vb6_ast::statements::loopwhileStmt loopwhile_st;
  test_grammar(os, "loopwhile_statement", vb6_grammar::statements::loopwhileStmt,
    R"vb(Do
          Print str
        Loop While cond(34, i)
      )vb", loopwhile_st);
  Print(loopwhile_st);

  vb6_ast::statements::dountilStmt dountil_st;
  test_grammar(os, "dountil_statement", vb6_grammar::statements::dountilStmt,
    R"vb(Do Until cond(34, i)
          Print str
        Loop
      )vb", dountil_st);
  Print(dountil_st);

  vb6_ast::statements::loopuntilStmt loopuntil_st;
  test_grammar(os, "loopuntil_statement", vb6_grammar::statements::loopuntilStmt,
    R"vb(Do
          Print str
        Loop Until cond(34, i)
      )vb", loopuntil_st);
  Print(loopuntil_st);

  vb6_ast::statements::forStmt forloop_stat;
  test_grammar(os, "for_statement", vb6_grammar::statements::forStmt,
    R"vb(For i = 1 To 100 Step 2
           Dim str As String
           str = CStr(i)
           Print str
         Next i
        )vb", forloop_stat);
  Print(forloop_stat);

  vb6_ast::statements::foreachStmt foreach_stat;
  test_grammar(os, "foreach_statement", vb6_grammar::statements::foreachStmt,
    R"vb(For Each el In Items
           Print el
         Next el
        )vb", foreach_stat);
  Print(foreach_stat);

  std::vector<vb6_ast::statements::if_branch> branches;
  //vb6_ast::if_branch branches;
  test_grammar(os, "elseif_branch", *vb6_grammar::statements::elsifBranch,
    R"vb(ElseIf func2(Nothing) Then
             proc2a "elseif branch", 2
         ElseIf func3(Nothing) Then
             proc2b "elseif branch", 2
             proc2c "elseif branch", 2
        )vb", branches);
  for(auto& el : branches)
  {
    os << "ElseIf ";
    Print(el.condition);
    os << " Then\n";

    // statements
    for(auto& st : el.block)
      Print(st);
  }

  vb6_ast::statements::ifelseStmt ifelse_stat1;
  test_grammar(os, "ifelse_statement", vb6_grammar::statements::ifelseStmt,
    R"vb(If func1("name", True, 45.8, -45) Then
                             proc1 "if branch", 1
                         ElseIf func2(Nothing) Then
                             proc2 "elseif branch", 2
                         Else
                             proc3 "else branch", 3
                         End If
                    )vb", ifelse_stat1);
  Print(ifelse_stat1);

  vb6_ast::statements::selectStmt select_stat;
  test_grammar(os, "select_statement", vb6_grammar::statements::selectStmt,
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
      )vb", select_stat);
  Print(select_stat);
}

void vb6_test2(ostream& os)
{
  vb6_ast_printer Print(os);

  vb6_ast::record rec;
  auto str3 = "Private Type myrec\r\n  name As String\r\n  age As Integer\r\nEnd Type\r\n"s;
  test_grammar(os, "record_declaration", vb6_grammar::record_declaration, str3, rec);
  Print(rec);

  vb6_ast::vb_enum enum_res;
  auto str4 = "Private Enum ComprKind\r\n  NoCompr = 0\r\n  Jpeg = 1\r\n  Tiff\r\nEnd Enum\r\n"s;
  test_grammar(os, "enum_declaration", vb6_grammar::enum_declaration, str4, enum_res);
  Print(enum_res);

  vb6_ast::externalSub extsub;
  auto str5 = "Private Declare Sub Beep Lib \"kernel32.dll\" Alias \"Beep\" (ByVal time As Long, ByVal xx As Single)\r\n"s;
  test_grammar(os, "DLL subroutine declaration", vb6_grammar::external_sub_decl, str5, extsub);
  Print(extsub);

  vb6_ast::externalFunction extfunc;
  auto str6 = "Private Declare Function Beep Lib \"kernel32.dll\" Alias \"Beep\" (ByVal time As Long, ByVal xx As Single) As Long\r\n"s;
  test_grammar(os, "DLL function declaration", vb6_grammar::external_function_decl, str6, extfunc);
  Print(extfunc);

  auto const declarations //= x3::rule<class _, vector<vb6_ast::STRICT_MODULE_STRUCTURE::declaration>>()
                          = *vb6_grammar::STRICT_MODULE_STRUCTURE::declaration;

#if 0
  vector<vb6_ast::STRICT_MODULE_STRUCTURE::declaration> decls;
  auto str7 = "Const e As Double = 2.8, pi As Double = 3.14, u As Integer = -1\r\n"
              "Global g_logger As Long, v1, XRes As Object, ptr As MyRec, g_active As Boolean\r\n"
              "Private Declare Sub PFoo Lib \"mylib.dll\" Alias \"PFoo\" (ByVal val As Long)\r\n"
              "Enum MyEnum1\r\n  c1 = 0\r\n  c2 = 1\r\nEnd Enum\r\n"
              "Public Type MyRecord1\r\n  v1 As String\r\n  v2 As Long\r\nEnd Type\r\n"s;
  test_grammar(os, "Declaration block", declarations, str7, decls);
  os << str7 << '\n';
  for(auto& el : decls)
    Print(el);
  os << '\n';
#endif

  vb6_ast::identifier_context ctx;
  test_grammar(os, "identifier_context", vb6_grammar::identifier_context, "var1.func().pnt1.", ctx);
  Print(ctx);
  os << '\n';

  vb6_ast::decorated_variable dec_var;
  test_grammar(os, "decorated_variable", vb6_grammar::decorated_variable, "var1.func().pnt1.X", dec_var);
  Print(dec_var);
  os << '\n';

  // Dim obj As VB.Form
  // pnt.x = 4.5
  // Call Module1.func1(45)
}
