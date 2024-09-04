#pragma once

#include <IR.h>

namespace IR {

  class Trace{
    public:
      Trace();
      std::vector<Basic_block*> trace_blocks;
      void append(Basic_block* block);
      int size();
  };
  //update

  Basic_block* getBlockFromLabel(Item* l);

  std::vector<Basic_block*> getSuccessors(Basic_block* block);

  string getEnding(Instruction* terminator);

  void generate_code(Program p);

}
