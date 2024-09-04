#pragma once

#include "L2.h"

namespace L2{

  void generate_code(Program p);

  void generate_function(Function* f);

  class CG_ItemLooker : public ItemVisitor {
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
