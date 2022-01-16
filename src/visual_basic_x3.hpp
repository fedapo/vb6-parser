//: visual_basic_x3.hpp

// vb6_parser
// Copyright (c) 2022 Federico Aponte
// This code is licensed under GNU Software License (see LICENSE.txt for details)

#pragma once

#include "color_console.hpp"
#include "vb6_ast.hpp"
#include "vb6_ast_adapt.hpp"
#include "vb6_parser.hpp"

#include <boost/fusion/include/std_pair.hpp>
#include <boost/spirit/home/x3.hpp>

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

/*
http://stackoverflow.com/questions/2366097/what-does-putting-an-exclamation-point-in-front-of-an-object-reference-varia
https://support.microsoft.com/en-us/kb/129287
http://bytecomb.com/the-bang-exclamation-operator-in-vba/
*/

/*
http://ciere.com/cppnow15/x3_docs/
http://ciere.com/cppnow15/

Parser Directory Structure
    * fun
        * ast.hpp
        * ast_adapted.hpp
        * common.hpp
        * expression.hpp
        * expression_def.hpp
    * src
        * expression.cpp
    * test
*/

/*
PARSING
http://stackoverflow.com/questions/3455456/what-kinds-of-patterns-could-i-enforce-on-the-code-to-make-it-easier-to-translat/3460977#3460977
http://www.semanticdesigns.com/Products/DMS/LifeAfterParsing.html
http://www.semanticdesigns.com/Products/DMS/DMSToolkit.html
https://support.microsoft.com/en-us/kb/216388
http://www.tangiblesoftwaresolutions.com/Product_Details/VB_to_CPlusPlus_Converter_Details.html
https://sourceforge.net/projects/vbtocconv/
http://www.vbto.net/
https://ubuntuforums.org/showthread.php?t=1590707
http://edndoc.esri.com/arcobjects/9.0/ArcGISDevHelp/DevelopmentEnvs/Cpp/ArcGIS%20Development/sampleconversions.htm
http://ezbasic.sourceforge.net/
https://github.com/antlr/grammars-v4/blob/master/vb6/VisualBasic6.g4
http://slebok.github.io/zoo/index.html

MIX
http://powerbasic.com/index.php
http://www.codeproject.com/Articles/710181/Visual-Basic-A-giant-more-powerful-than-ever
https://www.indiegogo.com/projects/a-replacement-to-visual-basic-6-ide-and-compiler#/
http://www.ioprogrammo.it/index.php?topic=20151.0;wap2
http://vb.mvps.org/
http://www.semanticdesigns.com/Products/Services/VB6Migration.html?Home=LegacyMigration
http://betselection.cc/resources/vb6-replacements/
https://blog.xojo.com/2013/06/19/a-modern-alternative-to-visual-basic/
http://www.tiobe.com/index.php/content/paperinfo/tpci/index.html
https://www.indiegogo.com/projects/a-replacement-to-visual-basic-6-ide-and-compiler#/
http://visualstudio.uservoice.com/forums/121579-visual-studio/suggestions/7462243-provide-a-visual-basic-6-community-edition-to-al

FRX
http://www.vbforums.com/showthread.php?221901-frx-file-format
http://planet-source-code.com/vb/scripts/ShowCode.asp?txtCodeId=5539&lngWId=1
https://forum.powerbasic.com/forum/user-to-user-discussions/special-interest-groups/vb-conversion/59957-extract-files-from-vb6-frx-discussion
https://forum.powerbasic.com/forum/user-to-user-discussions/source-code/59945-extract-files-from-visual-basic-vb5-vb6-frx-form-resource-files

putref_  -> Set
put_     -> Let
get_     -> Get
*/

/*
== Exclamation Point (!) Operator ==
Use the ! operator only on a class or interface as a dictionary access operator.
The class or interface must have a default property that accepts a single String argument.
The identifier immediately following the ! operator becomes the string argument to the default property.
*/

/*
== Implicit types ==

  * the type depends on a suffix
  * there are 6 suffixes: $ % & @ ! #
  * variables with a suffix cannot be explicitly declared (e.g. Dim name$ As String is forbiddden)
  * £ is a valid character for names just like a-z and _ (aaargh)

    my_str$ = "ciao"     ' String
    my_short% = 34       ' Integer
    my_long& = 65537     ' Long
    my_float! = 3.1      ' Single
    my_double# = 1.23    ' Double
    my_currency@ = 23.56 ' Currency

    Debug.Print "my_str$ -> "; TypeName(my_str$)           ' String
    Debug.Print "my_short% -> "; TypeName(my_short%)       ' Integer
    Debug.Print "my_long& -> "; TypeName(my_long&)         ' Long
    Debug.Print "my_float! -> "; TypeName(my_float!)       ' Single
    Debug.Print "my_double# -> "; TypeName(my_double#)     ' Double
    Debug.Print "my_currency@ -> "; TypeName(my_currency@) ' Currency
*/

/*
http://www.vb6.us/tutorials/error-handling

On [Local] Error { GoTo [ line | 0 | -1 ] | Resume Next }

-> "On Local Error" is a hold over from previous versions of VB

GoTo line      Enables the error-handling routine that starts at the line
               specified in the required line argument. The line argument is any
               line label or line number. If a run-time error occurs, control
               branches to the specified line, making the error handler active.
               The specified line must be in the same procedure as the On Error
               statement, or a compile-time error will occur.
               This form of the On Error statement redirects program execution
               to the line label specified. When you use this form of On Error,
               a block of error handling code is constructed following the label.
GoTo 0         Disables enabled error handler in the current procedure and
               resets it to Nothing.
GoTo -1        Disables enabled exception in the current procedure and resets it
               to Nothing.
Resume Next    Specifies that when a run-time error occurs, control goes to the
               statement immediately following the statement where the error
               occurred, and execution continues from that point. Use this form
               rather than On Error GoTo when accessing objects.
               This form of the On Error statement tells VB to continue with the
               line of code following the line where the error occurred. With
               this type of error trap, you would normally test for an error at
               selected points in the program code where you anticipate that an
               error may occur.

Private Sub cmd1_Click()
    On Error GoTo ErrHandler

    Dim val As Long
    val = CLng("we34")

    Debug.Print val
ErrHandler:
    Debug.Print "cmd1_Click -> " & VBA.Information.Err.Number & " - " & VBA.Information.Err.Description
End Sub

Private Sub cmd2_Click()
    On Error Resume Next

    Dim val As Long
    val = CLng("we34") ' fails with type mismatch

    Debug.Print val

    ' this succeeds, but the error stays because the operation above failed
    val = CLng("12")

    Debug.Print "cmd2_Click -> " & VBA.Information.Err.Number & " - " & VBA.Information.Err.Description
End Sub

Private Sub cmd3_Click()
    ' the error is apperently cleared at the beginning of each sub or function
    Debug.Print "cmd3_Click -> " & VBA.Information.Err.Number & " - " & VBA.Information.Err.Description
End Sub
*/

/*
http://stackoverflow.com/questions/9290731/what-is-the-effect-of-a-semicolon-at-the-end-of-a-line
http://www.vb6.us/tutorials/understanding-semicolans-and-print-method

 --> Print is more of a pseudo-method that the compiler handles directly.
     This is why it does not work within a With-block, etc.

Print is a fundamental BASIC statement that dates back to the first days of the
language in the mid-1960s. Print is used to display lines of data on a form,
picture box, printer, and the immediate (Debug) window; it can also be used to
write records of data to a file. In VB, Print is implemented as a method.

The general format for the Print method is:

[object.]Print [expressionlist]

where object refers to one of the objects mentioned above (Form, PictureBox,
Debug window, Printer) and expressionlist refers to a list of one or more
numeric or string expressions to print.

Items in the expression list may be separated with semicolons (;) or commas (,).
A semicolon or comma in the expression list determines where the next output begins:

; (semicolon) means print immediately after the last value.
, (comma) means print at the start of the next "print zone".

Items in the expression list of a Print statement that are separated by
semicolons print immediately after one another. In the statement

Print "Hello,"; strName; "How are you today?"
*/

/*
vbrun100.Dll -> msvbvm50.dll -> msvbvm60.dll
*/
struct vb6_keywords_ : boost::spirit::x3::symbols<unsigned>
{
  vb6_keywords_()
  {
    add("Option", 0) ("Explicit", 0) ("Compare", 0) ("Module", 0) ("Open", 0) ("Close", 0) ("Put", 0)
       ("Access", 0) ("Line", 0) ("Read", 0) ("Print", 0) ("Write", 0) ("Input", 0) ("Output", 0) ("Random", 0) ("Append", 0) ("Text", 0)
       ("Binary", 0) ("Lock", 0) ("Shared", 0) ("Tab", 0) ("On", 0) ("Error", 0) ("Resume", 0) ("Const", 0) ("True", 0) ("False", 0)

       // data types
       ("Boolean", 0) ("Byte", 0) ("Integer", 0) ("Long", 0) ("Single", 0) ("Double", 0)
       ("Currency", 0) ("Date", 0) ("String", 0) ("Variant", 0) ("Object", 0) ("Any", 0)

       ("Event", 0) ("Sub", 0) ("Function", 0) ("Propery", 0) ("Get", 0) ("Let", 0)
       ("Set", 0) ("Declare", 0) ("As", 0) ("Lib", 0) ("Alias", 0) ("Begin", 0) ("End", 0) ("ByVal", 0)
       ("ByRef", 0) ("ParamArray", 0) ("Optional", 0) /*("Default", 0)*/ ("AddressOf", 0) ("Static", 0)
       ("Public", 0) ("Private", 0) ("Global", 0) ("Friend", 0) ("If", 0) ("Then", 0) ("ElseIf", 0) ("Else", 0)
       ("For", 0) ("Each", 0) ("In", 0) ("To", 0) ("Step", 0) ("Next", 0)
       ("Debug.Assert", 0) ("Debug.Print", 0) ("Stop", 0)
       ("While", 0) ("Wend", 0) ("Do", 0) ("Loop", 0) ("Until", 0) ("Select", 0) ("Case", 0) ("Type", 0)
       ("Enum", 0) ("Dim", 0) ("ReDim", 0) ("Preserve", 0) ("Erase", 0) ("WithEvents", 0) ("Implements", 0)
       ("New", 0) ("Exit", 0) ("GoTo", 0) ("Call", 0) ("Rem", 0) ("Like", 0)
       ("GoSub", 0) ("RaiseEvent", 0) ("With", 0) ("Nothing", 0) ("Attribute", 0) ("TypeOf", 0)
       ("Not", 0) ("And", 0) ("Or", 0) ("Xor", 0) ("Is", 0) ("Mod", 0) ("Imp", 0)
    ;
  }
};

//inline vb6_keywords_ vb6_keywords;

/*
normally, the syntax is

    Open "filename.txt" for MODE [Lock] as #FILENO

whereas MODE is one of Input, Output, Random, Append, Binary (Random is default)
if you don't specify Lock, it normally shouldn't lock except on modes output and append.
don't forget to Close #FILENO after operation.
you might want to take a look at http://www.profsr.com/vb/vbless08.htm
*/

/*
Attribute VB_Name = "clsBulge"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = True
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = False
Attribute VB_Description = "Some text here"
Attribute VB_Ext_KEY = "SavedWithClassBuilder" ,"Yes"
Attribute VB_Ext_KEY = "Member0" ,"collBulges"
Attribute VB_Ext_KEY = "Top_Level" ,"Yes"

' Header
Attribute VB_Name = "ClassOrModuleName"
Attribute VB_GlobalNameSpace = False ' ignored
Attribute VB_Creatable = False ' ignored
Attribute VB_PredeclaredId = False ' a Value of True creates a default global instance
Attribute VB_Exposed = True ' Controls how the class can be instanced.

' Module Scoped Variables
Attribute variableName.VB_VarUserMemId = 0 ' Zero indicates that this is the default member of the class.
Attribute variableName.VB_VarDescription = "some string" ' Adds the text to the Object Browser information for this variable.

' Procedures
Attribute procName.VB_Description = "some string" ' Adds the text to the Object Browser information for the procedure.
Attribute procName.VB_UserMemId = someInteger
    '  0: Makes the function the default member of the class.
    ' -4: Specifies that the function returns an Enumerator.

https://christopherjmcclellan.wordpress.com/2015/04/21/vb-attributes-what-are-they-and-why-should-we-use-them/
https://stackoverflow.com/questions/33648764/what-does-the-attribute-keyword-do-in-vb6
*/

namespace vb6_grammar {

  namespace x3 = boost::spirit::x3;

  /*
    Sub ComboLoad(ByVal ctrl As VB.Control, ByVal D As String, ByVal active As Boolean, ByVal CodeVal As String)
    Function CodeText(ByVal s As String) As String

    Declare Function SendMessage Lib "user32" Alias "SendMessageA" (ByVal hWnd As Long, ByVal wMsg As Long, ByVal wParam As Long, lParam As Any) As Long
    Declare Sub xxx Lib "yyy" Alias "zzz" (ByVal hWnd As Long, ByVal wMsg As Long, ByVal wParam As Long, lParam As Any)

    Preprocessor!!!!

    #Const x = 6
    #If x = 5 Then
        ...
    #ElseIf
        ...
    #End If
  */

  /*
    == built-in functions, classes, constants ==

    Open Close Put Get - For Binary Random Append Read Write - As Binary Text

    == libraries (modules) embedded in the language ==

    VBA.Information   -> Err VarType VarName
    VBA.Conversion    -> Val Str CStr CInt CLong, Error
    VBA.Interaction   -> CreateObject IIf MsgBox
    VBA.Strings       -> StrConv Split Join InStr LCase UCase Len LenB Left$ Right$ Mid$ Space$
    VBA.FileSystem    -> FreeFile Kill
    VBA._HiddenModule -> VarPtr, StrPtr, ObjPtr
    VB                -> Load App

    == built-in constants ==

    VbStrConv -> vbUnicode
  */

  // not really keywords, these are subroutines, functions and constants that
  // are built into the language

  // file handling
  auto const kwOpen = x3::rule<class kwOpen, std::string>("kwOpen")
                    = x3::no_case[x3::lit("Open")];
  auto const kwClose = x3::rule<class kwClose, std::string>("kwClose")
                     = x3::no_case[x3::lit("Close")];
  auto const kwPut = x3::rule<class kwPut, std::string>("kwPut")
                   = x3::no_case[x3::lit("Put")];
  auto const kwAccess = x3::rule<class kwAccess, std::string>("kwAccess")
                      = x3::no_case[x3::lit("Access")];
  auto const kwLine = x3::rule<class kwLine, std::string>("kwLine")
                    = x3::no_case[x3::lit("Line")];
  auto const kwRead = x3::rule<class kwRead, std::string>("kwRead")
                    = x3::no_case[x3::lit("Read")];
  auto const kwWrite = x3::rule<class kwWrite, std::string>("kwWrite")
                      = x3::no_case[x3::lit("Write")];
  auto const kwInput = x3::rule<class kwInput, std::string>("kwInput")
                     = x3::no_case[x3::lit("Input")];
  auto const kwOutput = x3::rule<class kwOutput, std::string>("kwOutput")
                      = x3::no_case[x3::lit("Output")];
  auto const kwRandom = x3::rule<class kwRandom, std::string>("kwRandom")
                      = x3::no_case[x3::lit("Random")];
  auto const kwAppend = x3::rule<class kwAppend, std::string>("kwAppend")
                      = x3::no_case[x3::lit("Append")];
  auto const kwLock = x3::rule<class kwLock, std::string>("kwLock")
                    = x3::no_case[x3::lit("Lock")];
  auto const kwShared = x3::rule<class kwShared, std::string>("kwShared")
                      = x3::no_case[x3::lit("Shared")];
  auto const kwSeek = x3::rule<class kwSeek, std::string>("kwSeek")
                    = x3::no_case[x3::lit("Seek")];
  auto const kwPrint = x3::rule<class kwPrint, std::string>("kwPrint")
                     = x3::no_case[x3::lit("Print")];
  auto const kwTab = x3::rule<class kwTab, std::string>("kwTab")
                   = x3::no_case[x3::lit("Tab")];

  auto const kwDebugAssert = x3::rule<class kwDebugAssert, std::string>()
                           = x3::no_case[x3::lit("Debug.Assert")];
  auto const kwDebugPrint = x3::rule<class kwDebugPrint, std::string>()
                          = x3::no_case[x3::lit("Debug.Print")];

} // namespace vb6_grammar