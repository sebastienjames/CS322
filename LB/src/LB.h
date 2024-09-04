#pragma once

#include <vector>
#include <string>
#include <map>
#include <set>

using namespace std;

namespace LB {

  enum OperatorID {addq, subq, imultq, andq, salq, sarq, l, le, e, ge, g};
  enum TypeID {int64, tuple, code, void_type};
  class ItemVisitor;
  class InstructionVisitor;

  class Item {
    public:
      virtual string to_string() = 0;
      virtual vector<string> accept(ItemVisitor *visitor) = 0;
      virtual Item* translateSelf(ItemVisitor *visitor) =0;
  };

  class L : public Item {
    public:
      L (string name);
      virtual string to_string() override;
      string getName();
      vector<string> accept(ItemVisitor *visitor) override; 
      Item* translateSelf(ItemVisitor *visitor) override;
    private:
      string name;
  };

  class Label : public Item {
    public:
      Label (string name);
      virtual string to_string() override;
      string getName();
      vector<string> accept(ItemVisitor *visitor) override; 
      Item* translateSelf(ItemVisitor *visitor) override;
    private:
      string name;
  };

  class Name : public Item {
    public:
      Name (string name);
      virtual string to_string() override;
      string getName();
      vector<string> accept(ItemVisitor *visitor) override; 
      Item* translateSelf(ItemVisitor *visitor) override;
    private:
      string name;
  };

  class Number : public Item {
    public:
      Number (int64_t number);
      virtual string to_string() override;
      int64_t getNumber();
      vector<string> accept(ItemVisitor *visitor) override; 
      Item* translateSelf(ItemVisitor *visitor) override;
    private:
      int64_t number;
  };

  class Type : public Item {
    public:
      Type (TypeID type, int64_t dims);
      virtual string to_string() override;
      TypeID getType();
      vector<string> accept(ItemVisitor *visitor) override; 
      Item* translateSelf(ItemVisitor *visitor) override;
    private:
      TypeID t;
      int64_t num_dimensions;
  };

  class Operator : public Item {
    public:
      Operator (OperatorID op);
      virtual string to_string() override;
      OperatorID getOp();
      vector<string> accept(ItemVisitor *visitor) override; 
      Item* translateSelf(ItemVisitor *visitor) override;
    private:
      OperatorID op;
  };


  class Vars : public Item {
    public:
      Vars (vector<Item*> vars);
      virtual string to_string() override;
      int64_t getNumVars();
      vector<Item*> getVars();
      vector<string> accept(ItemVisitor *visitor) override; 
      Item* translateSelf(ItemVisitor *visitor) override;
      
    private:
      vector<Item*> vars;
  };

  class Args : public Item {
    public:
      Args (vector<Item*> args);
      virtual string to_string() override;
      int64_t getNumArgs();
      vector<Item*> getArgs();
      vector<string> accept(ItemVisitor *visitor) override; 
      Item* translateSelf(ItemVisitor *visitor) override;
    private:
      vector<Item*> args;
  };

  class Names : public Item {
    public:
      Names (vector<Item*> names);
      virtual string to_string() override;
      vector<Item*> getNames();
      vector<string> accept(ItemVisitor *visitor) override; 
      Item* translateSelf(ItemVisitor *visitor) override;
    private:
      vector<Item*> names;
  };

  /*
   * Instruction interface.
   */
  class Instruction{
    public:
    virtual void printInstruction() = 0;
    virtual string InstructionToLA() = 0;
    virtual void accept(InstructionVisitor *visitor) = 0;
    int lineNumber;
  };

  /*
   * Instructions.
   */

  class Instruction_return : public Instruction{
    public:
      virtual void printInstruction() override;
      virtual void accept(InstructionVisitor *visitor) override;
      virtual string InstructionToLA() override;
  };

  class Instruction_start_scope : public Instruction{
    public:
      virtual void printInstruction() override;
      virtual void accept(InstructionVisitor *visitor) override;
      virtual string InstructionToLA() override;
  };

  class Instruction_end_scope : public Instruction{
    public:
      virtual void printInstruction() override;
      virtual void accept(InstructionVisitor *visitor) override;
      virtual string InstructionToLA() override;
  };

  class Instruction_return_arg : public Instruction{
    public:
      Instruction_return_arg (Item *src);
      virtual void printInstruction() override;
      virtual string InstructionToLA() override;
      Item* getValue();
      virtual void accept(InstructionVisitor *visitor) override;
    private:
      Item *s;
  };

  class Instruction_declaration : public Instruction{
    public:
      Instruction_declaration (Item* type, Item* var);
      virtual void printInstruction() override;
      Item* getType();
      Item* getVar();
      virtual void accept(InstructionVisitor *visitor) override;
      virtual string InstructionToLA() override;
    private:
      Item *type;
      Item *var;
  };

  class Instruction_assignment : public Instruction{
    public:
      Instruction_assignment (Item *dst, Item *src);
      Item* getSource();
	    Item* getDestination();
      virtual void printInstruction() override;
      virtual string InstructionToLA() override;
      virtual void accept(InstructionVisitor *visitor) override;
    private:
      Item *s;
      Item *d;
  };

  class Instruction_assignment_op : public Instruction{
    public:
      Instruction_assignment_op (Item *dst, Item *t1, Item *op, Item *t2);
      virtual void printInstruction() override;
      virtual string InstructionToLA() override;
      Item* getSource1();
      Item* getSource2();
	    Item* getDestination();
	    Item* getOperator();
      virtual void accept(InstructionVisitor *visitor) override;
    private:
      Item *dst;
      Item *t1;
      Item *op;
      Item *t2;
  }; 

  class Instruction_read_tensor : public Instruction{
    public:
      Instruction_read_tensor (Item* dst, Item *src, vector<Item*> indices);
      virtual void printInstruction() override;
      virtual string InstructionToLA() override;
	    Item* getDestination();
	    Item* getSource();
      vector<Item*> getIndices();
      virtual void accept(InstructionVisitor *visitor) override;
    private:
      Item *dst;
      Item *src;
      vector<Item*> indices;
  }; 

  class Instruction_write_tensor : public Instruction{
    public:
      Instruction_write_tensor (Item* dst, Item *src, vector<Item*> indices);
      virtual void printInstruction() override;
      virtual string InstructionToLA() override;
	    Item* getDestination();
	    Item* getSource();
      vector<Item*> getIndices();
      virtual void accept(InstructionVisitor *visitor) override;
    private:
      Item *dst;
      Item *src;
      vector<Item*> indices;
  }; 

  class Instruction_length : public Instruction{
    public:
      Instruction_length (Item* dst, Item *src, Item* index);
      virtual void printInstruction() override;
      virtual string InstructionToLA() override;
	    Item* getDestination();
	    Item* getSource();
      Item* getIndex();
      virtual void accept(InstructionVisitor *visitor) override;
    private:
      Item *dst;
      Item *src;
      Item* index;
  }; 

  class Instruction_new_array : public Instruction{
    public:
      Instruction_new_array (Item* dst, Item* sizes);
      virtual void printInstruction() override;
      virtual string InstructionToLA() override;
	    Item* getDestination();
	    Item* getSizes();
      virtual void accept(InstructionVisitor *visitor) override;
    private:
      Item *dst;
      Item* sizes;
  }; 

  class Instruction_new_tuple : public Instruction{
    public:
      Instruction_new_tuple (Item* dst, Item* size);
      virtual void printInstruction() override;
      virtual string InstructionToLA() override;
	    Item* getDestination();
	    Item* getSize();
      virtual void accept(InstructionVisitor *visitor) override;
    private:
      Item *dst;
      Item* size;
  }; 


  class Instruction_label : public Instruction{
    public:
      Instruction_label (Item *dst);
      virtual string InstructionToLA() override;
      virtual void printInstruction() override;
      Item* getLabel();
      virtual void accept(InstructionVisitor *visitor) override;
    private:
      Item *d;
  }; 

  class Instruction_branch : public Instruction{
    public:
      Instruction_branch (Item *dst);
      virtual void printInstruction() override;
      virtual string InstructionToLA() override;
      Item* getDestination();
      virtual void accept(InstructionVisitor *visitor) override;
    private:
      Item *d;
  }; 

  class Instruction_branch_conditional : public Instruction{
    public:
      Instruction_branch_conditional (Item *cond, Item *dst1, Item* dst2);
      virtual void printInstruction() override;
      virtual string InstructionToLA() override;
      Item* getDestination1();
      Item* getDestination2();
      Item* getCondition();
      virtual void accept(InstructionVisitor *visitor) override;
    private:
      Item *c;
      Item *dst1;
      Item *dst2;

  }; 

  class Instruction_call : public Instruction{
    public:
      Instruction_call (Item *callee, Item* args);
      virtual void printInstruction() override;
      virtual string InstructionToLA() override;
      Item* getCallee();
      Item* getArgs();
      virtual void accept(InstructionVisitor *visitor) override;
    private:
      Item *callee;
      Item *args;
  }; 

  class Instruction_assignment_call: public Instruction{
    public:
      Instruction_assignment_call (Item *dst, Item *callee, Item* args);
      virtual void printInstruction() override;
      virtual string InstructionToLA() override;
      Item* getDst();
      Item* getCallee();
      Item* getArgs();
      virtual void accept(InstructionVisitor *visitor) override;
    private:
      Item *d;
      Item *callee;
      Item *args;
  }; 

  class Instruction_if : public Instruction{
    public:
      Instruction_if (Item *t1, Item *op, Item* t2, Item *label_true, Item* label_false);
      virtual void printInstruction() override;
      virtual string InstructionToLA() override;
      Item* getCompare1();
      Item* getOp();
      Item* getCompare2();
      Item* getTrue();
      Item* getFalse();
      virtual void accept(InstructionVisitor *visitor) override;
    private:
      Item *t1;
      Item *op;
      Item *t2;
      Item *label_true;
      Item *label_false;
  };

  class Instruction_while : public Instruction{
    public:
      Instruction_while (Item *t1, Item *op, Item* t2, Item *label_true, Item* label_false);
      virtual void printInstruction() override;
      virtual string InstructionToLA() override;
      Item* getCompare1();
      Item* getOp();
      Item* getCompare2();
      Item* getTrue();
      Item* getFalse();
      virtual void accept(InstructionVisitor *visitor) override;
    private:
      Item *t1;
      Item *op;
      Item *t2;
      Item *label_true;
      Item *label_false;
  };

  class Instruction_goto : public Instruction{
    public:
      Instruction_goto (Item *dst);
      virtual void printInstruction() override;
      virtual string InstructionToLA() override;
      Item* getDest();
      virtual void accept(InstructionVisitor *visitor) override;
    private:
      Item *dst;
  };

  class Instruction_continue : public Instruction{
    public:
      virtual void printInstruction() override;
      virtual string InstructionToLA() override;
      virtual void accept(InstructionVisitor *visitor) override;
  };

  class Instruction_break : public Instruction{
    public:
      virtual void printInstruction() override;
      virtual string InstructionToLA() override;
      virtual void accept(InstructionVisitor *visitor) override;
  };

  class Scope: public Instruction{
    public:
      Scope (vector<Instruction*>);
      virtual void printInstruction() override;
      virtual string InstructionToLA() override;
      vector<Instruction*> getInstructions();
      virtual void accept(InstructionVisitor *visitor) override;
    private:
      vector<Instruction*> instructions;
  }; 


  /*
   * Function.
   */
  class Function{
    public:
      Item* return_type;
      string name;
      Item* arguments;
      vector<Instruction *> instructions;
      string longest_name;
  };

  class Program{
    public:
      vector<Function *> functions;
      vector<string> function_names;
  };
  //update

  //map<string, string> types;

  void addType(string var_name,string type_name);

  void print(string line);

  class InstructionVisitor {
      public:
          virtual void visit(Instruction_start_scope *i) = 0;
          virtual void visit(Instruction_end_scope *i) = 0;
          virtual void visit(Instruction_declaration *i) = 0;
          virtual void visit(Instruction_assignment *i) = 0;
          virtual void visit(Instruction_assignment_op *i) = 0;
          virtual void visit(Instruction_read_tensor *i) = 0;
          virtual void visit(Instruction_write_tensor *i) = 0;
          virtual void visit(Instruction_length *i) = 0;
          virtual void visit(Instruction_new_array *i) = 0;
          virtual void visit(Instruction_new_tuple *i) = 0;
          virtual void visit(Instruction_assignment_call *i) = 0;
          virtual void visit(Instruction_return_arg *i) = 0;
          virtual void visit(Instruction_return *i) = 0;
          virtual void visit(Instruction_branch *i) = 0;
          virtual void visit(Instruction_branch_conditional *i) = 0;
          virtual void visit(Instruction_call *i) = 0;
          virtual void visit(Instruction_label *i) = 0;
          virtual void visit(Instruction_if *i) = 0;
          virtual void visit(Instruction_while *i) = 0;
          virtual void visit(Instruction_goto *i) = 0;
          virtual void visit(Instruction_continue *i) = 0;
          virtual void visit(Instruction_break *i) = 0;
          virtual void visit(Scope *i) = 0;
  }; 

  class CodeGenerator : public InstructionVisitor {
      public:
          virtual void visit(Instruction_start_scope *i) override;
          virtual void visit(Instruction_end_scope *i) override;
          virtual void visit(Instruction_declaration *i) override;
          virtual void visit(Instruction_assignment *i) override;
          virtual void visit(Instruction_assignment_op *i) override;
          virtual void visit(Instruction_read_tensor *i) override;
          virtual void visit(Instruction_write_tensor *i) override;
          virtual void visit(Instruction_length *i) override;
          virtual void visit(Instruction_new_array *i) override;
          virtual void visit(Instruction_new_tuple *i) override;
          virtual void visit(Instruction_assignment_call *i) override;
          virtual void visit(Instruction_return_arg *i) override;
          virtual void visit(Instruction_return *i) override;
          virtual void visit(Instruction_branch *i) override;
          virtual void visit(Instruction_branch_conditional *i) override;
          virtual void visit(Instruction_call *i) override;
          virtual void visit(Instruction_label *i) override;
          virtual void visit(Instruction_if *i) override;
          virtual void visit(Instruction_while *i) override;
          virtual void visit(Instruction_goto *i) override;
          virtual void visit(Instruction_continue *i) override;
          virtual void visit(Instruction_break *i) override;
          virtual void visit(Scope *i) override;
          vector<string> function_names;
          string code;
          string clean_func_name(string func_name);
  };

  class ScopeGenerator : public InstructionVisitor {
      public:
          virtual void visit(Instruction_start_scope *i) override;
          virtual void visit(Instruction_end_scope *i) override;
          virtual void visit(Instruction_declaration *i) override;
          virtual void visit(Instruction_assignment *i) override;
          virtual void visit(Instruction_assignment_op *i) override;
          virtual void visit(Instruction_read_tensor *i) override;
          virtual void visit(Instruction_write_tensor *i) override;
          virtual void visit(Instruction_length *i) override;
          virtual void visit(Instruction_new_array *i) override;
          virtual void visit(Instruction_new_tuple *i) override;
          virtual void visit(Instruction_assignment_call *i) override;
          virtual void visit(Instruction_return_arg *i) override;
          virtual void visit(Instruction_return *i) override;
          virtual void visit(Instruction_branch *i) override;
          virtual void visit(Instruction_branch_conditional *i) override;
          virtual void visit(Instruction_call *i) override;
          virtual void visit(Instruction_label *i) override;
          virtual void visit(Instruction_if *i) override;
          virtual void visit(Instruction_while *i) override;
          virtual void visit(Instruction_goto *i) override;
          virtual void visit(Instruction_continue *i) override;
          virtual void visit(Instruction_break *i) override;
          virtual void visit(Scope *i) override;
          vector<string> function_names;
          string code;
          string clean_func_name(string func_name);
  };


  class ItemVisitor {
    public:
      virtual vector<string> visit(Number *i) = 0;
      virtual vector<string> visit(Label *i) = 0;
      virtual vector<string> visit(L *i) = 0;
      virtual vector<string> visit(Name *i) = 0;
      virtual vector<string> visit(Operator *i) = 0;
      virtual vector<string> visit(Type *i) = 0;
      virtual vector<string> visit(Vars *i) = 0;
      virtual vector<string> visit(Args *i) = 0;
      virtual vector<string> visit(Names *i) = 0;
      virtual Item* translate(Number *i) = 0;
      virtual Item* translate(Label *i) = 0;
      virtual Item* translate(L *i) = 0;
      virtual Item* translate(Name *i) = 0;
      virtual Item* translate(Operator *i) = 0;
      virtual Item* translate(Type *i) = 0;
      virtual Item* translate(Vars *i) = 0;
      virtual Item* translate(Args *i) = 0;
      virtual Item* translate(Names *i) = 0;
  }; 

  class ItemLooker : public ItemVisitor {
    public:
      virtual vector<string> visit(Number *i) override;
      virtual vector<string> visit(Label *i) override;
      virtual vector<string> visit(L *i) override;
      virtual vector<string> visit(Name *i) override;
      virtual vector<string> visit(Operator *i) override;
      virtual vector<string> visit(Type *i) override;
      virtual vector<string> visit(Vars *i) override;
      virtual vector<string> visit(Args *i) override;
      virtual vector<string> visit(Names *i) override;
      virtual Item* translate(Number *i) override;
      virtual Item* translate(Label *i) override;
      virtual Item* translate(L *i) override;
      virtual Item* translate(Name *i) override;
      virtual Item* translate(Operator *i) override;
      virtual Item* translate(Type *i) override;
      virtual Item* translate(Vars *i) override;
      virtual Item* translate(Args *i) override;
      virtual Item* translate(Names *i) override;
  };

}
