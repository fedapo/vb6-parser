//: vb6_ast.hpp

// vb6_parser
// Copyright (c) 2022 Federico Aponte
// This code is licensed under GNU Software License (see LICENSE.txt for details)

#pragma once

#include <boost/optional/optional.hpp>
#include <boost/spirit/home/x3/support/ast/variant.hpp>
#include <boost/spirit/home/x3/support/ast/position_tagged.hpp>

#include <map>
#include <string>
#include <vector>

namespace vb6_ast {

namespace x3 = boost::spirit::x3;

enum class NativeType
{
  Unspecified,
  Boolean,
  Byte,
  Integer,
  Long,
  Single,
  Double,
  Currency,
  Date,
  String,
  Object,
  Variant,
  NonNative
};

enum class rel_operator_type
{
  less,
  greater,
  less_equal,
  greater_equal,
  equal,
  not_equal
};

enum class operator_type
{
  less,
  greater,
  less_equal,
  greater_equal,
  equal,
  not_equal,
  plus,
  minus,
  mult,
  div,
  div_int,
  exp,
  diff,
  assign,
  amp,
  mod,
  imp,
  not_, // unary
  and_,
  xor_,
  is,
  like
};

enum class access_type
{
  na,
  dim,
  global,
  public_,
  private_,
  friend_
};

enum class param_qualifier
{
  byval,
  byref
};

enum class assignmentType
{
  na = 0,
  let = 1,
  set = 2
};

enum class localvardeclType
{
  Dim = 0,
  Static = 1
};

enum class gotoType
{
  goto_v = 0,
  gosub_v = 1
};

enum class onerror_type
{
  goto_0, // disables enabled error handler in the current procedureand resets it to Nothing
  goto_neg_1, // disables enabled exception in the current procedure and resets it to Nothing
  goto_label,
  exit_sub,
  exit_func,
  exit_property,
  resume_next
};

enum class resume_type
{
  implicit,
  next,
  line_nr,
  label
};

enum class exit_type
{
  sub,
  function,
  property,
  while_,
  do_,
  for_
};

enum class module_option
{
  explicit_,
  base_0,
  base_1,
  compare_text,
  compare_binary,
  private_module
};

struct func_call;

struct nothing
{
};

struct empty_line
{
};

struct lonely_comment //: std::string
{
  std::string content;
};

struct quoted_string : std::string
{
};

using var_identifier = std::string;

struct identifier_context : x3::position_tagged
{
  bool leading_dot = false;
  std::vector<x3::variant<x3::forward_ast<func_call>, std::string>> elements;
};

//using simple_type_identifier  = std::pair<boost::optional<std::string>, std::string>;
//using complex_type_identifier = std::pair<boost::optional<std::string>, std::string>;
using simple_type_identifier = std::string;
using complex_type_identifier = std::string;

// TODO use this as the class to encode the type in a declaration for variables and parameters
struct type_identifier : x3::position_tagged
{
  NativeType type = NativeType::NonNative;

  boost::optional<std::string> library_or_module;
  std::string nonnative_type;
};

// Ex.: frm As VB.Form
// Ex.: frm As New MyLib.MyClass
struct variable : x3::position_tagged
{
  std::string name;
  bool construct = false; // has the 'New' specifier
  //boost::optional<std::string> library_or_module;
  //simple_type_identifier type; // not sure...
  boost::optional<simple_type_identifier> type; // not sure...
};

struct decorated_variable : x3::position_tagged
{
  identifier_context ctx;
  var_identifier var;
};

// Ex.: Private g_FinalName As String, g_FinalType As Integer
struct global_var_decls : x3::position_tagged
{
  access_type at = access_type::na;
  bool with_events = false;
  std::vector<variable> vars;
};

struct integer_dec { short val; };
struct integer_hex { short val; };
struct integer_oct { short val; };
struct long_dec { int val; };
struct long_hex { int val; };
struct long_oct { int val; };

using const_expr = x3::variant<float, double,
                               long_dec, long_hex, long_oct,
                               integer_dec, integer_hex, integer_oct,
                               quoted_string, bool, nothing>;

// Ex.: x As Integer = 100
struct const_var : x3::position_tagged
{
  variable var;
  const_expr value;
};

struct const_var_stat : std::vector<const_var>
{
};

// Ex.: Public Type MyPoint: x As Integer: y As Integer: End Type
struct record : x3::position_tagged
{
  access_type at = access_type::na;
  std::string name;
  std::vector<variable> members;
};

// FED ???? instead of a const_expr consider using an integer (how large?) for the value
using enum_item = std::pair<std::string, boost::optional<const_expr>>;

// Ex.: Public Enum ComprKind: NoCompr = 0: Jpeg = 1: Tiff = 2: End Enum
struct vb_enum : x3::position_tagged
{
  access_type at = access_type::na;
  std::string name;
  std::vector<enum_item> values;
};

struct expression : x3::variant<
                      const_expr,
                      decorated_variable,
                      x3::forward_ast<func_call>>
{
  using base_type::base_type;
  using base_type::operator=;
};

struct func_call : x3::position_tagged
{
  std::string func_name;
  std::vector<expression> params;
};

// Ex.: Optional ByVal flag As Boolean = True
struct func_param : x3::position_tagged
{
  bool isoptional = false;
  boost::optional<param_qualifier> qualifier; // byval, byref
  variable var;
  boost::optional<const_expr> defvalue;
};

struct external_decl : x3::position_tagged
{
  std::string name;
  access_type at;
  std::vector<func_param> params;
};

// Ex.: Private Sub CalcTotal(ByVal algorithm As Integer)
struct subHead : x3::position_tagged
{
  access_type at = access_type::na;
  std::string name;
  std::vector<func_param> params;
};

// Ex.: Public Event OnShow(ByVal text As String)
struct eventHead : x3::position_tagged
{
  access_type at; // private, public, friend
  std::string name;
  std::vector<func_param> params;
};

// Ex.: Private Function GetMagicWord(ByVal spell As String) As String
struct functionHead : x3::position_tagged
{
  access_type at = access_type::na;
  std::string name;
  std::vector<func_param> params;
  boost::optional<simple_type_identifier> return_type; // not sure...
};

// Ex.: Public Property Let Title(str As String)
struct propertyLetHead : x3::position_tagged
{
  access_type at = access_type::na;
  std::string name;
  std::vector<func_param> params;
};

// Ex.: Public Property Set Title(str As String)
struct propertySetHead : x3::position_tagged
{
  access_type at = access_type::na;
  std::string name;
  std::vector<func_param> params;
};

// Ex.: Public Property Get Title() As String
struct propertyGetHead : x3::position_tagged
{
  access_type at = access_type::na;
  std::string name;
  std::vector<func_param> params; // FED ???? does Property Get take parameters? maybe for arrays?
  boost::optional<simple_type_identifier> return_type; // not sure...
};

// Ex.: Private Sub Beep Lib "kernel32" Alias "Beep" (ByVal freq As Integer, ByVal duration As Integer)
struct externalSub : x3::position_tagged
{
  access_type at = access_type::na;
  std::string name;
  std::string lib;
  std::string alias;
  std::vector<func_param> params;
};

// Ex.: Private Function Compress Lib "myzip" Alias "Compress" (ByVal str As String) As Integer
struct externalFunction : x3::position_tagged
{
  access_type at = access_type::na;
  std::string name;
  std::string lib;
  std::string alias;
  std::vector<func_param> params;
  boost::optional<simple_type_identifier> return_type; // not sure...
};

// statements (things that go into functions, subroutines and property definitions)
namespace statements {

struct assignStmt : x3::position_tagged
{
  assignmentType type = assignmentType::na;
  decorated_variable var;
  expression rhs;
};

struct localVarDeclStmt : x3::position_tagged
{
  localvardeclType type;
  std::vector<variable> vars;
};

struct redimStmt : x3::position_tagged
{
  bool preserve = false;
  decorated_variable var;
  std::vector<expression> newsize;
};

struct exitStmt : x3::position_tagged
{
  exit_type type; // Sub, Function, Property, For, Do, While, ...
};

struct gotoStmt : x3::position_tagged
{
  gotoType type;
  std::string label;
};

struct onerrorStmt : x3::position_tagged
{
  onerror_type type;
  std::string label;
};

struct resumeStmt : x3::position_tagged
{
  resume_type type;
  x3::variant<std::string, int> label_or_line_nr;
};

struct labelStmt : x3::position_tagged
{
  std::string label;
};

struct callStmt : x3::position_tagged
{
  std::string sub_name;
  std::vector<expression> params;
  bool explicit_call;
};

struct raiseeventStmt : x3::position_tagged
{
  std::string event_name;
  std::vector<expression> params;
};

// forward declarations for compound statements
struct whileStmt;
struct doStmt;
struct dowhileStmt;
struct loopwhileStmt;
struct dountilStmt;
struct loopuntilStmt;
struct forStmt;
struct foreachStmt;
struct ifelseStmt;
struct withStmt;
struct selectStmt;

// FED ???? pazzesco! senza questi #define non compila
//#define BOOST_MPL_CFG_NO_PREPROCESSED_HEADERS
//#define BOOST_MPL_LIMIT_LIST_SIZE 30
struct singleStmt : x3::variant<
  lonely_comment,
  empty_line,
  assignStmt,
  localVarDeclStmt,
  redimStmt,
  exitStmt,
  gotoStmt,
  onerrorStmt,
  resumeStmt,
  labelStmt,
  callStmt,
  raiseeventStmt,
  // compound statements
  x3::forward_ast<whileStmt>,
  x3::forward_ast<doStmt>,
  x3::forward_ast<dowhileStmt>,
  x3::forward_ast<loopwhileStmt>,
  x3::forward_ast<dountilStmt>,
  x3::forward_ast<loopuntilStmt>,
  x3::forward_ast<forStmt>,
  x3::forward_ast<foreachStmt>,
  x3::forward_ast<ifelseStmt>,
  x3::forward_ast<selectStmt>,
  x3::forward_ast<withStmt>
>
{
  using base_type::base_type;
  using base_type::operator=;
};

using statement_block = std::vector<singleStmt>;

// compound statements

struct whileStmt : x3::position_tagged
{
  expression condition;
  statement_block block;
};

struct doStmt : x3::position_tagged
{
  statement_block block;
};

struct dowhileStmt : x3::position_tagged
{
  expression condition;
  statement_block block;
};

struct loopwhileStmt : x3::position_tagged
{
  statement_block block;
  expression condition;
};

struct dountilStmt : x3::position_tagged
{
  expression condition;
  statement_block block;
};

struct loopuntilStmt : x3::position_tagged
{
  statement_block block;
  expression condition;
};

struct forStmt : x3::position_tagged
{
  decorated_variable for_variable;
  expression from;
  expression to;
  boost::optional<expression> step;
  statement_block block;
};

// TODO
struct foreachStmt : x3::position_tagged
{
  decorated_variable for_variable;
  expression container;
  statement_block block;
};

struct if_branch : x3::position_tagged
{
  expression condition;
  statement_block block;
};

// TODO
struct ifelseStmt : x3::position_tagged
{
  //expression condition;
  //statement_block block;
#ifdef SIMPLE_IF_STATEMENT
  if_branch first_branch;
#else
  if_branch first_branch;
  std::vector<if_branch> if_branches;
#endif
  boost::optional<statement_block> else_branch;
};

struct withStmt : x3::position_tagged
{
  decorated_variable with_variable;
  statement_block block;
};

// TODO
struct case_relational_expr : x3::position_tagged
{
  rel_operator_type rel_op;
  expression rexpr;
};

// TODO
struct case_block : x3::position_tagged
{
  expression case_expr;
  //std::vector<x3::variant<case_relational_expr, std::pair<expression, expression>>> case_expr_list;
  statement_block block;
};

struct selectStmt : x3::position_tagged
{
  expression condition;
  std::vector<case_block> blocks;
};

} // namespace statements

struct subDef : x3::position_tagged
{
  subHead header;
  statements::statement_block statements;
};

struct functionDef : x3::position_tagged
{
  functionHead header;
  statements::statement_block statements;
};

struct get_prop : x3::position_tagged
{
  std::string name;
  access_type at;
  std::vector<func_param> params;
  statements::statement_block statements;
};

struct let_prop : x3::position_tagged
{
  std::string name;
  access_type at;
  std::vector<func_param> params;
  statements::statement_block stats;
};

struct set_prop : x3::position_tagged
{
  std::string name;
  access_type at;
  statements::statement_block statements;
};

namespace STRICT_MODULE_STRUCTURE
{
  struct module_attributes : x3::position_tagged
    , std::map<std::string, quoted_string>
  {
  };

  struct option_block : x3::position_tagged
  {
    std::vector<
#if 0
      module_option
#else
      x3::variant<lonely_comment, empty_line, module_option>
#endif
    > items;
  };

  struct declaration : x3::position_tagged
                     , x3::variant<
                            lonely_comment,
                            empty_line,
                            global_var_decls,
                            const_var_stat,
                            vb_enum,
                            record,
                            //external_decl,
                            externalSub,
                            externalFunction,
                            eventHead
                       >
  {
    using base_type::base_type;
    using base_type::operator=;
  };

  using functionList = std::vector<
                         x3::variant<//lonely_comment,
                           functionDef,
                           subDef
                           //get_prop,
                           //let_prop,
                           //set_prop
                         >
                       >;

  struct vb_module : x3::position_tagged
  {
    module_attributes attrs;
    option_block opts;
    std::vector<declaration> declarations;
    functionList functions;
  };
} // STRICT_MODULE_STRUCTURE

struct declaration : x3::position_tagged
                   , x3::variant<
                       global_var_decls,
                       const_var_stat,
                       vb_enum,
                       record,
                       //external_decl,
                       externalSub,
                       externalFunction,
                       eventHead
                      >
{
  using base_type::base_type;
  using base_type::operator=;
};

using module_attribute = std::pair<std::string, std::string>;

using vb_module = std::vector<
                    x3::variant<
                      lonely_comment,
                      empty_line,
                      module_attribute,
                      module_option,
                      declaration,
                      functionDef,
                      subDef
                      //get_prop,
                      //let_prop,
                      //set_prop
                    >
                  >;
/*
struct vb_module : x3::position_tagged
{
  std::vector<
    x3::variant<
    lonely_comment,
    empty_line,
    module_attribute,
    module_option,
    declaration,
    functionDef,
    subDef
    //get_prop,
    //let_prop,
    //set_prop
    >
  > items;
};
*/

} // namespace vb6_ast