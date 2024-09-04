#include "IR.h"
#include <iostream>

namespace IR {
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
  //update

  

   string convert_compare(OperatorID op) {
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
     }
     return "FAIL COMPARE";
   }

   string convert_arithmetic(OperatorID op) {
     switch (op) {
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



  // to_string methodes

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
    string ret=convert_compare(op);
    if(ret=="FAIL COMPARE"){
      ret=convert_arithmetic(op);
    }
    return ret;
    //return "compare";
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

  string Instruction_declaration::instruction_to_L3(){
    types.insert({var->to_string(), type->to_string()});
    return ""; //THERE IS NO DECLARATION IN L3
  }

  void Instruction_return::print_instruction() {
    cout << "return" << "\n";
  }

  string Instruction_return::instruction_to_L3(){
    return "return\n";
  }

  void Instruction_return_arg::print_instruction() {
    cout << "return " + s->to_string() << "\n";
  }

  string Instruction_return_arg::instruction_to_L3(){
    return "return "+s->to_string()+"\n";
  }

  void Instruction_assignment::print_instruction() {
    cout << d->to_string() + " <- " + s->to_string() << "\n";
  }

  string Instruction_assignment::instruction_to_L3(){
    return d->to_string() + " <- " + s->to_string() + "\n";
  }

  void Instruction_assignment_op::print_instruction() {
    cout << dst->to_string() + " <- " + t1->to_string() + " " + op->to_string() + " " + t2->to_string() << "\n";
  }

  string Instruction_assignment_op::instruction_to_L3(){
    return dst->to_string() + " <- " + t1->to_string() + " " + op->to_string() + " " + t2->to_string()+"\n";
  }

  void Instruction_read_tensor::print_instruction() {
    string output;
    output += dst->to_string() + " <- " + src->to_string();
    for (auto i : indices) {
      output += "[" + i->to_string() + "]";
    }
    cout << "READ: "+output << "\n";
  }

  string CalculateIndex(vector<Item*> indices,string array){
    string output;
    //output += dst->to_string() + " <- " + src->to_string();
    int t_buff=0;
    if(types[array]=="tuple"){
      t_buff=-8;
    }
    int dimension=0;
    vector<string> loadOrder;
    vector<string> finalVars;
    //string array=src->to_string();
    //cout<<("Start Calculating index:"+to_string(indices.size())+"\n");
    for (auto i : indices) {
      if(dimension==0){dimension++;continue;}
      else{
        string str_dim=to_string(dimension);
        loadOrder.push_back("hold"+str_dim);
        int buffer=(dimension*8)+8;
        output+=array+"_"+str_dim+" <- "+ array +" + "+to_string(buffer)+"\n";
        output+="%hold"+str_dim+" <- load "+array+"_"+str_dim+"\n";
        output+="%hold"+str_dim+" <- %hold"+str_dim+" >> 1 \n";
      }
      dimension++;
    }
    //cout<<"\n"+output+"\n";
    //cout<<("LoadOrder Size: "+to_string(loadOrder.size()));
    //cout<<("Set up Done:"+to_string(indices.size())+"\n");
    output+="%newVar0 <- "+indices[indices.size()-1]->to_string()+"\n";
    finalVars.push_back("%newVar0");
    for (int i=indices.size()-1;i>0;i--){
      //cout<<("Inside loop 1:"+to_string(i)+"\n");
      int k=i-1;
      //cout<<("Got No Data:"+to_string(indices.size())+"\n");
      string currentIndex=indices[k]->to_string();
      //string currentLoad=loadOrder[i];
      //cout<<("Got Half Data:"+to_string(indices.size())+"\n");
      string buffer_name=loadOrder[k];
      //cout<<("Got Data:"+to_string(indices.size())+"\n");
      for(int j=i;j<loadOrder.size();j++){
        //cout<<("Made it inside:"+to_string(indices.size())+"\n");
        output+="%"+loadOrder[j]+"_"+buffer_name+" <- %"+loadOrder[j]+" * %"+buffer_name+"\n";
        buffer_name=loadOrder[j]+"_"+buffer_name;
      }
      output+="%newVar"+to_string(indices.size()-i)+" <- "+currentIndex+" * %"+buffer_name+"\n";
      finalVars.push_back("%newVar"+to_string(indices.size()-i));
    }
    //cout<<("New Vars Created:"+to_string(indices.size()-1)+"\n");
    output+="%index_poop <- "+finalVars.back()+"\n";
    finalVars.pop_back();
    for(auto var : finalVars){
      output+="%index_poop <- %index_poop + "+var+"\n";
    }
    output+="%index_poop <- %index_poop * 8\n";
    output+="%index_poop <- %index_poop + "+to_string(((indices.size())*8)+8+t_buff)+"\n";
    output+="%index_poop <- "+array+" + %index_poop\n";
    return output;
  }

  string Instruction_read_tensor::instruction_to_L3(){
    string output=CalculateIndex(indices,src->to_string()); 
    //cout<<dst->to_string()+"\n";
    //cout<<src->to_string()+"\n";
    output+=dst->to_string()+" <- load %index_poop\n";

    //output+="store %index_poop <- "+dst->to_string()+"\n";
    return output;
  }

  void Instruction_write_tensor::print_instruction() {
    string output;
    output += dst->to_string();
    for (auto i : indices) {
      output += "[" + i->to_string() + "]";
    }
    output += " <- " + src->to_string();
    cout << "WRITE:"+ output << "\n";
  }

  string Instruction_write_tensor::instruction_to_L3(){
    string output=CalculateIndex(indices,dst->to_string()); 
    //cout<<dst->to_string()+"\n";
    //cout<<src->to_string()+"\n";
    output+="store %index_poop <- "+src->to_string()+"\n";
    //output+=dst->to_string()+" <- load %index_poop\n";
    return output;
  }

  void Instruction_length::print_instruction() {
    cout << dst->to_string() + " <- length " + src->to_string() + " " + index->to_string() << "\n";
  }

  string Instruction_length::instruction_to_L3(){
    string dest=dst->to_string();
    string srce=src->to_string();
    auto num=index->to_string();
    int t_buff=0;
    //cout<<"TYPE (LENGTH)"+types[srce];
    if(types[srce]=="tuple"){
      t_buff=-8;
    }
    //if(num.front()!='%'){num=to_string(stoi(num)*8);}
    string return_string = "";
    //return_string+="%offset <- "+index->to_string()+" * 8\n";
    return_string+="%address <- "+num+ " * 8\n"; //POTENTIAL ERROR
    return_string+="%address <- %address + "+to_string(8+t_buff)+"\n"; //NEEDS TO BE 0 FOR TUPLES
    return_string+="%address <- %address + "+srce+"\n";
    return_string+=dest + "<- load %address\n";
    if(t_buff!=0){
      return_string+=dest + " <- "+ dest +" << 1\n";//ONLY LIKES TUPLES
      return_string+=dest +" <- "+dest+" + 1\n"; //ONLY LIKES TUPLES
    }
    return return_string;
  }

  void Instruction_new_array::print_instruction() {
    cout << dst->to_string() + " <- new Array(" + sizes->to_string() + ")" << "\n";
  }

  string Instruction_new_array::instruction_to_L3(){
    string return_string="";
    //return "";
    int cur_eight=8;
    auto args=dynamic_cast<Args*>(sizes)->getArgs();
    return_string+="%hold_0 <- 1\n";
    for(int i=0;i<args.size();i++){
      return_string+="%hold_"+to_string(i+1)+" <- "+args[i]->to_string()+" >> 1 \n";
      return_string+="%hold_0 <- %hold_0 * %hold_"+to_string(i+1)+"\n";
    }
    return_string+="%hold_0 <- %hold_0 << 1\n";
    return_string+="%hold_0 <- %hold_0 + 1\n";
    return_string+="%hold_0 <- %hold_0 + "+to_string(2*args.size())+"\n";
    return_string+=dst->to_string()+" <- call allocate(%hold_0,1)\n";
    for(int i=0;i<args.size();i++){
      return_string+="%hold_"+to_string(i+1)+" <- "+dst->to_string()+" + "+to_string(cur_eight)+"\n";
      return_string+="store %hold_"+to_string(i+1)+" <- "+args[i]->to_string()+"\n";
      cur_eight+=8;
    }
    

    
    return return_string;
  }

  void Instruction_new_tuple::print_instruction() {
    cout << dst->to_string() + " <- new Tuple(" + size->to_string() + ")" << "\n";
  }

  string Instruction_new_tuple::instruction_to_L3(){
    string return_string="";
    //return "";
    auto num=size->to_string();
    int Num;
    if(num.front()!='%'){
      Num=(stoi(num));
      //return_string+="%temp <- "+to_string(Num)+"\n";
      num=to_string(Num);
    }
    return_string+=dst->to_string()+" <- call allocate("+num+",1"+")\n";
    return return_string;
  }


  void Instruction_label::print_instruction() {
    cout << d->to_string() << "\n";
  }

  string Instruction_label::instruction_to_L3(){
    return d->to_string()+"\n";
  }

  void Instruction_branch::print_instruction() {
    cout << "br " + d->to_string() << "\n";
  }

  string Instruction_branch::instruction_to_L3(){
    return "br "+d->to_string()+"\n";
  }

  void Instruction_branch_conditional::print_instruction() {
    cout << "br " + c->to_string() + " " + dst1->to_string() + " " + dst2->to_string() << "\n";
  }  

  string Instruction_branch_conditional::instruction_to_L3(){
    string return_string= "br " + c->to_string() + " " + dst1->to_string() + "\n";
    return_string+="br " + dst2->to_string()+"\n";
    return return_string;
  }

  void Instruction_call::print_instruction() {
    cout << "call " + callee->to_string() + "(" + args->to_string() + ")" << "\n";
  }

  string Instruction_call::instruction_to_L3(){
    return "call " + callee->to_string() + "(" + args->to_string() + ")\n";
  }

  void Instruction_assignment_call::print_instruction() {
    cout << d->to_string() + " <- call " + callee->to_string() + "(" + args->to_string() + ")" << "\n";
  }

  string Instruction_assignment_call::instruction_to_L3(){
    return d->to_string() + " <- call " + callee->to_string() + "(" + args->to_string() + ")\n";
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
