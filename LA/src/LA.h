#pragma once

#include <vector>
#include <string>
#include <map>
#include <set>

using namespace std;

namespace LA {

  enum OperatorID {addq, subq, imultq, andq, salq, sarq, l, le, e, ge, g};
  enum TypeID {int64, tuple, code, void_type};
  class ItemVisitor;
  class InstructionVisitor;

  class Item {
    public:
      virtual string to_string() = 0;
      virtual vector<string> accept(ItemVisitor *visitor) = 0;
  };

  class L : public Item {
    public:
      L (string name);
      virtual string to_string() override;
      string get_name();
      vector<string> accept(ItemVisitor *visitor) override; 
    private:
      string name;
  };

  class Label : public Item {
    public:
      Label (string name);
      virtual string to_string() override;
      string get_name();
      vector<string> accept(ItemVisitor *visitor) override; 
    private:
      string name;
  };

  class Name : public Item {
    public:
      Name (string name);
      virtual string to_string() override;
      string get_name();
      vector<string> accept(ItemVisitor *visitor) override; 
    private:
      string name;
  };

  class Number : public Item {
    public:
      Number (int64_t number);
      virtual string to_string() override;
      int64_t get_number();
      vector<string> accept(ItemVisitor *visitor) override; 
    private:
      int64_t number;
  };

  class Type : public Item {
    public:
      Type (TypeID type, int64_t dims);
      virtual string to_string() override;
      TypeID get_type();
      vector<string> accept(ItemVisitor *visitor) override; 
    private:
      TypeID t;
      int64_t num_dimensions;
  };

  class Operator : public Item {
    public:
      Operator (OperatorID op);
      virtual string to_string() override;
      OperatorID get_op();
      vector<string> accept(ItemVisitor *visitor) override; 
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
    private:
      vector<Item*> args;
  };

  /*
   * Instruction interface.
   */
  class Instruction{
    public:
    virtual void print_instruction() = 0;
    virtual string instruction_to_IR() = 0;
    virtual void accept(InstructionVisitor *visitor) = 0;
    int line_number;
  };

  /*
   * Instructions.
   */

  class Instruction_return : public Instruction{
    public:
      virtual void print_instruction() override;
      virtual void accept(InstructionVisitor *visitor) override;
      virtual string instruction_to_IR() override;
  };

  class Instruction_return_arg : public Instruction{
    public:
      Instruction_return_arg (Item *src);
      virtual void print_instruction() override;
      virtual string instruction_to_IR() override;
      Item* get_value();
      virtual void accept(InstructionVisitor *visitor) override;
    private:
      Item *s;
  };

  class Instruction_declaration : public Instruction{
    public:
      Instruction_declaration (Item* type, Item* var);
      virtual void print_instruction() override;
      Item* get_type();
      Item* get_var();
      virtual void accept(InstructionVisitor *visitor) override;
      virtual string instruction_to_IR() override;
    private:
      Item *type;
      Item *var;
  };

  class Instruction_assignment : public Instruction{
    public:
      Instruction_assignment (Item *dst, Item *src);
      Item* getSource();
	    Item* getDestination();
      virtual void print_instruction() override;
      virtual string instruction_to_IR() override;
      virtual void accept(InstructionVisitor *visitor) override;
    private:
      Item *s;
      Item *d;
  };

  class Instruction_assignment_op : public Instruction{
    public:
      Instruction_assignment_op (Item *dst, Item *t1, Item *op, Item *t2);
      virtual void print_instruction() override;
      virtual string instruction_to_IR() override;
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
      virtual void print_instruction() override;
      virtual string instruction_to_IR() override;
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
      virtual void print_instruction() override;
      virtual string instruction_to_IR() override;
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
      virtual void print_instruction() override;
      virtual string instruction_to_IR() override;
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
      virtual void print_instruction() override;
      virtual string instruction_to_IR() override;
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
      virtual void print_instruction() override;
      virtual string instruction_to_IR() override;
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
      virtual string instruction_to_IR() override;
      virtual void print_instruction() override;
      Item* get_label();
      virtual void accept(InstructionVisitor *visitor) override;
    private:
      Item *d;
  }; 

  class Instruction_branch : public Instruction{
    public:
      Instruction_branch (Item *dst);
      virtual void print_instruction() override;
      virtual string instruction_to_IR() override;
      Item* getDestination();
      virtual void accept(InstructionVisitor *visitor) override;
    private:
      Item *d;
  }; 

  class Instruction_branch_conditional : public Instruction{
    public:
      Instruction_branch_conditional (Item *cond, Item *dst1, Item* dst2);
      virtual void print_instruction() override;
      virtual string instruction_to_IR() override;
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
      virtual void print_instruction() override;
      virtual string instruction_to_IR() override;
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
      virtual void print_instruction() override;
      virtual string instruction_to_IR() override;
      Item* getDst();
      Item* getCallee();
      Item* getArgs();
      virtual void accept(InstructionVisitor *visitor) override;
    private:
      Item *d;
      Item *callee;
      Item *args;
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
  }; 

  class ValueEncoding : public InstructionVisitor {
      public:
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
          string addVar(string var);
          string LL;
          Function* f;
          int64_t instruction_index;
          set<string> newVars;
  };

  class ArrayChecking : public InstructionVisitor {
      public:
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
          string LL;
          Function* f;
          int64_t instruction_index;
          set<string> newVars;
          map<string, string> types;
  };

  class BBGenerator : public InstructionVisitor {
      public:
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
          string LL;
          Function* f;
          vector<Instruction*> new_instructions;
          int64_t instruction_index;
          bool startBB;
          int64_t num_declarations;
  };

  class CodeGenerator : public InstructionVisitor {
      public:
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
  };

}
