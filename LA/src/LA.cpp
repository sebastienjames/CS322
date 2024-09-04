#include "LA.h"
#include <iostream>

namespace LA {
  map<string, string> types;
  void print(std::string line) {
    // Change this to activate all prints
    if (false) {
      std::cout << line << "\n";
    }
  }

  void addType(string var_name, string type_name){
    types.insert({var_name, type_name});
  }

  

  string convert_op(OperatorID op) {
    switch (op) {
      case OperatorID::l:
        return "<";
      case OperatorID::le:
        return "<=";
      case OperatorID::e:
        return "=";
      case OperatorID::ge:
        return ">=";
      case OperatorID::g:
        return ">";
      case OperatorID::addq:
        return "+";
      case OperatorID::subq:
        return "-";
      case OperatorID::imultq:
        return "*";
      case OperatorID::andq:
        return "&";
      case OperatorID::sarq:
        return ">>";
      case OperatorID::salq:
        return "<<";
    }
    return "FAIL OP";
  }

  L::L (std::string name)
    : name { name }{
    return ;
  }

  string L::get_name() {
    return name;
  }

  Label::Label (std::string name)
    : name { name }{
    return ;
  }

  string Label::get_name() {
    return name;
  }

  Name::Name (std::string name)
    : name { name }{
    return ;
  }

  string Name::get_name() {
    return name;
  }

  Number::Number (int64_t number)
    : number { number }{
    return ;
  }

  int64_t Number::get_number() {
    return number;
  }

  Type::Type (TypeID type, int64_t dims)
    : t { type },
      num_dimensions { dims } {
      return;
  }

  TypeID Type::get_type() {
    return t;
  }

  Operator::Operator (OperatorID op)
    : op { op } {
    return ;
  }

  OperatorID Operator::get_op() {
    return op;
  }

  Vars::Vars (std::vector<Item*> vars)
    : vars { vars }{
    return ;
  }

  vector<Item*> Vars::getVars () {
    return vars;
  }

  int64_t Vars::getNumVars () {
    return vars.size();
  }

  Args::Args (std::vector<Item*> args)
    : args { args }{
    return ;
  }

  vector<Item*> Args::getArgs () {
    return args;
  }

  int64_t Args::getNumArgs () {
    return args.size();
  }


  Instruction_return_arg::Instruction_return_arg (Item *src)
    : s { src } {
    return ;
  }

  Item* Instruction_return_arg::get_value() {
    return s;
  }

  Instruction_declaration::Instruction_declaration (Item *type, Item *var)
    : type { type },
      var { var } {
    return ;
  }

  Item* Instruction_declaration::get_type() {
    return type;
  }

  Item* Instruction_declaration::get_var() {
    return var;
  }

  Instruction_assignment::Instruction_assignment (Item *dst, Item *src)
    : s { src },
      d { dst } {
    return ;
  }

  Item* Instruction_assignment::getSource(){
    return s;
  }

  Item* Instruction_assignment::getDestination(){
    return d;
  }

  Instruction_assignment_op::Instruction_assignment_op (Item *dst, Item *t1, Item *op, Item *t2)
    : dst { dst },
      t1 { t1 },
      op { op },
      t2 { t2 } {
    return ;
  }

  Item* Instruction_assignment_op::getSource1(){
    return t1;
  }

  Item* Instruction_assignment_op::getSource2(){
    return t2;
  }

  Item* Instruction_assignment_op::getDestination(){
    return dst;
  }

  Item* Instruction_assignment_op::getOperator(){
    return op;
  }

  Instruction_read_tensor::Instruction_read_tensor (Item *dst, Item *src, vector<Item *> indices)
    : dst { dst },
      src { src },
      indices { indices } {
    return ;
  }

  Item* Instruction_read_tensor::getDestination() {
    return dst;
  }

  Item* Instruction_read_tensor::getSource() {
    return src;
  }

  vector<Item*> Instruction_read_tensor::getIndices() {
    return indices;
  }

  Instruction_write_tensor::Instruction_write_tensor (Item *dst, Item *src, vector<Item *> indices)
    : dst { dst },
      src { src },
      indices { indices } {
    return ;
  }

  Item* Instruction_write_tensor::getDestination() {
    return dst;
  }

  Item* Instruction_write_tensor::getSource() {
    return src;
  }

  vector<Item*> Instruction_write_tensor::getIndices() {
    return indices;
  }

  Instruction_length::Instruction_length (Item *dst, Item *src, Item * index)
    : dst { dst },
      src { src },
      index { index } {
    return ;
  }

  Item* Instruction_length::getDestination() {
    return dst;
  }

  Item* Instruction_length::getSource() {
    return src;
  }

  Item* Instruction_length::getIndex() {
    return index;
  }

  Instruction_new_array::Instruction_new_array (Item* dst, Item* sizes)
    : dst { dst },
      sizes { sizes } {
    return ;
  }

  Item* Instruction_new_array::getDestination() {
    return dst;
  }

  Item* Instruction_new_array::getSizes() {
    return sizes;
  }

  Instruction_new_tuple::Instruction_new_tuple (Item* dst, Item* size)
    : dst { dst },
      size { size } {
    return ;
  }

  Item* Instruction_new_tuple::getDestination() {
    return dst;
  }

  Item* Instruction_new_tuple::getSize() {
    return size;
  }

  Instruction_label::Instruction_label (Item *dst)
    : d { dst } {
    return ;
  }

  Item* Instruction_label::get_label () {
    return d;
  }

  Instruction_branch::Instruction_branch (Item *dst)
    : d { dst } {
    return ;
  }

  Item* Instruction_branch::getDestination(){
    return d;
  }

  Instruction_branch_conditional::Instruction_branch_conditional (Item *cond, Item *dst1, Item *dst2)
    : c { cond },
      dst1 { dst1 },
      dst2 { dst2 } {
    return ;
  }

  Item* Instruction_branch_conditional::getDestination1(){
    return dst1;
  }

  Item* Instruction_branch_conditional::getDestination2(){
    return dst2;
  }

  Item* Instruction_branch_conditional::getCondition(){
    return c;
  }

  Instruction_call::Instruction_call (Item *callee, Item* args)
    : args { args },
      callee { callee } {
    return ;
  }

  Item* Instruction_call::getCallee () {
    return callee;
  }

  Item* Instruction_call::getArgs () {
    return args;
  }
  
  Instruction_assignment_call::Instruction_assignment_call (Item* dst, Item *callee, Item* args)
    : args { args },
      d { dst },
      callee { callee } {
    return ;
  }

  Item* Instruction_assignment_call::getDst() {
    return d;
  }

  Item* Instruction_assignment_call::getCallee () {
    return callee;
  }

  Item* Instruction_assignment_call::getArgs () {
    return args;
  }



  // to_string methods

  string L::to_string() {
    return name;
  }
  
  string Label::to_string() {
    return name;
  }

  string Name::to_string() {
    return "%" + name;
  }

  string Number::to_string() {
    return std::to_string(number);
  }

  string Type::to_string() {
    if (t == TypeID::code) {
      return "code";
    } else if (t == TypeID::tuple) {
      return "tuple";
    } else if (t == TypeID::void_type) {
      return "void";
    } else if (t == TypeID::int64) {
      string output = "int64";
      for (int i = 0; i < num_dimensions; i++) {
        output += "[]";
      }
      return output;
    } else {
      return "ERROR IN TYPE";
    }
  }

  string Operator::to_string() {
    string ret=convert_op(op);
    return ret;
    //return "compare";
  }


  string Vars::to_string() {
    std::string output;
    for (int i = 0; i < vars.size(); i+=2) {
      output += vars[i]->to_string() + " " + vars[i+1]->to_string() + ", ";
    }
    if(output.size()!=0){
      output.erase(output.size()-1);
      output.erase(output.size()-1);
    }
    return output;
  }

  string Args::to_string() {
    std::string output;
    for (auto arg : args) {
      output += arg->to_string()+",";
    }
    if(output.size()!=0){
      output.erase(output.size()-1);
    }
    return output;
  }

  void Instruction_declaration::print_instruction() {
    cout << type->to_string() + " " + var->to_string() << "\n";
  }

  void Instruction_return::print_instruction() {
    cout << "return" << "\n";
  }

  void Instruction_return_arg::print_instruction() {
    cout << "return " + s->to_string() << "\n";
  }

  void Instruction_assignment::print_instruction() {
    cout << d->to_string() + " <- " + s->to_string() << "\n";
  }

  void Instruction_assignment_op::print_instruction() {
    cout << dst->to_string() + " <- " + t1->to_string() + " " + op->to_string() + " " + t2->to_string() << "\n";
  }

  void Instruction_read_tensor::print_instruction() {
    string output;
    output += dst->to_string() + " <- " + src->to_string();
    for (auto i : indices) {
      output += "[" + i->to_string() + "]";
    }
    cout << output << "\n";
  }

  void Instruction_write_tensor::print_instruction() {
    string output;
    output += dst->to_string();
    for (auto i : indices) {
      output += "[" + i->to_string() + "]";
    }
    output += " <- " + src->to_string();
    cout << output << "\n";
  }


  void Instruction_length::print_instruction() {
    cout << dst->to_string() + " <- length " + src->to_string() + " " + index->to_string() << "\n";
  }


  void Instruction_new_array::print_instruction() {
    cout << dst->to_string() + " <- new Array(" + sizes->to_string() + ")" << "\n";
  }


  void Instruction_new_tuple::print_instruction() {
    cout << dst->to_string() + " <- new Tuple(" + size->to_string() + ")" << "\n";
  }


  void Instruction_label::print_instruction() {
    cout << d->to_string() << "\n";
  }

  void Instruction_branch::print_instruction() {
    cout << "br " + d->to_string() << "\n";
  }

  void Instruction_branch_conditional::print_instruction() {
    cout << "br " + c->to_string() + " " + dst1->to_string() + " " + dst2->to_string() << "\n";
  }  

  void Instruction_call::print_instruction() {
    cout << "call " + callee->to_string() + "(" + args->to_string() + ")" << "\n";
  }

  void Instruction_assignment_call::print_instruction() {
    cout << d->to_string() + " <- call " + callee->to_string() + "(" + args->to_string() + ")" << "\n";
  }









  string Instruction_declaration::instruction_to_IR(){
    // types.insert({var->to_string(), type->to_string()});
    return type->to_string() + " " + var->to_string() + "\n"; 
  }

  string Instruction_return::instruction_to_IR(){
    return "return\n";
  }

  string Instruction_return_arg::instruction_to_IR(){
    return "return " + s->to_string() + "\n";
  }

  string Instruction_assignment::instruction_to_IR(){
    return d->to_string() + " <- " + s->to_string() + "\n";
  }

  string Instruction_assignment_op::instruction_to_IR(){
    return dst->to_string() + " <- " + t1->to_string() + " " + op->to_string() + " " + t2->to_string() + "\n";
  }

  string Instruction_write_tensor::instruction_to_IR(){
    string output;

    output += dst->to_string();
    for (auto elem : indices) {
      output += "[" + elem->to_string() + "]";
    }
    output += " <- " + src->to_string() + "\n";
    return output;
  }

  string Instruction_read_tensor::instruction_to_IR(){
    string output;
    output += dst->to_string() + " <- " + src->to_string();
    for (auto elem : indices) {
      output += "[" + elem->to_string() + "]";
    }
    output += "\n";
    return output;
  }

  string Instruction_length::instruction_to_IR(){
    return dst->to_string() + " <- length " + src->to_string() + " " + index->to_string() + "\n";
  }

  string Instruction_new_array::instruction_to_IR(){
    return dst->to_string() + " <- new Array(" + sizes->to_string() + ")\n";
  }

  string Instruction_new_tuple::instruction_to_IR(){
    return dst->to_string() + " <- new Tuple(" + size->to_string() + ")\n";
  }

  string Instruction_label::instruction_to_IR(){
    return d->to_string()+"\n";
  }

  string Instruction_branch::instruction_to_IR(){
    return "br "+d->to_string()+"\n";
  }

  

  string Instruction_branch_conditional::instruction_to_IR(){
    return "br " + c->to_string() + " " + dst1->to_string() + " " + dst2->to_string() + "\n";
  }

  string clean_func_name(string func_name) {
    
    func_name.at(0) = '@';
    if (func_name == "@print") {
      func_name = "print";
    } else if (func_name == "@tensor-error") {
      func_name = "tensor-error";
    } else if (func_name == "@input") {
      func_name = "input";
    }
    return func_name;
  }

  string Instruction_call::instruction_to_IR(){
    
    string func_name = callee->to_string();
    func_name = clean_func_name(func_name);
    return "call " + func_name + "(" + args->to_string() + ")\n";
  }

  string Instruction_assignment_call::instruction_to_IR(){
    string func_name = callee->to_string();
    func_name = clean_func_name(func_name);
    return d->to_string() + " <- call " + func_name + "(" + args->to_string() + ")\n";
  }

  vector<string> L::accept(ItemVisitor *visitor) {
    return visitor->visit(this);
  }

  vector<string> Label::accept(ItemVisitor *visitor) {
    return visitor->visit(this);
  }

  vector<string> Name::accept(ItemVisitor *visitor) {
    return visitor->visit(this);
  }

  vector<string> Type::accept(ItemVisitor *visitor) {
    return visitor->visit(this);
  }

  vector<string> Operator::accept(ItemVisitor *visitor) {
    return visitor->visit(this);
  }

  vector<string> Number::accept(ItemVisitor *visitor) {
    return visitor->visit(this);
  }

  vector<string> Vars::accept(ItemVisitor *visitor) {
    return visitor->visit(this);
  }

  vector<string> Args::accept(ItemVisitor *visitor) {
    return visitor->visit(this);
  }

  void Instruction_declaration::accept(InstructionVisitor *visitor) {
    return visitor->visit(this);
  }

  void Instruction_assignment::accept(InstructionVisitor *visitor) {
    return visitor->visit(this);
  }

  void Instruction_assignment_op::accept(InstructionVisitor *visitor) {
    return visitor->visit(this);
  }

  void Instruction_read_tensor::accept(InstructionVisitor *visitor) {
    return visitor->visit(this);
  }

  void Instruction_write_tensor::accept(InstructionVisitor *visitor) {
    return visitor->visit(this);
  }

  void Instruction_length::accept(InstructionVisitor *visitor) {
    return visitor->visit(this);
  }

  void Instruction_new_array::accept(InstructionVisitor *visitor) {
    return visitor->visit(this);
  }

  void Instruction_new_tuple::accept(InstructionVisitor *visitor) {
    return visitor->visit(this);
  }

  void Instruction_assignment_call::accept(InstructionVisitor *visitor) {
    return visitor->visit(this);
  }

  void Instruction_return_arg::accept(InstructionVisitor *visitor) {
    return visitor->visit(this);
  }

  void Instruction_return::accept(InstructionVisitor *visitor) {
    return visitor->visit(this);
  }

  void Instruction_branch::accept(InstructionVisitor *visitor) {
    return visitor->visit(this);
  }

  void Instruction_branch_conditional::accept(InstructionVisitor *visitor) {
    return visitor->visit(this);
  }

  void Instruction_call::accept(InstructionVisitor *visitor) {
    return visitor->visit(this);
  }

  void Instruction_label::accept(InstructionVisitor *visitor) {
    return visitor->visit(this);
  }


}
