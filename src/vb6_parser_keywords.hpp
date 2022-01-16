//: vb6_parser_keywords.hpp

// vb6_parser
// Copyright (c) 2022 Federico Aponte
// This code is licensed under GNU Software License (see LICENSE.txt for details)

#pragma once

#include <boost/spirit/home/x3.hpp>

namespace vb6_grammar {

  namespace x3 = boost::spirit::x3;

  // constants
  auto const kwTrue    = x3::no_case[x3::lit("True")];
  auto const kwFalse   = x3::no_case[x3::lit("False")];
  auto const kwNothing = x3::no_case[x3::lit("Nothing")];

  // primitive types
  auto const kwBoolean  = x3::rule<class kwBoolean, std::string>("kwBoolean")
                        = x3::no_case[x3::lit("Boolean")];
  auto const kwByte     = x3::rule<class kwByte, std::string>("kwByte")
                        = x3::no_case[x3::lit("Byte")];
  auto const kwInteger  = x3::rule<class kwInteger, std::string>("kwInteger")
                        = x3::no_case[x3::lit("Integer")];
  auto const kwLong     = x3::rule<class kwLong, std::string>("kwLong")
                        = x3::no_case[x3::lit("Long")];
  auto const kwSingle   = x3::rule<class kwSingle, std::string>("kwSingle")
                        = x3::no_case[x3::lit("Single")];
  auto const kwDouble   = x3::rule<class kwDouble, std::string>("kwDouble")
                        = x3::no_case[x3::lit("Double")];
  auto const kwCurrency = x3::rule<class kwCurrency, std::string>("kwCurrency")
                        = x3::no_case[x3::lit("Currency")];
  auto const kwDate     = x3::rule<class kwDate, std::string>("kwDate")
                        = x3::no_case[x3::lit("Date")];
  auto const kwString   = x3::rule<class kwString, std::string>("kwString")
                        = x3::no_case[x3::lit("String")];
  auto const kwObject   = x3::rule<class kwObject, std::string>("kwObject")
                        = x3::no_case[x3::lit("Object")];
  auto const kwVariant  = x3::rule<class kwVariant, std::string>("kwVariant")
                        = x3::no_case[x3::lit("Variant")];
  // only for external functions and subroutines
  // "As Any" disables type checking and allows any data type to be passed in or returned
  auto const kwAny      = x3::rule<class kwAny, std::string>("kwAny")
                        = x3::no_case[x3::lit("Any")];

  // VB6 keywords

  auto const kwLet  = x3::no_case[x3::lit("Let")];
  auto const kwSet  = x3::no_case[x3::lit("Set")];
  auto const kwGet  = x3::no_case[x3::lit("Get")]; // also for file handling
  auto const kwRSet = x3::no_case[x3::lit("RSet")];

  //auto const kwBegin = x3::no_case[x3::lit("Begin")];
  auto const kwEnd = x3::no_case[x3::lit("End")];

  auto const kwOn     = x3::no_case[x3::lit("On")];
  auto const kwLocal  = x3::no_case[x3::lit("Local")];
  auto const kwError  = x3::no_case[x3::lit("Error")];
  auto const kwResume = x3::no_case[x3::lit("Resume")];

  auto const kwEvent = x3::no_case[x3::lit("Event")];
  auto const kwProperty = x3::no_case[x3::lit("Property")];

  auto const kwParamArray = x3::rule<class kwParamArray, std::string>("kwParamArray")
                          = x3::no_case[x3::lit("ParamArray")];
  auto const kwDefault = x3::rule<class kwDefault, std::string>("kwDefault")
                       = x3::no_case[x3::lit("Default")];
  auto const kwAddressOf = x3::rule<class kwAddressOf, std::string>("kwAddressOf")
                         = x3::no_case[x3::lit("AddressOf")];
  auto const kwStatic = x3::no_case[x3::lit("Static")];

  auto const kwFriend = x3::no_case[x3::lit("Friend")]; // used only in class modules

  auto const kwIf     = x3::no_case[x3::lit("If")];
  auto const kwThen   = x3::no_case[x3::lit("Then")];
  auto const kwElseIf = x3::no_case[x3::lit("ElseIf")];
  auto const kwElse   = x3::no_case[x3::lit("Else")];

  auto const kwFor  = x3::no_case[x3::lit("For")];
  auto const kwEach = x3::no_case[x3::lit("Each")];
  auto const kwIn   = x3::no_case[x3::lit("In")];
  auto const kwTo   = x3::no_case[x3::lit("To")];
  auto const kwStep = x3::no_case[x3::lit("Step")];
  auto const kwNext = x3::no_case[x3::lit("Next")];

  auto const kwWhile = x3::no_case[x3::lit("While")];
  auto const kwWend  = x3::no_case[x3::lit("Wend")];
  auto const kwDo    = x3::no_case[x3::lit("Do")];
  auto const kwLoop  = x3::no_case[x3::lit("Loop")];
  auto const kwUntil = x3::no_case[x3::lit("Until")];

  auto const kwSelect = x3::no_case[x3::lit("Select")];
  auto const kwCase   = x3::no_case[x3::lit("Case")];
  auto const kwIs     = x3::no_case[x3::lit("Is")];

  auto const kwReDim      = x3::no_case[x3::lit("ReDim")];
  auto const kwPreserve   = x3::no_case[x3::lit("Preserve")];
  auto const kwWithEvents = x3::no_case[x3::lit("WithEvents")];
  auto const kwNew        = x3::no_case[x3::lit("New")];
  auto const kwExit       = x3::no_case[x3::lit("Exit")];
  auto const kwCall       = x3::no_case[x3::lit("Call")];
  auto const kwRaiseEvent = x3::no_case[x3::lit("RaiseEvent")];
  auto const kwGoTo       = x3::no_case[x3::lit("GoTo")];
  auto const kwGoSub      = x3::no_case[x3::lit("GoSub")];
  auto const kwReturn     = x3::no_case[x3::lit("Return")];
  auto const kwWith       = x3::no_case[x3::lit("With")];
  auto const kwAttribute  = x3::no_case[x3::lit("Attribute")];
  auto const kwTypeOf     = x3::no_case[x3::lit("TypeOf")];

  /*struct GoTo_table : x3::symbols<vb6_ast::gotoType> {
    GoTo_table() {
      add("GoTo", vb6_ast::gotoType::goto_v)
         ("GoSub", vb6_ast::gotoType::gosub_v);
    }
  } const goto_or_gosub;*/

  auto const kwConst    = x3::no_case[x3::lit("Const")];
  auto const kwDim      = x3::no_case[x3::lit("Dim")];
  auto const kwType     = x3::no_case[x3::lit("Type")];
  auto const kwEnum     = x3::no_case[x3::lit("Enum")];
  auto const kwPublic   = x3::no_case[x3::lit("Public")];
  auto const kwPrivate  = x3::no_case[x3::lit("Private")];
  auto const kwGlobal   = x3::no_case[x3::lit("Global")];
  auto const kwSub      = x3::no_case[x3::lit("Sub")];
  auto const kwFunction = x3::no_case[x3::lit("Function")];
  auto const kwDeclare  = x3::no_case[x3::lit("Declare")];
  auto const kwAs       = x3::no_case[x3::lit("As")];
  auto const kwLib      = x3::no_case[x3::lit("Lib")];
  auto const kwAlias    = x3::no_case[x3::lit("Alias")];
  auto const kwText     = x3::no_case[x3::lit("Text")]; // used in Option Compare and file functions
  auto const kwBinary   = x3::no_case[x3::lit("Binary")]; // used in Option Compare and file functions

  // start of unit
  auto const kwOption   = x3::no_case[x3::lit("Option")];
  auto const kwExplicit = x3::no_case[x3::lit("Explicit")];
  auto const kwCompare  = x3::no_case[x3::lit("Compare")];
  auto const kwBase     = x3::no_case[x3::lit("Base")];
  auto const kwModule   = x3::no_case[x3::lit("Module")];
  /* NB:
     L'istruzione Option Private Module risulta utile solo per applicazioni host
     che supportano il caricamento simultaneo di più progetti e riferimenti
     tra progetti caricati. Microsoft Excel, ad esempio, consente di
     caricare più progetti e Option Private Module può essere utilizzata per
     limitare la visibilità tra progetti. Sebbene Visual Basic consenta di
     caricare più progetti, i riferimenti tra progetti non sono mai ammessi.
   */

  auto const kwByVal = x3::no_case[x3::lit("ByVal")] >> x3::attr(vb6_ast::param_qualifier::byval);
  auto const kwByRef = x3::no_case[x3::lit("ByRef")] >> x3::attr(vb6_ast::param_qualifier::byref);
  auto const kwOptional = x3::no_case[x3::lit("Optional")];
}