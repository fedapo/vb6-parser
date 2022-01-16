//: vb6_parser.hpp

// vb6_parser
// Copyright (c) 2022 Federico Aponte
// This code is licensed under GNU Software License (see LICENSE.txt for details)

#pragma once

#include "vb6_ast.hpp"

#include <boost/spirit/home/x3.hpp>

#include <iostream>
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
support TypeOf
----
Implicit type variables
----
*/

namespace vb6_grammar {

  std::string getParserInfo();

  namespace x3 = boost::spirit::x3;

  auto const kwRem = x3::no_case[x3::lit("Rem")];

  auto const comment = x3::rule<class comment, std::string>("comment")
                     = (kwRem | '\'') >> x3::no_skip[*(x3::char_ - x3::eol)] >> x3::eol;
                     //= (kwRem | '\'') >> x3::seek[x3::eol];

#if 0
  struct skip_class;
  //using skip_type = x3::rule<skip_class, std::string>;
  using skip_type = x3::rule<skip_class, x3::unused_type>;
  skip_type const skip("vb6.skip");
  //BOOST_SPIRIT_DECLARE(skip_type);

  auto dbg_comment = [](auto& ctx) { std::cout << "comment: " << x3::_attr(ctx) << '\n'; };

  // x3::blank only matches spaces or tabs, no newlines
  auto const skip_def = x3::blank | (comment [dbg_comment]);
                      //= x3::blank | ('_' >> x3::eol) | comment;

  BOOST_SPIRIT_DEFINE(skip)
#else
  using skip_type = x3::ascii::blank_type;
  skip_type const skip;
#endif

  struct empty_line_class              ;//: x3::annotation_base, error_handler_base {};
  struct lonely_comment_class          ;//: x3::annotate_on_success {};
  struct quoted_string_class           ;//: x3::annotate_on_success {};
  struct basic_identifier_class        ;//: x3::annotate_on_success {};
  struct decorated_variable_class      ;//: x3::annotate_on_success {};
  struct identifier_context_class      ;//: x3::annotate_on_success {};
  struct simple_type_identifier_class  ;//: x3::annotate_on_success {};
  struct complex_type_identifier_class ;//: x3::annotate_on_success {};
  struct type_identifier_class         ;//: x3::annotate_on_success {};
  struct single_var_declaration_class  ;//: x3::annotate_on_success {};
  struct global_var_declaration_class  ;//: x3::annotate_on_success {};
  struct record_declaration_class      ;//: x3::annotate_on_success {};
  struct const_expression_class        ;//: x3::annotate_on_success {};
  struct expression_class              ;//: x3::annotate_on_success {};
  struct enum_declaration_class        ;//: x3::annotate_on_success {};
  struct const_var_declaration_class   ;//: x3::annotate_on_success {};
  struct param_decl_class              ;//: x3::annotate_on_success {};
  struct external_sub_decl_class       ;//: x3::annotate_on_success {};
  struct external_function_decl_class  ;//: x3::annotate_on_success {};
  struct subHead_class                 ;//: x3::annotate_on_success {};
  struct eventHead_class               ;//: x3::annotate_on_success {};
  struct functionHead_class            ;//: x3::annotate_on_success {};
  struct subDef_class                  ;//: x3::annotate_on_success {};
  struct functionDef_class             ;//: x3::annotate_on_success {};
  struct propertyDef_class             ;//: x3::annotate_on_success {};
  struct functionCall_class            ;//: x3::annotate_on_success {};
  struct property_letHead_class        ;//: x3::annotate_on_success {};
  struct property_setHead_class        ;//: x3::annotate_on_success {};
  struct property_getHead_class        ;//: x3::annotate_on_success {};
  struct attributeDef_class            ;//: x3::annotate_on_success {};
  struct option_item_class             ;//: x3::annotate_on_success {};

  namespace statements {
    struct singleStmt_class      ;//: x3::annotate_on_success {};
    struct statement_block_class ;//: x3::annotate_on_success {};
    struct assignmentStmt_class  ;//: x3::annotate_on_success {};
    struct localvardeclStmt_class;//: x3::annotate_on_success {};
    struct redimStmt_class       ;//: x3::annotate_on_success {};
    struct exitStmt_class        ;//: x3::annotate_on_success {};
    struct gotoStmt_class        ;//: x3::annotate_on_success {};
    struct onerrorStmt_class     ;//: x3::annotate_on_success {};
    struct resumeStmt_class      ;//: x3::annotate_on_success {};
    struct labelStmt_class       ;//: x3::annotate_on_success {};
    struct callimplicitStmt_class;//: x3::annotate_on_success {};
    struct callexplicitStmt_class;//: x3::annotate_on_success {};
    struct raiseeventStmt_class  ;//: x3::annotate_on_success {};

    // compound statements
    struct whileStmt_class       ;//: x3::annotate_on_success {};
    struct doStmt_class          ;//: x3::annotate_on_success {};
    struct dowhileStmt_class     ;//: x3::annotate_on_success {};
    struct loopwhileStmt_class   ;//: x3::annotate_on_success {};
    struct dountilStmt_class     ;//: x3::annotate_on_success {};
    struct loopuntilStmt_class   ;//: x3::annotate_on_success {};
    struct forStmt_class         ;//: x3::annotate_on_success {};
    struct foreachStmt_class     ;//: x3::annotate_on_success {};
    struct ifelseStmt_class      ;//: x3::annotate_on_success {};
    struct withStmt_class        ;//: x3::annotate_on_success {};
    struct selectStmt_class      ;//: x3::annotate_on_success {};
  }

  using empty_line_type              = x3::rule<empty_line_class,              vb6_ast::empty_line>;
  using lonely_comment_type          = x3::rule<lonely_comment_class,          vb6_ast::lonely_comment>;
  using quoted_string_type           = x3::rule<quoted_string_class,           vb6_ast::quoted_string>;
  using basic_identifier_type        = x3::rule<basic_identifier_class,        vb6_ast::var_identifier>;
  using decorated_variable_type      = x3::rule<decorated_variable_class,      vb6_ast::decorated_variable>;
  using identifier_context_type      = x3::rule<identifier_context_class,      vb6_ast::identifier_context>;
  using simple_type_identifier_type  = x3::rule<simple_type_identifier_class,  vb6_ast::simple_type_identifier>;
  using complex_type_identifier_type = x3::rule<complex_type_identifier_class, vb6_ast::complex_type_identifier>;
  using type_identifier_type         = x3::rule<type_identifier_class,         std::string>;
  using single_var_declaration_type  = x3::rule<single_var_declaration_class,  vb6_ast::variable>;
  using global_var_declaration_type  = x3::rule<global_var_declaration_class,  vb6_ast::global_var_decls>;
  using record_declaration_type      = x3::rule<record_declaration_class,      vb6_ast::record>;
  using const_expression_type        = x3::rule<const_expression_class,        vb6_ast::const_expr>;
  using expression_type              = x3::rule<expression_class,              vb6_ast::expression>;
  using enum_declaration_type        = x3::rule<enum_declaration_class,        vb6_ast::vb_enum>;
  using const_var_declaration_type   = x3::rule<const_var_declaration_class,   vb6_ast::const_var_stat>;
  using param_decl_type              = x3::rule<param_decl_class,              vb6_ast::func_param>;
  using external_sub_decl_type       = x3::rule<external_sub_decl_class,       vb6_ast::externalSub>;
  using external_function_decl_type  = x3::rule<external_function_decl_class,  vb6_ast::externalFunction>;
  using subHead_type                 = x3::rule<subHead_class,                 vb6_ast::subHead>;
  using eventHead_type               = x3::rule<eventHead_class,               vb6_ast::eventHead>;
  using functionHead_type            = x3::rule<functionHead_class,            vb6_ast::functionHead>;
  using property_letHead_type        = x3::rule<property_letHead_class,        vb6_ast::propertyLetHead>;
  using property_setHead_type        = x3::rule<property_setHead_class,        vb6_ast::propertySetHead>;
  using property_getHead_type        = x3::rule<property_getHead_class,        vb6_ast::propertyGetHead>;
  using subDef_type                  = x3::rule<subDef_class,                  vb6_ast::subDef>;
  using functionDef_type             = x3::rule<functionDef_class,             vb6_ast::functionDef>;
  //using propertyDef_type             = x3::rule<propertyDef_class,             vb6_ast::propertyDef>;
  using functionCall_type            = x3::rule<functionCall_class,            vb6_ast::func_call>;
  using attributeDef_type            = x3::rule<attributeDef_class,            std::pair<std::string, vb6_ast::quoted_string>>;
  using option_item_type             = x3::rule<option_item_class,             vb6_ast::module_option>;

  namespace statements {
    using singleStmt_type       = x3::rule<singleStmt_class,       vb6_ast::statements::singleStmt>;
    using statement_block_type  = x3::rule<statement_block_class,  vb6_ast::statements::statement_block>;
    using assignmentStmt_type   = x3::rule<assignmentStmt_class,   vb6_ast::statements::assignStmt>;
    using localvardeclStmt_type = x3::rule<localvardeclStmt_class, vb6_ast::statements::localVarDeclStmt>;
    using redimStmt_type        = x3::rule<redimStmt_class,        vb6_ast::statements::redimStmt>;
    using exitStmt_type         = x3::rule<exitStmt_class,         vb6_ast::statements::exitStmt>;
    using gotoStmt_type         = x3::rule<gotoStmt_class,         vb6_ast::statements::gotoStmt>;
    using onerrorStmt_type      = x3::rule<onerrorStmt_class,      vb6_ast::statements::onerrorStmt>;
    using resumeStmt_type       = x3::rule<resumeStmt_class,       vb6_ast::statements::resumeStmt>;
    using labelStmt_type        = x3::rule<labelStmt_class,        vb6_ast::statements::labelStmt>;
    using callimplicitStmt_type = x3::rule<callimplicitStmt_class, vb6_ast::statements::callStmt>;
    using callexplicitStmt_type = x3::rule<callexplicitStmt_class, vb6_ast::statements::callStmt>;
    using raiseeventStmt_type   = x3::rule<raiseeventStmt_class,   vb6_ast::statements::raiseeventStmt>;

    // compound statements
    using whileStmt_type        = x3::rule<whileStmt_class,        vb6_ast::statements::whileStmt>;
    using doStmt_type           = x3::rule<doStmt_class,           vb6_ast::statements::doStmt>;
    using dowhileStmt_type      = x3::rule<dowhileStmt_class,      vb6_ast::statements::dowhileStmt>;
    using loopwhileStmt_type    = x3::rule<loopwhileStmt_class,    vb6_ast::statements::loopwhileStmt>;
    using dountilStmt_type      = x3::rule<dountilStmt_class,      vb6_ast::statements::dountilStmt>;
    using loopuntilStmt_type    = x3::rule<loopuntilStmt_class,    vb6_ast::statements::loopuntilStmt>;
    using forStmt_type          = x3::rule<forStmt_class,          vb6_ast::statements::forStmt>;
    using foreachStmt_type      = x3::rule<foreachStmt_class,      vb6_ast::statements::foreachStmt>;
    using ifelseStmt_type       = x3::rule<ifelseStmt_class,       vb6_ast::statements::ifelseStmt>;
    using withStmt_type         = x3::rule<withStmt_class,         vb6_ast::statements::withStmt>;
    using selectStmt_type       = x3::rule<selectStmt_class,       vb6_ast::statements::selectStmt>;

    using ifBranch_type    = x3::rule<class ifBranch,    vb6_ast::statements::if_branch>;
    using elsifBranch_type = x3::rule<class elsifBranch, vb6_ast::statements::if_branch>;
    using elseBranch_type  = x3::rule<class elseBranch,  vb6_ast::statements::statement_block>;

    ifBranch_type    const ifBranch("ifBranch");
    elsifBranch_type const elsifBranch("elsifBranch");
    elseBranch_type  const elseBranch("elseBranch");

    using case_block_type = x3::rule<class case_block, vb6_ast::statements::case_block>;
    case_block_type const case_block("case_block");
  }

  namespace STRICT_MODULE_STRUCTURE
  {
    struct declaration_class; //: x3::annotation_base, error_handler_base {};
    using declaration_type = x3::rule<declaration_class, vb6_ast::STRICT_MODULE_STRUCTURE::declaration>;
    declaration_type const declaration("declaration");

    struct basModDef_class; //: x3::annotation_base, error_handler_base {};
    using basModDef_type = x3::rule<basModDef_class, vb6_ast::STRICT_MODULE_STRUCTURE::vb_module>;
    basModDef_type const basModDef("basModDef");

    BOOST_SPIRIT_DECLARE(
        declaration_type
      , basModDef_type
    );
  }

  struct declaration_class; //: x3::annotation_base, error_handler_base {};
  using declaration_type = x3::rule<class declaration, vb6_ast::declaration>;
  declaration_type const declaration("declaration");

  struct basModDef_class; //: x3::annotation_base, error_handler_base {};
  using basModDef_type = x3::rule<basModDef_class, vb6_ast::vb_module>;
  basModDef_type const basModDef("basModDef");

  empty_line_type              const empty_line              ("empty_line");
  lonely_comment_type          const lonely_comment          ("lonely_comment");
  quoted_string_type           const quoted_string           ("quoted_string");
  basic_identifier_type        const basic_identifier        ("basic_identifier");
  decorated_variable_type      const decorated_variable      ("decorated_variable");
  identifier_context_type      const identifier_context      ("identifier_context");
  simple_type_identifier_type  const simple_type_identifier  ("simple_type_identifier");
  complex_type_identifier_type const complex_type_identifier ("complex_type_identifier");
  type_identifier_type         const type_identifier         ("type_identifier");
  single_var_declaration_type  const single_var_declaration  ("single_var_declaration");
  global_var_declaration_type  const global_var_declaration  ("global_var_declaration");
  record_declaration_type      const record_declaration      ("record_declaration");
  const_expression_type        const const_expression        ("const_expression");
  expression_type              const expression              ("expression");
  enum_declaration_type        const enum_declaration        ("enum_declaration");
  const_var_declaration_type   const const_var_declaration   ("const_var_declaration");
  param_decl_type              const param_decl              ("param_decl");
  external_sub_decl_type       const external_sub_decl       ("external_sub_decl");
  external_function_decl_type  const external_function_decl  ("external_function_decl");
  subHead_type                 const subHead                 ("subHead");
  eventHead_type               const eventHead               ("eventHead");
  functionHead_type            const functionHead            ("functionHead");
  property_letHead_type        const property_letHead        ("property_letHead");
  property_setHead_type        const property_setHead        ("property_setHead");
  property_getHead_type        const property_getHead        ("property_getHead");
  subDef_type                  const subDef                  ("subDef");
  functionDef_type             const functionDef             ("functionDef");
  //propertyDef_type             const propertyDef             ("propertyDef");
  functionCall_type            const functionCall            ("functionCall");
  attributeDef_type            const attributeDef            ("attributeDef");
  option_item_type             const option_item             ("option_item");

  namespace statements {
    singleStmt_type        const singleStmt       ("singleStmt");
    statement_block_type   const statement_block  ("statement_block");
    assignmentStmt_type    const assignmentStmt   ("assignmentStmt");
    localvardeclStmt_type  const localvardeclStmt ("localvardeclStmt");
    redimStmt_type         const redimStmt        ("redimStmt");
    exitStmt_type          const exitStmt         ("exitStmt");
    gotoStmt_type          const gotoStmt         ("gotoStmt");
    onerrorStmt_type       const onerrorStmt      ("onerrorStmt");
    resumeStmt_type        const resumeStmt       ("resumeStmt");
    labelStmt_type         const labelStmt        ("labelStmt");
    callimplicitStmt_type  const callimplicitStmt ("callimplicitStmt");
    callexplicitStmt_type  const callexplicitStmt ("callexplicitStmt");
    raiseeventStmt_type    const raiseeventStmt   ("raiseeventStmt");

    // compound statements
    whileStmt_type         const whileStmt        ("whileStmt");
    doStmt_type            const doStmt           ("doStmt");
    dowhileStmt_type       const dowhileStmt      ("dowhileStmt");
    loopwhileStmt_type     const loopwhileStmt    ("loopwhileStmt");
    dountilStmt_type       const dountilStmt      ("dountilStmt");
    loopuntilStmt_type     const loopuntilStmt    ("loopuntilStmt");
    forStmt_type           const forStmt          ("forStmt");
    foreachStmt_type       const foreachStmt      ("foreachStmt");
    ifelseStmt_type        const ifelseStmt       ("ifelseStmt");
    withStmt_type          const withStmt         ("withStmt");
    selectStmt_type        const selectStmt       ("selectStmt");
  }

  auto const var_identifier    = basic_identifier;
  auto const record_identifier = basic_identifier;
  auto const enum_identifier   = basic_identifier;
  auto const sub_identifier    = basic_identifier;
  auto const func_identifier   = basic_identifier;
  auto const event_identifier  = basic_identifier;
  auto const prop_identifier   = basic_identifier;
  auto const lib_name          = quoted_string;
  auto const alias_name        = quoted_string;

  /*
  template <typename Iterator, typename Context>
  bool parse_rule(
      ????_type rule_
    , Iterator& first, Iterator const& last
    , Context const& context, ????_type::attribute_type& attr);
  */
  BOOST_SPIRIT_DECLARE(
      empty_line_type
    , lonely_comment_type
    , quoted_string_type
    , basic_identifier_type
    , identifier_context_type
    , decorated_variable_type
    , simple_type_identifier_type
    , complex_type_identifier_type
    , type_identifier_type
    , single_var_declaration_type
    , global_var_declaration_type
    , record_declaration_type
    , const_expression_type
    , expression_type
    , enum_declaration_type
    , const_var_declaration_type
    , param_decl_type
    , external_sub_decl_type
    , external_function_decl_type
    , subHead_type
    , eventHead_type
    , functionHead_type
    , property_letHead_type
    , property_setHead_type
    , property_getHead_type
    , subDef_type
    , functionDef_type
    //, propertyDef_type
    , functionCall_type
    , attributeDef_type
    , option_item_type
    , declaration_type
    , basModDef_type
  )

  BOOST_SPIRIT_DECLARE(
      statements::singleStmt_type
    , statements::statement_block_type
    , statements::assignmentStmt_type
    , statements::localvardeclStmt_type
    , statements::redimStmt_type
    , statements::exitStmt_type
    , statements::gotoStmt_type
    , statements::onerrorStmt_type
    , statements::resumeStmt_type
    , statements::labelStmt_type
    , statements::callimplicitStmt_type
    , statements::callexplicitStmt_type
    , statements::raiseeventStmt_type
  )

  BOOST_SPIRIT_DECLARE(
      statements::ifBranch_type
    , statements::elsifBranch_type
    , statements::elseBranch_type
    , statements::case_block_type
  )

  // compound statements
  BOOST_SPIRIT_DECLARE(
      statements::whileStmt_type
    , statements::doStmt_type
    , statements::dowhileStmt_type
    , statements::loopwhileStmt_type
    , statements::dountilStmt_type
    , statements::loopuntilStmt_type
    , statements::forStmt_type
    , statements::foreachStmt_type
    , statements::ifelseStmt_type
    , statements::withStmt_type
    , statements::selectStmt_type
  )
}