#pragma once

#include <L3.h>
#include <map>

namespace L3{

  class Node{
    public:
      Node(Item* item, std::vector<string> type,Node* dad);
      Item* curItem;
      std::vector<string> type;
      std::vector<Node*> children;
      bool hasChildren();
      bool hasParent();
      Node* getChild(int index);
      int childrenCount();
      Node* parent;
      void readNode(int tracker);
  };

  //Variable Item Holder
  class VIH{
    public:
    /*
      VIH(Item* var, string var_name, Item* item, string item_type);
      Item* variable;
      string variableName;
      Item* itemHeld;
      string itemHeldType;
      */

      VIH(string var_name, string replace_with);
      string varName;
      string replace;
      string replaceVar(string var_name);
  };

  class Pattern{
    public:
      Pattern(int siz, Node* RootOrLeafNode, string in);
      int size;
      Node* strNode;
      std::string instruction;
  };
/*
  class LabelMapper{
    public:
      LabelMapper(string suf)
      string suffix;
      int counter;
      std::map<string,string> labelMap;
      void updateCounter();
  };
  */

  class Context{
    public:
      std::vector<Instruction*> myInstructions;
      Context();
      //void AddInstruction (Instruction_assignment* ins);
      std::vector<Node*> myNodes;
  };

  void printe(std::string);

  //void generate_all_contexts(Program p);

  std::vector<Context> generate_contexts(Function* f,string LL);
  string generateOneContext(std::vector<Instruction*> ins,string LL);

  string scanAllVIH(string var_name);
  void removeVar(string var_name);

  int UseLabelMap(string input);
  void refreshLabelMap();


  string generate_L2_instructions(Context c);
  string getPaternFromNode(Node* node);
  string checkL2Paterns_length2(Node* smallest);
  string writePattern(Node* smalest, Pattern pat);
  bool compareNodeTypes(Node* node, Node* strNode);

  void init_L2_patterns();
  void init_len_1();
  void init_len_2();
  void init_len_3();



}
