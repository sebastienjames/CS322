#pragma once

#include "L2.h"

namespace L2{

  void generate_code_to_L1(Program p);

  void generate_function_to_L1(Function* f);

  class CG2_ItemLooker : public ItemVisitor {
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
  class CodeGenerator2 : public InstructionVisitor {
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

}
