#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <unordered_map>
#include <deque>
#include <algorithm>

#include <code_generator.h>

using namespace std;


namespace LB {
  void printg(string output) {
    if (true) {
      cout << output << "\n";
    }
  }

  Scopes::Scopes(Instruction* startIns, int id){
    startOfScope=startIns;
    scopeId=id;
    return;
  }

  void Scopes::append(Instruction* instruction){
    //printg("Started Append");
   contentsOfScope.push_back(instruction); 
   //printg("Passed Append");
  }

  int Scopes::size(){
    return contentsOfScope.size();
  }


  WhileLoop::WhileLoop(Instruction* whileIns,int while_id, string condLabel_str){
    Instruction_while* ins=dynamic_cast<Instruction_while*>(whileIns);
    startOfWhile=ins->getTrue();
    startOfWhile_str=dynamic_cast<Label*>(startOfWhile)->getName();
    endOfWhile=ins->getFalse();
    endOfWhile_str=dynamic_cast<Label*>(endOfWhile)->getName();
    id=while_id;
    condLabel=condLabel_str;
    return;
  }

  bool WhileLoop::isTrueLabel(Instruction* ins){
    auto maybeLabel=dynamic_cast<Instruction_label*>(ins);
    if(maybeLabel==nullptr){return false;}
    auto maybe=(dynamic_cast<Label*>(maybeLabel->getLabel()))->getName();
    //printg(maybe);
    bool check=(maybe==startOfWhile_str);
    //ins->printInstruction();
    //if(check){printg("TRUE");}
    //printg(to_string(check));
    return check;
  }

  bool WhileLoop::isFalseLabel(Instruction* ins){
    auto maybeLabel=dynamic_cast<Instruction_label*>(ins);
    if(maybeLabel==nullptr){return false;}
    auto maybe=(dynamic_cast<Label*>(maybeLabel->getLabel()))->getName();
    //printg(maybe);
    bool check=(maybe==endOfWhile_str);
    //ins->printInstruction();
    //if(check){printg("TRUE");}
    return check;
  }


  void WhileLoop::append(Instruction* instruction){
    //printg("Started Append");
   contentsOfWhile.push_back(instruction); 
   //printg("Passed Append");
  }

  int WhileLoop::size(){
    return contentsOfWhile.size();
  }

  // string myOutput = "";
  bool isScopeEntry(Instruction* possibleEntry){
    if(dynamic_cast<Instruction_start_scope*>(possibleEntry)!=nullptr){
      return true;
    }
    return false;
  }
  bool isScopeExit(Instruction* possibleEntry){
    if(dynamic_cast<Instruction_end_scope*>(possibleEntry)!=nullptr){
      return true;
    }
    return false;
  }

  bool isWhileEntry(Instruction* possibleEntry){
    if(dynamic_cast<Instruction_while*>(possibleEntry)!=nullptr){
      return true;
    }
    return false;
  }

  



  map<int,Scopes*> allScopes;
  map<Instruction*,Instruction*> instructionMap;
  map<string,string> varMap;
  vector<Scopes*> scopeBuffer;
  Scopes* currentScope=nullptr;
  int currentScopeId=0;
  Scopes* myScope;
  Instruction* myInstruction;
  string myOutput;
  ItemLooker looker;
  map<Instruction*,WhileLoop*> trueLabelLoopMap;
  map<Instruction*,WhileLoop*> falseLabelLoopMap;
  map<Instruction*,WhileLoop*> continueLabelLoopMap;
  map<Instruction*,WhileLoop*> breakLabelLoopMap;
  map<WhileLoop*,string> whileToString;
  void generate_code(Program p){
    
    std::ofstream outputFile;
    outputFile.open("prog.a");
    myOutput="";
    //MAKE SCOPES
    for (auto f : p.functions) {
      printg("\n-Started New Function-");
      bool firstScope=true;
       for (auto i : f->instructions) {
         //i->printInstruction();
        //for(auto s : scopeBuffer){
          //s->append(i);
        //}
        if(currentScope!=nullptr){
          auto declaration=dynamic_cast<Instruction_declaration*>(i);
          if(declaration!=nullptr){
            vector<string> uniqueVars=declaration->getVar()->accept(&looker);
            for(auto var : uniqueVars){
              currentScope->scopeVarMap[var]=var+"_"+to_string(currentScopeId);
            }
          }
          currentScope->append(i);
        }
         if(isScopeEntry(i)){
          //printg("Found New Scope");
          if(currentScope!=nullptr){
            scopeBuffer.push_back(currentScope);
          }
            currentScope=new Scopes(i,currentScopeId);
            currentScope->append(i);
          if(firstScope){
            //printg("First Scope");
            auto vars = dynamic_cast<Vars*>(f->arguments);
            if(vars!=nullptr){
              vector<Item*> uniqueVars=vars->getVars();
              //printg(to_string(vars->getNumVars()));
              for(auto var : uniqueVars){
                string v=var->accept(&looker).front();
                if(v=="TYPE"){continue;}
                currentScope->scopeVarMap[v]=v+"_1";
                //printg(currentScope->scopeVarMap[v]);
              }
            }
            firstScope=false;
          }
          if(scopeBuffer.size()>0){
            currentScope->scopeVarMap=scopeBuffer.back()->scopeVarMap;
          }
            currentScopeId+=1;
         }
         else if(isScopeExit(i)){
          //printg("End of Scope");
          currentScope->endOfScope=i;
          //currentScope->append(i);
          //printg(to_string(currentScope->scopeId));
          allScopes[currentScope->scopeId]=currentScope;
          if(scopeBuffer.size()>0){
            currentScope=scopeBuffer.back();
            scopeBuffer.pop_back();
          }
         }
        else{
          //printg("Niether");
        }
         
       }
    
    printg("-Finished Processing Scopes-");
    ScopeGenerator generator;
    CodeGenerator code_generator;
    for(int i=0;i<currentScopeId;i++){
      //printg("Size:"+to_string(allScopes.size()));
      Scopes* s=allScopes[i];
      //printg("Start Scope "+to_string(i));
      myScope=s;
      for(int j=0; j<s->contentsOfScope.size();j++){
        //s->contentsOfScope[j]->printInstruction();
        s->contentsOfScope[j]->accept(&generator);
        instructionMap[s->contentsOfScope[j]]=myInstruction;
        myInstruction=nullptr;
        //s->contentsOfScope[j]=new Instruction_return_arg(new Number(s->scopeId));
        //s->contentsOfScope[j]->printInstruction();
      }
      //printg("End Scope\n\n");
    }
    
    vector<WhileLoop*> whileBuffer;
    vector<WhileLoop*> activeWhileBuffer;
    printg("-Finished New Instruction-");
    vector<Instruction*> allInst;
    string longest_label="";
    for(auto i : f->instructions){
      Instruction_label* maybe = dynamic_cast<Instruction_label*>(i);
      if(maybe!=nullptr){
        string try_str=dynamic_cast<Label*>(maybe->getLabel())->getName();
        if(try_str.length()>longest_label.length()){
          longest_label=try_str;
        }
      }
    }
    int c=0;
    for(int i=0;i< f->instructions.size();i++){
      Instruction* ins=f->instructions[i];
      if(isWhileEntry(ins)){
        whileBuffer.push_back(new WhileLoop(ins,c,(longest_label+"_condition_"+to_string(c))));
        allInst.push_back(new Instruction_label(new Label(longest_label+"_condition_"+to_string(c))));
        //printg(whileBuffer.back()->condLabel);
        c+=1;
      }
      allInst.push_back(ins);
    }
    //printg(to_string(whileBuffer.back()->id));
    f->instructions=allInst;
    WhileLoop* currentWhile=nullptr;
    vector<WhileLoop*> stack;
    for(auto i : f->instructions){
      //printg(to_string(stack.size()));
      //i->printInstruction();
      if(stack.size()>0){
        currentWhile=stack.back();
        if(dynamic_cast<Instruction_continue*>(i)!=nullptr){
          continueLabelLoopMap[i]=currentWhile;
          currentWhile->continueIns=i;
          //printg(currentWhile->condLabel);
        }
        if(dynamic_cast<Instruction_break*>(i)!=nullptr){
          breakLabelLoopMap[i]=currentWhile;
          currentWhile->breakIns=i;
          //printg(currentWhile->condLabel);
        }
      }
      for(int j=0;j<whileBuffer.size();j++){
        WhileLoop* whil=whileBuffer[j];
        if(whil->isTrueLabel(i)){
          //printg("GOT TRUE");
          stack.push_back(whil);
        }
        if(whil->isFalseLabel(i)){
          //printg("GOT FALSE");
          stack.pop_back();
        }
      }
    }
    
    
    printg("-Starting Tranlation of Function Vars-");
    auto hold=dynamic_cast<Vars*>(f->arguments)->translateSelf(&looker);
    Vars* hold2=dynamic_cast<Vars*>(hold);
    /*
    for(Item* var : hold2->getVars()){
      string n_str=var->accept(&looker).back();
      printg(n_str);
    }
    */
    f->arguments=hold2;
    myOutput+= f->return_type->to_string() + " " + f->name + " ( " + f->arguments->to_string() + " ) ";
    //currentScope->scopeVarMap[var]=var+"_"+to_string(currentScopeId);
    myOutput+="{\n";  

    printg("-Starting Function Read-");
    for(auto i : f->instructions){
      if(instructionMap[i]!=nullptr){
        //printg("used map");
        //instructionMap[i]->printInstruction();
        instructionMap[i]->accept(&code_generator);
      }else{
        i->accept(&code_generator);
      }
    }
    
    myOutput+="}\n";
  }
  //print("Starting Final Function Read");

    printg(myOutput);
    outputFile<<myOutput;
    outputFile.close();
   
    return ;
  }

  vector<string> ItemLooker::visit(Number *i) {
     vector<string> item_string;
     return item_string;
  }

  vector<string> ItemLooker::visit(Label *i) {
     vector<string> item_string;
     return item_string;
  }

  vector<string> ItemLooker::visit(L *i) {
     vector<string> item_string;
     return item_string;
  }

   vector<string> ItemLooker::visit(Name *i) {
     vector<string> item_string;
     string name_str=i->getName();
     item_string.push_back(name_str);
     return item_string;
   }

   vector<string> ItemLooker::visit(Type *i) {
     vector<string> item_string;
     string name_str="TYPE";
     string translated=i->to_string();
     item_string.push_back(name_str);
     item_string.push_back(translated);
     return item_string;
   }

   vector<string> ItemLooker::visit(Operator *i) {
     vector<string> item_string;
     return item_string;
   }

   vector<string> ItemLooker::visit(Vars *i) {
     vector<string> item_string;
     return item_string;
   }

   vector<string> ItemLooker::visit(Args *i) {
    vector<string> item_string;
    return item_string;
   }

   
   vector<string> ItemLooker::visit(Names *i) {
    vector<string> item_string;
    //printg("Visited Names");
    for(Item* n : i->getNames()){
      string n_str=n->accept(&looker).back();
      //printg(n_str);
      item_string.push_back(n_str);
    }
    return item_string;
   }

  Item* ItemLooker::translate(Number *i) {
     return i;
  }

  Item* ItemLooker::translate(Label *i) {
     return i;
  }

  Item* ItemLooker::translate(L *i) {
     return i;
  }

  Item* ItemLooker::translate(Name *i) {
     string name_str=i->getName();
     if(name_str=="input" || name_str=="print" || name_str=="tensor-error"){
      return i;
     }
     string newName_str=myScope->scopeVarMap[name_str];
     //printg(name_str);
     if(!(myScope->scopeVarMap.count(name_str))){
        printg("Variable "+name_str+" is not in scope");
     }
     if(newName_str==""){return i;}
     //printg("mapped to:"+newName_str);
     Name* newName=new Name(newName_str);
     return newName;
     
  }

   Item* ItemLooker::translate(Type *i) {
     return i;
   }

   Item* ItemLooker::translate(Operator *i) {
     return i;
   }

   Item* ItemLooker::translate(Vars *i) {
      Item* item_string;
      vector<Item*> listNames;
      for(Item* n : i->getVars()){
        listNames.push_back(n->translateSelf(&looker));
      }
      Vars* hold=new Vars(listNames);
      return hold;
   }

   Item* ItemLooker::translate(Args *i) {
      Item* item_string;
      vector<Item*> listNames;
      for(Item* n : i->getArgs()){
        listNames.push_back(n->translateSelf(&looker));
      }
      Args* hold=new Args(listNames);
      return hold;
   }

   Item* ItemLooker::translate(Names *i) {
    Item* item_string;
    vector<Item*> listNames;
    for(Item* n : i->getNames()){
      listNames.push_back(n->translateSelf(&looker));
    }
    Names* hold=new Names(listNames);
    return hold;
   }

  void ScopeGenerator::visit(Instruction_start_scope *i){
    return;
  }

  void ScopeGenerator::visit(Instruction_end_scope *i){
    return;
  }

  void ScopeGenerator::visit(Instruction_declaration *i) {
    Item* type=i->getType()->translateSelf(&looker);
    Item* names=i->getVar()->translateSelf(&looker);
    myInstruction=new Instruction_declaration(type,names);
    return;
  }

  void ScopeGenerator::visit(Instruction_assignment *i) {
    
     auto src = i->getSource()->translateSelf(&looker);
     auto dst = i->getDestination()->translateSelf(&looker);
     myInstruction = new Instruction_assignment(dst, src);    
     return;
  }

   void ScopeGenerator::visit(Instruction_assignment_op *i) {
     auto src = i->getSource1()->translateSelf(&looker);
     auto src2 = i->getSource2()->translateSelf(&looker);
     auto dst = i->getDestination()->translateSelf(&looker);
     auto op = i->getOperator()->translateSelf(&looker);
     myInstruction = new Instruction_assignment_op(dst, src, op, src2);    
     return;
}

 void ScopeGenerator::visit(Instruction_read_tensor *i) {
    auto dst = i->getDestination()->translateSelf(&looker);
	  auto src = i->getSource()->translateSelf(&looker);
    auto itms = i->getIndices();
    vector<Item*> newItems;
    for(auto itm : itms){
      newItems.push_back(itm->translateSelf(&looker));
    }
    myInstruction = new Instruction_read_tensor(dst, src, newItems);   
    return;
  }

   void ScopeGenerator::visit(Instruction_write_tensor *i) {
    auto dst = i->getDestination()->translateSelf(&looker);
	  auto src = i->getSource()->translateSelf(&looker);
    auto itms = i->getIndices();
    vector<Item*> newItems;
    for(auto itm : itms){
      newItems.push_back(itm->translateSelf(&looker));
    }
    myInstruction = new Instruction_write_tensor(dst, src, newItems);   
    return;
   }

   void ScopeGenerator::visit(Instruction_length *i) {
    auto dst = i->getDestination()->translateSelf(&looker);
	  auto src = i->getSource()->translateSelf(&looker);
    auto itms = i->getIndex()->translateSelf(&looker);
    myInstruction = new Instruction_length(dst, src, itms);   
    return;
   }

   void ScopeGenerator::visit(Instruction_new_array *i) {
      auto dst = i->getDestination()->translateSelf(&looker);
      auto itms = i->getSizes()->translateSelf(&looker);
      /*/
      vector<Item*> newItems;
      for(auto itm : itms){
        newItems.push_back(itm->translateSelf(&looker));
      }
      */
      myInstruction = new Instruction_new_array(dst, itms);   
     return;
   }

  void ScopeGenerator::visit(Instruction_new_tuple *i) {
      auto dst = i->getDestination()->translateSelf(&looker);
      auto itms = i->getSize()->translateSelf(&looker);
      myInstruction = new Instruction_new_tuple(dst, itms);   
     return;
   }

   void ScopeGenerator::visit(Instruction_return *i) {
    
     return;
   }

   void ScopeGenerator::visit(Instruction_return_arg *i) {
      auto itms = i->getValue()->translateSelf(&looker);
      myInstruction = new Instruction_return_arg(itms);   
     return;
   }

   void ScopeGenerator::visit(Instruction_branch *i) {
     return;
   }

   void ScopeGenerator::visit(Instruction_branch_conditional *i) {
     return;
   }

   void ScopeGenerator::visit(Instruction_call *i) {
      auto callee = i->getCallee()->translateSelf(&looker);
      auto args = i->getArgs()->translateSelf(&looker);
      myInstruction = new Instruction_call(callee, args); 
     return;
   }

   void ScopeGenerator::visit(Instruction_assignment_call *i) {
    auto dst = i->getDst()->translateSelf(&looker);
      auto callee = i->getCallee()->translateSelf(&looker);
      auto args = i->getArgs()->translateSelf(&looker);
      myInstruction = new Instruction_assignment_call(dst,callee, args); 
     return;
   }

  void ScopeGenerator::visit(Instruction_label *i) {
    return;
  }
  void ScopeGenerator::visit(Instruction_if *i) {
      auto c1 = i->getCompare1()->translateSelf(&looker);
      auto op = i->getOp()->translateSelf(&looker);
      auto c2 = i->getCompare2()->translateSelf(&looker);
      auto t = i->getTrue()->translateSelf(&looker);
      auto f = i->getFalse()->translateSelf(&looker);
      myInstruction = new Instruction_if(c1,op, c2,t,f); 
     return;
  }

  void ScopeGenerator::visit(Instruction_while *i) {
      auto c1 = i->getCompare1()->translateSelf(&looker);
      auto op = i->getOp()->translateSelf(&looker);
      auto c2 = i->getCompare2()->translateSelf(&looker);
      auto t = i->getTrue()->translateSelf(&looker);
      auto f = i->getFalse()->translateSelf(&looker);
      myInstruction = new Instruction_while(c1,op, c2,t,f); 
  }

  void ScopeGenerator::visit(Instruction_goto *i) {
    //printg("Called Goto");
    auto dst = i->getDest()->translateSelf(&looker);
    myInstruction = new Instruction_goto(dst); 
    return;
  }

  void ScopeGenerator::visit(Instruction_continue *i) {
    return;
  }

  void ScopeGenerator::visit(Instruction_break *i) {
    return;
  }
  
  void ScopeGenerator::visit(Scope *i) {
    return;
  }

/////
///// Start Code Generator
/////
  void CodeGenerator::visit(Instruction_start_scope *i){
    //printg("Called Start Scope");
    //myOutput+=i->InstructionToLA();
    return;
  }

  void CodeGenerator::visit(Instruction_end_scope *i){
    //printg("Called End Scope");
    //myOutput+=i->InstructionToLA();
    return;
  }

  void CodeGenerator::visit(Instruction_declaration *i) {
   // printg("Called Instruction_declaration");
    myOutput+=i->InstructionToLA();
    return;
  }

  void CodeGenerator::visit(Instruction_assignment *i) {
    //printg("Called Assignement");
     myOutput+=i->InstructionToLA();
     return;
  }

   void CodeGenerator::visit(Instruction_assignment_op *i) {
    //printg("Called Assignment_op");
     myOutput+=i->InstructionToLA();
     return;
}

 void CodeGenerator::visit(Instruction_read_tensor *i) {
      //printg("Called Read_Tensor");
     myOutput+=i->InstructionToLA();
     return;
  }

   void CodeGenerator::visit(Instruction_write_tensor *i) {
       // printg("Called Write_Tensor");
     myOutput+=i->InstructionToLA();
     return;
   }

   void CodeGenerator::visit(Instruction_length *i) {
    //printg("Called Length");
     myOutput+=i->InstructionToLA();
     return;
   }

   void CodeGenerator::visit(Instruction_new_array *i) {
     //   printg("Called New Array");
     myOutput+=i->InstructionToLA();
     return;
   }

  void CodeGenerator::visit(Instruction_new_tuple *i) {
     //   printg("Called New Tuple");
     myOutput+=i->InstructionToLA();
    return;
   }

   void CodeGenerator::visit(Instruction_return *i) {
      //  printg("Called Return");
     myOutput+=i->InstructionToLA();
     return;
   }

   void CodeGenerator::visit(Instruction_return_arg *i) {
     //   printg("Called Return Arg");
     myOutput+=i->InstructionToLA();
     return;
   }

   void CodeGenerator::visit(Instruction_branch *i) {
     //   printg("Called Branch");
     myOutput+=i->InstructionToLA();
     return;
   }

   void CodeGenerator::visit(Instruction_branch_conditional *i) {
     //   printg("Called Branch Conditional");
     myOutput+=i->InstructionToLA();
     return;
   }

   void CodeGenerator::visit(Instruction_call *i) {
     //   printg("Called Call");
     myOutput+=i->InstructionToLA();
     return;
   }

   void CodeGenerator::visit(Instruction_assignment_call *i) {
     //   printg("Called Assignment_Call");
     myOutput+=i->InstructionToLA();
     return;
   }

  void CodeGenerator::visit(Instruction_label *i) {
     //   printg("Called Label");
     myOutput+=i->InstructionToLA();
    return;
  }
  void CodeGenerator::visit(Instruction_if *i) {
    //printg("Called IF");
     myOutput+=i->InstructionToLA();
    return;
  }

  void CodeGenerator::visit(Instruction_while *i) {
    //printg("Called While");
    myOutput+=i->InstructionToLA();
    return;
  }

  void CodeGenerator::visit(Instruction_goto *i) {
        //printg("Called Goto");
     myOutput+=i->InstructionToLA();
    return;
  }

  void CodeGenerator::visit(Instruction_continue *i) {
    printg("Called Continue");
    auto l=continueLabelLoopMap[i];
    string str_in=l->condLabel;
    myOutput+="br "+str_in+"\n";
    return;
  }

  void CodeGenerator::visit(Instruction_break *i) {
    printg("Called Break");
    auto l=breakLabelLoopMap[i];
    string str_in=l->endOfWhile_str;
    myOutput+="br "+str_in+"\n";
    return;
  }
  
  void CodeGenerator::visit(Scope *i) {
        printg("Called Scope");
     myOutput+=i->InstructionToLA();
    return;
  }
  // void ArrayChecking::visit(Instruction_declaration *i) {
  //   types[i->get_var()->to_string()] = i->get_type()->to_string();
  //   return;
  // }

  // void ArrayChecking::visit(Instruction_assignment *i) {
  //   return;
  // }

  // void ArrayChecking::visit(Instruction_assignment_op *i) {
  //   return;
  // }

  

  // void ArrayChecking::visit(Instruction_read_tensor *i) {
  //   printg("start read");
  //   auto comp = new Name(LL + "_comp");
  //   auto correct = new Label(":" + LL + "_CORRECT_" + to_string(instruction_index));
  //   auto error = new Label(":" + LL + "_ERROR_" + to_string(instruction_index));
  //   auto linenumber = new Name(LL + "_LineNumber");
  //   printg("setup");
  //   auto i1 = new Instruction_assignment(linenumber, new Number((i->line_number << 1) + 1));
  //   // auto i2 = new Instruction_declaration(new Type(TypeID::int64, 0), newV);
  //   auto i3 = new Instruction_assignment_op(comp, i->getSource(), new Operator(OperatorID::e), new Number(0));
  //   auto i4 = new Instruction_branch_conditional(comp, error, correct);
  //   auto i5 = new Instruction_label(error);
  //   vector<Item*> new_args;
  //   new_args.push_back(linenumber);
  //   auto i6 = new Instruction_call(new Name("tensor-error"), new Args(new_args));
  //   auto i7 = new Instruction_label(correct);
  //   f->instructions.insert(f->instructions.begin() + instruction_index, i7);
  //   // f->instructions.insert(instruction_index, i2);
  //   f->instructions.insert(f->instructions.begin() + instruction_index, i6);
  //   f->instructions.insert(f->instructions.begin() + instruction_index, i5);
  //   f->instructions.insert(f->instructions.begin() + instruction_index, i4);
  //   f->instructions.insert(f->instructions.begin() + instruction_index, i3);
  //   f->instructions.insert(f->instructions.begin() + instruction_index, i1);
  //   instruction_index += 6;
  //   // Check array access 
  //   // Check if tuple
  //   if (types[i->getSource()->to_string()] != "tuple") {
  //     // Single-dimension array
  //     if (i->getIndices().size() == 1) {
  //       auto length = new Name(LL + "_length");
  //       auto good = new Label(":" + LL + "_GOOD_" + to_string(instruction_index));
  //       auto bad = new Label(":" + LL + "_BAD_" + to_string(instruction_index));
  //       Item* encoded_index;
  //       if (i->getIndices().front()->to_string().at(0) == '%') {
  //         encoded_index = i->getIndices().front();
  //       } else {
  //         encoded_index = new Number((stol(i->getIndices().front()->to_string()) << 1) + 1);
  //       }

  //       auto a1 = new Instruction_length(length, i->getSource(), new Number(0));
  //       auto a2 = new Instruction_assignment_op(comp, encoded_index, new Operator(OperatorID::l), length);
  //       auto a3 = new Instruction_branch_conditional(comp, good, bad);
  //       auto a4 = new Instruction_label(bad);
  //       vector<Item*> new_args2 = {linenumber, length, encoded_index};
  //       auto a5 = new Instruction_call(new Name("tensor-error"), new Args(new_args2));
  //       auto a6 = new Instruction_label(good);
  //       f->instructions.insert(f->instructions.begin() + instruction_index, a6);
  //       f->instructions.insert(f->instructions.begin() + instruction_index, a5);
  //       f->instructions.insert(f->instructions.begin() + instruction_index, a4);
  //       f->instructions.insert(f->instructions.begin() + instruction_index, a3);
  //       f->instructions.insert(f->instructions.begin() + instruction_index, a2);
  //       f->instructions.insert(f->instructions.begin() + instruction_index, a1);
  //       instruction_index += 6;
  //     } else {
  //       Instruction* a1;
  //       Instruction* a2;
  //       Instruction* a3;
  //       Instruction* a4;
  //       Instruction* a5;
  //       Instruction* a6;
  //       vector<Item*> new_args2;
  //       for (int arg_index = 0; arg_index < i->getIndices().size(); arg_index++) {
  //         cout << i->getIndices().at(arg_index)->to_string() << "\n";
  //         auto length = new Name(LL + "_length");
  //         auto good = new Label(":" + LL + "_GOOD_" + to_string(instruction_index) + "_" + to_string(arg_index));
  //         auto bad = new Label(":" + LL + "_BAD_" + to_string(instruction_index) + "_" + to_string(arg_index));
  //         Item* encoded_index;
  //         if (i->getIndices().at(arg_index)->to_string().at(0) == '%') {
            
  //           encoded_index = i->getIndices().at(arg_index);
  //         } else {
            
  //           encoded_index = new Number((stol(i->getIndices().at(arg_index)->to_string()) << 1) + 1);
  //         }

  //         a1 = new Instruction_length(length, i->getSource(), new Number(arg_index));
  //         a2 = new Instruction_assignment_op(comp, encoded_index, new Operator(OperatorID::l), length);
  //         a3 = new Instruction_branch_conditional(comp, good, bad);
  //         a4 = new Instruction_label(bad);
  //         new_args2 = {linenumber, new Number((arg_index << 1) + 1), length, encoded_index};
  //         a5 = new Instruction_call(new Name("tensor-error"), new Args(new_args2));
  //         a6 = new Instruction_label(good);
  //         f->instructions.insert(f->instructions.begin() + instruction_index, a6);
  //         f->instructions.insert(f->instructions.begin() + instruction_index, a5);
  //         f->instructions.insert(f->instructions.begin() + instruction_index, a4);
  //         f->instructions.insert(f->instructions.begin() + instruction_index, a3);
  //         f->instructions.insert(f->instructions.begin() + instruction_index, a2);
  //         f->instructions.insert(f->instructions.begin() + instruction_index, a1);
          
  //         instruction_index += 6;
  //       }
  //     }
  //   }


  //   return;
  // }

  // void ArrayChecking::visit(Instruction_write_tensor *i) {
  //   cout << "start write";
  //   auto comp = new Name(LL + "_comp");
  //   auto correct = new Label(":" + LL + "_CORRECT_" + to_string(instruction_index));
  //   auto error = new Label(":" + LL + "_ERROR_" + to_string(instruction_index));
  //   auto linenumber = new Name(LL + "_LineNumber");
  //   auto i1 = new Instruction_assignment(linenumber, new Number((i->line_number << 1) + 1));
  //   // auto i2 = new Instruction_declaration(new Type(TypeID::int64, 0), newV);
  //   auto i3 = new Instruction_assignment_op(comp, i->getDestination(), new Operator(OperatorID::e), new Number(0));
  //   auto i4 = new Instruction_branch_conditional(comp, error, correct);
  //   auto i5 = new Instruction_label(error);
  //   vector<Item*> new_args;
  //   new_args.push_back(linenumber);
  //   auto i6 = new Instruction_call(new Name("tensor-error"), new Args(new_args));
  //   auto i7 = new Instruction_label(correct);
  //   f->instructions.insert(f->instructions.begin() + instruction_index, i7);
  //   // f->instructions.insert(instruction_index, i2);
  //   f->instructions.insert(f->instructions.begin() + instruction_index, i6);
  //   f->instructions.insert(f->instructions.begin() + instruction_index, i5);
  //   f->instructions.insert(f->instructions.begin() + instruction_index, i4);
  //   f->instructions.insert(f->instructions.begin() + instruction_index, i3);
  //   f->instructions.insert(f->instructions.begin() + instruction_index, i1);
  //   instruction_index += 6;

  //   // Check array access 
  //   // Check if tuple
    

  //   if (types[i->getDestination()->to_string()] != "tuple") {
  //     // Single-dimension array
  //     if (i->getIndices().size() == 1) {
  //       auto length = new Name(LL + "_length");
  //       auto decoded_length = new Name(LL + "_length_decoded");
  //       auto good = new Label(":" + LL + "_GOOD_" + to_string(instruction_index));
  //       auto bad = new Label(":" + LL + "_BAD_" + to_string(instruction_index));
  //       Item* encoded_index;
  //       Item* decoded_index;
  //       if (i->getIndices().front()->to_string().at(0) == '%') {
  //         decoded_index = i->getIndices().front();
  //         encoded_index = new Name(LL + "_encoded_index");
  //         auto aencode = new Instruction_assignment_op(encoded_index, decoded_index, new Operator(OperatorID::salq), new Number(1));
  //         auto aencode2 = new Instruction_assignment_op(encoded_index, encoded_index, new Operator(OperatorID::addq), new Number(1));
  //         f->instructions.insert(f->instructions.begin() + instruction_index, aencode2);
  //         f->instructions.insert(f->instructions.begin() + instruction_index, aencode);
  //         instruction_index += 2;
  //       } else {
  //         encoded_index = new Number((stol(i->getIndices().front()->to_string()) << 1) + 1);
  //         decoded_index = i->getIndices().front();
  //       }

  //       auto a1 = new Instruction_length(length, i->getDestination(), new Number(0));
  //       auto atest = new Instruction_assignment_op(decoded_length, length, new Operator(OperatorID::sarq), new Number(1));
  //       auto a2 = new Instruction_assignment_op(comp, decoded_index, new Operator(OperatorID::l), decoded_length);
  //       auto a3 = new Instruction_branch_conditional(comp, good, bad);
  //       auto a4 = new Instruction_label(bad);
  //       vector<Item*> new_args2 = {linenumber, length, encoded_index};
  //       auto a5 = new Instruction_call(new Name("tensor-error"), new Args(new_args2));
  //       auto a6 = new Instruction_label(good);
  //       f->instructions.insert(f->instructions.begin() + instruction_index, a6);
  //       f->instructions.insert(f->instructions.begin() + instruction_index, a5);
  //       f->instructions.insert(f->instructions.begin() + instruction_index, a4);
  //       f->instructions.insert(f->instructions.begin() + instruction_index, a3);
  //       f->instructions.insert(f->instructions.begin() + instruction_index, a2);

  //       f->instructions.insert(f->instructions.begin() + instruction_index, atest);
  //       f->instructions.insert(f->instructions.begin() + instruction_index, a1);
  //       instruction_index += 7;
  //     } else {
  //       Instruction* a1;
  //       Instruction* a2;
  //       Instruction* a3;
  //       Instruction* a4;
  //       Instruction* a5;
  //       Instruction* a6;
  //       vector<Item*> new_args2;
  //       for (int arg_index = 0; arg_index < i->getIndices().size(); arg_index++) {
  //         auto length = new Name(LL + "_length");
  //         auto good = new Label(":" + LL + "_GOOD_" + to_string(instruction_index) + "_" + to_string(arg_index));
  //         auto bad = new Label(":" + LL + "_BAD_" + to_string(instruction_index) + "_" + to_string(arg_index));
  //         Item* encoded_index;
  //         if (i->getIndices().at(arg_index)->to_string().at(0) == '%') {
  //           encoded_index = i->getIndices().at(arg_index);
  //         } else {
  //           encoded_index = new Number((stol(i->getIndices().at(arg_index)->to_string()) << 1) + 1);
  //         }

  //         a1 = new Instruction_length(length, i->getDestination(), new Number(arg_index));
  //         a2 = new Instruction_assignment_op(comp, encoded_index, new Operator(OperatorID::l), length);
  //         a3 = new Instruction_branch_conditional(comp, good, bad);
  //         a4 = new Instruction_label(bad);
  //         new_args2 = {linenumber, new Number((arg_index << 1) + 1), length, encoded_index};
  //         a5 = new Instruction_call(new Name("tensor-error"), new Args(new_args2));
  //         a6 = new Instruction_label(good);
  //         f->instructions.insert(f->instructions.begin() + instruction_index, a6);
  //         f->instructions.insert(f->instructions.begin() + instruction_index, a5);
  //         f->instructions.insert(f->instructions.begin() + instruction_index, a4);
  //         f->instructions.insert(f->instructions.begin() + instruction_index, a3);
  //         f->instructions.insert(f->instructions.begin() + instruction_index, a2);
  //         f->instructions.insert(f->instructions.begin() + instruction_index, a1);
          
  //         instruction_index += 6;
  //       }
  //     }
  //   }


  //   return;
  // }

  // void ArrayChecking::visit(Instruction_length *i) {
  //   return;
  // }

  // void ArrayChecking::visit(Instruction_new_array *i) {
  //   return;
  // }

  // void ArrayChecking::visit(Instruction_new_tuple *i) {
  //   return;
  // }

  // void ArrayChecking::visit(Instruction_return *i) {
    
  //   return;
  // }

  // void ArrayChecking::visit(Instruction_return_arg *i) {
  //   return;
  // }

  // void ArrayChecking::visit(Instruction_branch *i) {
  //   return;
  // }

  // void ArrayChecking::visit(Instruction_branch_conditional *i) {
  //   return;
  // }

  // void ArrayChecking::visit(Instruction_call *i) {
  //   return;
  // }

  // void ArrayChecking::visit(Instruction_assignment_call *i) {
  //   return;
  // }

  // void ArrayChecking::visit(Instruction_label *i) {
  //   return;
  // }

  // void BBGenerator::visit(Instruction_declaration *i) {
  //   if (startBB) {
  //     auto l = new Instruction_label(new Label(":" + LL + "_label_" + to_string(instruction_index)));
  //     new_instructions.push_back(l);
  //     startBB = false;
  //   }
  //   new_instructions.insert(new_instructions.begin() + 1, i);
  //   num_declarations++;
  //   if (i->get_type()->to_string() == "int64") {
  //     new_instructions.insert(new_instructions.begin() + num_declarations, new Instruction_assignment(i->get_var(), new Number(1)));
  //   } else {
  //     new_instructions.insert(new_instructions.begin() + num_declarations, new Instruction_assignment(i->get_var(), new Number(0)));
    
  //   }
    

  //   return;
  // }

  // void BBGenerator::visit(Instruction_assignment *i) {
  //   if (startBB) {
  //     auto l = new Instruction_label(new Label(":" + LL + "_label_" + to_string(instruction_index)));
  //     new_instructions.push_back(l);
  //     startBB = false;
  //   }
  //   new_instructions.push_back(i);
  //   return;
  // }

  // void BBGenerator::visit(Instruction_assignment_op *i) {
  //   if (startBB) {
  //     auto l = new Instruction_label(new Label(":" + LL + "_label_" + to_string(instruction_index)));
  //     new_instructions.push_back(l);
  //     startBB = false;
  //   }
  //   new_instructions.push_back(i);
  //   return;
  // }

  // void BBGenerator::visit(Instruction_read_tensor *i) {
  //   if (startBB) {
  //     auto l = new Instruction_label(new Label(":" + LL + "_label_" + to_string(instruction_index)));
  //     new_instructions.push_back(l);
  //     startBB = false;
  //   }
  //   new_instructions.push_back(i);
  //   return;
  // }

  // void BBGenerator::visit(Instruction_write_tensor *i) {
  //   if (startBB) {
  //     auto l = new Instruction_label(new Label(":" + LL + "_label_" + to_string(instruction_index)));
  //     new_instructions.push_back(l);
  //     startBB = false;
  //   }
  //   new_instructions.push_back(i);
  //   return;
  // }

  // void BBGenerator::visit(Instruction_length *i) {
  //   if (startBB) {
  //     auto l = new Instruction_label(new Label(":" + LL + "_label_" + to_string(instruction_index)));
  //     new_instructions.push_back(l);
  //     startBB = false;
  //   }
  //   new_instructions.push_back(i);
  //   return;
  // }

  // void BBGenerator::visit(Instruction_new_array *i) {
  //   if (startBB) {
  //     auto l = new Instruction_label(new Label(":" + LL + "_label_" + to_string(instruction_index)));
  //     new_instructions.push_back(l);
  //     startBB = false;
  //   }
  //   new_instructions.push_back(i);
  //   return;
  // }

  // void BBGenerator::visit(Instruction_new_tuple *i) {
  //   if (startBB) {
  //     auto l = new Instruction_label(new Label(":" + LL + "_label_" + to_string(instruction_index)));
  //     new_instructions.push_back(l);
  //     startBB = false;
  //   }
  //   new_instructions.push_back(i);
  //   return;
  // }

  // void BBGenerator::visit(Instruction_return *i) {
  //   if (startBB) {
  //     auto l = new Instruction_label(new Label(":" + LL + "_label_" + to_string(instruction_index)));
  //     new_instructions.push_back(l);
  //     startBB = false;
  //   }
  //   new_instructions.push_back(i);
  //   startBB = true;
  //   return;
  // }

  // void BBGenerator::visit(Instruction_return_arg *i) {
  //   if (startBB) {
  //     auto l = new Instruction_label(new Label(":" + LL + "_label_" + to_string(instruction_index)));
  //     new_instructions.push_back(l);
  //     startBB = false;
  //   }
  //   new_instructions.push_back(i);
  //   startBB = true;
  //   return;
  // }

  // void BBGenerator::visit(Instruction_branch *i) {
  //   if (startBB) {
  //     auto l = new Instruction_label(new Label(":" + LL + "_label_" + to_string(instruction_index)));
  //     new_instructions.push_back(l);
  //     startBB = false;
  //   }
  //   new_instructions.push_back(i);
  //   startBB = true;
  //   return;
  // }

  // void BBGenerator::visit(Instruction_branch_conditional *i) {
  //   if (startBB) {
  //     auto l = new Instruction_label(new Label(":" + LL + "_label_" + to_string(instruction_index)));
  //     new_instructions.push_back(l);
  //     startBB = false;
  //   }
  //   new_instructions.push_back(i);
  //   startBB = true;
  //   return;
  // }

  // void BBGenerator::visit(Instruction_call *i) {
  //   if (startBB) {
  //     auto l = new Instruction_label(new Label(":" + LL + "_label_" + to_string(instruction_index)));
  //     new_instructions.push_back(l);
  //     startBB = false;
  //   }
  //   new_instructions.push_back(i);
  //   return;
  // }

  // void BBGenerator::visit(Instruction_assignment_call *i) {
  //   if (startBB) {
  //     auto l = new Instruction_label(new Label(":" + LL + "_label_" + to_string(instruction_index)));
  //     new_instructions.push_back(l);
  //     startBB = false;
  //   }
  //   new_instructions.push_back(i);
  //   return;
  // }

  // void BBGenerator::visit(Instruction_label *i) {
  //   if (!startBB) {
  //     auto g = new Instruction_branch(i->get_label());
  //     new_instructions.push_back(g);
  //   }
  //   startBB = false;
  //   new_instructions.push_back(i);
  //   return;
  // }

  // void CodeGenerator::visit(Instruction_declaration *i) {
  //   code += i->get_type()->to_string() + " " + i->get_var()->to_string() + "\n"; 
  //   return;
  // }

  // void CodeGenerator::visit(Instruction_assignment *i) {
  //   auto source = i->getSource()->to_string();
  //   if (source.at(0) == '%') {
  //     source.erase(0, 1);
  //     if (std::find(function_names.begin(), function_names.end(), source) != function_names.end()) {
  //       source.insert(0, ":");
  //     } else {
  //       source = i->getSource()->to_string();
  //     }
  //   }
  //   code += i->getDestination()->to_string() + " <- " + source + "\n";
  //   return;
  // }

  // void CodeGenerator::visit(Instruction_assignment_op *i) {
  //   code += i->getDestination()->to_string() + " <- " + i->getSource1()->to_string() + " " + i->getOperator()->to_string() + " " + i->getSource2()->to_string() + "\n";
  //   return;
  // }

  

  // void CodeGenerator::visit(Instruction_read_tensor *i) {
  //   code += i->getDestination()->to_string() + " <- " + i->getSource()->to_string();
  //   for (auto elem : i->getIndices()) {
  //     code += "[" + elem->to_string() + "]";
  //   }
  //   code += "\n";
  //   return;
  // }

  // void CodeGenerator::visit(Instruction_write_tensor *i) {
  //   code += i->getDestination()->to_string();
  //   for (auto elem : i->getIndices()) {
  //     code += "[" + elem->to_string() + "]";
  //   }
  //   code += " <- " + i->getSource()->to_string() + "\n";
  //   return;
  // }

  // void CodeGenerator::visit(Instruction_length *i) {
  //   code += i->getDestination()->to_string() + " <- length " + i->getSource()->to_string() + " " + i->getIndex()->to_string() + "\n";
  //   return;
  // }

  // void CodeGenerator::visit(Instruction_new_array *i) {
  //   code += i->getDestination()->to_string() + " <- new Array(" + i->getSizes()->to_string() + ")\n";
  //   return;
  // }

  // void CodeGenerator::visit(Instruction_new_tuple *i) {
  //   code += i->getDestination()->to_string() + " <- new Tuple(" + i->getSize()->to_string() + ")\n";
  //   return;
  // }

  // void CodeGenerator::visit(Instruction_return *i) {
  //   code += "return\n";
  //   return;
  // }

  // void CodeGenerator::visit(Instruction_return_arg *i) {
  //   code += "return " + i->get_value()->to_string() + "\n";
  //   return;
  // }

  // void CodeGenerator::visit(Instruction_branch *i) {
  //   code += "br "+ i->getDestination()->to_string()+"\n";
  //   return;
  // }

  // void CodeGenerator::visit(Instruction_branch_conditional *i) {
  //   code += "br " + i->getCondition()->to_string() + " " + i->getDestination1()->to_string() + " " + i->getDestination2()->to_string() + "\n";
  //   return;
  // }

  // string CodeGenerator::clean_func_name(string func_name) {
  //   string clean_name = func_name;
  //   clean_name.erase(0, 1);
  //   if (std::find(function_names.begin(), function_names.end(), clean_name) != function_names.end()) {
  //     func_name.at(0) = '@';
  //   } 
  //   if (func_name == "%print") {
  //     func_name = "print";
  //   } else if (func_name == "%tensor-error") {
  //     func_name = "tensor-error";
  //   } else if (func_name == "%input") {
  //     func_name = "input";
  //   }
  //   return func_name;
  // }

  // void CodeGenerator::visit(Instruction_call *i) {
  //   string func_name = i->getCallee()->to_string();
  //   func_name = clean_func_name(func_name);
  //   code += "call " + func_name + "(" + i->getArgs()->to_string() + ")\n";
  //   return;
  // }

  // void CodeGenerator::visit(Instruction_assignment_call *i) {
  //   string func_name = i->getCallee()->to_string();
  //   func_name = clean_func_name(func_name);
  //   code += i->getDst()->to_string() + " <- call " + func_name + "(" + i->getArgs()->to_string() + ")\n";
  //   return;
  // }

  // void CodeGenerator::visit(Instruction_label *i) {
  //   code += i->get_label()->to_string()+"\n";
  //   return;
  // }
}
