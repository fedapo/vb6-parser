//: cpp_ast_printer.hpp

// vb6_parser
// Copyright (c) 2022 Federico Aponte
// This code is licensed under GNU Software License (see LICENSE.txt for details)

#pragma once

#include "vb6_ast.hpp"
#include <iostream>

class cpp_ast_printer
{
public:
  explicit cpp_ast_printer(std::ostream& os) : os(os)
  {
  }

  void operator()(vb6_ast::empty_line const&) const;
  void operator()(vb6_ast::lonely_comment const&) const;
  void operator()(vb6_ast::identifier_context const&) const;
  void operator()(vb6_ast::variable const&) const;
  void operator()(vb6_ast::decorated_variable const&) const;
  void operator()(vb6_ast::const_expr const&) const;
  void operator()(vb6_ast::expression const&) const;
  void operator()(vb6_ast::func_call const&) const;
  void operator()(vb6_ast::global_var_decls const&) const;
  void operator()(vb6_ast::const_var_stat const&) const;
  void operator()(vb6_ast::record const&) const;
  void operator()(vb6_ast::vb_enum const&) const;
  void operator()(vb6_ast::func_param const&) const;
  void operator()(vb6_ast::externalSub const&) const;
  void operator()(vb6_ast::externalFunction const&) const;
  void operator()(vb6_ast::subHead const&) const;
  void operator()(vb6_ast::eventHead const&) const;
  void operator()(vb6_ast::functionHead const&) const;
  void operator()(vb6_ast::propertyLetHead const&) const;
  void operator()(vb6_ast::propertySetHead const&) const;
  void operator()(vb6_ast::propertyGetHead const&) const;
  void operator()(vb6_ast::subDef const&) const;
  void operator()(vb6_ast::functionDef const&) const;

  void operator()(vb6_ast::module_attribute) const;
  void operator()(vb6_ast::module_option opt) const;
  //void operator()(vb6_ast::STRICT_MODULE_STRUCTURE::module_attributes const&) const;
  void operator()(vb6_ast::STRICT_MODULE_STRUCTURE::declaration const&) const;
  void operator()(vb6_ast::STRICT_MODULE_STRUCTURE::vb_module const&) const;
  void operator()(vb6_ast::declaration const&) const;
  void operator()(vb6_ast::vb_module const&) const;

  void operator()(vb6_ast::statements::assignStmt const&) const;
  void operator()(vb6_ast::statements::exitStmt const&) const;
  void operator()(vb6_ast::statements::gotoStmt const&) const;
  void operator()(vb6_ast::statements::onerrorStmt const&) const;
  void operator()(vb6_ast::statements::resumeStmt const&) const;
  void operator()(vb6_ast::statements::localVarDeclStmt const&) const;
  void operator()(vb6_ast::statements::redimStmt const&) const;
  void operator()(vb6_ast::statements::callStmt const&) const;
  void operator()(vb6_ast::statements::raiseeventStmt const&) const;
  void operator()(vb6_ast::statements::labelStmt const&) const;
  void operator()(vb6_ast::statements::whileStmt const&) const;
  void operator()(vb6_ast::statements::doStmt const&) const;
  void operator()(vb6_ast::statements::dowhileStmt const&) const;
  void operator()(vb6_ast::statements::loopwhileStmt const&) const;
  void operator()(vb6_ast::statements::dountilStmt const&) const;
  void operator()(vb6_ast::statements::loopuntilStmt const&) const;
  void operator()(vb6_ast::statements::forStmt const&) const;
  void operator()(vb6_ast::statements::foreachStmt const&) const;
  void operator()(vb6_ast::statements::ifelseStmt const&) const;
  void operator()(vb6_ast::statements::withStmt const&) const;
  void operator()(vb6_ast::statements::case_block const&) const;
  void operator()(vb6_ast::statements::selectStmt const&) const;
  void operator()(vb6_ast::statements::singleStmt const&) const;

private:
  void print_type(vb6_ast::access_type) const;
  void print_block(vb6_ast::statements::statement_block const&) const;

  std::ostream& os;
  mutable int indent = 0;
  static int indent_size;
};