#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <string>
#include "L2.h"

namespace L2{

  class Line {
    public:
      Line(std::string name,std::set<string> gen, std::set<string> kill, std::vector<string> fs);
      std::string name;
      std::set<string> gen;
      std::set<string> kill;
      std::set<string> in;
      std::set<string> out;
      std::string sopName;
      std::vector<Line*> successors;
      std::vector<string> future_successors;
      bool updateSelf();
  };

  vector<Line*> liveness_analysis(Function* f, bool printOutput);
}
