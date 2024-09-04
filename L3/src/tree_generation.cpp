#include <string>
#include <iostream>
#include <fstream>

#include <tree_generation.h>
//#include "instruction_selection.h"

using namespace std;

namespace L3{
/*
  Node::Node(bool isroot, Item* item){
    isRoot=isroot;
    curItem=item;
    return;
  }

  void Node::readNode(int tracker=0){
    printe("---Node Found at "+to_string(tracker));
    for(auto child : children){
        child.readNode(tracker+1);
    }
  }
*/



  ItemLookerTree looker_tree;
  TreeGenerator cglooker_tree;
  //string myOutput;
  //string LL;
  //int64_t LL_counter;
  //int64_t ret_add_counter;
  bool printAll_TREE=false;
  //vector<Node> allRootNodes;
  Node* currentRootNode;



  vector<Node*> make_trees(Context con){
    if(printAll_TREE){printe("-Tree");}
    vector<Node*> allRootNodes;
    currentRootNode=nullptr;
    for (auto i : con.myInstructions) {
      //Go and set up a new currentRootNode to the instruction
      i->accept(&cglooker_tree);
      //Read it to make sure
      //if(printAll_TREE){currentRootNode->readNode();}
      //Store it allRootNodes (which holds all root nodes of the given context)
      allRootNodes.push_back(currentRootNode);
    }
    return allRootNodes;
  }

  void try_merge_trees(Node t1, Node t2){
    /*
    Item* targetItem=t2.curItem;
    vector<Node> childrenInT1=t1.children;
    for(auto child : childrenInT1){ //gets a list of nodes that are children
      if(child.curItem == targetItem){
        //T2 Head found in T1 children
      }
    }
    */
  }



  vector<string> ItemLookerTree::visit(Number *i) {
    vector<string> item_string;
    if(printAll_TREE){printe("Number");}
    item_string.push_back("Number");
    return item_string;
  }

  vector<string> ItemLookerTree::visit(Label *i) {
    vector<string> item_string;
    if(printAll_TREE){printe("Label");}
    item_string.push_back("Label");
    return item_string;
  }

  vector<string> ItemLookerTree::visit(L *i) {
    vector<string> item_string;
    if(printAll_TREE){printe("L");}
    item_string.push_back("L");
    return item_string;
  }

  vector<string> ItemLookerTree::visit(Variable *i) {
    vector<string> item_string;
    if(printAll_TREE){printe("Variable");}
    item_string.push_back("Variable");
    return item_string;
  }

  vector<string> ItemLookerTree::visit(Compare_Operator *i) {
    vector<string> item_string;
    if(printAll_TREE){printe("Compare_Operator");}
    item_string.push_back("Compare_Operator");
    return item_string;
  }

  vector<string> ItemLookerTree::visit(Arithmetic_Operator *i) {
    vector<string> item_string;
    if(printAll_TREE){printe("Arithmetic_Operator");}
    item_string.push_back("Arithmetic_Operator");
    return item_string;
  }

  vector<string> ItemLookerTree::visit(Vars *i) {
    vector<string> item_string;
    if(printAll_TREE){printe("new vars");}
    item_string.push_back(to_string(i->getNumVars()));
    for (auto arg : i->getVars()) {
      string arg_string = arg->accept(&looker_tree).front();
      item_string.push_back(arg_string);
    }
    return item_string;
  }

  vector<string> ItemLookerTree::visit(Args *i) {
    vector<string> item_string;
    if(printAll_TREE){printe("Args");}
    item_string.push_back("Args");
    /*
    for (auto arg : i->getArgs()) {
      string arg_string = arg->accept(&looker_tree).front();
      item_string.push_back(arg_string);
    }
    */

    return item_string;
  }


  void TreeGenerator::visit(Instruction_assignment *i) {
    if(printAll_TREE){printe("Assignment");}
    Item* source = i->getSource();
    Item* dest = i->getDestination();

    string str = source->accept(&looker_tree).front();
    string dst = dest->accept(&looker_tree).front();

    currentRootNode= new Node(dest,{dst},nullptr);
    Node* child = new Node(source,{str},currentRootNode);
    //currentRootNode->children.push_back(child);
    //Just ADD
    //currentContext->myInstructions.push_back(i);
    return;
  }

  void TreeGenerator::visit(Instruction_assignment_op *i) {
    vector<string> item_string;
    if(printAll_TREE){printe("Assignment op");}
    vector<Item*> sources = i->getSources();
    Item* dest = i->getDestination();
    Item* op = i->getOperator();
    string dst = dest->accept(&looker_tree).front();
    string o = op->accept(&looker_tree).front();
    currentRootNode= new Node(dest,{dst},nullptr);
    Node* child = new Node(op,{o},currentRootNode);
    for (auto source : sources){
      string str = source->accept(&looker_tree).front();
      Node* opChild = new Node(source,{str},child);
      //child->children.push_back(opChild);
    }
    //currentRootNode->children.push_back(child);
    //Just ADD
    //currentContext->myInstructions.push_back(i);
    return;
  }

  void TreeGenerator::visit(Instruction_assignment_cmp *i) {
    vector<string> item_string;
    if(printAll_TREE){printe("Assignment cmp");}
    vector<Item*> sources = i->getSources();
    Item* dest = i->getDestination();
    Item* cmp = i->getCompareOperator();
    string dst = dest->accept(&looker_tree).front();
    string cm = cmp->accept(&looker_tree).front();
    currentRootNode= new Node(dest,{dst},nullptr);
    Node* child = new Node(cmp,{cm},currentRootNode);
    for (auto source : sources){
      string str = source->accept(&looker_tree).front();
      Node* cmpChild = new Node(source,{str},child);
      //child->children.push_back(cmpChild);
    }
    //currentRootNode->children.push_back(child);
    //Just ADD
    //currentContext->myInstructions.push_back(i);
    return;
  }

  void TreeGenerator::visit(Instruction_return *i) {
    if(printAll_TREE){printe("return");} 
    currentRootNode= new Node(nullptr,{"Return"},nullptr);
    //currentRootNode->isReturn=true;
    //ADD AND RETURN
    //currentContext->myInstructions.push_back(i);
    //allContexts.push_back(*currentContext);
    //currentContext=new Context();
    return;
  }

  void TreeGenerator::visit(Instruction_return_arg *i) {
    vector<string> item_string;
    if(printAll_TREE){printe("return arg");}
    Item* value = i->get_value();
    string val =value->accept(&looker_tree).front();
    currentRootNode= new Node(nullptr,{"Return"},nullptr);
    Node* child= new Node(value,{val},currentRootNode);
    //currentRootNode->children.push_back(child);
    //currentRootNode->isReturn=true;
    //ADD AND RETURN
    //currentContext->myInstructions.push_back(i);
    //allContexts.push_back(*currentContext);
    //currentContext=new Context();
    return;
  }

  void TreeGenerator::visit(Instruction_load *i) {
    vector<string> item_string;
    if(printAll_TREE){printe("load");}
    Item* source = i->getSource();
    Item* dest = i->getDestination();
    string dst =dest->accept(&looker_tree).front();
    string src =source->accept(&looker_tree).front();
    currentRootNode = new Node(nullptr,{"Load"},nullptr);
    Node* child = new Node(dest,{dst},currentRootNode);
    Node* child1 = new Node(source,{src},currentRootNode);

    //currentRootNode->children.push_back(child);
    //Just ADD
    //currentContext->myInstructions.push_back(i);
    return;
  }

  void TreeGenerator::visit(Instruction_store *i) {
    vector<string> item_string;
    if(printAll_TREE){printe("store");}
    Item* source = i->getSource();
    Item* dest = i->getDestination();
    string dst =dest->accept(&looker_tree).front();
    string src =source->accept(&looker_tree).front();
    currentRootNode= new Node(nullptr,{"Store"},nullptr);
    Node* child = new Node(dest,{dst},currentRootNode);
    Node* child1 = new Node(source,{src},currentRootNode);
    //currentRootNode->children.push_back(child);
    //Just ADD
    //currentContext->myInstructions.push_back(i);
    return;
  }

  void TreeGenerator::visit(Instruction_branch *i) {
    vector<string> item_string;
    if(printAll_TREE){printe("branch");}
    Item* dest = i->getDestination();
    string dst =dest->accept(&looker_tree).front();
    currentRootNode= new Node(nullptr,{"Branch"},nullptr);
    Node* child= new Node(dest,{dst},currentRootNode);
    //Node* child = new Node(false,source);
    //currentRootNode->children.push_back(child);
    //ADD AND RETURN
    //currentContext->myInstructions.push_back(i);
    //allContexts.push_back(*currentContext);
    //currentContext=new Context();
    return;
  }

  void TreeGenerator::visit(Instruction_branch_conditional *i) {
    vector<string> item_string;
    if(printAll_TREE){printe("branch conditional");}
    Item* conditional = i->getCondition();
    Item* dest = i->getDestination();
    string dst =dest->accept(&looker_tree).front();
    string con =conditional->accept(&looker_tree).front();
    currentRootNode= new Node(nullptr,{"Branch"},nullptr);
    Node* child = new Node(dest,{dst},currentRootNode);
    Node* child1 = new Node(conditional,{con},currentRootNode);
    //currentRootNode->children.push_back(child);
    //ADD AND RETURN
    //currentContext->myInstructions.push_back(i);
    //allContexts.push_back(*currentContext);
    //currentContext=new Context();
    return;
  }

  void TreeGenerator::visit(Instruction_call *i) {
    vector<string> item_string;
    if(printAll_TREE){printe("call");}
    //Just Return
    //allContexts.push_back(*currentContext);
    //currentContext=new Context();
    return;
  }

  void TreeGenerator::visit(Instruction_assignment_call *i) {
    vector<string> item_string;
    if(printAll_TREE){printe("Assignment call");}
    Item* dest = i->getDst();
    string dst =dest->accept(&looker_tree).front();
    Item* arg = i->getArgs();
    string ar =dest->accept(&looker_tree).front();
    currentRootNode= new Node(dest,{dst},nullptr);
    Node* call = new Node(nullptr,{"Call"},currentRootNode);
    Node* args = new Node(arg,{ar},call);
    //Just Return
    //allContexts.push_back(*currentContext);
    //currentContext=new Context();
    return;
  }

  void TreeGenerator::visit(Instruction_label *i) {
    vector<string> item_string;
    if(printAll_TREE){printe("label");}
    //Just Return
    //allContexts.push_back(*currentContext);
    //currentContext=new Context();
    return;
  }
  
}
