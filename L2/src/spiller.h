#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include "L2.h"

#include "liveness.h"
#include "interference.h"
#include "coloring.h"

namespace L2 {
    Function* spill(Function* f, string var, string prefix);

    Function* spill_all(Function* f,int varCount);

    class SpillerVisitor : public InstructionVisitor {
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

    class SpillerItemLooker : public ItemVisitor {
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
}
