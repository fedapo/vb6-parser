//: vb6_parser_statements_def.hpp

// vb6_parser
// Copyright (c) 2022 Federico Aponte
// This code is licensed under GNU Software License (see LICENSE.txt for details)

#pragma once

#include "vb6_parser.hpp"
#include "vb6_ast_adapt.hpp"
#include "vb6_parser_keywords.hpp"
#include "vb6_parser_operators.hpp"

#include <boost/fusion/include/std_pair.hpp>
#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/x3/support/utility/annotate_on_success.hpp>

// http://boost.2283326.n4.nabble.com/Horrible-compiletimes-and-memory-usage-while-compiling-a-parser-with-X3-td4689104.html

/*
----
Function return value
VB:  The function's name treated as a variable is assigned the return value of
     the function. No explicit return statement needed, though it can be used (Exit Function/Property).
C++: Explicit return value needed.
----
Macros
----
Comments
----
Multi-line statements, continuation symbol _
----
On Error GoTo/Resume
----
Exclamation mark operator
----
support ParamArray, Array
----
support AddressOf
----
Implicit type variables
----
*/

namespace vb6_grammar {

  namespace x3 = boost::spirit::x3;

  // the terminator for every VB6 statement
  auto const cmdTermin = x3::eol | ':';

  // keyword groups
  auto const kwgEndIf       = kwEnd > kwIf       > cmdTermin;
  auto const kwgEndWith     = kwEnd > kwWith     > cmdTermin;
  auto const kwgEndSelect   = kwEnd > kwSelect   > cmdTermin;

  // statements
  namespace statements {
#if 0
    // FED ???? there seems to be a limit here
    //          I cannot define this rule with more than 21 alternatives!
    auto const singleStmt_def = lonely_comment // critical to have this as the first element
                              | empty_line
                              | assignmentStmt
                              | localvardeclStmt
                              | redimStmt
                              | exitStmt
                              | gotoStmt
                              | onerrorStmt
                              //| resumeStmt
                              | labelStmt
                              | callimplicitStmt
                              | callexplicitStmt
                              | raiseeventStmt
                              | whileStmt
                              | doStmt
                              | dowhileStmt
                              | loopwhileStmt
                              | dountilStmt
                              | loopuntilStmt
                              | forStmt
                              //| foreachStmt
                              | ifelseStmt
                              //| selectStmt
                              | withStmt
                              ;
#else
    auto const singleStmt1 = assignmentStmt
                           | localvardeclStmt
                           | redimStmt
                           | exitStmt
                           | gotoStmt
                           | onerrorStmt
                           | resumeStmt
                           | labelStmt
                           | callimplicitStmt
                           | callexplicitStmt
                           | raiseeventStmt
                           ;
    auto const singleStmt2 = whileStmt
                           | doStmt
                           | dowhileStmt
                           | loopwhileStmt
                           | dountilStmt
                           | loopuntilStmt
                           | forStmt
                           | foreachStmt
                           | ifelseStmt
                           | selectStmt
                           | withStmt
                           ;
    auto const singleStmt_def = lonely_comment // critical to have this as the first element
                              | empty_line
                              | singleStmt1
                              | singleStmt2;
#endif

    auto const statement_block_def = *singleStmt;

    auto const assignmentStmt_def = ( (kwSet   >> x3::attr(vb6_ast::assignmentType::set))
                                    | (kwLet   >> x3::attr(vb6_ast::assignmentType::let))
                                    | (x3::eps >> x3::attr(vb6_ast::assignmentType::na))
                                    ) >> decorated_variable >> opEqual >> expression >> cmdTermin;

    auto const localvardeclStmt_def = ( kwDim    >> x3::attr(vb6_ast::localvardeclType::Dim)
                                      | kwStatic >> x3::attr(vb6_ast::localvardeclType::Static)
                                      )
                                   >> (single_var_declaration % ',')
                                   >> cmdTermin;

    // TODO redim statements must be able to act on several variables
    auto const redimStmt_def = kwReDim >> -(kwPreserve >> x3::attr(true))
                            //>> ((decorated_variable > '(' > (expression % ',') > ')') % ',')
                            >> (decorated_variable > '(' > (expression % ',') > ')')
                            >> cmdTermin;

    auto const exitStmt_def = kwExit > ( kwSub      >> x3::attr(vb6_ast::exit_type::sub)
                                       | kwFunction >> x3::attr(vb6_ast::exit_type::function)
                                       | kwProperty >> x3::attr(vb6_ast::exit_type::property)
                                       | kwDo       >> x3::attr(vb6_ast::exit_type::do_)
                                       | kwWhile    >> x3::attr(vb6_ast::exit_type::while_)
                                       | kwFor      >> x3::attr(vb6_ast::exit_type::for_)
                                       )
                           >> cmdTermin;

    auto const gotoLabel = basic_identifier;

    auto const gotoStmt_def = ( (kwGoTo  >> x3::attr(vb6_ast::gotoType::goto_v))
                              | (kwGoSub >> x3::attr(vb6_ast::gotoType::gosub_v))
                              ) > gotoLabel >> cmdTermin;

    auto const onerrorStmt_def = (kwOn > kwError)
                              >> ( (kwResume > kwNext >> x3::attr(std::string()) >> x3::attr(vb6_ast::onerror_type::resume_next))
                                 | (kwGoTo >> ('0' >> x3::attr(std::string()) >> x3::attr(vb6_ast::onerror_type::goto_0)))
                                 | (kwGoTo >> ("-1" >> x3::attr(std::string()) >> x3::attr(vb6_ast::onerror_type::goto_neg_1)))
                                 | (kwGoTo >> (gotoLabel >> x3::attr(vb6_ast::onerror_type::goto_label)))
                                 | (kwExit >> (kwSub >> x3::attr(std::string()) >> x3::attr(vb6_ast::onerror_type::exit_sub)))
                                 | (kwExit >> (kwFunction >> x3::attr(std::string()) >> x3::attr(vb6_ast::onerror_type::exit_func)))
                                 | (kwExit >> (kwProperty >> x3::attr(std::string()) >> x3::attr(vb6_ast::onerror_type::exit_property)))
                                 ) >> cmdTermin;

    auto const resumeStmt_def = kwResume
                             >> ( (kwNext    >> x3::attr(0) >> x3::attr(vb6_ast::resume_type::next))
                                | (             gotoLabel   >> x3::attr(vb6_ast::resume_type::label))
                                | (             x3::int_    >> x3::attr(vb6_ast::resume_type::line_nr))
                                | (x3::eps   >> x3::attr(0) >> x3::attr(vb6_ast::resume_type::implicit))
                                )
                             >> cmdTermin;

    auto const labelStmt_def = gotoLabel >> x3::lit(':') >> cmdTermin;

    auto const callimplicitStmt_def = sub_identifier >> -(expression % ',') >> x3::attr(false) >> cmdTermin;
    auto const callexplicitStmt_def //= kwCall >> functionCall >> x3::attr(true) >> cmdTermin;
                                    //= ( kwCall       >> x3::attr()
                                    //  | kWRaiseEvent >> x3::attr()
                                    //  ) >> sub_identifier
                                    = kwCall >> sub_identifier
                                   >> '(' >> -(expression % ',') >> ')' >> x3::attr(true) >> cmdTermin;

    // raising an event is very similar to calling a subroutine
    auto const raiseeventStmt_def = kwRaiseEvent >> event_identifier >> '(' >> -(expression % ',') >> ')' >> cmdTermin;

    // compound statements

    auto const whileStmt_def = kwWhile >> expression >> cmdTermin
                            >> statement_block
                            >> kwWend
                            >> cmdTermin;

    // infinite loop
    auto const doStmt_def = kwDo >> cmdTermin
                         >> statement_block
                         >> kwLoop
                         >> cmdTermin;

    auto const dowhileStmt_def = kwDo >> kwWhile >> expression >> cmdTermin
                              >> statement_block
                              >> kwLoop
                              >> cmdTermin;

    auto const loopwhileStmt_def = kwDo >> cmdTermin
                                >> statement_block
                                >> kwLoop >> kwWhile >> expression
                                >> cmdTermin;

    auto const dountilStmt_def = kwDo >> kwUntil >> expression >> cmdTermin
                              >> statement_block
                              >> kwLoop
                              >> cmdTermin;

    auto const loopuntilStmt_def = kwDo >> cmdTermin
                                >> statement_block
                                >> kwLoop >> kwUntil >> expression
                                >> cmdTermin;

    // TODO match the variable after 'Next' with the one of the loop
    auto const forStmt_def = kwFor >> decorated_variable >> opEqual
                          >> expression >> kwTo >> expression
                          >> -(kwStep >> expression) >> cmdTermin
                          >> statement_block
                          >> kwNext >> -x3::omit[decorated_variable]
                          >> cmdTermin;

    // TODO match the variable after 'Next' with the one of the loop
    auto const foreachStmt_def = kwFor >> kwEach >> decorated_variable
                              >> kwIn >> expression >> cmdTermin
                              >> statement_block
                              >> kwNext >> -x3::omit[decorated_variable]
                              >> cmdTermin;

    auto const ifBranch_def = x3::rule<class ifBranch, vb6_ast::statements::if_branch>("ifBranch")
                            = kwIf >> expression
                           >> kwThen >> cmdTermin
                           >> statement_block;
    auto const elsifBranch_def = x3::rule<class elsifBranch, vb6_ast::statements::if_branch>("elsifBranch")
                               = kwElseIf >> expression
                              >> kwThen >> cmdTermin
                              >> statement_block;
    auto const elseBranch_def = x3::rule<class elseBranch, vb6_ast::statements::statement_block>("elseBranch")
                              = kwElse >> cmdTermin
                             >> statement_block;

    //auto const ifelseifBranches = x3::rule<class ifelseifBranches, std::vector<vb6_ast::if_branch>>()
    //                            = ifBranch >> (*elsifBranch);

    auto const ifelseStmt_def = //ifelseifBranches
                                ifBranch
                             >> (*elsifBranch)
                             >> (-elseBranch)
                             >> kwgEndIf;

    // TODO with-statement should also take a function returning a reference to an object
    auto const withStmt_def = kwWith >> decorated_variable
                           //>> -(kwNew >> type_identifier) // FED ????
                           >> cmdTermin
                           >> statement_block
                           >> kwgEndWith;

    // 'Is' is legal only when used with the 6 relational operators
    // it must appear, alone, on the left side of the operator with
    // an expression on the right side
    auto const case_relational_expr = kwIs >> ( (opLess         >> x3::attr(vb6_ast::rel_operator_type::less))
                                              | (opGreater      >> x3::attr(vb6_ast::rel_operator_type::greater))
                                              | (opLessEqual    >> x3::attr(vb6_ast::rel_operator_type::less_equal))
                                              | (opGreaterEqual >> x3::attr(vb6_ast::rel_operator_type::greater_equal))
                                              | (opNotEqual     >> x3::attr(vb6_ast::rel_operator_type::not_equal))
                                              | (opEqual        >> x3::attr(vb6_ast::rel_operator_type::equal))
                                              )
                                   >> expression;

    // TODO
    /*
    Select Case frm.Type
      Case 0:
        Print 0
      Case 1:
        Print 1
      Case 1 To 4, 7 To 9, 11, 13, Is > MaxNumber
      Case Else:
        Print "Default"
    End Select
    */
    auto const case_block_def = kwCase >> expression >> cmdTermin
                             >> statement_block;
    auto const case_block2_def = kwCase >> ( kwElse
                                           | (( (expression >> -(kwTo >> expression))
                                              | case_relational_expr
                                              ) % ',')
                                           )
                              >> cmdTermin
                              >> statement_block;

    // TODO select-statement
    auto const selectStmt_def = (kwSelect > kwCase)
                             >> expression >> cmdTermin
                             >> (*case_block)
                             >> kwgEndSelect;
  } // namespace statements

  BOOST_SPIRIT_DEFINE(
      statements::ifBranch
    , statements::elsifBranch
    , statements::elseBranch
    , statements::case_block
  )

  BOOST_SPIRIT_DEFINE(
      statements::singleStmt
    , statements::statement_block
    , statements::assignmentStmt
    , statements::localvardeclStmt
    , statements::redimStmt
    , statements::exitStmt
    , statements::gotoStmt
    , statements::onerrorStmt
    , statements::resumeStmt
    , statements::labelStmt
    , statements::callimplicitStmt
    , statements::callexplicitStmt
    , statements::raiseeventStmt
  )

  // compound statements
  BOOST_SPIRIT_DEFINE(
      statements::whileStmt
    , statements::doStmt
    , statements::dowhileStmt
    , statements::loopwhileStmt
    , statements::dountilStmt
    , statements::loopuntilStmt
    , statements::forStmt
    , statements::foreachStmt
    , statements::ifelseStmt
    , statements::withStmt
    , statements::selectStmt
  )
}