#pragma once

#include <vector>
#include <string>

using namespace std;

namespace L2 {
  class InstructionVisitor;
  class ItemVisitor;
  enum RegisterID {rax, rbx, rbp, r10, r11, r12, r13, r14, r15, rdi, rsi, rdx, rcx, r8, r9, rsp, none};
  enum OperatorID {addq, subq, imulq, andq, salq, sarq, l, le, e};

  class Item {
    public:
      virtual vector<string> accept(ItemVisitor *visitor) = 0;
      virtual void print_item() = 0;
  };

  class Register : public Item {
    public:
      Register (RegisterID r);
      RegisterID getID();
      vector<string> accept(ItemVisitor *visitor) override; 
      void print_item() override;
    private:
      RegisterID ID;
  };

  class Number : public Item {
    public:
      Number (int64_t number);
      int64_t getNum();
      vector<string> accept(ItemVisitor *visitor) override; 
      void print_item() override;
    private:
      int64_t number;
  };

  class Label : public Item {
    public:
      Label (std::string label);
      std::string getLabel();
      vector<string> accept(ItemVisitor *visitor) override;
      void print_item() override;
    private:
      std::string label;
  };

  class Variable : public Item {
    public:
      Variable (std::string var);
      std::string getVar();
      vector<string> accept(ItemVisitor *visitor) override;
      void print_item() override;
    private:
      std::string var;
  };

  class L : public Item{
  	public:
  		L (std::string l);
  		std::string getL();
      vector<string> accept(ItemVisitor *visitor) override;
      void print_item() override;
  	private:
  		std::string l;
  };

  class Operator : public Item {
    public:
      Operator (OperatorID o);
      OperatorID getID();
      vector<string> accept(ItemVisitor *visitor) override;
      void print_item() override;
    private:
      OperatorID ID;
  };

  class Compare : public Item {
    public:
      Compare (Item* b, Item* a, Item* op);
      Item* getA();
      Item* getB();
      Item* getOp();
      vector<string> accept(ItemVisitor *visitor) override;
      void print_item() override;
    private:
      Item* a;
      Item* b;
      Item* op;
  };

  class Mem : public Item {
    public:
      Mem (Item* x, Item* M);
      Item* getX();
      Item* getM();
      vector<string> accept(ItemVisitor *visitor) override;
      void print_item() override;
    private:
      Item* x;
      Item* M;
  };

  /*
   * Instruction interface.
   */
  class Instruction{
    public:
      virtual void accept(InstructionVisitor *visitor) = 0;
      virtual void print_instruction() = 0;
  };

  /*
   * Instructions.
   */
  class Instruction_ret : public Instruction{
    public:
      void accept(InstructionVisitor *visitor) override; 
      void print_instruction() override;
  };

  class Instruction_assignment : public Instruction{
    public:
      Instruction_assignment (Item *dst, Item *src);
      Item* getSource();
	    Item* getDestination();
      void accept(InstructionVisitor *visitor) override;
      void print_instruction() override;
    private:
      Item *s;
      Item *d;
  };

  class Instruction_assignment_compare : public Instruction{
    public:
      Instruction_assignment_compare (Item *dst, Item *src);
      Item* getSource();
	    Item* getDestination();
      void accept(InstructionVisitor *visitor) override;
      void print_instruction() override;
    private:
      Item *s;
      Item *d;
  };

  class Instruction_assignment_stack_arg : public Instruction{
    public:
      Instruction_assignment_stack_arg (Item *dst, Item *src);
      Item* getSource();
	    Item* getDestination();
      void accept(InstructionVisitor *visitor) override;
      void print_instruction() override;
    private:
      Item *s;
      Item *d;
  };

  class Instruction_inc : public Instruction{
    public:
      Instruction_inc (Item *dst);
	    Item* getDestination();
      void accept(InstructionVisitor *visitor) override;
      void print_instruction() override;
    private:
      Item *d;
  };

  class Instruction_dec : public Instruction{
    public:
      Instruction_dec (Item *dst);
	    Item* getDestination();
      void accept(InstructionVisitor *visitor) override;
      void print_instruction() override;
    private:
      Item *d;
  };

  class Instruction_arithmetic : public Instruction {
    public:
      Instruction_arithmetic (Item *dst, Item *src, Item* op);
      Item* getSource();
      Item* getDestination();
      Item* getOperator();
      void accept(InstructionVisitor* v) override;
      void print_instruction() override;
    private:
      Item *s;
      Item *d;
      Item* o;
  };

  class Instruction_shift : public Instruction {
    public:
      Instruction_shift (Item *dst, Item *src, Item* op);
      Item* getSource();
      Item* getDestination();
      Item* getOperator();
      void accept(InstructionVisitor* v) override;
      void print_instruction() override;
    private:
      Item *s;
      Item *d;
      Item* o;
  };

  class Instruction_goto : public Instruction {
    public:
      Instruction_goto (Item* dst);
      Item* getDestination();
      void accept(InstructionVisitor* v) override;
      void print_instruction() override;
    private:
      Item *dst;
    };

  class Instruction_cjump : public Instruction {
    public:
      Instruction_cjump (Item* cmp, Item* label);
      Item* getCompare();
      Item* getLabel();
      void accept(InstructionVisitor* v) override;
      void print_instruction() override;
    private:
      Item* cmp;
      Item* label;
  };
      
  class Instruction_at : public Instruction {
    public:
      Instruction_at (Item* saveTo, Item* add, Item* mult, Item* by);
      Item* getSaveTo();
      Item* getAdd();
      Item* getMult();
      Item* getBy();
      void accept(InstructionVisitor* v) override;
      void print_instruction() override;
    private:
      Item* saveTo;
      Item* add;
      Item* mult;
      Item* by;
  };

  class Instruction_call : public Instruction {
    public:
      Instruction_call (Item* func, Item* params);
      Item* getFunc();
      Item* getParamNum();
      void accept(InstructionVisitor* v) override;
      void print_instruction() override;
    private:
      Item* func;
      Item* params;
      };

  class Instruction_label : public Instruction {
    public:
      Instruction_label (Item* label);
      Item* getLabel();
      void accept(InstructionVisitor* v) override;
      void print_instruction() override;
    private:
      Item* label;
    };

  /*
   * Function.
   */
  class Function{
    public:
      std::string name;
      int64_t arguments;
      int64_t locals;
      
      std::vector<Instruction *> instructions;

      void print_function();
  };

  class Program{
    public:
      std::string entryPointLabel;
      std::string toReplace;
      std::vector<Function *> functions;
  };








  

  /*
  * Visitors
  */
  class InstructionVisitor {
      public:
          virtual void visit(Instruction_ret *i) = 0;
          virtual void visit(Instruction_assignment *i) = 0;
          virtual void visit(Instruction_assignment_compare *i) = 0;
          virtual void visit(Instruction_assignment_stack_arg *i) = 0;
          virtual void visit(Instruction_arithmetic *i) = 0;
          virtual void visit(Instruction_shift *i) = 0;
          virtual void visit(Instruction_inc *i) = 0;
          virtual void visit(Instruction_dec *i) = 0;
          virtual void visit(Instruction_goto *i) = 0;
          virtual void visit(Instruction_cjump *i) = 0;
          virtual void visit(Instruction_at *i) = 0;
          virtual void visit(Instruction_call *i) = 0;
          virtual void visit(Instruction_label *i) = 0;
  }; 

  class Liveness : public InstructionVisitor {
      public:
          virtual void visit(Instruction_ret *i) override;
          virtual void visit(Instruction_assignment *i) override;
          virtual void visit(Instruction_assignment_compare *i) override;
          virtual void visit(Instruction_assignment_stack_arg *i) override;
          virtual void visit(Instruction_arithmetic *i) override;
          virtual void visit(Instruction_shift *i) override;
          virtual void visit(Instruction_inc *i) override;
          virtual void visit(Instruction_dec *i) override;
          virtual void visit(Instruction_goto *i) override;
          virtual void visit(Instruction_cjump *i) override;
          virtual void visit(Instruction_at *i) override;
          virtual void visit(Instruction_call *i) override;
          virtual void visit(Instruction_label *i) override;
  }; 

    class CodeGenerator : public InstructionVisitor {
      public:
          virtual void visit(Instruction_ret *i) override;
          virtual void visit(Instruction_assignment *i) override;
          virtual void visit(Instruction_assignment_compare *i) override;
          virtual void visit(Instruction_assignment_stack_arg *i) override;
          virtual void visit(Instruction_arithmetic *i) override;
          virtual void visit(Instruction_shift *i) override;
          virtual void visit(Instruction_inc *i) override;
          virtual void visit(Instruction_dec *i) override;
          virtual void visit(Instruction_goto *i) override;
          virtual void visit(Instruction_cjump *i) override;
          virtual void visit(Instruction_at *i) override;
          virtual void visit(Instruction_call *i) override;
          virtual void visit(Instruction_label *i) override;
  };



  class ItemVisitor {
    public:
      virtual vector<string> visit(Register *i) = 0;
      virtual vector<string> visit(Number *i) = 0;
      virtual vector<string> visit(Label *i) = 0;
      virtual vector<string> visit(Variable *i) = 0;
      virtual vector<string> visit(L *i) = 0;
      virtual vector<string> visit(Operator *i) = 0;
      virtual vector<string> visit(Compare *i) = 0;
      virtual vector<string> visit(Mem *i) = 0;
  }; 

  class ItemLooker : public ItemVisitor {
    public:
      virtual vector<string> visit(Register *i) override;
      virtual vector<string> visit(Number *i) override;
      virtual vector<string> visit(Label *i) override;
      virtual vector<string> visit(Variable *i) override;
      virtual vector<string> visit(L *i) override;
      virtual vector<string> visit(Operator *i) override;
      virtual vector<string> visit(Compare *i) override;
      virtual vector<string> visit(Mem *i) override;
  }; 

  void print(std::string str);


}
