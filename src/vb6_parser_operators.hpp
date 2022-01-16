//: vb6_parser_operators.hpp

// vb6_parser
// Copyright (c) 2022 Federico Aponte
// This code is licensed under GNU Software License (see LICENSE.txt for details)

#pragma once

#include <boost/spirit/home/x3.hpp>

namespace vb6_grammar {

  namespace x3 = boost::spirit::x3;

  auto const opDot          = x3::lit('.');
  auto const opExclamation  = x3::lit('!');

  // arithmetic operators
  auto const opExp          = x3::lit('^');
  auto const opMult         = x3::lit('*');
  auto const opDiv          = x3::lit('/');
  auto const opDivint       = x3::lit('\\');
  auto const opMod          = x3::no_case[x3::lit("Mod")];
  auto const opPlus         = x3::lit('+');
  auto const opMinus        = x3::lit('-');
  auto const opAmp          = x3::lit('&');

  // relational operators
  auto const opLess         = x3::lit('<');
  auto const opGreater      = x3::lit('>');
  auto const opLessEqual    = x3::lit("<=");
  auto const opGreaterEqual = x3::lit(">=");
  auto const opEqual        = x3::lit('=');
  auto const opNotEqual     = x3::lit("<>");

  // boolean and logical operators
  auto const opNot  = x3::no_case[x3::lit("Not")];
  auto const opAnd  = x3::no_case[x3::lit("And")];
  auto const opOr   = x3::no_case[x3::lit("Or")];
  auto const opXor  = x3::no_case[x3::lit("Xor")];
  auto const opEqv  = x3::no_case[x3::lit("Eqv")]; // equivalence: (a IMP b) AND (b IMP a)
  auto const opImp  = x3::no_case[x3::lit("Imp")]; // material implication: NOT a OR b
  auto const opIs   = x3::no_case[x3::lit("Is")];
  auto const opLike = x3::no_case[x3::lit("Like")];

  /*
    # Arithmetic Operator Precedence Order
        ^
        - (unary negation)
        *, /
        \
        Mod
        +, - (binary addition/subtraction)
        &
    # Comparison Operator Precedence Order
        =
        <>
        <
        >
        <=
        >=
        Like, Is
    # Logical Operator Precedence Order
        Not
        And
        Or
        Xor
        Eqv
        Imp
    # Source
        Sams
        Teach Yourself Visual Basic 6 in 24 Hours
        Appendix A: Operator Precedence
  */
}