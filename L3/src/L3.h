#pragma once

#include <vector>
#include <string>

using namespace std;

namespace L3 {

  enum Compare_OperatorID {l, le, e, ge, g};
  enum Arithmetic_OperatorID {addq, subq, imultq, andq, salq, sarq};
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

  class Variable : public Item {
    public:
      Variable (string name);
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

  class Compare_Operator : public Item {
    public:
      Compare_Operator (Compare_OperatorID op);
      virtual string to_string() override;
      Compare_OperatorID get_op();
      vector<string> accept(ItemVisitor *visitor) override; 
    private:
      Compare_OperatorID op;
  };

  class Arithmetic_Operator : public Item {
    public:
      Arithmetic_Operator (Arithmetic_OperatorID op);
      virtual string to_string() override;
      Arithmetic_OperatorID get_op();
      vector<string> accept(ItemVisitor *visitor) override; 
    private:
      Arithmetic_OperatorID op;
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
    virtual void accept(InstructionVisitor *visitor) = 0;
  };

  /*
   * Instructions.
   */

  class Instruction_return : public Instruction{
    public:
      virtual void print_instruction() override;
      virtual void accept(InstructionVisitor *visitor) override;
  };

  class Instruction_return_arg : public Instruction{
    public:
      Instruction_return_arg (Item *src);
      virtual void print_instruction() override;
      Item* get_value();
      virtual void accept(InstructionVisitor *visitor) override;
    private:
      Item *s;
  };

  class Instruction_assignment : public Instruction{
    public:
      Instruction_assignment (Item *dst, Item *src);
      Item* getSource();
	    Item* getDestination();
      virtual void print_instruction() override;
      virtual void accept(InstructionVisitor *visitor) override;
    private:
      Item *s;
      Item *d;
  };

  class Instruction_assignment_op : public Instruction{
    public:
      Instruction_assignment_op (Item *dst, Item *t1, Item *op, Item *t2);
      virtual void print_instruction() override;
      std::vector<Item*> getSources();
	    Item* getDestination();
	    Item* getOperator();
      virtual void accept(InstructionVisitor *visitor) override;
    private:
      Item *dst;
      Item *t1;
      Item *op;
      Item *t2;
  }; 

  class Instruction_assignment_cmp : public Instruction{
    public:
      Instruction_assignment_cmp (Item *dst, Item *t1, Item *cmp, Item *t2);
      virtual void print_instruction() override;
      std::vector<Item*> getSources();
	    Item* getDestination();
	    Item* getCompareOperator();
      virtual void accept(InstructionVisitor *visitor) override;
    private:
      Item *dst;
      Item *t1;
      Item *cmp;
      Item *t2;
  };  

  class Instruction_store : public Instruction{
    public:
      Instruction_store (Item *dst, Item *src);
      virtual void print_instruction() override;
      Item* getSource();
	    Item* getDestination();
      virtual void accept(InstructionVisitor *visitor) override;
    private:
      Item *s;
      Item *d;
  }; 

  class Instruction_load : public Instruction{
    public:
      Instruction_load (Item *dst, Item *src);
      virtual void print_instruction() override;
      Item* getSource();
	    Item* getDestination();
      virtual void accept(InstructionVisitor *visitor) override;
    private:
      Item *s;
      Item *d;
  }; 

  class Instruction_label : public Instruction{
    public:
      Instruction_label (Item *dst);
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
      Item* getDestination();
      virtual void accept(InstructionVisitor *visitor) override;
    private:
      Item *d;
  }; 

  class Instruction_branch_conditional : public Instruction{
    public:
      Instruction_branch_conditional (Item *cond, Item *dst);
      virtual void print_instruction() override;
      Item* getDestination();
      Item* getCondition();
      virtual void accept(InstructionVisitor *visitor) override;
    private:
      Item *c;
      Item *d;
  }; 

  class Instruction_call : public Instruction{
    public:
      Instruction_call (Item *callee, Item* args);
      virtual void print_instruction() override;
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
      string name;
      Item* arguments;
      vector<Instruction *> instructions;
  };

  class Program{
    public:
      vector<Function *> functions;
      string longest_label;
  };


  void print(string line);

  // string convert_compare(Compare_OperatorID op);
  // string convert_arithmetic(Arithmetic_OperatorID op);

  class InstructionVisitor {
      public:
          virtual void visit(Instruction_assignment *i) = 0;
          virtual void visit(Instruction_assignment_op *i) = 0;
          virtual void visit(Instruction_assignment_cmp *i) = 0;
          virtual void visit(Instruction_assignment_call *i) = 0;
          virtual void visit(Instruction_return_arg *i) = 0;
          virtual void visit(Instruction_return *i) = 0;
          virtual void visit(Instruction_load *i) = 0;
          virtual void visit(Instruction_store *i) = 0;
          virtual void visit(Instruction_branch *i) = 0;
          virtual void visit(Instruction_branch_conditional *i) = 0;
          virtual void visit(Instruction_call *i) = 0;
          virtual void visit(Instruction_label *i) = 0;
  }; 

  class CodeGenerator : public InstructionVisitor {
      public:
          virtual void visit(Instruction_assignment *i) override;
          virtual void visit(Instruction_assignment_op *i) override;
          virtual void visit(Instruction_assignment_cmp *i) override;
          virtual void visit(Instruction_assignment_call *i) override;
          virtual void visit(Instruction_return_arg *i) override;
          virtual void visit(Instruction_return *i) override;
          virtual void visit(Instruction_load *i) override;
          virtual void visit(Instruction_store *i) override;
          virtual void visit(Instruction_branch *i) override;
          virtual void visit(Instruction_branch_conditional *i) override;
          virtual void visit(Instruction_call *i) override;
          virtual void visit(Instruction_label *i) override;
  };

    class InstructionGenerator : public InstructionVisitor {
      public:
          virtual void visit(Instruction_assignment *i) override;
          virtual void visit(Instruction_assignment_op *i) override;
          virtual void visit(Instruction_assignment_cmp *i) override;
          virtual void visit(Instruction_assignment_call *i) override;
          virtual void visit(Instruction_return_arg *i) override;
          virtual void visit(Instruction_return *i) override;
          virtual void visit(Instruction_load *i) override;
          virtual void visit(Instruction_store *i) override;
          virtual void visit(Instruction_branch *i) override;
          virtual void visit(Instruction_branch_conditional *i) override;
          virtual void visit(Instruction_call *i) override;
          virtual void visit(Instruction_label *i) override;
  };
  class TreeGenerator : public InstructionVisitor {
      public:
          virtual void visit(Instruction_assignment *i) override;
          virtual void visit(Instruction_assignment_op *i) override;
          virtual void visit(Instruction_assignment_cmp *i) override;
          virtual void visit(Instruction_assignment_call *i) override;
          virtual void visit(Instruction_return_arg *i) override;
          virtual void visit(Instruction_return *i) override;
          virtual void visit(Instruction_load *i) override;
          virtual void visit(Instruction_store *i) override;
          virtual void visit(Instruction_branch *i) override;
          virtual void visit(Instruction_branch_conditional *i) override;
          virtual void visit(Instruction_call *i) override;
          virtual void visit(Instruction_label *i) override;
  };

  class Liveness : public InstructionVisitor {
    public:
        virtual void visit(Instruction_assignment *i) override;
        virtual void visit(Instruction_assignment_op *i) override;
        virtual void visit(Instruction_assignment_cmp *i) override;
        virtual void visit(Instruction_assignment_call *i) override;
        virtual void visit(Instruction_return_arg *i) override;
        virtual void visit(Instruction_return *i) override;
        virtual void visit(Instruction_load *i) override;
        virtual void visit(Instruction_store *i) override;
        virtual void visit(Instruction_branch *i) override;
        virtual void visit(Instruction_branch_conditional *i) override;
        virtual void visit(Instruction_call *i) override;
        virtual void visit(Instruction_label *i) override;
  };


  class ItemVisitor {
    public:
      virtual vector<string> visit(Number *i) = 0;
      virtual vector<string> visit(Label *i) = 0;
      virtual vector<string> visit(L *i) = 0;
      virtual vector<string> visit(Variable *i) = 0;
      virtual vector<string> visit(Compare_Operator *i) = 0;
      virtual vector<string> visit(Arithmetic_Operator*i) = 0;
      virtual vector<string> visit(Vars *i) = 0;
      virtual vector<string> visit(Args *i) = 0;
  }; 

  class ItemLooker : public ItemVisitor {
    public:
      virtual vector<string> visit(Number *i) override;
      virtual vector<string> visit(Label *i) override;
      virtual vector<string> visit(L *i) override;
      virtual vector<string> visit(Variable *i) override;
      virtual vector<string> visit(Compare_Operator *i) override;
      virtual vector<string> visit(Arithmetic_Operator*i) override;
      virtual vector<string> visit(Vars *i) override;
      virtual vector<string> visit(Args *i) override;
  }; 

  class ItemLookerInstruction : public ItemVisitor {
    public:
      virtual vector<string> visit(Number *i) override;
      virtual vector<string> visit(Label *i) override;
      virtual vector<string> visit(L *i) override;
      virtual vector<string> visit(Variable *i) override;
      virtual vector<string> visit(Compare_Operator *i) override;
      virtual vector<string> visit(Arithmetic_Operator*i) override;
      virtual vector<string> visit(Vars *i) override;
      virtual vector<string> visit(Args *i) override;
  }; 

  class ItemLookerTree : public ItemVisitor {
    public:
      virtual vector<string> visit(Number *i) override;
      virtual vector<string> visit(Label *i) override;
      virtual vector<string> visit(L *i) override;
      virtual vector<string> visit(Variable *i) override;
      virtual vector<string> visit(Compare_Operator *i) override;
      virtual vector<string> visit(Arithmetic_Operator*i) override;
      virtual vector<string> visit(Vars *i) override;
      virtual vector<string> visit(Args *i) override;
  }; 

  
  class ItemLookerLiveness : public ItemVisitor {
    public:
      virtual vector<string> visit(Number *i) override;
      virtual vector<string> visit(Label *i) override;
      virtual vector<string> visit(L *i) override;
      virtual vector<string> visit(Variable *i) override;
      virtual vector<string> visit(Compare_Operator *i) override;
      virtual vector<string> visit(Arithmetic_Operator*i) override;
      virtual vector<string> visit(Vars *i) override;
      virtual vector<string> visit(Args *i) override;
  }; 

}
