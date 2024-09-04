#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <deque>

#include <code_generator.h>

using namespace std;


namespace IR {
  void printg(string output) {
    cout << output << "\n";
  }

  deque<Basic_block*> allBlocks;
  vector<Basic_block*> allBlocksForever;
  map<Basic_block*,bool> isMarked;


  // ItemLooker looker;
  // CodeGenerator cglooker;
  string myOutput;
//   string LL;
//   int64_t LL_counter;
//   int64_t ret_add_counter;
//   map<string,int> labelMap;
//   bool printAll=false;
//   vector<Instruction*> currCont;

  Trace::Trace(){
    return;
  }

  void Trace::append(Basic_block* block){
    //printg("Started Append");
   trace_blocks.push_back(block); 
   //printg("Passed Append");
  }

  int Trace::size(){
    return trace_blocks.size();
  }

  vector<Basic_block*> getSuccessors(Basic_block* block){
    Instruction* terminator=block->terminator;
    vector<Basic_block*> succs;
    //printg("MADE IT IN SUCC");
    auto branch = dynamic_cast<Instruction_branch*>(terminator);
    if(branch!=nullptr){
      //printg("DID BRANCH");
      succs.push_back(getBlockFromLabel(branch->getDestination()));
      //return succs;
    }
    auto con_branch = dynamic_cast<Instruction_branch_conditional*>(terminator);
    if(con_branch!=nullptr){
      //printg("DID CON BRANCH");
      succs.push_back(getBlockFromLabel(con_branch->getDestination1()));
      succs.push_back(getBlockFromLabel(con_branch->getDestination2()));
      //return succs;
    }
    //printg("RETURNED SOMETHING");
    return succs;
  }

  Basic_block* getBlockFromLabel(Item* label){
    Basic_block* block_return;
    string lookFor=dynamic_cast<Label*>(label)->get_name();
    for(auto block : allBlocksForever){
      //printg("Seached for Label");
      auto found=dynamic_cast<Instruction_label*>(block->label);
      //if(found==nullptr){printg("fuck");}
      string l=dynamic_cast<Label*>(found->get_label())->get_name();
      //printg("Got currentLabel");
      if(l==lookFor){
        //printg("Found my label");
        block_return=block;
      }
    }
    return block_return;
  }

  string getEnding(Instruction* terminator){
    auto con=dynamic_cast<Instruction_branch*>(terminator);
    auto con_branch=dynamic_cast<Instruction_branch_conditional*>(terminator);
    if(con!=nullptr){return con->getDestination()->to_string()+"\n";}
    else if(con!=nullptr){return con_branch->getDestination2()->to_string()+"\n";}
    else{return "\n";}
  }

  void generate_code(Program p){
    
  //printg("Is this running?");
//     /* 
//      * Open the output file.
//      */ 
    std::ofstream outputFile;
    outputFile.open("prog.L3");
    //printg(to_string(p.functions.size()));
   
//     
//       printg("Start code_generator");
//       printg("LL is " + p.longest_label);
//     }
//     LL = p.longest_label;
//     LL += "_global_";
//     LL_counter = 0;
//     ret_add_counter=0;
/*
  printg(to_string(p.functions.size()));
  for (auto f : p.functions) {
    printg(f->name);
    for (auto b : f->blocks) {
      printg("LABEL");
      b->label->print_instruction();
      printg("INSTRUCTION");
      for (auto i : b->instructions) {
        i->print_instruction();
      }
      printg("TERMINATOR");
      b->terminator->print_instruction();
      printg("LITERAL TERMINATOR: "+getEnding(b->terminator));
    }
  }
  */
  
  


  //myOutput += "define\n";
    
//     for (auto f : p.functions) {
//         generate_contexts(f,LL);
//     }
//update
for(auto f :p.functions){
  for(int i=f->blocks.size()-1;i>=0;i--){
    allBlocks.push_front(f->blocks[i]);
    allBlocksForever.push_back(f->blocks[i]);
  }
  //printg("Doing Function Setup");
  auto args=dynamic_cast<Vars*>(f->arguments);
  if(args==nullptr){printg("Fuck");}
  auto array=args->getVars();
  
  myOutput+="define "+f->name+"( ";
  string type="";
  auto vars = dynamic_cast<Vars*>(f->arguments)->getVars();
  //printg("got vars");
  //printg("var size:"+to_string(vars.size()));
  for (int i = 0; i < vars.size(); i += 2) {
    auto t = vars[i]->to_string();
    auto v = vars[i+1]->to_string();
    addType(v,t);
  }
  //printg("all vars loaded");
  for(auto arg : dynamic_cast<Vars*>(f->arguments)->getVars()){
    if (arg->to_string().front()=='%'){
      myOutput+=arg->to_string()+",";
      //types.insert({var->to_string(), type->to_string()})
    }
  }
  //printg("funciton name set up");
  myOutput.erase(myOutput.size()-1);
  myOutput+=" ){\n";
  for (auto block : allBlocks){
    isMarked[block]=false;
  }
  //printg("Finished Function Setup");
  vector<Trace*> allTrace;
  //printg("Searching for Trace");
  while(allBlocks.size()!=0){
    //printg("Entering loop with:"+ to_string(allBlocks.size()));
    Trace* tr = new Trace();
    Basic_block* current=allBlocks.front();
    vector<Basic_block*> currentQueue;
    currentQueue.push_back(current);
    //bool first=true;
    //printg("Current: ");
    //current->label->print_instruction();
    while(currentQueue.size()!=0){
      //printg("Made it in");
      current=currentQueue.back();
      currentQueue.pop_back();
      //first=false;
      //current->label->print_instruction();
      if(!isMarked[current]){
        isMarked[current]=true;
        //printg(":)");
        tr->append(current);
        //printg("Appended!");
        vector<Basic_block*> successors=getSuccessors(current);
        //printg("Found successors:"+to_string(successors.size()));
        for(auto c : successors){
          if(!isMarked[c]){
            currentQueue.push_back(c);
          } 
        }
      }
    }
    allTrace.push_back(tr);
    allBlocks.pop_front();
  }//while(allBlocks.size!=0);
  //printg("Found Traces");
  Trace* bestTrace=allTrace.front();
  int bestSize=allTrace.front()->size();
  for (auto curTrace : allTrace){
    if(curTrace->size()>bestSize){
      bestTrace=curTrace;
      bestSize=curTrace->size();
    }
  }
  //Now we have the best Trace
  ///printg("Found Best Trace");
  //printg("Trace Size: "+to_string(bestSize));
  bool skipFirst=true;
  string label_skip="";
  for (int i=0;i<bestSize;i++){
    auto b = bestTrace->trace_blocks[i];
    //printg("GOT IT");
    //if(skipFirst){skipFirst=false;}
    //b->label->print_instruction();
    myOutput+="\n";
    string maybeLabel=b->label->instruction_to_L3();
    //if(skipFirst){skipFirst=false;}
    //else{myOutput+=maybeLabel;}
    myOutput+=maybeLabel;
    //else if(maybeLabel!=label_skip){myOutput+=maybeLabel;}
    for (auto i : b->instructions) {
      //i->print_instruction();
      myOutput+=i->instruction_to_L3();
    }
    //printg("GOT IT");
    //b->terminator->print_instruction();
    string maybeOutput2=b->terminator->instruction_to_L3();
    label_skip=getEnding(b->terminator);
    if(i<bestTrace->trace_blocks.size()-1){ //Is not the last one
      auto nextLabel=bestTrace->trace_blocks[i+1]->label->instruction_to_L3();
      //printg(label_skip+" ? "+nextLabel);
      //printg(nextLabel);
      if(label_skip!=nextLabel){
        myOutput+=maybeOutput2;
      }
      else{
        //printg("THIS RAN");
      }
    }
    else{
      myOutput+=maybeOutput2;
    }
    //printg("GOT IT");
    //output+=maybeOutput;
    //label_skip=getEnding(b->terminator);
  }
   myOutput += "}\n";
}

//myOutput += ")\n";    
//printg(myOutput);
outputFile<<myOutput;
outputFile.close();
 
/*
printg(to_string(p.functions.size()));
    for (auto f : p.functions) {
      cout << f->name << f->arguments->to_string() << "\n";
      for (auto b : f->blocks) {
        b->label->print_instruction();
        for (auto i : b->instructions) {
          i->print_instruction();
        }
        b->terminator->print_instruction();
      }
    }
    */

    
//       refreshLabelMap();
//       printg("New function");}
//       myOutput += "    (" + f->name + "\n";
//       // printg("Got name");
//       // printg(f->arguments->to_string());
//       // auto a = f->arguments->accept(&looker);
//       // printg("A");
//       auto args = f->arguments->accept(&looker);
//       myOutput += "    " + args.front() + "\n";
//       printg("Got num args");}

//       // Get args
      

//       for (int arg_index = stoi(args.front()); arg_index > 0; arg_index--) {
//       printg("index is " + to_string(arg_index));}
//       if (arg_index == 1) {
//         myOutput += "    " + args[1] + " <- rdi\n";
//       } else if (arg_index == 2) {
//         myOutput += "    " + args[2] + " <- rsi\n";
//       } else if (arg_index == 3) {
//         myOutput += "    " + args[3] + " <- rdx\n";
//       } else if (arg_index == 4) {
//         myOutput += "    " + args[4] + " <- rcx\n";
//       } else if (arg_index == 5) {
//         myOutput += "    " + args[5] + " <- r8\n";
//       } else if (arg_index == 6) {
//         myOutput += "    " + args[6] + " <- r9\n";
//       } else {
//         // stack-args?
//         //printg(to_string(arg_index-6));
//         myOutput += "    " + args[stoi(args.front()) - (arg_index-7)] + " <- stack-arg " + to_string((arg_index-7) * 8) + "\n";
//       }
//     }


//       for (auto i : f->instructions) {
//         i->accept(&cglooker);
//       }

//       myOutput += "    )\n";
//     }

//     myOutput += ")\n";
    
//     //printg(myOutput);
//     outputFile<<myOutput;
//     outputFile.close();
   
    return ;
  }

  // // void release(){
  // //   //printe("Called Release");
  // //   myOutput+=generateOneContext(currCont,LL);
  // //   //printe("DID RELEASE");
  // //   currCont.clear();
  // //   return;
  // // }

  vector<string> ItemLooker::visit(Number *i) {
    vector<string> item_string;
    printg("Number");
    item_string.push_back(to_string(i->get_number()));
    return item_string;
  }

  vector<string> ItemLooker::visit(Label *i) {
    vector<string> item_string;
    printg("Label");
    item_string.push_back(i->get_name());
    return item_string;
  }

  vector<string> ItemLooker::visit(L *i) {
    vector<string> item_string;
    printg("L");
    item_string.push_back(i->get_name());
    return item_string;
  }

  vector<string> ItemLooker::visit(Variable *i) {
    vector<string> item_string;
    printg("Variable");
    item_string.push_back(i->get_name());
    return item_string;
  }

  vector<string> ItemLooker::visit(Type *i) {
    vector<string> item_string;
    // printg("Operator");
    // item_string.push_back(to_string(i->get_op()));
    return item_string;
  }

  vector<string> ItemLooker::visit(Operator *i) {
    vector<string> item_string;
    printg("Operator");
    item_string.push_back(to_string(i->get_op()));
    return item_string;
  }

  vector<string> ItemLooker::visit(Vars *i) {
    vector<string> item_string;
    printg("new vars");
    // item_string.push_back(to_string(i->getNumVars()));
    // for (auto arg : i->getVars()) {
    //   string arg_string = arg->accept(&looker).front();
    //   item_string.push_back(arg_string);
    // }
    return item_string;
  }

  vector<string> ItemLooker::visit(Args *i) {
    vector<string> item_string;
    printg("Args");
    // item_string.push_back(to_string(i->getNumArgs()));
    // for (auto arg : i->getArgs()) {
    //   string arg_string = arg->accept(&looker).front();
    //   item_string.push_back(arg_string);
    // }

    return item_string;
  }

  void CodeGenerator::visit(Instruction_declaration *i) {
    return;
  }

  void CodeGenerator::visit(Instruction_assignment *i) {
    // vector<string> item_string;
    // printg("Assignment");}
    // currCont.push_back(i);
    return;
  }

  void CodeGenerator::visit(Instruction_assignment_op *i) {
    // vector<string> item_string;
    // printg("Assignment op");}
    // currCont.push_back(i);
    return;
  }

  void CodeGenerator::visit(Instruction_read_tensor *i) {
    return;
  }

  void CodeGenerator::visit(Instruction_write_tensor *i) {
    return;
  }

  void CodeGenerator::visit(Instruction_length *i) {
    return;
  }

  void CodeGenerator::visit(Instruction_new_array *i) {
    return;
  }

  void CodeGenerator::visit(Instruction_new_tuple *i) {
    return;
  }

  void CodeGenerator::visit(Instruction_return *i) {
    
    // printg("return");}
    // currCont.push_back(i);
    // //RELEASE
    // release();
    // //myOutput += "    return\n";
    return;
  }

  void CodeGenerator::visit(Instruction_return_arg *i) {
    // vector<string> item_string;
    // printg("return arg");}
    // currCont.push_back(i);
    // release();
    // //myOutput += "    rax <- " + i->get_value()->accept(&looker).front() + "\n";
    // //myOutput += "    return\n";
    return;
  }

  void CodeGenerator::visit(Instruction_branch *i) {
    // vector<string> item_string;
    // printg("branch");}
    // currCont.push_back(i);
    // release();
    return;
  }

  void CodeGenerator::visit(Instruction_branch_conditional *i) {
    // vector<string> item_string;
    // printg("branch conditional");}
    // currCont.push_back(i);
    // release();
    return;
  }

  void CodeGenerator::visit(Instruction_call *i) {
    // vector<string> item_string;
    // bool putBack=false;
    // bool putBackPer=false;
    // printg("call");}
    // //printg(myOutput+to_string(currCont.size()));
    // currCont.push_back(i);
    // release();
    // //printg(myOutput+to_string(currCont.size()));
    // Item* args_items = i->getArgs();
    // auto args = args_items->accept(&looker);

    // Item* return_item = i->getCallee();//what is this
    // string function_name = return_item->accept(&looker).front();
    // string return_label;
    // bool ingrainedFunction=false;
    // if (function_name=="tensor-error"){
    //   return_label = ":tensor_error"+to_string(ret_add_counter)+"_ret";
    // }
    // else if(function_name.at(0)=='@'){
    //   function_name.erase(0,1);
    //   putBack=true;
    //   return_label = ":"+function_name+to_string(ret_add_counter)+"_ret";
    // }
    // else if(function_name.at(0)=='%'){
    //   function_name.erase(0,1);
    //   putBackPer=true;
    //   return_label = ":"+function_name+to_string(ret_add_counter)+"_ret";
    // }
    // else{
    // return_label = ":"+function_name+to_string(ret_add_counter)+ "_ret";
    // }
    
    // myOutput += "    mem rsp -8 <- " + return_label + "\n";
    // // Args
    // for (int arg_index = stoi(args.front()); arg_index > 0; arg_index--) {
    //   printg("index is " + to_string(arg_index));}
    //   if (arg_index == 1) {
    //     myOutput += "    rdi <- " + args[1] + "\n";
    //   } else if (arg_index == 2) {
    //     myOutput += "    rsi <- " + args[2] + "\n";
    //   } else if (arg_index == 3) {
    //     myOutput += "    rdx <- " + args[3] + "\n";
    //   } else if (arg_index == 4) {
    //     myOutput += "    rcx <- " + args[4] + "\n";
    //   } else if (arg_index == 5) {
    //     myOutput += "    r8 <- " + args[5] + "\n";
    //   } else if (arg_index == 6) {
    //     myOutput += "    r9 <- " + args[6] + "\n";
    //   } else {
    //     myOutput += "    mem rsp -" + to_string((arg_index-5) * 8) + " <- " + args[arg_index] + "\n";
    //   }
    // }

    // myOutput += "    call "; 
    // if(putBack){myOutput+="@";}
    // if(putBackPer){myOutput+="%";}
    // myOutput += function_name +  " " + args.front() + "\n";
    // myOutput += "    " + return_label + "\n";
    // ret_add_counter+=1;
    return;
  }

  void CodeGenerator::visit(Instruction_assignment_call *i) {
    // vector<string> item_string;
    // printg("Assignment call");}
    // currCont.push_back(i);
    // bool putBack=false;
    // bool putBackPer=false;
    // release();

    // Item* args_items = i->getArgs();
    // auto args = args_items->accept(&looker);

    // Item* return_item = i->getCallee();
    // string function_name = return_item->accept(&looker).front();
    // //printg(function_name);
    // string return_label;
    // string dst = i->getDst()->accept(&looker).front();
    // bool ingrainedFunction=false;
    // if (function_name=="tensor-error"){
    //   return_label = ":tensor_error"+to_string(ret_add_counter)+"_ret";
    // }
    // else if(function_name.at(0)=='@' ){
    //   function_name.erase(0,1);
    //   putBack=true;
    //   return_label = ":"+function_name+to_string(ret_add_counter)+"_ret";
    // }
    // else if(function_name.at(0)=='%'){
    //   function_name.erase(0,1);
    //   putBackPer=true;
    //   return_label = ":"+function_name+to_string(ret_add_counter)+"_ret";
    // }
    // else{
    // return_label = ":"+function_name+to_string(ret_add_counter)+ "_ret";
    // }
    // myOutput += "    mem rsp -8 <- " + return_label + "\n";
    // // Args
    // for (int arg_index = stoi(args.front()); arg_index > 0; arg_index--) {
    //   printg("index is " + to_string(arg_index));}
    //   if (arg_index == 1) {
    //     myOutput += "    rdi <- " + args[1] + "\n";
    //   } else if (arg_index == 2) {
    //     myOutput += "    rsi <- " + args[2] + "\n";
    //   } else if (arg_index == 3) {
    //     myOutput += "    rdx <- " + args[3] + "\n";
    //   } else if (arg_index == 4) {
    //     myOutput += "    rcx <- " + args[4] + "\n";
    //   } else if (arg_index == 5) {
    //     myOutput += "    r8 <- " + args[5] + "\n";
    //   } else if (arg_index == 6) {
    //     myOutput += "    r9 <- " + args[6] + "\n";
    //   } else {
    //     myOutput += "    mem rsp -" + to_string((arg_index-5) * 8) + " <- " + args[arg_index] + "\n";
    //   }
    // }

    // myOutput += "    call ";
    // if(putBack){myOutput+="@";}
    // if(putBackPer){myOutput+="%";}
    // myOutput += function_name +  " " + args.front() + "\n";
    // myOutput += "    " + return_label + "\n";
    // myOutput += "    " + dst + " <- rax\n";
    // ret_add_counter+=1;
    return;
  }

  void CodeGenerator::visit(Instruction_label *i) {
    // vector<string> item_string;
    // printg("label");}
    // currCont.push_back(i);
    // release();
    // string label=i->get_label()->accept(&looker).front();
    // int num=UseLabelMap(label);
    


    // myOutput += "    " + LL + to_string(num) + "\n";

    
    return;
  }
}
