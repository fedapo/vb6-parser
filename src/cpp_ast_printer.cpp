//: cpp_ast_printer.cpp

// vb6_parser
// Copyright (c) 2022 Federico Aponte
// This code is licensed under GNU Software License (see LICENSE.txt for details)

#include "cpp_ast_printer.hpp"

#include <boost/optional/optional_io.hpp>
#include <boost/variant/apply_visitor.hpp>

using namespace std;

// static
int cpp_ast_printer::indent_size = 4;

void cpp_ast_printer::operator()(vb6_ast::empty_line const&) const
{
  os << '\n';
}

void cpp_ast_printer::operator()(vb6_ast::lonely_comment const& ast) const
{
  os << string(indent, ' ');
  os << "//" << ast.content << '\n';
}

void cpp_ast_printer::operator()(vb6_ast::identifier_context const& ast) const
{
  if(ast.leading_dot)
    os << '.';

  struct {
    void operator()(std::string s) { os << s; }
    void operator()(vb6_ast::func_call const& s) { (*p)(s); }
    ostream& os;
    cpp_ast_printer const* p;
  } visitor{ os, this };

  for(auto& el : ast.elements)
  {
    boost::apply_visitor(visitor, el.get());
    os << '.';
  }
}

void cpp_ast_printer::operator()(vb6_ast::variable const& ast) const
{
  //if(ast.library_or_module)
  //  os << *ast.library_or_module << "::";
  os << (ast.type ? *ast.type : "std::any") << " " << ast.name << ";";
}

void cpp_ast_printer::operator()(vb6_ast::decorated_variable const& ast) const
{
  (*this)(ast.ctx);
  os << ast.var;
}

void cpp_ast_printer::operator()(vb6_ast::func_call const& ast) const
{
  os << ast.func_name << "(";

  bool first = true;
  for(auto& el : ast.params)
  {
    if(first)
      first = false;
    else
      os << ", ";

    (*this)(el);
  }

  os << ");\n";
}

void cpp_ast_printer::operator()(vb6_ast::global_var_decls const& ast) const
{
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
  os << '\n';
}

void cpp_ast_printer::operator()(vb6_ast::const_expr const& ast) const
{
  struct {
    void operator()(float v) { os << v << 'f'; }
    void operator()(double v) { os << v; }
    void operator()(vb6_ast::long_dec v) { os << v.val; }
    void operator()(vb6_ast::long_hex v) { os << showbase << hex << v.val << dec; }
    void operator()(vb6_ast::long_oct v) { os << showbase << oct << v.val << dec; }
    void operator()(vb6_ast::integer_dec v) { os << v.val; }
    void operator()(vb6_ast::integer_hex v) { os << showbase << hex << v.val << dec; }
    void operator()(vb6_ast::integer_oct v) { os << showbase << oct << v.val << dec; }
    void operator()(bool v) { os << (v ? "true" : "false"); }
    void operator()(vb6_ast::quoted_string const& s) { os << "\"" << s << "\""; }
    void operator()(vb6_ast::nothing const&) { os << "nullptr"; }
    ostream& os;
  } visitor{os};

  boost::apply_visitor(visitor, ast.get());
}

void cpp_ast_printer::operator()(vb6_ast::expression const& ast) const
{
  boost::apply_visitor(*this, ast.get());
}

void cpp_ast_printer::operator()(vb6_ast::const_var_stat const& cvars) const
{
  os << "Const ";
  bool first = true;
  for(auto& el : cvars)
  {
    if(first)
      first = false;
    else
      os << ", ";

    os << (el.var.type ? *el.var.type : "std::any")
       << el.var.name << " = ";
    (*this)(el.value);
  }
}

void cpp_ast_printer::print_type(vb6_ast::access_type t) const
{
  switch(t)
  {
    case vb6_ast::access_type::na:                         break;
    case vb6_ast::access_type::dim:      os << "Dim ";     break;
    case vb6_ast::access_type::private_: os << "Private "; break;
    case vb6_ast::access_type::public_:  os << "Public ";  break;
    case vb6_ast::access_type::global:   os << "Global ";  break;
    case vb6_ast::access_type::friend_:  os << "Friend ";  break;
  }
}

void cpp_ast_printer::operator()(vb6_ast::record const& ast) const
{
  print_type(ast.at);
  os << "struct " << ast.name << '\n' << "{";
  indent += indent_size;
  for(auto& el : ast.members)
  {
    os << string(indent, ' ');
    (*this)(el);
    os << ";\n";
  }
  indent -= indent_size;
  os << "};\n";
}

void cpp_ast_printer::operator()(vb6_ast::vb_enum const& ast) const
{
  print_type(ast.at);
  os << "enum " << ast.name << '\n' << "{";
  indent += indent_size;
  for(auto& el : ast.values)
  {
    os << string(indent, ' ') << el.first;
    if(el.second)
    {
      os << " = ";
      (*this)(*el.second);
    }
    os << ",\n";
  }
  indent -= indent_size;
  os << "};\n";
}

void cpp_ast_printer::operator()(vb6_ast::func_param const& ast) const
{
  os << (ast.var.type ? *ast.var.type : "std::any");

  if(!ast.qualifier || *ast.qualifier == vb6_ast::param_qualifier::byref)
    os << "&";

  os << " " << ast.var.name;

  if(ast.defvalue)
  {
    os << " = ";
    (*this)(*ast.defvalue);
  }
}

void cpp_ast_printer::operator()(vb6_ast::externalSub const& ast) const
{
  print_type(ast.at);
  os << "__declspec(dllimport)\n"
        "void " << ast.name << "(";
  bool first = true;
  for(auto& el : ast.params)
  {
    if(first)
      first = false;
    else
      os << ", ";

    (*this)(el);
  }
  os << ");\n";
}

void cpp_ast_printer::operator()(vb6_ast::subHead const& ast) const
{
  print_type(ast.at);
  os << "void " << ast.name << "(";
  bool first = true;
  for(auto& el : ast.params)
  {
    if(first)
      first = false;
    else
      os << ", ";

    (*this)(el);
  }
  os << ");\n";
}

void cpp_ast_printer::operator()(vb6_ast::eventHead const& ast) const
{
  print_type(ast.at);
  os << "void /* event */ " << ast.name << "(";
  bool first = true;
  for(auto& el : ast.params)
  {
    if(first)
      first = false;
    else
      os << ", ";

    (*this)(el);
  }
  os << ");\n";
}

void cpp_ast_printer::operator()(vb6_ast::functionHead const& ast) const
{
  print_type(ast.at);
  os << ast.return_type << " " << ast.name << "(";
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

void cpp_ast_printer::operator()(vb6_ast::propertyLetHead const& ast) const
{
  print_type(ast.at);
  os << "void let_" << ast.name << "(";
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

void cpp_ast_printer::operator()(vb6_ast::propertySetHead const& ast) const
{
  print_type(ast.at);
  os << "void set_" << ast.name << "(";
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

void cpp_ast_printer::operator()(vb6_ast::propertyGetHead const& ast) const
{
  print_type(ast.at);
  os << ast.return_type << " get_" << ast.name << "(";
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
  os << '\n';
}

void cpp_ast_printer::operator()(vb6_ast::subDef const& ast) const
{
  (*this)(ast.header);
  print_block(ast.statements);
}

void cpp_ast_printer::operator()(vb6_ast::functionDef const& ast) const
{
  (*this)(ast.header);
  print_block(ast.statements);
}

void cpp_ast_printer::operator()(vb6_ast::externalFunction const& ast) const
{
  print_type(ast.at);
  os << "__declspec(dllimport)\n"
     << *ast.return_type << " " << ast.name << "(";
  bool first = true;
  for(auto& el : ast.params)
  {
    if(first)
      first = false;
    else
      os << ", ";

    (*this)(el);
  }
  os << ");\n";
}

void cpp_ast_printer::operator()(vb6_ast::declaration const& ast) const
{
  boost::apply_visitor(*this, ast);
}

void cpp_ast_printer::operator()(vb6_ast::module_attribute) const
{
}

void cpp_ast_printer::operator()(vb6_ast::module_option) const
{
}

//void cpp_ast_printer::operator()(vb6_ast::STRICT_MODULE_STRUCTURE::module_attributes const&) const
//{
//}

void cpp_ast_printer::operator()(vb6_ast::STRICT_MODULE_STRUCTURE::vb_module const& ast) const
{
  for(auto& el : ast.declarations)
  {
    boost::apply_visitor(*this, el);
  }

  for(auto& el : ast.functions)
  {
    boost::apply_visitor(*this, el);
  }
}

void cpp_ast_printer::operator()(vb6_ast::vb_module const& ast) const
{
  for(auto& el : ast)
  {
    boost::apply_visitor(*this, el);
  }
}

void cpp_ast_printer::print_block(vb6_ast::statements::statement_block const& block) const
{
  os << string(indent, ' ') << "{\n";

  indent += indent_size;
  for(auto& st : block)
    (*this)(st);
  indent -= indent_size;

  os << string(indent, ' ') << "}\n";
}

void cpp_ast_printer::operator()(vb6_ast::statements::assignStmt const& ast) const
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
  os << " // " << ast.rhs.get().which() << '\n';
}

void cpp_ast_printer::operator()(vb6_ast::statements::exitStmt const& ast) const
{
  os << string(indent, ' ');
  switch(ast.type)
  {
    case vb6_ast::exit_type::function: os << "return ???;"; break;
    case vb6_ast::exit_type::sub:      os << "return;";     break;
    case vb6_ast::exit_type::property: os << "Property";    break;
    case vb6_ast::exit_type::while_:   os << "break;";      break;
    case vb6_ast::exit_type::do_:      os << "break;";      break;
    case vb6_ast::exit_type::for_:     os << "break;";      break;
  }
  os << '\n';
}

void cpp_ast_printer::operator()(vb6_ast::statements::gotoStmt const& ast) const
{
  os << string(indent, ' ') << (ast.type == vb6_ast::gotoType::goto_v ? "GoTo" : "GoSub")
     << " " << ast.label << '\n';
}

void cpp_ast_printer::operator()(vb6_ast::statements::onerrorStmt const& ast) const
{
  os << string(indent, ' ') << "On Error ";
  switch(ast.type)
  {
  case vb6_ast::onerror_type::goto_0:        os << "GoTo 0";  break;
  case vb6_ast::onerror_type::goto_neg_1:    os << "GoTo -1";  break;
  case vb6_ast::onerror_type::goto_label:    os << "goto " << ast.label;  break;
  case vb6_ast::onerror_type::resume_next:   os << "Resume Next";  break;
  case vb6_ast::onerror_type::exit_sub:      os << "Exit Sub";  break;
  case vb6_ast::onerror_type::exit_func:     os << "Exit Func";  break;
  case vb6_ast::onerror_type::exit_property: os << "Exit Property";  break;
  }
  os << '\n';
}

void cpp_ast_printer::operator()(vb6_ast::statements::resumeStmt const& ast) const
{
  os << string(indent, ' ') << "Resume";
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
  os << '\n';
}

void cpp_ast_printer::operator()(vb6_ast::statements::localVarDeclStmt const& ast) const
{
  os << string(indent, ' ');

  if(ast.type == vb6_ast::localvardeclType::Static)
    os << "static ";

  bool first = true;
  for(auto& el : ast.vars)
  {
    if(first)
      first = false;
    else
      os << ", ";

    os << (el.type ? *el.type : "std::any") << " " << el.name;
  }
  os << ";\n";
}

void cpp_ast_printer::operator()(vb6_ast::statements::redimStmt const& ast) const
{
  os << string(indent, ' ');

  if(ast.preserve)
  {
    // TODO
  }

  (*this)(ast.var);

  os << " = new " << "Type" << "[";

  bool first = true;
  for(auto& el : ast.newsize)
  {
    if(first)
      first = false;
    else
      os << ", ";

    (*this)(el);
  }

  os << "];\n";
}

// vb6_ast::returnStmt

void cpp_ast_printer::operator()(vb6_ast::statements::callStmt const& ast) const
{
  os << string(indent, ' ') << ast.sub_name << "(";

  bool first = true;
  for(auto& el : ast.params)
  {
    if(first)
      first = false;
    else
      os << ", ";

    (*this)(el);
  }

  os << ");\n";
}

void cpp_ast_printer::operator()(vb6_ast::statements::raiseeventStmt const& ast) const
{
  os << string(indent, ' ') << ast.event_name << "(";

  bool first = true;
  for(auto& el : ast.params)
  {
    if(first)
      first = false;
    else
      os << ", ";

    (*this)(el);
  }

  os << ");\n";
}

void cpp_ast_printer::operator()(vb6_ast::statements::labelStmt const& ast) const
{
  os << ast.label << ":\n"; // no indentation for labels
}

void cpp_ast_printer::operator()(vb6_ast::statements::whileStmt const& ast) const
{
  os << string(indent, ' ') << "while(";
  (*this)(ast.condition);
  os << ")\n";
  os << string(indent, ' ') << "{\n";
  os << string(indent, ' ') << "}\n";
  os << string(indent, ' ') << "while(";
  (*this)(ast.condition);
  os << ")\n";
  print_block(ast.block);
}

void cpp_ast_printer::operator()(vb6_ast::statements::doStmt const& ast) const
{
  os << string(indent, ' ') << "for(;;)" << '\n';
  print_block(ast.block);
}

void cpp_ast_printer::operator()(vb6_ast::statements::dowhileStmt const& ast) const
{
  os << string(indent, ' ') << "while(";
  (*this)(ast.condition);
  os << ")\n";
  print_block(ast.block);
}

void cpp_ast_printer::operator()(vb6_ast::statements::loopwhileStmt const& ast) const
{
  os << string(indent, ' ') << "do\n";
  os << string(indent, ' ') << "{\n";

  indent += indent_size;
  for(auto& st : ast.block)
    (*this)(st);
  indent -= indent_size;

  os << string(indent, ' ') << "} while(";
  (*this)(ast.condition);
  os << ")\n";
}

void cpp_ast_printer::operator()(vb6_ast::statements::dountilStmt const& ast) const
{
  os << string(indent, ' ') << "while(not (";
  (*this)(ast.condition);
  os << "))\n";
  print_block(ast.block);
}

void cpp_ast_printer::operator()(vb6_ast::statements::loopuntilStmt const& ast) const
{
  os << string(indent, ' ') << "do\n";
  os << string(indent, ' ') << "{\n";

  indent += indent_size;
  for(auto& st : ast.block)
    (*this)(st);
  indent -= indent_size;

  os << string(indent, ' ') << "} while(not (";
  (*this)(ast.condition);
  os << "))\n";
}

void cpp_ast_printer::operator()(vb6_ast::statements::forStmt const& ast) const
{
  os << string(indent, ' ') << "for(";
  (*this)(ast.for_variable);
  os << " = ";
  (*this)(ast.from);
  os << "; ";
  (*this)(ast.for_variable);
  os << " <= ";
  (*this)(ast.to);
  if(ast.step)
  {
    os << "; ";
    (*this)(ast.for_variable);
    os << " += ";
    (*this)(*ast.step);
  }
  else
  {
    os << "; ++";
    (*this)(ast.for_variable);
  }

  os << ")\n";
  print_block(ast.block);
}

void cpp_ast_printer::operator()(vb6_ast::statements::foreachStmt const& ast) const
{
  os << string(indent, ' ') << "for(";
  (*this)(ast.for_variable);
  os << " : ";
  (*this)(ast.container);
  os << ")\n";
  print_block(ast.block);
}

void cpp_ast_printer::operator()(vb6_ast::statements::ifelseStmt const& ast) const
{
  // TODO
#ifdef SIMPLE_IF_STATEMENT
  os << string(indent, ' ') << "if(";
  //(*this)(ast.first_branch);
  os << ")\n";
  print_block(ast.first_branch.block);
#else
  bool first = true;
  for(auto& el : ast.if_branches)
  {
    if(first)
    {
      os << string(indent, ' ') << "if(";
      (*this)(el.condition);
      os << ")\n";
    }
    else
    {
      os << string(indent, ' ') << "else if(";
      (*this)(el.condition);
      os << ")\n";
    }
    print_block(el.block);
  }
#endif

  if(ast.else_branch.has_value())
  {
    os << "else\n";
    print_block(ast.else_branch.get());
  }
}

void cpp_ast_printer::operator()(vb6_ast::statements::withStmt const& ast) const
{
  // TODO
  os << string(indent, ' ') << "auto& dummy = ";
  (*this)(ast.with_variable);
  os << ";\n";

  // statements
  indent += indent_size;
  for(auto& st : ast.block)
    (*this)(st);
  indent -= indent_size;
}

void cpp_ast_printer::operator()(vb6_ast::statements::case_block const& ast) const
{
  // TODO
  os << string(indent, ' ') << "case ";
  (*this)(ast.case_expr);
  os << ":\n";

  indent += indent_size;
  for(auto& st : ast.block)
    (*this)(st);
  indent -= indent_size;

  os << "break;\n";
}

void cpp_ast_printer::operator()(vb6_ast::statements::selectStmt const& ast) const
{
  // TODO
  os << string(indent, ' ') << "switch(";
  (*this)(ast.condition);
  os << ")\n";
  os << string(indent, ' ') << "{\n";

  indent += indent_size;
  for(auto& st : ast.blocks)
    (*this)(st);
  indent -= indent_size;

  os << string(indent, ' ') << "}\n";
}

void cpp_ast_printer::operator()(vb6_ast::statements::singleStmt const& ast) const
{
  boost::apply_visitor(*this, ast.get());
}
