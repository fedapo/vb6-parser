//: raw_ast_printer.cpp

// vb6_parser
// Copyright (c) 2022 Federico Aponte
// This code is licensed under GNU Software License (see LICENSE.txt for details)

#include "raw_ast_printer.hpp"
#include <boost/variant/apply_visitor.hpp>

using namespace std;

// static
int raw_ast_printer::indent_size = 4;

void raw_ast_printer::operator()(vb6_ast::empty_line const& ast) const
{
  // (empty_line)
  os << string(indent, ' ') << '(' << typeid(ast).name() << ")\n";
}

void raw_ast_printer::operator()(vb6_ast::lonely_comment const& ast) const
{
  // (lonely_comment "this is a comment")
  os << string(indent, ' ') << '(' << typeid(ast).name() << " \"" << ast.content << "\")\n";
}

void raw_ast_printer::operator()(vb6_ast::identifier_context const& ast) const
{
  // (identifier_context no_dot (func_call foo) depth)
  os << string(indent, ' ') << '(' << typeid(ast).name();

  os << (ast.leading_dot ? "dot" : "no_dot");

  struct {
    void operator()(string s) { os << s; }
    void operator()(vb6_ast::func_call const& s) { (*p)(s); }
    ostream& os;
    raw_ast_printer const* p;
  } visitor{ os, this };

  for(auto& el : ast.elements)
  {
    boost::apply_visitor(visitor, el.get());
    os << " ";
  }

  os << ")\n";
}

void raw_ast_printer::operator()(vb6_ast::variable const& ast) const
{
  // (variable x Integer no_new)
  os << string(indent, ' ') << '(' << typeid(ast).name()
     << " " << ast.name;

  if(ast.type)
  {
    os << " " << *ast.type;
    if(ast.construct)
      os << " new";
    //if(ast.library_or_module)
    //  os << *ast.library_or_module << '.';
  }

  os << ")\n";
}

void raw_ast_printer::operator()(vb6_ast::decorated_variable const& ast) const
{
  os << string(indent, ' ') << '(' << typeid(ast).name()
     << " " << ast.var;
  (*this)(ast.ctx);
  os << ")\n";
}

void raw_ast_printer::operator()(vb6_ast::func_call const& ast) const
{
  os << string(indent, ' ') << '(' << typeid(ast).name()
     << " " << ast.func_name;

  bool first = true;
  for(auto& el : ast.params)
  {
    if(first)
      first = false;
    else
      os << ", ";

    (*this)(el);
  }

  os << ")\n";
}

void raw_ast_printer::operator()(vb6_ast::global_var_decls const& ast) const
{
  os << string(indent, ' ') << '(' << typeid(ast).name();

  print_type(ast.at);

  bool first = true;
  for(auto& el : ast.vars)
  {
    if(first)
      first = false;
    else
      os << ", ";

    (*this)(el);
  }

  os << ")\n";
}

void raw_ast_printer::operator()(vb6_ast::const_expr const& ast) const
{
  os << string(indent, ' ') << '(' << typeid(ast).name();

  struct {
    void operator()(float v) { os << v << '!'; }
    void operator()(double v) { os << v << '#'; }
    void operator()(vb6_ast::long_dec v) { os << v.val << '&'; }
    void operator()(vb6_ast::long_hex v) { os << "&H" << noshowbase << hex << v.val << dec << '&'; }
    void operator()(vb6_ast::long_oct v) { os << "&0" << noshowbase << oct << v.val << dec << '&'; }
    void operator()(vb6_ast::integer_dec v) { os << v.val << '%'; }
    void operator()(vb6_ast::integer_hex v) { os << "&H" << noshowbase << hex << v.val << dec << '%'; }
    void operator()(vb6_ast::integer_oct v) { os << "&0" << noshowbase << hex << v.val << dec << '%'; }
    void operator()(bool v) { os << (v ? "True" : "False"); }
    void operator()(vb6_ast::quoted_string const& s) { os << "\"" << s << "\""; }
    void operator()(vb6_ast::nothing const&) { os << "Nothing"; }
    ostream& os;
  } visitor{os};

  boost::apply_visitor(visitor, ast.get());

  os << ")\n";
}

void raw_ast_printer::operator()(vb6_ast::expression const& ast) const
{
  os << string(indent, ' ') << '(' << typeid(ast).name();

  boost::apply_visitor(*this, ast.get());

  os << ")\n";
}

void raw_ast_printer::operator()(vb6_ast::const_var_stat const& ast) const
{
  os << string(indent, ' ') << '(' << typeid(ast).name();

  bool first = true;
  for(auto& el : ast)
  {
    if(first)
      first = false;
    else
      os << ", ";

    os << el.var.name;
    //os << " As " << (el.var.type ? *el.var.type : "<unspecified>");
    if(el.var.type)
      os << " As " << *el.var.type;
    os << " = ";
    (*this)(el.value);
  }

  os << ")\n";
}

void raw_ast_printer::print_type(vb6_ast::access_type t) const
{
  switch(t)
  {
    case vb6_ast::access_type::na:                        break;
    case vb6_ast::access_type::dim:     os << "Dim ";     break;
    case vb6_ast::access_type::private_: os << "Private "; break;
    case vb6_ast::access_type::public_:  os << "Public ";  break;
    case vb6_ast::access_type::global:  os << "Global ";  break;
    case vb6_ast::access_type::friend_:  os << "Friend ";  break;
  }
}

void raw_ast_printer::operator()(vb6_ast::record const& ast) const
{
  os << string(indent, ' ') << '(' << typeid(ast).name();

  print_type(ast.at);
  os << "Type " << ast.name << '\n';
  indent += indent_size;
  for(auto& el : ast.members)
  {
    os << string(indent, ' ');
    (*this)(el);
    os << '\n';
  }
  indent -= indent_size;

  os << ")\n";
}

void raw_ast_printer::operator()(vb6_ast::vb_enum const& ast) const
{
  os << string(indent, ' ') << '(' << typeid(ast).name();

  print_type(ast.at);
  os << ast.name << '\n';
  indent += indent_size;
  for(auto& el : ast.values)
  {
    os << string(indent, ' ') << el.first;
    if(el.second)
    {
      os << " = ";
      (*this)(*el.second);
    }
    os << '\n';
  }
  indent -= indent_size;

  os << ")\n";
}

void raw_ast_printer::operator()(vb6_ast::func_param const& ast) const
{
  os << string(indent, ' ') << '(' << typeid(ast).name();

  if(ast.isoptional)
    os << "Optional ";

  if(ast.qualifier)
    os << (*ast.qualifier == vb6_ast::param_qualifier::byref ? "ByRef" : "ByVal") << " ";

  os << ast.var.name;
  //os << " As " << (ast.var.type ? *ast.var.type : "<unspecified>");
  if(ast.var.type)
    os << " As " << *ast.var.type;

  if(ast.defvalue)
  {
    os << " = ";
    (*this)(*ast.defvalue);
  }

  os << ")\n";
}

void raw_ast_printer::operator()(vb6_ast::externalSub const& ast) const
{
  os << string(indent, ' ') << '(' << typeid(ast).name();

  print_type(ast.at);
  os << ast.name << " Lib \"" << ast.lib << "\" Alias \"" << ast.alias << "\" (";
  bool first = true;
  for(auto& el : ast.params)
  {
    if(first)
      first = false;
    else
      os << ", ";

    (*this)(el);
  }

  os << ")\n";
}

void raw_ast_printer::operator()(vb6_ast::subHead const& ast) const
{
  os << string(indent, ' ') << '(' << typeid(ast).name();

  print_type(ast.at);
  os << "Sub " << ast.name << "(";
  bool first = true;
  for(auto& el : ast.params)
  {
    if(first)
      first = false;
    else
      os << ", ";

    (*this)(el);
  }

  os << ")\n";
}

void raw_ast_printer::operator()(vb6_ast::eventHead const& ast) const
{
  os << string(indent, ' ') << '(' << typeid(ast).name();

  print_type(ast.at);
  os << ast.name << "(";
  bool first = true;
  for(auto& el : ast.params)
  {
    if(first)
      first = false;
    else
      os << ", ";

    (*this)(el);
  }

  os << ")\n";
}

void raw_ast_printer::operator()(vb6_ast::functionHead const& ast) const
{
  os << string(indent, ' ') << '(' << typeid(ast).name();

  print_type(ast.at);
  os << "Function " << ast.name << "(";
  bool first = true;
  for(auto& el : ast.params)
  {
    if(first)
      first = false;
    else
      os << ", ";

    (*this)(el);
  }
  os << ")";
  if(ast.return_type)
    os << " As " << *ast.return_type;

  os << ")\n";
}

void raw_ast_printer::operator()(vb6_ast::propertyLetHead const& ast) const
{
  os << string(indent, ' ') << '(' << typeid(ast).name();

  print_type(ast.at);
  os << ast.name << "(";
  bool first = true;
  for(auto& el : ast.params)
  {
    if(first)
      first = false;
    else
      os << ", ";

    (*this)(el);
  }

  os << ")\n";
}

void raw_ast_printer::operator()(vb6_ast::propertySetHead const& ast) const
{
  os << string(indent, ' ') << '(' << typeid(ast).name();

  print_type(ast.at);
  os << ast.name << "(";
  bool first = true;
  for(auto& el : ast.params)
  {
    if(first)
      first = false;
    else
      os << ", ";

    (*this)(el);
  }

  os << ")\n";
}

void raw_ast_printer::operator()(vb6_ast::propertyGetHead const& ast) const
{
  os << string(indent, ' ') << '(' << typeid(ast).name();

  print_type(ast.at);
  os << ast.name << "(";
  bool first = true;
  for(auto& el : ast.params)
  {
    if(first)
      first = false;
    else
      os << ", ";

    (*this)(el);
  }
  os << ")";
  if(ast.return_type)
    os << " As " << *ast.return_type;

  os << ")\n";
}

void raw_ast_printer::operator()(vb6_ast::subDef const& ast) const
{
  os << string(indent, ' ') << '(' << typeid(ast).name();

  (*this)(ast.header);
  indent += indent_size;
  for(auto& el : ast.statements)
  {
    (*this)(el);
  }
  indent -= indent_size;

  os << ")\n";
}

void raw_ast_printer::operator()(vb6_ast::functionDef const& ast) const
{
  os << string(indent, ' ') << '(' << typeid(ast).name();

  (*this)(ast.header);
  indent += indent_size;
  for(auto& el : ast.statements)
  {
    (*this)(el);
  }
  indent -= indent_size;

  os << ")\n";
}

void raw_ast_printer::operator()(vb6_ast::externalFunction const& ast) const
{
  os << string(indent, ' ') << '(' << typeid(ast).name();

  print_type(ast.at);
  os << "Declare Function " << ast.name << " Lib \"" << ast.lib << "\" Alias \"" << ast.alias << "\" (";
  bool first = true;
  for(auto& el : ast.params)
  {
    if(first)
      first = false;
    else
      os << ", ";

    (*this)(el);
  }
  os << ")";
  if(ast.return_type)
    os << " As " << *ast.return_type;

  os << ")\n";
}

void raw_ast_printer::operator()(vb6_ast::module_attribute ast) const
{
  os << string(indent, ' ') << '(' << typeid(ast).name();

  os << " " << ast.first << " " << ast.second;

  os << ")\n";
}

void raw_ast_printer::operator()(vb6_ast::module_option ast) const
{
  os << string(indent, ' ') << '(' << typeid(ast).name();

  switch(ast)
  {
    case vb6_ast::module_option::explicit_:
      os << "Option Explicit";
      break;
    case vb6_ast::module_option::base_0:
      os << "Option Base 0";
      break;
    case vb6_ast::module_option::base_1:
      os << "Option Base 1";
      break;
    case vb6_ast::module_option::compare_binary:
      os << "Option Compare Binary";
      break;
    case vb6_ast::module_option::compare_text:
      os << "Option Compare Text";
      break;
    case vb6_ast::module_option::private_module:
      os << "Option Private Module";
      break;
  }

  os << ")\n";
}

void raw_ast_printer::operator()(vb6_ast::STRICT_MODULE_STRUCTURE::module_attributes const& ast) const
{
  os << string(indent, ' ') << '(' << typeid(ast).name();

  for(auto& el : ast)
  {
    os << el.first << " " << el.second << "\"\n";
  }

  os << ")\n";
}

void raw_ast_printer::operator()(vb6_ast::STRICT_MODULE_STRUCTURE::declaration const& ast) const
{
  os << string(indent, ' ') << '(' << typeid(ast).name();

  boost::apply_visitor(*this, ast);

  os << ")\n";
}

void raw_ast_printer::operator()(vb6_ast::STRICT_MODULE_STRUCTURE::vb_module const& ast) const
{
  (*this)(ast.attrs);

  os << "'---------------------------------------- options\n";

  for(auto& el : ast.opts.items)
  {
#if 0
    (*this)(el);
#else
    boost::apply_visitor(*this, el);
#endif
  }

  os << "'---------------------------------------- declarations\n";

  for(auto& el : ast.declarations)
  {
    (*this)(el);
  }

  os << "'---------------------------------------- functions\n";

  for(auto& el : ast.functions)
  {
    boost::apply_visitor(*this, el);
  }
}

void raw_ast_printer::operator()(vb6_ast::declaration const& ast) const
{
  boost::apply_visitor(*this, ast);
}

void raw_ast_printer::operator()(vb6_ast::vb_module const& ast) const
{
  for(auto& el : ast)
  {
    boost::apply_visitor(*this, el);
  }
}

void raw_ast_printer::operator()(vb6_ast::statements::assignStmt const& ast) const
{
  os << string(indent, ' ');
  switch(ast.type)
  {
    case vb6_ast::assignmentType::na:                break;
    case vb6_ast::assignmentType::set: os << "Set "; break;
    case vb6_ast::assignmentType::let: os << "Let "; break;
  }

  (*this)(ast.var);
  os << " = ";

  (*this)(ast.rhs);

  // print the index of the type in a comment
  os << " ' " << ast.rhs.get().which() << '\n';
}

void raw_ast_printer::operator()(vb6_ast::statements::exitStmt const& ast) const
{
  os << string(indent, ' ') << '(' << typeid(ast).name();

  switch(ast.type)
  {
    case vb6_ast::exit_type::function: os << "Function"; break;
    case vb6_ast::exit_type::sub:      os << "Sub";      break;
    case vb6_ast::exit_type::property: os << "Property"; break;
    case vb6_ast::exit_type::while_:    os << "While";    break;
    case vb6_ast::exit_type::do_:       os << "Do";       break;
    case vb6_ast::exit_type::for_:      os << "For";      break;
  }

  os << ")\n";
}

void raw_ast_printer::operator()(vb6_ast::statements::gotoStmt const& ast) const
{
  os << string(indent, ' ') << '(' << typeid(ast).name();

  os << ' ' << (ast.type == vb6_ast::gotoType::goto_v ? "GoTo" : "GoSub")
     << " " << ast.label
     << ")\n";
}

void raw_ast_printer::operator()(vb6_ast::statements::onerrorStmt const& ast) const
{
  os << string(indent, ' ') << '(' << typeid(ast).name();

  switch(ast.type)
  {
    case vb6_ast::onerror_type::goto_0:  os << "GoTo 0";  break;
    case vb6_ast::onerror_type::goto_neg_1:  os << "GoTo -1";  break;
    case vb6_ast::onerror_type::goto_label:  os << "GoTo " << ast.label;  break;
    case vb6_ast::onerror_type::resume_next:  os << "Resume Next";  break;
    case vb6_ast::onerror_type::exit_sub:  os << "Exit Sub";  break;
    case vb6_ast::onerror_type::exit_func:  os << "Exit Func";  break;
    case vb6_ast::onerror_type::exit_property:  os << "Exit Property";  break;
  }

  os << ")\n";
}

void raw_ast_printer::operator()(vb6_ast::statements::resumeStmt const& ast) const
{
  os << string(indent, ' ') << '(' << typeid(ast).name();

  switch(ast.type)
  {
    case vb6_ast::resume_type::implicit:
      break;
    case vb6_ast::resume_type::next:
      os << " Next";
      break;
    case vb6_ast::resume_type::label:
      os << " " << boost::get<string>(ast.label_or_line_nr);
      break;
    case vb6_ast::resume_type::line_nr:
      os << " " << boost::get<int>(ast.label_or_line_nr);
      break;
  }

  os << ")\n";
}

void raw_ast_printer::operator()(vb6_ast::statements::localVarDeclStmt const& ast) const
{
  os << string(indent, ' ') << '(' << typeid(ast).name();

  if(ast.type == vb6_ast::localvardeclType::Static)
    os << "Static ";
  else
    os << "Dim ";

  bool first = true;
  for(auto& el : ast.vars)
  {
    if(first)
      first = false;
    else
      os << ", ";

    os << el.name;
    if(el.type)
    {
      os << " As ";
      if(el.construct)
        os << "New ";
      os << *el.type;
    }
    //else
    //  os << " As <unspecified>";
  }

  os << ")\n";
}

void raw_ast_printer::operator()(vb6_ast::statements::redimStmt const& ast) const
{
  os << string(indent, ' ') << '(' << typeid(ast).name();

  if(ast.preserve)
    os << "preserve ";

  (*this)(ast.var);

  os << " ";

  bool first = true;
  for(auto& el : ast.newsize)
  {
    if(first)
      first = false;
    else
      os << ", ";

    (*this)(el);
  }

  os << ")\n";
}

// vb6_ast::returnStmt

void raw_ast_printer::operator()(vb6_ast::statements::callStmt const& ast) const
{
  os << string(indent, ' ') << '(' << typeid(ast).name();

  if(ast.explicit_call)
    os << "Call " << ast.sub_name << "(";
  else
    os << ast.sub_name << " ";

  bool first = true;
  for(auto& el : ast.params)
  {
    if(first)
      first = false;
    else
      os << ", ";

    (*this)(el);
  }

  os << ")\n";
}

void raw_ast_printer::operator()(vb6_ast::statements::raiseeventStmt const& ast) const
{
  os << string(indent, ' ') << '(' << typeid(ast).name();

  os << " " << ast.event_name;

  bool first = true;
  for(auto& el : ast.params)
  {
    if(first)
      first = false;
    else
      os << ", ";

    (*this)(el);
  }

  os << ")\n";
}

void raw_ast_printer::operator()(vb6_ast::statements::labelStmt const& ast) const
{
  os << string(indent, ' ') << '(' << typeid(ast).name()
     << " " << ast.label << ")\n";
}

void raw_ast_printer::operator()(vb6_ast::statements::whileStmt const& ast) const
{
  os << string(indent, ' ') << '(' << typeid(ast).name();

  (*this)(ast.condition);
  os << '\n';

  indent += indent_size;
  for(auto& st : ast.block)
    (*this)(st);
  indent -= indent_size;

  os << ")\n";
}

void raw_ast_printer::operator()(vb6_ast::statements::doStmt const& ast) const
{
  os << string(indent, ' ') << '(' << typeid(ast).name();

  indent += indent_size;
  for(auto& st : ast.block)
    (*this)(st);
  indent -= indent_size;

  os << ")\n";
}

void raw_ast_printer::operator()(vb6_ast::statements::dowhileStmt const& ast) const
{
  os << string(indent, ' ') << '(' << typeid(ast).name();

  (*this)(ast.condition);
  os << '\n';

  indent += indent_size;
  for(auto& st : ast.block)
    (*this)(st);
  indent -= indent_size;

  os << ")\n";
}

void raw_ast_printer::operator()(vb6_ast::statements::dountilStmt const& ast) const
{
  os << string(indent, ' ') << '(' << typeid(ast).name();

  (*this)(ast.condition);
  os << '\n';

  indent += indent_size;
  for(auto& st : ast.block)
    (*this)(st);
  indent -= indent_size;

  os << ")\n";
}

void raw_ast_printer::operator()(vb6_ast::statements::loopwhileStmt const& ast) const
{
  os << string(indent, ' ') << '(' << typeid(ast).name();

  indent += indent_size;
  for(auto& st : ast.block)
    (*this)(st);
  indent -= indent_size;

  os << string(indent, ' ') << "Loop While ";
  (*this)(ast.condition);

  os << ")\n";
}

void raw_ast_printer::operator()(vb6_ast::statements::loopuntilStmt const& ast) const
{
  os << string(indent, ' ') << '(' << typeid(ast).name();

  indent += indent_size;
  for(auto& st : ast.block)
    (*this)(st);
  indent -= indent_size;

  os << string(indent, ' ') << "Loop Until ";
  (*this)(ast.condition);

  os << ")\n";
}

void raw_ast_printer::operator()(vb6_ast::statements::forStmt const& ast) const
{
  os << string(indent, ' ') << '(' << typeid(ast).name();

  (*this)(ast.for_variable);
  os << " = ";
  (*this)(ast.from);
  os << " To ";
  (*this)(ast.to);
  if(ast.step)
  {
    os << " Step ";
    (*this)(*ast.step);
  }
  os << '\n';

  indent += indent_size;
  for(auto& el : ast.block)
    (*this)(el);
  indent -= indent_size;

  os << string(indent, ' ') << "Next ";
  (*this)(ast.for_variable);

  os << ")\n";
}

void raw_ast_printer::operator()(vb6_ast::statements::foreachStmt const& ast) const
{
  os << string(indent, ' ') << '(' << typeid(ast).name();

  (*this)(ast.for_variable);
  os << " In ";
  (*this)(ast.container);
  os << '\n';

  indent += indent_size;
  for(auto& el : ast.block)
    (*this)(el);
  indent -= indent_size;

  os << string(indent, ' ') << "Next ";
  (*this)(ast.for_variable);

  os << ")\n";
}

void raw_ast_printer::operator()(vb6_ast::statements::ifelseStmt const& ast) const
{
  os << string(indent, ' ') << '(' << typeid(ast).name();

#ifdef SIMPLE_IF_STATEMENT
  (*this)(ast.first_branch.condition);

  // statements
  indent += indent_size;
  for(auto& st : ast.first_branch.block)
    (*this)(st);
  indent -= indent_size;
#else
  (*this)(ast.first_branch.condition);

  // statements
  indent += indent_size;
  for(auto& st : ast.first_branch.block)
    (*this)(st);
  indent -= indent_size;

  //bool first = true;
  for(auto& el : ast.if_branches)
  {
    os << string(indent, ' ');
    //if(first)
    //{
    //  os << "If ";
    //  first = false;
    //}
    //else
      os << "ElseIf ";

    (*this)(el.condition);
    os << " Then\n";

    indent += indent_size;
    for(auto& st : el.block)
      (*this)(st);
    indent -= indent_size;
  }
#endif

  if(ast.else_branch.has_value())
  {
    os << string(indent, ' ') << "Else " << '\n';
    indent += indent_size;
    for(auto& el : ast.else_branch.get())
      (*this)(el);
    indent -= indent_size;
  }

  os << string(indent, ' ') << "End If\n";
}

void raw_ast_printer::operator()(vb6_ast::statements::withStmt const& ast) const
{
  os << string(indent, ' ') << '(' << typeid(ast).name();

  (*this)(ast.with_variable);
  os << '\n';

  // statements
  indent += indent_size;
  for(auto& st : ast.block)
    (*this)(st);
  indent -= indent_size;

  os << string(indent, ' ') << "End With\n";
}

void raw_ast_printer::operator()(vb6_ast::statements::case_block const& ast) const
{
  os << string(indent, ' ') << '(' << typeid(ast).name();

  // TODO
  (*this)(ast.case_expr);
  os << "\n";

  indent += indent_size;
  for(auto& st : ast.block)
    (*this)(st);
  indent -= indent_size;
}

void raw_ast_printer::operator()(vb6_ast::statements::selectStmt const& ast) const
{
  os << string(indent, ' ') << '(' << typeid(ast).name();

  // TODO
  (*this)(ast.condition);
  os << '\n';

  indent += indent_size;
  for(auto& st : ast.blocks)
    (*this)(st);
  indent -= indent_size;

  os << string(indent, ' ') << "End Case\n";
}

void raw_ast_printer::operator()(vb6_ast::statements::singleStmt const& ast) const
{
  //os << string(indent, ' '); // FED ???? wouldn't it be better to indent here rather than at each statement?
                               //          => problem with some statements, e.g. else within the if-statement
                               //             perhaps the else should be a statement of its own!!!
  boost::apply_visitor(*this, ast.get());
}
