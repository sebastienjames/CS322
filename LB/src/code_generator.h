#pragma once

#include <LB.h>

namespace LB {
  
  void generate_code(Program p);

  class Scopes{
    public:
      Scopes(Instruction* startIns, int id);
      Instruction* startOfScope;
      Instruction* endOfScope;
      std::vector<Instruction*> contentsOfScope;
      map<string,string> scopeVarMap;
      void append(Instruction* content);
      int size();
      int scopeId;
  };

  class WhileLoop{
    public:
      WhileLoop(Instruction* whileIns,int while_id, string condLabel_str);
      Item* startOfWhile;
      string startOfWhile_str;
      bool isTrueLabel(Instruction* ins);
      Item* endOfWhile;
      string endOfWhile_str;
      string condLabel;
      Instruction* continueIns;
      Instruction* breakIns;
      int id;
      bool isFalseLabel(Instruction* ins);
      std::vector<Instruction*> contentsOfWhile;
      void append(Instruction* content);
      int size();
  };

  bool isScopeEntry(Instruction* possibleEntry);

  bool isScopeExit(Instruction* possibleEntry);

}
