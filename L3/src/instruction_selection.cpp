#include <string>
#include <iostream>
#include <fstream>

#include <instruction_selection.h>
#include <tree_generation.h>
#include <liveness.h>

using namespace std;

namespace L3{
  void printe(string output) {
    cout << output << "\n";
  }

  ItemLookerInstruction looker_instruction;
  InstructionGenerator cglooker_instruction;
  //string myOutput;
  string LL0;
  //int64_t LL_counter;
  //int64_t ret_add_counter;
  bool printAll_IS=false;
  Context* currentContext;
  vector<Context> allContexts;
  vector<Pattern> allL2Patterns;
  vector<VIH> variablesOnStandby;
  map<string,int> LabelMap;
  int LLcount=0;
  bool hasInit=false;
  bool ignoreUndefined=false;
  string cg3_op_operators[6]={"+=","-=","*=","&=","<<=",">>="};
  string cg3_cmp_operators[5]={"<","<=","=",">=",">"};

  Context::Context(){
      return;
  }

  Node::Node(Item* item,vector<string> str, Node* dad){
    curItem=item;
    type=str;
    parent=dad;
    if(dad!=nullptr){
      dad->children.push_back(this);
    }
    return;
  }

  void Node::readNode(int tracker=0){
    printe("---Node of type "+type.front()+" found at "+to_string(tracker));
    for(auto child : children){
        child->readNode(tracker+1);
    }
  }

  bool Node::hasChildren(){
    return children.size()>0;
  }

  
  bool Node::hasParent(){
    return parent!=nullptr;
  }

  int Node::childrenCount(){
    return children.size();
  }

  Node* Node::getChild(int index){
    if(index<children.size()){
        return children[index];
    }
    printe("Tried to access child that does not exist");
    return nullptr;
  }

  Pattern::Pattern(int siz, Node* RootOrLeafNode, string in){
    size=siz;
    strNode=RootOrLeafNode;
    instruction=in;
    return;
  }
/*
  VIH::VIH(Item* var, string var_name, Item* item, string item_type){
    variable=var;
    variableName=var_name;
    itemHeld=item;
    itemHeldType=item_type;
  }
  */

  VIH::VIH(string var_name, string replace_with){
      if(scanAllVIH(var_name)!=""){removeVar(var_name);}
      varName=var_name;
      replace=replace_with;
  }

  string VIH::replaceVar(string var_name){
    if(var_name==varName){
      //printe(varName+"|"+var_name);
      return replace;
    }
    return "";
  }

  string scanAllVIH(string var_name){
    string replace="";
    for(VIH vih : variablesOnStandby){
        replace=vih.replaceVar(var_name);
        if(replace!=""){break;}  
    }
    return replace;
  }

  int UseLabelMap(string input){
    int num;
    if(LabelMap.count(input)!=1){
      LabelMap[input]=LLcount;
      LLcount+=1;
    }
    num=LabelMap[input];
    return num;
  }

  void refreshLabelMap(){
    LabelMap.clear();
  }

  void removeVar(string var_name){
    for(int i=0;i<variablesOnStandby.size();i++){
      if(variablesOnStandby[i].varName==var_name){
        variablesOnStandby.erase(variablesOnStandby.begin()+i);
        return;
      }
    }
  }

  /*
  void Context::AddInstruction (Instruction_assignment* ins){
    myInstructions.push_back(ins);
    return;
  }
  
  void Context::AddInstruction (Instruction_assignment_call ins)
    myInstructions.push_back(ins);
  }
  void Context::AddInstruction (Instruction_assignment_cmp ins)
    myInstructions.push_back(ins);
  }
  void Context::AddInstruction (Instruction_assignment_op ins)
    myInstructions.push_back(ins);
  }
  void Context::AddInstruction (Instruction_branch ins)
    myInstructions.push_back(ins);
  }
  */
  


  vector<Context> generate_contexts(Function* f,string LL){
    LL0=LL;
    ignoreUndefined=true;
    variablesOnStandby.clear();
    init_L2_patterns();
    //liveness_analysis(f);
    currentContext=new Context();
    allContexts.empty();
    for (auto i : f->instructions) {
      i->accept(&cglooker_instruction);
    }

    for (int64_t context_index=0; context_index < allContexts.size(); context_index++) {
      //cout << "------------Start-----------\n";
      for (auto cotn : allContexts) {
        //cout << "myNodes is :" << to_string(cotn.myNodes.size()) + "\n";
      }
      //printe("New Context");
      int count=0;
      //printe("-Instruction Count: "+to_string(allContexts[context_index].myInstructions.size()));
      // vector<Node> hold = 
      // cout << "hold is " << hold.size();
      // for(auto i : hold){
      //   if(printAll_IS){i.readNode();}
      // }
      allContexts[context_index].myNodes=make_trees(allContexts[context_index]);
      //cout << "size of myNodes after assignment: " << to_string(allContexts[context_index].myNodes.size()) << "\n";
    }
    for(auto con : allContexts){
      generate_L2_instructions(con);
    }
    return allContexts;
  }

  string generateOneContext(vector<Instruction*> ins,string LL){
    LL0=LL;
    ignoreUndefined=false;
    init_L2_patterns();
    //liveness_analysis(f);
    currentContext=new Context();
    allContexts.clear();
    for (auto i : ins) {
      i->accept(&cglooker_instruction);
    }
    if(allContexts.size()>0){
    allContexts[0].myNodes=make_trees(allContexts[0]);
    string output=generate_L2_instructions(allContexts[0]);
    return output;
    }
    else{
      return "";
    }
  }

  string generate_L2_instructions(Context c){
    vector<Node*> allNodes=c.myNodes;
    string output="";
    for(Node* headNode : allNodes){
      output+=getPaternFromNode(headNode);
    }
    //printe(output);
    return output;
  }

  string getPaternFromNode(Node* node){
    Node* smallest = node;
    //node->readNode(0);
    string output="";
    //Get to the Smallest
    while(smallest->children.size()!=0){
      vector<Node*> hold =smallest->children;
      smallest=hold[0];
    }
    //smallest->parent->readNode(0);
    return checkL2Paterns_length2(smallest);
  }

  string checkL2Paterns_length2(Node* curNode){
    //curNode->parent->readNode(0);
    string output="";
    int currentSize=1;
    //printe("MadeItHere");
    for(auto pattern : allL2Patterns){
      //curNode->parent->readNode(0);
      if(pattern.size>currentSize){
        currentSize+=1;
        //printe("Iterated");
        if(curNode->hasParent()){
          //printe("Node updated to parent");
          curNode=curNode->parent;
        }
        else{
          break;
        }
      }
      Node* strNode = pattern.strNode;
      //printe("Got To Compare");
      if(compareNodeTypes(curNode, strNode)){
        //printe("Nodes Compared Correctly");
        output+=writePattern(curNode,pattern);
      }
    }
    return output;
  }

  bool compareNodeTypes(Node* node,Node* strNode){
    vector<string> options=strNode->type;
    for(auto option : options){
      //printe(node->type.front()+"|"+option);
      if(node->type.front()==option){
        //printe("Got Here");
          if(!node->hasChildren() && !strNode->hasChildren()){
            return true;
          }
          else{
            if(node->childrenCount()==strNode->childrenCount()){
              for(int i=0;i<node->childrenCount();i++){
                return compareNodeTypes(node->getChild(0),strNode->getChild(0));
              }
            }
          }
      }
    }
    return false;
  }

  string writePattern(Node* root, Pattern pat){
    string instruction=pat.instruction;
    string output="NO PATTERN WRITTEN (THIS IS BAD)";
    if(instruction=="Return"){
        output="    return\n";
    }
    else if(instruction=="Return Assignment"){
        string source=root->getChild(0)->curItem->accept(&looker_instruction).front();
        output ="    rax <- "+source+"\n";
        output+="    return\n";
    }
    else if(instruction=="Assignment"){
        //root=destination and root.child=source
        string destination=root->curItem->accept(&looker_instruction).front();
        string source=root->getChild(0)->curItem->accept(&looker_instruction).front();
        output="    "+destination+" <- "+source+"\n";
    }
    else if(instruction=="Load"){
        //root=destination and root.child=source
        string destination=root->getChild(0)->curItem->accept(&looker_instruction).front();
        string source=root->getChild(1)->curItem->accept(&looker_instruction).front();
        output="    "+destination+" <- mem "+source+" 0\n";
    }
    else if(instruction=="Store"){
        //root=destination and root.child=source
        string destination=root->getChild(0)->curItem->accept(&looker_instruction).front();
        string source=root->getChild(1)->curItem->accept(&looker_instruction).front();
        output="    mem "+destination+" 0 <- "+source+"\n";
    }
    else if(instruction=="Branch"){
        //root=destination and root.child=source
        string destination=root->getChild(0)->curItem->accept(&looker_instruction).front();
        int num= UseLabelMap(destination);
        output="    goto "+ LL0 + to_string(num) + "\n";
    }
    else if(instruction=="Branch Conditional" && !ignoreUndefined){
        //root=destination and root.child=source
        string destination=root->getChild(0)->curItem->accept(&looker_instruction).front();
        string condition=root->getChild(1)->curItem->accept(&looker_instruction).front();
        int num= UseLabelMap(destination);
        string proper_condition="1 = "+condition;
        //if(proper_condition==nullptr){printe("VAR USED BEFORE INSTANTIATED");return nullptr;}
        output="    cjump "+proper_condition+" "+LL0+to_string(num)+"\n";
    }
    /*
    else if(instruction=="Branch Conditional" && !ignoreUndefined){
        //root=destination and root.child=source
        string destination=root->getChild(0)->curItem->accept(&looker_instruction).front();
        string condition=root->getChild(1)->curItem->accept(&looker_instruction).front();
        int num= UseLabelMap(destination);
        string proper_condition=scanAllVIH(condition);
        if(proper_condition==""){
          printe("BOUT TO CALL STOI(CONDITION)");
          if(stoi(condition)!=0){
            printe("Passed");
            output="    goto "+ LL0 + to_string(num) + "\n";
          }
          else{output="";}
        }else{
        //if(proper_condition==nullptr){printe("VAR USED BEFORE INSTANTIATED");return nullptr;}
          output="    cjump "+proper_condition+" "+LL0+to_string(num)+"\n";
        }
    }
    */
    else if(instruction=="Assignment Arithmetic_Operator"){
        string destination=root->curItem->accept(&looker_instruction).front();
        string op_id=root->getChild(0)->curItem->accept(&looker_instruction).front();
        //printe("MAX6:"+op_id);
        string op=cg3_op_operators[stoi(op_id)];
        string source1=root->getChild(0)->getChild(0)->curItem->accept(&looker_instruction).front();
        string source2=root->getChild(0)->getChild(1)->curItem->accept(&looker_instruction).front();
        if(destination==source1){output="    "+destination+" "+op+" "+source2+"\n";}
        else if (destination==source2) {
          output="    %temppoop <- "+source1+"\n";
          output+="    %temppoop"+op+" "+source2+"\n";
          output+="    "+destination+" <- %temppoop\n";
        }
        else{
          output="    "+destination+" <- "+source1+"\n";
          output+="    "+destination+" "+op+" "+source2+"\n";
        }
        string secondPart=source1+"="+source2;
        variablesOnStandby.push_back(VIH(destination,secondPart));
    }
    else if(instruction=="Assignment Compare_Operator"){
        string destination=root->curItem->accept(&looker_instruction).front();
        string op_id=root->getChild(0)->curItem->accept(&looker_instruction).front();
        //printe("MAX5:"+op_id);
        string op=cg3_cmp_operators[stoi(op_id)];
        string source1=root->getChild(0)->getChild(0)->curItem->accept(&looker_instruction).front();
        string source2=root->getChild(0)->getChild(1)->curItem->accept(&looker_instruction).front();
        output="    "+destination+" <- ";
        string secondPart="";
        if(op==">"){secondPart=source2+" < "+source1;}
        else if(op==">="){secondPart=source2+" <= "+source1;}
        else{secondPart=source1+" "+op+" "+source2;}
        output+=secondPart+"\n";
        variablesOnStandby.push_back(VIH(destination,secondPart));
    }
    else if(instruction=="Call Assignment"){
        output+="I FOUTND IT";
    }
    return output;
  }

  void init_L2_patterns(){
    if(hasInit){return;}
    init_len_1();
    init_len_2();
    init_len_3();
    hasInit=true;
  }

  void init_len_1(){
    //SET UP
    bool isRoot=true;
    bool isNotRoot=false;
    string ret = "Return";

    //Return return(return)
    Node* root = new Node(nullptr,{ret},nullptr);
    allL2Patterns.push_back(Pattern(1,root,"Return"));
  }

  void init_len_2(){
    //SET UP
    bool isRoot=true;
    bool isNotRoot=false;
    string ret = "Return";
    string var = "Variable";
    string number = "Number";
    string load = "Load";
    string store= "Store";
    string label= "Label";
    string L="L";
    string Call="Call";
    string branch ="Branch";

    //Return Argument
    Node* root = new Node(nullptr,{ret},nullptr);
    Node* child = new Node(nullptr,{var,number},root);
    //root->children.push_back(child);
    allL2Patterns.push_back(Pattern(2,root,"Return Assignment"));

    //Assignment dst(var) <- src(var/num) 
    root = new Node(nullptr,{var},nullptr);
    child = new Node(nullptr,{var,number,label,L},root);
    //root->children.push_back(child);
    allL2Patterns.push_back(Pattern(2,root,"Assignment"));

    //Load dst(var) <- Load src(var)
    root = new Node(nullptr,{load},nullptr);
    Node* child1 = new Node(nullptr,{var},root);
    Node* child2 = new Node(nullptr,{var},root);
    allL2Patterns.push_back(Pattern(2,root,"Load"));

    //Store Store dst(var) <- src(var/num/label)
    root = new Node(nullptr,{store},nullptr);
    child1 = new Node(nullptr,{var},root);
    child2 = new Node(nullptr,{var,number,label},root);
    allL2Patterns.push_back(Pattern(2,root,"Store"));

    //Branch goto dst(label)
    root = new Node(nullptr,{branch},nullptr);
    child1 = new Node(nullptr,{label},root);
    allL2Patterns.push_back(Pattern(2,root,"Branch"));

    //Branch Conditional cjump compare dst(label) <- src(var/num/label)
    root = new Node(nullptr,{branch},nullptr);
    child1 = new Node(nullptr,{label},root);
    child2 = new Node(nullptr,{var,number},root);
    allL2Patterns.push_back(Pattern(2,root,"Branch Conditional"));
  }

  void init_len_3(){
    //SET UP
    bool isRoot=true;
    bool isNotRoot=false;
    string ret = "Return";
    string var = "Variable";
    string number = "Number";
    string arithOP = "Arithmetic_Operator";
    string compOP = "Compare_Operator";
    string Call="Call";
    string Args="Args";

    //Assignment Operation dst(var) <- src1(var/num) op(Arit_op) src2(var/num)
    Node* root = new Node(nullptr,{var},nullptr);
    Node* op = new Node(nullptr,{arithOP},root); 
    Node* child1=new Node(nullptr,{var,number},op);
    Node* child2=new Node(nullptr,{var,number},op);
    allL2Patterns.push_back(Pattern(3,root,"Assignment Arithmetic_Operator"));

    //Assignment Compare dst(var) <- src1(var/num) op(Arit_op) src2(var/num)
    root = new Node(nullptr,{var},nullptr);
    op = new Node(nullptr,{compOP},root); 
    child1=new Node(nullptr,{var,number},op);
    child2=new Node(nullptr,{var,number},op);
    allL2Patterns.push_back(Pattern(3,root,"Assignment Compare_Operator"));

    //Call Assignemnt
    root = new Node(nullptr,{var},nullptr);
    child1 = new Node(nullptr,{Call},root);
    child2 = new Node (nullptr,{Args},child1);
    allL2Patterns.push_back(Pattern(3,root,"Call Assignment"));

  }



  vector<string> ItemLookerInstruction::visit(Number *i) {
    vector<string> item_string;
    if(printAll_IS){printe("Number");}
    item_string.push_back(to_string(i->get_number()));
    return item_string;
  }

  vector<string> ItemLookerInstruction::visit(Label *i) {
    vector<string> item_string;
    if(printAll_IS){printe("Label");}
    item_string.push_back(i->get_name());
    return item_string;
  }

  vector<string> ItemLookerInstruction::visit(L *i) {
    vector<string> item_string;
    if(printAll_IS){printe("L");}
    item_string.push_back(i->get_name());
    return item_string;
  }

  vector<string> ItemLookerInstruction::visit(Variable *i) {
    vector<string> item_string;
    if(printAll_IS){printe("Variable");}
    item_string.push_back(i->get_name());
    return item_string;
  }

  vector<string> ItemLookerInstruction::visit(Compare_Operator *i) {
    vector<string> item_string;
    if(printAll_IS){printe("Compare_Operator");}
    item_string.push_back(to_string(i->get_op()));
    return item_string;
  }

  vector<string> ItemLookerInstruction::visit(Arithmetic_Operator *i) {
    vector<string> item_string;
    if(printAll_IS){printe("Arithmetic_Operator");}
    item_string.push_back(to_string(i->get_op()));
    return item_string;
  }

  vector<string> ItemLookerInstruction::visit(Vars *i) {
    vector<string> item_string;
    if(printAll_IS){printe("new vars");}
    item_string.push_back(to_string(i->getNumVars()));
    for (auto arg : i->getVars()) {
      string arg_string = arg->accept(&looker_instruction).front();
      item_string.push_back(arg_string);
    }
    return item_string;
  }

  vector<string> ItemLookerInstruction::visit(Args *i) {
    vector<string> item_string;
    if(printAll_IS){printe("Args");}
    item_string.push_back(to_string(i->getNumArgs()));
    for (auto arg : i->getArgs()) {
      string arg_string = arg->accept(&looker_instruction).front();
      item_string.push_back(arg_string);
    }

    return item_string;
  }



  void InstructionGenerator::visit(Instruction_assignment *i) {
    vector<string> item_string;
    if(printAll_IS){printe("Assignment");}
    //Just ADD
    currentContext->myInstructions.push_back(i);
    return;
  }

  void InstructionGenerator::visit(Instruction_assignment_op *i) {
    vector<string> item_string;
    if(printAll_IS){printe("Assignment op");}
    //Just ADD
    currentContext->myInstructions.push_back(i);
    return;
  }

  void InstructionGenerator::visit(Instruction_assignment_cmp *i) {
    vector<string> item_string;
    if(printAll_IS){printe("Assignment cmp");}
    //Just ADD
    currentContext->myInstructions.push_back(i);
    return;
  }

  void InstructionGenerator::visit(Instruction_return *i) {
    if(printAll_IS){printe("return");}
    //ADD AND RETURN
    currentContext->myInstructions.push_back(i);
    allContexts.push_back(*currentContext);
    currentContext=new Context();
    return;
  }

  void InstructionGenerator::visit(Instruction_return_arg *i) {
    vector<string> item_string;
    if(printAll_IS){printe("return arg");}
    //ADD AND RETURN
    currentContext->myInstructions.push_back(i);
    allContexts.push_back(*currentContext);
    currentContext=new Context();
    return;
  }

  void InstructionGenerator::visit(Instruction_load *i) {
    vector<string> item_string;
    if(printAll_IS){printe("load");}
    //Just ADD
    currentContext->myInstructions.push_back(i);
    return;
  }

  void InstructionGenerator::visit(Instruction_store *i) {
    vector<string> item_string;
    if(printAll_IS){printe("store");}
    //Just ADD
    currentContext->myInstructions.push_back(i);
    return;
  }

  void InstructionGenerator::visit(Instruction_branch *i) {
    vector<string> item_string;
    if(printAll_IS){printe("branch");}
    //ADD AND RETURN
    currentContext->myInstructions.push_back(i);
    allContexts.push_back(*currentContext);
    currentContext=new Context();
    return;
  }

  void InstructionGenerator::visit(Instruction_branch_conditional *i) {
    vector<string> item_string;
    if(printAll_IS){printe("branch conditional");}
    //ADD AND RETURN
    currentContext->myInstructions.push_back(i);
    allContexts.push_back(*currentContext);
    currentContext=new Context();
    return;
  }

  void InstructionGenerator::visit(Instruction_call *i) {
    vector<string> item_string;
    if(printAll_IS){printe("call");}
    //Just Return
    allContexts.push_back(*currentContext);
    currentContext=new Context();
    return;
  }

  void InstructionGenerator::visit(Instruction_assignment_call *i) {
    vector<string> item_string;
    if(printAll_IS){printe("Assignment call");}
    //Just Add
    currentContext->myInstructions.push_back(i);
    allContexts.push_back(*currentContext);
    currentContext=new Context();
    return;
  }

  void InstructionGenerator::visit(Instruction_label *i) {
    vector<string> item_string;
    if(printAll_IS){printe("label");}
    //Just Return
    allContexts.push_back(*currentContext);
    currentContext=new Context();
    return;
  }
  
}
