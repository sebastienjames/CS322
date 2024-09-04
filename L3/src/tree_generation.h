#pragma once

#include <L3.h>
#include <instruction_selection.h>

namespace L3{
/*
  class Node{
    public:
      Node(bool isroot,Item* item);
      bool isRoot;
      //Node root;
      Item* curItem;
      bool isReturn;
      std::vector<Node> children;
      void readNode(int tracker);
  };
  */

  std::vector<Node*> make_trees(Context con);

}
