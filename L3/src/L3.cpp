#include "L3.h"
#include <iostream>

namespace L3 {

  void print(std::string line) {
    // Change this to activate all prints
    if (false) {
      std::cout << line << "\n";
    }
  }

  string convert_compare(Compare_OperatorID op) {
    switch (op) {
      case Compare_OperatorID::l:
        return "<";
      case Compare_OperatorID::le:
        return "<=";
      case Compare_OperatorID::e:
        return "=";
      case Compare_OperatorID::ge:
        return ">=";
      case Compare_OperatorID::g:
        return ">";
    }
    return "FAIL COMPARE";
  }

  string convert_arithmetic(Arithmetic_OperatorID op) {
    switch (op) {
      case Arithmetic_OperatorID::addq:
        return "+";
      case Arithmetic_OperatorID::subq:
        return "-";
      case Arithmetic_OperatorID::imultq:
        return "*";
      case Arithmetic_OperatorID::andq:
        return "&";
      case Arithmetic_OperatorID::sarq:
        return ">>";
      case Arithmetic_OperatorID::salq:
        return "<<";
    }
    return "FAIL ARITHMETIC";
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

  Variable::Variable (std::string name)
    : name { name }{
    return ;
  }

  string Variable::get_name() {
    return name;
  }

  Number::Number (int64_t number)
    : number { number }{
    return ;
  }

  int64_t Number::get_number() {
    return number;
  }

  Compare_Operator::Compare_Operator (Compare_OperatorID op)
    : op { op }{
    return ;
  }

  Compare_OperatorID Compare_Operator::get_op() {
    return op;
  }

  Arithmetic_Operator::Arithmetic_Operator (Arithmetic_OperatorID op)
    : op { op }{
    return ;
  }

  Arithmetic_OperatorID Arithmetic_Operator::get_op() {
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

  Instruction_assignment::Instruction_assignment (Item *dst, Item *src)
    : s { src },
      d { dst } {
    return ;
  }


  Instruction_assignment_op::Instruction_assignment_op (Item *dst, Item *t1, Item *op, Item *t2)
    : dst { dst },
      t1 { t1 },
      op { op },
      t2 { t2 } {
    return ;
  }

  Instruction_assignment_cmp::Instruction_assignment_cmp (Item *dst, Item *t1, Item *cmp, Item *t2)
    : dst { dst },
      t1 { t1 },
      cmp { cmp },
      t2 { t2 } {
    return ;
  }

  Instruction_store::Instruction_store (Item *dst, Item *src)
    : s { src },
      d { dst } {
    return ;
  }

  Instruction_load::Instruction_load (Item *dst, Item *src)
    : s { src },
      d { dst } {
    return ;
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

  Instruction_branch_conditional::Instruction_branch_conditional (Item *cond, Item *dst)
    : c { cond },
      d { dst } {
    return ;
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

  string L::to_string() {
    return name;
  }
  
  string Label::to_string() {
    return name;
  }

  string Variable::to_string() {
    return name;
  }

  string Number::to_string() {
    return std::to_string(number);
  }

  string Compare_Operator::to_string() {
    return convert_compare(op);
    // return "compare";
  }

  string Arithmetic_Operator::to_string() {
    return convert_arithmetic(op);
    // return "arithmetic";
  }

  string Vars::to_string() {
    std::string output;
    for (auto var : vars) {
      output += var->to_string();
    }
    return output;
  }

  string Args::to_string() {
    std::string output;
    for (auto arg : args) {
      output += arg->to_string();
    }
    return output;
  }

  void Instruction_return::print_instruction() {
    print("return");
  }

  void Instruction_return_arg::print_instruction() {
    print("return " + s->to_string());
  }

  void Instruction_assignment::print_instruction() {
    print(d->to_string() + " <- " + s->to_string());
  }

  Item* Instruction_assignment::getSource(){
    return s;
  }

  Item* Instruction_assignment::getDestination(){
    return d;
  }

  void Instruction_assignment_op::print_instruction() {
    print(dst->to_string() + " <- " + t1->to_string() + " " + op->to_string() + " " + t2->to_string());
  }

  std::vector<Item*> Instruction_assignment_op::getSources(){
    std::vector<Item*> sources;
    sources.push_back(t1);
    sources.push_back(t2);
    return sources;
  }

  Item* Instruction_assignment_op::getDestination(){
    return dst;
  }


  Item* Instruction_assignment_op::getOperator(){
    return op;
  }

  void Instruction_assignment_cmp::print_instruction() {
    print(dst->to_string() + " <- " + t1->to_string() + " " + cmp->to_string() + " " + t2->to_string());
  }

  std::vector<Item*> Instruction_assignment_cmp::getSources(){
    std::vector<Item*> sources;
    sources.push_back(t1);
    sources.push_back(t2);
    return sources;
  }

  Item* Instruction_assignment_cmp::getDestination(){
    return dst;
  }


  Item* Instruction_assignment_cmp::getCompareOperator(){
    return cmp;
  }

  void Instruction_store::print_instruction() {
    print("store " + d->to_string() + " <- " + s->to_string());
  }

  Item* Instruction_store::getSource(){
    return s;
  }

  Item* Instruction_store::getDestination(){
    return d;
  }

  void Instruction_load::print_instruction() {
    print(d->to_string() + " <- load " + s->to_string());
  }

  Item* Instruction_load::getSource(){
    return s;
  }

  Item* Instruction_load::getDestination(){
    return d;
  }

  void Instruction_label::print_instruction() {
    print(d->to_string());
  }

  void Instruction_branch::print_instruction() {
    print("br " + d->to_string());
  }

  Item* Instruction_branch::getDestination(){
    return d;
  }

  void Instruction_branch_conditional::print_instruction() {
    print("br " + c->to_string() + " " + d->to_string());
  }  

  Item* Instruction_branch_conditional::getDestination(){
    return d;
  }

  Item* Instruction_branch_conditional::getCondition(){
    return c;
  }

  void Instruction_call::print_instruction() {
    print("call " + callee->to_string() + "(" + args->to_string() + ")");
  }

  void Instruction_assignment_call::print_instruction() {
    print(d->to_string() + " <- call " + callee->to_string() + "(" + args->to_string() + ")");
  }



  vector<string> L::accept(ItemVisitor *visitor) {
    return visitor->visit(this);
  }

  vector<string> Label::accept(ItemVisitor *visitor) {
    return visitor->visit(this);
  }

  vector<string> Variable::accept(ItemVisitor *visitor) {
    return visitor->visit(this);
  }

  vector<string> Number::accept(ItemVisitor *visitor) {
    return visitor->visit(this);
  }

  vector<string> Compare_Operator::accept(ItemVisitor *visitor) {
    return visitor->visit(this);
  }

  vector<string> Arithmetic_Operator::accept(ItemVisitor *visitor) {
    return visitor->visit(this);
  }

  vector<string> Vars::accept(ItemVisitor *visitor) {
    return visitor->visit(this);
  }

  vector<string> Args::accept(ItemVisitor *visitor) {
    return visitor->visit(this);
  }


  void Instruction_assignment::accept(InstructionVisitor *visitor) {
    return visitor->visit(this);
  }

  void Instruction_assignment_op::accept(InstructionVisitor *visitor) {
    return visitor->visit(this);
  }

  void Instruction_assignment_cmp::accept(InstructionVisitor *visitor) {
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

  void Instruction_load::accept(InstructionVisitor *visitor) {
    return visitor->visit(this);
  }

  void Instruction_store::accept(InstructionVisitor *visitor) {
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
