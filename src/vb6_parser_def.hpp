//: vb6_parser_def.hpp

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
  auto const kwgEndType     = kwEnd > kwType     > cmdTermin;
  auto const kwgEndEnum     = kwEnd > kwEnum     > cmdTermin;
  auto const kwgEndFunction = kwEnd > kwFunction > cmdTermin;
  auto const kwgEndSub      = kwEnd > kwSub      > cmdTermin;
  auto const kwgEndProperty = kwEnd > kwProperty > cmdTermin;

  auto const bool_const = x3::rule<class bool_const, bool>("bool_const")
                        = (kwTrue  >> x3::attr(true))
                        | (kwFalse >> x3::attr(false));

  auto const keywords = kwFor | kwEach | kwStep | kwTo | kwNext | kwEnd
                      | kwWhile | kwWend | kwLoop | kwDo | kwUntil
                      | kwIf | kwElse | kwElseIf | kwExit | kwSelect | kwCase
                      | kwPublic| kwPrivate | kwGlobal | kwDim
                      | kwGet | kwSet | kwLet
                      | kwOn | kwLocal | kwGoTo | kwGoSub | kwReturn
                      | kwCall | kwRaiseEvent
                      | kwSub | kwFunction | kwEvent | kwProperty
                      | kwByVal | kwByRef | kwAs | kwIn | kwOption;

  auto const reserved = keywords >> !x3::char_("a-zA-Z0-9_£");
  //auto const reserved = keywords >> (x3::char_ - x3::char_("a-zA-Z0-9_£"));

  // FED ???? dovrebbe andare bene, rivedere e pulire
  auto const empty_line_def = //x3::omit[x3::no_skip[*x3::blank]]
                           //>> x3::attr(vb6_ast::empty_line()) >> x3::eol;
                              x3::omit[x3::no_skip[*x3::blank >> x3::eol]]
                           >> x3::attr(vb6_ast::empty_line());

  // line composed only of a comment, no VB6 code
  //auto const lonely_comment_def = x3::no_skip[   x3::omit[*x3::blank]
  //                                            >> (kwRem | x3::lit('\''))
  //                                            >> *(x3::char_ - x3::eol)] >> x3::eol;
  // FED ???? dovrebbe andare bene, rivedere e pulire
  auto const lonely_comment_def = x3::no_skip[   x3::omit[*x3::blank]
                                              >> (kwRem | x3::lit('\''))
                                              >> *(x3::char_ - x3::eol) >> x3::eol];

  // FED ???? the expression ~x3::char_('"') does not behave as I expect
  //auto const quoted_string_def = x3::lexeme['"' >> *(~x3::char_('"')) >> '"'];
  auto const quoted_string_def = x3::lexeme['"' >> *(x3::char_ - '"') >> '"'];

  // need to remove the reserved words from the possible identifiers
  auto const basic_identifier_def = x3::lexeme[x3::no_case[x3::char_("a-zA-Z_£") >> *x3::char_("a-zA-Z0-9_£")] - reserved];
                                  //= x3::lexeme[x3::no_case[(x3::alpha | '_' | '£') >> *(x3::alnum | '_' | '£')] - reserved];
  /*
  Dim x As String ' simple identifier, variable name
  x.y.z = 5       ' composed, variable name
  Call foo(x.y.z) ' composed, variable name
  Enum types : a = 1 : b = 2 : End Enum ' simple, type name
  Type point : x As Double : y As Double : End Type ' simple, type name
  Dim x As Module.types ' composed, type name
  Call Module.foo() ' composed, sub name
  Call .foo() ' composed, sub name, with-statement
  a = GetValue().x ' composed
  */

  auto const identifier_context_def = -(opDot >> x3::attr(true))
                                   >> *((functionCall | basic_identifier) >> opDot);

  auto const param_qualifier = kwByVal | kwByRef;
  auto const simple_type_identifier_def = //x3::attr(boost::optional<std::string>()) >>
                                        ( kwBoolean
                                        | kwByte
                                        | kwInteger
                                        | kwLong
                                        | kwSingle
                                        | kwDouble
                                        | kwCurrency
                                        | kwDate
                                        | kwString
                                        | kwObject
                                        | kwVariant);
/*
  auto const simple_type_identifier_def = (kwBoolean  >> x3::attr(vb6_ast::NativeType::Boolean))
                                        | (kwByte     >> x3::attr(vb6_ast::NativeType::Byte))
                                        | (kwInteger  >> x3::attr(vb6_ast::NativeType::Integer))
                                        | (kwLong     >> x3::attr(vb6_ast::NativeType::Long))
                                        | (kwSingle   >> x3::attr(vb6_ast::NativeType::Single))
                                        | (kwDouble   >> x3::attr(vb6_ast::NativeType::Double))
                                        | (kwCurrency >> x3::attr(vb6_ast::NativeType::Currency))
                                        | (kwDate     >> x3::attr(vb6_ast::NativeType::Date))
                                        | (kwString   >> x3::attr(vb6_ast::NativeType::String))
                                        | (kwObject   >> x3::attr(vb6_ast::NativeType::Object))
                                        | (kwVariant  >> x3::attr(vb6_ast::NativeType::Variant));
*/
  auto const complex_type_identifier_def = x3::omit[-(basic_identifier >> opDot)] >> basic_identifier;
  auto const type_identifier_def = (simple_type_identifier | complex_type_identifier);

  auto const array_size_decl = x3::omit["(" >> x3::int_ >> ")"];

  auto const tmp //= x3::rule<class tmp_class, std::tuple<bool, boost::optional<vb6_ast::simple_type_identifier>>>()
                 =   (kwAs >> kwNew >> x3::attr(true) >> type_identifier);
                   //| (kwAs >> x3::attr(false) >> type_identifier)
                   //| (x3::attr(false) >> x3::attr(boost::none));

  auto const tmp1 //= x3::rule<class tmp1_class, std::tuple<bool, boost::optional<vb6_ast::simple_type_identifier>>>()
                  = kwAs >> kwNew >> x3::attr(true) >> simple_type_identifier;
  auto const tmp2 //= x3::rule<class tmp2_class, std::tuple<bool, boost::optional<vb6_ast::simple_type_identifier>>>()
                  = kwAs >>          x3::attr(false) >> simple_type_identifier;

  auto const single_var_implicit_declaration_def = var_identifier >> -array_size_decl >> x3::attr(false) >> x3::attr(boost::none);

  auto const single_var_declaration_def = var_identifier >> -array_size_decl >> (tmp1);
  //auto const single_var_declaration_def = var_identifier >> -array_size_decl >> (tmp1 | tmp2);
  //auto const single_var_declaration_def = var_identifier >> -array_size_decl >> -(kwAs >> -(kwNew >> x3::attr(true)) >> type_identifier);
  auto const global_var_declaration_def = ( (kwDim >> x3::attr(vb6_ast::access_type::dim))
                                          | (kwGlobal >> x3::attr(vb6_ast::access_type::global))
                                          | (kwPublic >> x3::attr(vb6_ast::access_type::public_))
                                          | (kwPrivate >> x3::attr(vb6_ast::access_type::private_))
                                          ) >> -(kwWithEvents >> x3::attr(true)) >> (single_var_declaration % ',') >> cmdTermin;

  auto const private_or_public = (kwPrivate >> x3::attr(vb6_ast::access_type::private_))
                               | (kwPublic >> x3::attr(vb6_ast::access_type::public_))
                               | (x3::eps >> x3::attr(vb6_ast::access_type::na));

  auto const record_declaration_def = private_or_public >> kwType >> record_identifier >> cmdTermin
                                   //>> (single_var_declaration % cmdTermin) // this fails, but why?
                                   >> +(single_var_declaration >> cmdTermin)
                                   >> kwgEndType;

  auto const single_float = x3::rule<class single_float, float>("single_float")
                          = x3::lexeme[x3::float_ >> '!'];
  auto const double_float = x3::rule<class double_float, double>("double_float")
                          = x3::lexeme[x3::double_ >> '#'];
  auto const long_dec     = x3::rule<class long_dec, vb6_ast::long_dec>("long_dec")
                          = x3::lexeme[x3::int_ >> '&'];
  auto const long_hex     = x3::rule<class long_hex, vb6_ast::long_hex>("long_hex")
                          = x3::lexeme["&H" >> x3::hex >> '&'];
  auto const long_oct     = x3::rule<class long_oct, vb6_ast::long_oct>("long_oct")
                          = x3::lexeme["&0" >> x3::oct >> '&'];
  auto const integer_dec  = x3::rule<class integer_dec, vb6_ast::integer_dec>("integer_dec")
                          = x3::lexeme[x3::short_ >> '%']
                          | x3::short_;
  auto const integer_hex  = x3::rule<class integer_hex, vb6_ast::integer_hex>("integer_hex")
                          = x3::lexeme["&H" >> x3::hex >> '%'];
  auto const integer_oct  = x3::rule<class integer_oct, vb6_ast::integer_oct>("integer_oct")
                          = x3::lexeme["&0" >> x3::oct >> '%'];
  auto const currency     = x3::rule<class currency, double>("currency")
                          = x3::lexeme[x3::double_ >> '@'];

  x3::real_parser<float, x3::strict_real_policies<float>> const float_ = {};
  //x3::real_parser<double, x3::strict_real_policies<double>> const double_ = {};

  auto const const_expression_def = double_float
                                  | single_float
                                  | float_
                                  | long_dec
                                  | long_hex
                                  | long_oct
                                  | integer_dec
                                  | integer_hex
                                  | integer_oct
                                  | quoted_string
                                  | bool_const
                                  | (kwNothing >> x3::attr(vb6_ast::nothing()));

  namespace expr_take_1
  {
    struct factor_class;
    struct term_class;

    using factor_type = x3::rule<factor_class>;
    using term_type   = x3::rule<term_class>;

    factor_type const factor("factor");
    term_type   const term("term");

    auto const strong_op = opMult | opDiv | opDivint | opMod | opAnd;
    auto const weak_op   = opPlus | opMinus | opOr;

    auto const term_def = factor >> *(strong_op >> factor);
    auto const factor_def = '(' >> expression >> ')'
                          | const_expression
                          | functionCall // recursive
                          | decorated_variable
                          | (opNot >> factor);
    auto const simpleExpression = -(opPlus|opMinus) >> term >> *(weak_op >> term);

    BOOST_SPIRIT_DEFINE(
        factor
      , term
    )
  } // namespace expr_take_1

  namespace expr_take_2
  {
    // https://levelup.gitconnected.com/create-your-own-expression-parser-d1f622077796
    // https://panthema.net/2018/0912-Boost-Spirit-Tutorial/

    struct expr_class;
    struct mulexpr_class;
    struct powexpr_class;
    struct atom_class;

    using expr_type    = x3::rule<expr_class>;
    using mulexpr_type = x3::rule<mulexpr_class>;
    using powexpr_type = x3::rule<powexpr_class>;
    using atom_type    = x3::rule<atom_class>;

    expr_type    const expr("expr");
    mulexpr_type const mulexpr("mulexpr");
    powexpr_type const powexpr("powexpr");
    atom_type    const atom("atom");

    auto const addop = opPlus | opMinus;
    auto const mulop = opMult | opDiv;

    // expression  |  RPN
    // ------------+---------------
    // 2+3*4       |  2 3 4 * +
    // 2*3+4       |  2 3 * 4 +
    // (2+3)*4     |  2 3 + 4 *
    // 2^3*4+5     |  2 3 ^ 4 * 5 +
    // 2+3*4^5     |  2 3 4 5 ^ * +
    auto const expr_def    = mulexpr >> *(addop >> mulexpr);
    auto const mulexpr_def = powexpr >> *(mulop >> powexpr);
    auto const powexpr_def = (-(opPlus|opMinus) >> powexpr)
                           | (atom >> -("^" >> powexpr));
    auto const atom_def    = functionCall
                           | const_expression
                           | ("(" >> expr >> ")");

    BOOST_SPIRIT_DEFINE(
        expr
      , mulexpr
      , powexpr
      , atom
    )
  } // namespace expr_take_3

  auto const decorated_variable_def = identifier_context >> var_identifier;

  //auto const decorated_functionCall = x3::omit[identifier_context] >> functionCall;

  auto const expression_def = const_expression
                            | functionCall
                            | decorated_variable
                            ;

  auto const enum_declaration_def = private_or_public >> kwEnum >> enum_identifier >> cmdTermin
                                 >> +(basic_identifier >> -(opEqual >> const_expression) >> cmdTermin) // FED ???? basic_identifier?
                                 >> kwgEndEnum;
  auto const const_var_declaration_def = -kwPrivate >> kwConst
                                      >> ((single_var_declaration >> opEqual >> const_expression) % ',')
                                      >> cmdTermin;
  auto const param_decl_def = -(kwOptional >> x3::attr(true)) >> -param_qualifier
                           >> single_var_declaration // FED ??? this can have 'New', change it
                           >> -(opEqual >> const_expression); // default value for optional parameter

  // helper definition
  auto const param_list_decl = -(param_decl % ',');

  auto const external_sub_decl_def = private_or_public >> kwDeclare >> kwSub >> sub_identifier
                                  >> kwLib >> lib_name >> -(kwAlias >> alias_name)
                                  >> '(' >> param_list_decl >> ')'
                                  >> cmdTermin;
  auto const external_function_decl_def = private_or_public >> kwDeclare >> kwFunction >> func_identifier
                                       >> kwLib >> lib_name >> -(kwAlias >> alias_name)
                                       >> '(' >> param_list_decl >> ')' >> -(kwAs >> type_identifier)
                                       >> cmdTermin;
  auto const subHead_def = private_or_public >> kwSub
                        >> sub_identifier
                        >> '(' >> param_list_decl >> ')'
                        >> cmdTermin;
  auto const eventHead_def = private_or_public >> kwEvent
                          >> sub_identifier
                          >> '(' >> param_list_decl >> ')'
                          >> cmdTermin;
  auto const functionHead_def = private_or_public >> kwFunction >> func_identifier
                             >> '(' >> param_list_decl >> ')' >> -(kwAs >> type_identifier)
                             >> cmdTermin;
  auto const property_letHead_def = private_or_public >> (kwProperty > kwLet) >> prop_identifier
                                 >> '(' >> param_list_decl >> ')'
                                 >> cmdTermin;
  auto const property_setHead_def = private_or_public >> (kwProperty > kwSet) >> prop_identifier
                                 >> '(' >> param_list_decl >> ')'
                                 >> cmdTermin;
  auto const property_getHead_def = private_or_public >> (kwProperty > kwGet) >> prop_identifier
                                 >> '(' >> param_list_decl >> ')' >> -(kwAs >> type_identifier)
                                 >> cmdTermin;;

  auto const functionCall_def = func_identifier >> '(' >> -(expression % ',') >> ')';

  //auto const decorated_functionCall = x3::omit[identifier_context] >> functionCall;

  //auto const decorated_sub_identifier = x3::omit[identifier_context] >> sub_identifier;

  // these are defined after statement_block as they depend on it

  auto const subDef_def = subHead
                       >> statements::statement_block
                       >> kwgEndSub;
  auto const functionDef_def = functionHead
                            >> statements::statement_block
                            >> kwgEndFunction;
  //auto const propertyDef_def = (property_getHead | property_letHead | property_setHead)
  //                          >> statements::statement_block
  //                          >> kwgEndProperty;

  auto const attr_name = basic_identifier;
  auto const attributeDef_def = kwAttribute
                             >> attr_name >> opEqual >> quoted_string
                             >> cmdTermin;

  auto const option_item_def = kwOption > ( (kwExplicit              >> x3::attr(vb6_ast::module_option::explicit_))
                                          | (kwCompare > ((kwText    >> x3::attr(vb6_ast::module_option::compare_text)) |
                                                          (kwBinary  >> x3::attr(vb6_ast::module_option::compare_binary))))
                                          | (kwBase > ((x3::lit('0') >> x3::attr(vb6_ast::module_option::base_0)) |
                                                       (x3::lit('1') >> x3::attr(vb6_ast::module_option::base_1))))
                                          | ((kwPrivate > kwModule)  >> x3::attr(vb6_ast::module_option::private_module))
                                          ) >> cmdTermin;

  namespace STRICT_MODULE_STRUCTURE
  {
#if 0
    auto const option_block = *option_item;
#else
    auto const option_block = x3::rule<class option_block, vb6_ast::STRICT_MODULE_STRUCTURE::option_block>()
                            = -(   *(lonely_comment | empty_line | option_item)
                                //>> +option_item
                               );
#endif

    auto const preamble = *(attributeDef)
                       >> option_block;

    auto const declaration_def = lonely_comment // critical to have this as the first element
                               | empty_line
                               | global_var_declaration
                               | const_var_declaration
                               | enum_declaration
                               | record_declaration
                               | external_sub_decl
                               | external_function_decl
                               | eventHead
                               ;

    //auto const funcList = x3::rule<class funcList, vb6_ast::functionList>("funcList")
    //                    = *( subDef
    //                       | functionDef
    //                       //| propertyDef
    //                       );

    auto const func_subDef = subDef
                           | functionDef
                           //| propertyDef
                           ;

    auto const basModDef_def = preamble
                            >> (*declaration)
                            >> (*func_subDef);
    auto const clsModDef = preamble >> *declaration >> *func_subDef;
    auto const frmModDef = preamble >> /*formDef >>*/ *declaration >> *func_subDef;
    auto const ctlModDef = preamble >> /*formDef >>*/ *declaration >> *func_subDef;

    auto const unitDef = basModDef | clsModDef | frmModDef | ctlModDef;

    BOOST_SPIRIT_DEFINE(
        declaration
      , basModDef
    )
  } // namespace STRICT_MODULE_STRUCTURE

  auto const declaration_def = global_var_declaration
                             | const_var_declaration
                             | enum_declaration
                             | record_declaration
                             | external_sub_decl
                             | external_function_decl
                             | eventHead
                             ;

  auto const func_subDef = subDef
                         | functionDef
                         //| propertyDef
                         ;

  auto const basModDef_def = *(lonely_comment // critical to have this as the first element
                           | empty_line
                           | attributeDef
                           | option_item
                           | declaration
                           | func_subDef);

  auto const unitDef = basModDef;

  BOOST_SPIRIT_DEFINE(
      empty_line
    , lonely_comment
    , quoted_string
    , basic_identifier
    , identifier_context
    , decorated_variable
    , simple_type_identifier
    , complex_type_identifier
    , type_identifier
    , single_var_declaration
    , global_var_declaration
    , record_declaration
    , const_expression
    , expression
    , enum_declaration
    , const_var_declaration
    , param_decl
    , external_sub_decl
    , external_function_decl
    , subHead
    , eventHead
    , functionHead
    , property_letHead
    , property_setHead
    , property_getHead
    , subDef
    , functionDef
    , functionCall
    , attributeDef
    , option_item
    , declaration
    , basModDef
  )
    //, propertyDef
 }
