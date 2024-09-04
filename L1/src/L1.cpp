#include "L1.h"
#include <iostream>

using namespace std;

namespace L1 {

  Register::Register (RegisterID r)
    : ID { r } {
    return ;
  }

  RegisterID Register::getID(){
    return ID;
  }

  Number::Number (int64_t number)
    : number { number } { 
    return ;
  }

  int64_t Number::getNum(){
    return number;
  }

  Label::Label (std::string label)
    : label { label } { 
      return ;
  }

  std::string Label::getLabel(){
    return label;
  }

  L::L (std::string l)
    : l { l }{
    return;
  }

  std::string L::getL(){
    return l;
  }

  Operator::Operator (OperatorID o)
    : ID { o } {
      return ;
  }

  OperatorID Operator::getID(){
    return ID;
  }

  Compare::Compare (Item* b, Item* a, Item* op)
    : b { b },
      a { a },
      op { op } {
      return ;
      }

  Item* Compare::getA(){
    return a;
  }

  Item* Compare::getB(){
    return b;
  }

  Item* Compare::getOp(){
    return op;
  }

  Mem::Mem (Item* x, Item* M)
    : x { x },
      M { M } {
      return ;
  }

  Item* Mem::getX(){
    return x;
  }

  Item* Mem::getM(){
    return M;
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

  Instruction_assignment_compare::Instruction_assignment_compare (Item *dst, Item *src)
    : s { src },
      d { dst } {
    return ;
  }
  
  Item* Instruction_assignment_compare::getSource(){
    return s;
  }

  Item* Instruction_assignment_compare::getDestination(){
    return d;
  }

  Instruction_inc::Instruction_inc (Item* dst)
    : d { dst } {
    return ;
  }

  Item* Instruction_inc::getDestination(){
    return d;
  }

  Instruction_dec::Instruction_dec (Item* dst)
    : d { dst } {
    return ;
  }

  Item* Instruction_dec::getDestination(){
    return d;
  }

  Instruction_arithmetic::Instruction_arithmetic (Item* dst, Item* src, Item* op)
    : s { src },
      d { dst },
      o { op } {
    return ;
  }

  Item* Instruction_arithmetic::getSource(){
    return s;
  }

  Item* Instruction_arithmetic::getDestination(){
    return d;
  }

  Item* Instruction_arithmetic::getOperator(){
    return o;
  }

  Instruction_shift::Instruction_shift (Item* dst, Item* src, Item* op)
    : s { src },
      d { dst },
      o { op } {
    return ;
  }

  Item* Instruction_shift::getSource(){
    return s;
  }

  Item* Instruction_shift::getDestination(){
    return d;
  }

  Item* Instruction_shift::getOperator(){
    return o;
  }



  Instruction_goto::Instruction_goto (Item *dst)
    : dst { dst } {
    return ;
  }
  Item* Instruction_goto::getDestination(){
    return dst;
  }

  Instruction_cjump::Instruction_cjump (Item* cmp, Item* label)
    : cmp { cmp },
      label { label } {
      return;
    }

  Item* Instruction_cjump::getCompare(){
    return cmp;
  }

  Item* Instruction_cjump::getLabel(){
    return label;
  }

  Instruction_at::Instruction_at (Item* saveTo, Item* add, Item* mult, Item* by)
    : saveTo { saveTo },
      add { add },
      mult { mult },
      by { by } {
      return;
    }

  Item* Instruction_at::getSaveTo(){
    return saveTo;
  }

  Item* Instruction_at::getAdd(){
    return add;
  }

  Item* Instruction_at::getMult(){
    return mult;
  }

  Item* Instruction_at::getBy(){
    return by;
  }

  Instruction_call::Instruction_call (Item* func, Item* params)
    : func { func },
      params { params } {
      return;
    }
  Item* Instruction_call::getFunc(){
    return func;
  }

  Item* Instruction_call::getParamNum(){
    return params;
  }

  Instruction_label::Instruction_label (Item* label)
    : label { label } {
      return;
    }
  Item* Instruction_label::getLabel(){
    return label;
  }

  vector<string> Register::accept(ItemVisitor *visitor) {
    return visitor->visit(this);
  }

  vector<string> Number::accept(ItemVisitor *visitor) {
    return visitor->visit(this);
  }

  vector<string> Label::accept(ItemVisitor *visitor) {
    return visitor->visit(this);
  }

  vector<string> L::accept(ItemVisitor *visitor) {
    return visitor->visit(this);
  }

  vector<string> Operator::accept(ItemVisitor *visitor) {
    return visitor->visit(this);
  }

  vector<string> Compare::accept(ItemVisitor *visitor) {
    return visitor->visit(this);
  }

  vector<string> Mem::accept(ItemVisitor *visitor) {
    return visitor->visit(this);
  }

  void Instruction_ret::accept(InstructionVisitor *visitor) {
    visitor->visit(this);
  }

  void Instruction_assignment::accept(InstructionVisitor *visitor) {
    visitor->visit(this);
  }

  void Instruction_assignment_compare::accept(InstructionVisitor *visitor) {
    visitor->visit(this);
  }

  void Instruction_inc::accept(InstructionVisitor *visitor) {
    visitor->visit(this);
  }

  void Instruction_dec::accept(InstructionVisitor *visitor) {
    visitor->visit(this);
  }

  void Instruction_arithmetic::accept(InstructionVisitor *visitor) {
    visitor->visit(this);
  }

  void Instruction_shift::accept(InstructionVisitor *visitor) {
    visitor->visit(this);
  }

  void Instruction_goto::accept(InstructionVisitor *visitor) {
    visitor->visit(this);
  }

  void Instruction_cjump::accept(InstructionVisitor *visitor) {
    visitor->visit(this);
  }

  void Instruction_at::accept(InstructionVisitor *visitor) {
    visitor->visit(this);
  }

  void Instruction_call::accept(InstructionVisitor *visitor) {
    visitor->visit(this);
  }

  void Instruction_label::accept(InstructionVisitor *visitor) {
    visitor->visit(this);
  }



  void print(std::string str) {
    std::cout << str << "\n";
  }

}
