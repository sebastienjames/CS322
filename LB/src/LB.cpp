#include "LB.h"
#include <iostream>

namespace LB {
  map<string, string> types;
  void print(std::string line) {
    // Change this to activate all prints
    if (true) {
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

  string L::getName() {
    return name;
  }

  Label::Label (std::string name)
    : name { name }{
    return ;
  }

  string Label::getName() {
    return name;
  }

  Name::Name (std::string name)
    : name { name }{
    return ;
  }

  string Name::getName() {
    return name;
  }

  Number::Number (int64_t number)
    : number { number }{
    return ;
  }

  int64_t Number::getNumber() {
    return number;
  }

  Type::Type (TypeID type, int64_t dims)
    : t { type },
      num_dimensions { dims } {
      return;
  }

  TypeID Type::getType() {
    return t;
  }

  Operator::Operator (OperatorID op)
    : op { op } {
    return ;
  }

  OperatorID Operator::getOp() {
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

  Names::Names (std::vector<Item*> names)
    : names { names }{
    return ;
  }

  vector<Item*> Names::getNames () {
    return names;
  }

  Instruction_return_arg::Instruction_return_arg (Item *src)
    : s { src } {
    return ;
  }

  Item* Instruction_return_arg::getValue() {
    return s;
  }

  Instruction_declaration::Instruction_declaration (Item *type, Item *var)
    : type { type },
      var { var } {
    return ;
  }

  Item* Instruction_declaration::getType() {
    return type;
  }

  Item* Instruction_declaration::getVar() {
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

  Item* Instruction_label::getLabel () {
    return d;
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


  Scope::Scope (vector<Instruction*> instructions)
    : instructions { instructions } {
    return ;
  }

  vector<Instruction*> Scope::getInstructions() {
    return instructions;
  }

  Instruction_if::Instruction_if (Item* t1, Item *op, Item* t2, Item *label_true, Item *label_false)
    : t1 { t1 },
      op { op },
      t2 { t2 },
      label_true { label_true },
      label_false { label_false } {
    return ;
  }

  Item* Instruction_if::getCompare1() {
    return t1;
  }

  Item* Instruction_if::getOp() {
    return op;
  }

  Item* Instruction_if::getCompare2() {
    return t2;
  }

  Item* Instruction_if::getTrue() {
    return label_true;
  }

  Item* Instruction_if::getFalse() {
    return label_false;
  }

  Instruction_while::Instruction_while(Item *t1, Item *op, Item* t2, Item *label_true, Item* label_false)
    : t1 { t1 },
      op { op },
      t2 { t2 },
      label_true { label_true },
      label_false { label_false } {
    return ;
  }

  Item* Instruction_while::getCompare1() {
    return t1;
  }

  Item* Instruction_while::getOp() {
    return op;
  }

  Item* Instruction_while::getCompare2() {
    return t2;
  }

  Item* Instruction_while::getTrue() {
    return label_true;
  }

  Item* Instruction_while::getFalse() {
    return label_false;
  }

  Instruction_goto::Instruction_goto (Item* dst)
    : dst { dst }{
    return ;
  }

  Item* Instruction_goto::getDest() {
    return dst;
  }



  // to_string methods

  string L::to_string() {
    return name;
  }
  
  string Label::to_string() {
    return name;
  }

  string Name::to_string() {
    return name;
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

  string Names::to_string() {
    string output;

    for (auto i: names) {
      output += i->to_string() + ", ";
    }
    
    return output;
  }

  void Instruction_start_scope::printInstruction() {
    cout << "{\n";
  }

  void Instruction_end_scope::printInstruction() {
    cout << "}\n";
  }

  void Instruction_declaration::printInstruction() {
    cout << type->to_string() + " " + var->to_string() << "\n";
  }

  void Instruction_return::printInstruction() {
    cout << "return" << "\n";
  }

  void Instruction_return_arg::printInstruction() {
    cout << "return " + s->to_string() << "\n";
  }

  void Instruction_assignment::printInstruction() {
    cout << d->to_string() + " <- " + s->to_string() << "\n";
  }

  void Instruction_assignment_op::printInstruction() {
    cout << dst->to_string() + " <- " + t1->to_string() + " " + op->to_string() + " " + t2->to_string() << "\n";
  }

  void Instruction_read_tensor::printInstruction() {
    string output;
    output += dst->to_string() + " <- " + src->to_string();
    for (auto i : indices) {
      output += "[" + i->to_string() + "]";
    }
    cout << output << "\n";
  }

  void Instruction_write_tensor::printInstruction() {
    string output;
    output += dst->to_string();
    for (auto i : indices) {
      output += "[" + i->to_string() + "]";
    }
    output += " <- " + src->to_string();
    cout << output << "\n";
  }


  void Instruction_length::printInstruction() {
    cout << dst->to_string() + " <- length " + src->to_string() + " " + index->to_string() << "\n";
  }


  void Instruction_new_array::printInstruction() {
    cout << dst->to_string() + " <- new Array(" + sizes->to_string() + ")" << "\n";
  }


  void Instruction_new_tuple::printInstruction() {
    cout << dst->to_string() + " <- new Tuple(" + size->to_string() + ")" << "\n";
  }


  void Instruction_label::printInstruction() {
    cout << d->to_string() << "\n";
  }


  void Instruction_call::printInstruction() {
    cout << "call " + callee->to_string() + "(" + args->to_string() + ")" << "\n";
  }

  void Instruction_assignment_call::printInstruction() {
    cout << d->to_string() + " <- call " + callee->to_string() + "(" + args->to_string() + ")" << "\n";
  }

  void Scope::printInstruction() {
    cout << "SCOPE\n";
  }

  void Instruction_if::printInstruction() {
    cout << "if (" + t1->to_string() + " " + op->to_string() + " " + t2->to_string() + ") " + label_true->to_string() + " " + label_false->to_string() + "\n";
  }

  void Instruction_while::printInstruction() {
    cout << "while (" + t1->to_string() + " " + op->to_string() + " " + t2->to_string() + ") " + label_true->to_string() + " " + label_false->to_string() + "\n";
  }

  void Instruction_goto::printInstruction() {
    cout << "goto " + dst->to_string() + "\n";
  }

  void Instruction_continue::printInstruction() {
    cout << "continue\n";
  }

  void Instruction_break::printInstruction() {
    cout << "break\n";
  }

  string Instruction_start_scope::InstructionToLA(){
    // types.insert({var->to_string(), type->to_string()});
    return "{\n"; 
  }

  string Instruction_end_scope::InstructionToLA(){
    return "}\n"; 
  }

  string Instruction_declaration::InstructionToLA(){
    string output_str=" ";
    //Item* Var=var->getVar();
    Names* names=dynamic_cast<Names*>(var);
    for(auto name : names->getNames()){
      output_str+=type->to_string() + " " + name->to_string() + "\n"; 
    }
    return output_str;
  }

  string Instruction_return::InstructionToLA(){
    return " return\n";
  }

  string Instruction_return_arg::InstructionToLA(){
    return " return " + s->to_string() + "\n";
  }

  string Instruction_assignment::InstructionToLA(){
    return d->to_string() + " <- " + s->to_string() + "\n";
  }

  string Instruction_assignment_op::InstructionToLA(){
    return dst->to_string() + " <- " + t1->to_string() + " " + op->to_string() + " " + t2->to_string() + "\n";
  }

  string Instruction_write_tensor::InstructionToLA(){
    string output;

    output += dst->to_string();
    for (auto elem : indices) {
      output += "[" + elem->to_string() + "]";
    }
    output += " <- " + src->to_string() + "\n";
    return output;
  }

  string Instruction_read_tensor::InstructionToLA(){
    string output;
    output += dst->to_string() + " <- " + src->to_string();
    for (auto elem : indices) {
      output += "[" + elem->to_string() + "]";
    }
    output += "\n";
    return output;
  }

  string Instruction_length::InstructionToLA(){
    return dst->to_string() + " <- length " + src->to_string() + " " + index->to_string() + "\n";
  }

  string Instruction_new_array::InstructionToLA(){
    return dst->to_string() + " <- new Array(" + sizes->to_string() + ")\n";
  }

  string Instruction_new_tuple::InstructionToLA(){
    return dst->to_string() + " <- new Tuple(" + size->to_string() + ")\n";
  }

  string Instruction_label::InstructionToLA(){
    return d->to_string()+"\n";
  }



  string Instruction_call::InstructionToLA(){
    
    return callee->to_string() + "(" + args->to_string() + ")\n";
  }

  string Instruction_assignment_call::InstructionToLA() {
    return d->to_string() + " <- " + callee->to_string() + "(" + args->to_string() + ")\n";
  }

  string Instruction_if::InstructionToLA(){
    string add=label_true->to_string();
    string newVar="hold_"+add.substr(1,add.length());
    string str_out="int64 "+newVar+"\n";
    str_out+=newVar+" <- "+t1->to_string()+" "+op->to_string()+" "+t2->to_string()+"\n";
    str_out+="br "+newVar+" "+label_true->to_string()+" "+label_false->to_string()+"\n";
    return str_out;
  }

  string Instruction_while::InstructionToLA(){
    string add=label_true->to_string();
    string newVar="hold_"+add.substr(1,add.length());
    string str_out="int64 "+newVar+"\n";
    str_out+=newVar+" <- "+t1->to_string()+" "+op->to_string()+" "+t2->to_string()+"\n";
    str_out+="br "+newVar+" "+label_true->to_string()+" "+label_false->to_string()+"\n";
    return str_out;
  }

  string Instruction_goto::InstructionToLA(){
    //cout<<("HERE");
    return "br "+dst->to_string()+"\n";
  }

  string Instruction_continue::InstructionToLA(){
    return "continue\n";
  }

  string Instruction_break::InstructionToLA(){
    return "break\n";
  }

  string Instruction_branch::InstructionToLA(){
    return "br " + d->to_string() + "\n";
  }

  string Instruction_branch_conditional::InstructionToLA(){
    return "br " + c->to_string() + " " + dst1->to_string() + " " + dst2->to_string() + "\n";
  }

  string Scope::InstructionToLA(){
    return "SCOPE\n";
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

  vector<string> Names::accept(ItemVisitor *visitor) {
    return visitor->visit(this);
  }

  Item* L::translateSelf(ItemVisitor *visitor) {
    return visitor->translate(this);
  }

  Item* Label::translateSelf(ItemVisitor *visitor) {
    return visitor->translate(this);
  }

  Item* Name::translateSelf(ItemVisitor *visitor) {
    return visitor->translate(this);
  }

  Item* Type::translateSelf(ItemVisitor *visitor) {
    return visitor->translate(this);
  }

  Item* Operator::translateSelf(ItemVisitor *visitor) {
    return visitor->translate(this);
  }

  Item* Number::translateSelf(ItemVisitor *visitor) {
    return visitor->translate(this);
  }

  Item* Vars::translateSelf(ItemVisitor *visitor) {
    return visitor->translate(this);
  }

  Item* Args::translateSelf(ItemVisitor *visitor) {
    return visitor->translate(this);
  }

  Item* Names::translateSelf(ItemVisitor *visitor) {
    return visitor->translate(this);
  }

  void Instruction_start_scope::accept(InstructionVisitor *visitor) {
    return visitor->visit(this);
  }

  void Instruction_end_scope::accept(InstructionVisitor *visitor) {
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

  void Instruction_call::accept(InstructionVisitor *visitor) {
    return visitor->visit(this);
  }

  void Instruction_label::accept(InstructionVisitor *visitor) {
    return visitor->visit(this);
  }

  void Instruction_if::accept(InstructionVisitor *visitor) {
    return visitor->visit(this);
  }

  void Instruction_while::accept(InstructionVisitor *visitor) {
    return visitor->visit(this);
  }

  void Instruction_goto::accept(InstructionVisitor *visitor) {
    return visitor->visit(this);
  }

  void Instruction_continue::accept(InstructionVisitor *visitor) {
    return visitor->visit(this);
  }

  void Instruction_break::accept(InstructionVisitor *visitor) {
    return visitor->visit(this);
  }

  void Scope::accept(InstructionVisitor *visitor) {
    return visitor->visit(this);
  }

  


}
