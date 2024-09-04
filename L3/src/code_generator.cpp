#include <string>
#include <iostream>
#include <fstream>
#include <map>

#include <code_generator.h>
#include <instruction_selection.h>

using namespace std;

namespace L3{
  void printg(string output) {
    cout << output << "\n";
  }
/*
  LabelMapper::LabelMapper(string suf){

  }
  */

  ItemLooker looker;
  CodeGenerator cglooker;
  string myOutput;
  string LL;
  int64_t LL_counter;
  int64_t ret_add_counter;
  map<string,int> labelMap;
  bool printAll=false;
  vector<Instruction*> currCont;

  void generate_code(Program p){
    //printg("Is this running?");
    /* 
     * Open the output file.
     */ 
    std::ofstream outputFile;
    outputFile.open("prog.L2");
   
    if(printAll){
      printg("Start code_generator");
      printg("LL is " + p.longest_label);
    }
    LL = p.longest_label;
    LL += "_global_";
    LL_counter = 0;
    ret_add_counter=0;
    
    myOutput += "(@main\n";
    
    for (auto f : p.functions) {
        generate_contexts(f,LL);
    }
    


    for (auto f : p.functions) {
      refreshLabelMap();
      if(printAll){printg("New function");}
      myOutput += "    (" + f->name + "\n";
      // printg("Got name");
      // printg(f->arguments->to_string());
      // auto a = f->arguments->accept(&looker);
      // printg("A");
      auto args = f->arguments->accept(&looker);
      myOutput += "    " + args.front() + "\n";
      if(printAll){printg("Got num args");}

      // Get args
      

      for (int arg_index = stoi(args.front()); arg_index > 0; arg_index--) {
      if(printAll){printg("index is " + to_string(arg_index));}
      if (arg_index == 1) {
        myOutput += "    " + args[1] + " <- rdi\n";
      } else if (arg_index == 2) {
        myOutput += "    " + args[2] + " <- rsi\n";
      } else if (arg_index == 3) {
        myOutput += "    " + args[3] + " <- rdx\n";
      } else if (arg_index == 4) {
        myOutput += "    " + args[4] + " <- rcx\n";
      } else if (arg_index == 5) {
        myOutput += "    " + args[5] + " <- r8\n";
      } else if (arg_index == 6) {
        myOutput += "    " + args[6] + " <- r9\n";
      } else {
        // stack-args?
        //printg(to_string(arg_index-6));
        myOutput += "    " + args[stoi(args.front()) - (arg_index-7)] + " <- stack-arg " + to_string((arg_index-7) * 8) + "\n";
      }
    }


      for (auto i : f->instructions) {
        i->accept(&cglooker);
      }

      myOutput += "    )\n";
    }

    myOutput += ")\n";
    
    //printg(myOutput);
    outputFile<<myOutput;
    outputFile.close();
   
    return ;
  }

  void release(){
    //printe("Called Release");
    myOutput+=generateOneContext(currCont,LL);
    //printe("DID RELEASE");
    currCont.clear();
    return;
  }

  vector<string> ItemLooker::visit(Number *i) {
    vector<string> item_string;
    if(printAll){printg("Number");}
    item_string.push_back(to_string(i->get_number()));
    return item_string;
  }

  vector<string> ItemLooker::visit(Label *i) {
    vector<string> item_string;
    if(printAll){printg("Label");}
    item_string.push_back(i->get_name());
    return item_string;
  }

  vector<string> ItemLooker::visit(L *i) {
    vector<string> item_string;
    if(printAll){printg("L");}
    item_string.push_back(i->get_name());
    return item_string;
  }

  vector<string> ItemLooker::visit(Variable *i) {
    vector<string> item_string;
    if(printAll){printg("Variable");}
    item_string.push_back(i->get_name());
    return item_string;
  }

  vector<string> ItemLooker::visit(Compare_Operator *i) {
    vector<string> item_string;
    if(printAll){printg("Compare_Operator");}
    item_string.push_back(to_string(i->get_op()));
    return item_string;
  }

  vector<string> ItemLooker::visit(Arithmetic_Operator *i) {
    vector<string> item_string;
    if(printAll){printg("Arithmetic_Operator");}
    item_string.push_back(to_string(i->get_op()));
    return item_string;
  }

  vector<string> ItemLooker::visit(Vars *i) {
    vector<string> item_string;
    if(printAll){printg("new vars");}
    item_string.push_back(to_string(i->getNumVars()));
    for (auto arg : i->getVars()) {
      string arg_string = arg->accept(&looker).front();
      item_string.push_back(arg_string);
    }
    return item_string;
  }

  vector<string> ItemLooker::visit(Args *i) {
    vector<string> item_string;
    if(printAll){printg("Args");}
    item_string.push_back(to_string(i->getNumArgs()));
    for (auto arg : i->getArgs()) {
      string arg_string = arg->accept(&looker).front();
      item_string.push_back(arg_string);
    }

    return item_string;
  }


  void CodeGenerator::visit(Instruction_assignment *i) {
    vector<string> item_string;
    if(printAll){printg("Assignment");}
    currCont.push_back(i);
    return;
  }

  void CodeGenerator::visit(Instruction_assignment_op *i) {
    vector<string> item_string;
    if(printAll){printg("Assignment op");}
    currCont.push_back(i);
    return;
  }

  void CodeGenerator::visit(Instruction_assignment_cmp *i) {
    vector<string> item_string;
    if(printAll){printg("Assignment cmp");}
    currCont.push_back(i);
    return;
  }

  void CodeGenerator::visit(Instruction_return *i) {
    
    if(printAll){printg("return");}
    currCont.push_back(i);
    //RELEASE
    release();
    //myOutput += "    return\n";
    return;
  }

  void CodeGenerator::visit(Instruction_return_arg *i) {
    vector<string> item_string;
    if(printAll){printg("return arg");}
    currCont.push_back(i);
    release();
    //myOutput += "    rax <- " + i->get_value()->accept(&looker).front() + "\n";
    //myOutput += "    return\n";
    return;
  }

  void CodeGenerator::visit(Instruction_load *i) {
    vector<string> item_string;
    if(printAll){printg("load");}
    currCont.push_back(i);
    return;
  }

  void CodeGenerator::visit(Instruction_store *i) {
    vector<string> item_string;
    if(printAll){printg("store");}
    currCont.push_back(i);
    return;
  }

  void CodeGenerator::visit(Instruction_branch *i) {
    vector<string> item_string;
    if(printAll){printg("branch");}
    currCont.push_back(i);
    release();
    return;
  }

  void CodeGenerator::visit(Instruction_branch_conditional *i) {
    vector<string> item_string;
    if(printAll){printg("branch conditional");}
    currCont.push_back(i);
    release();
    return;
  }

  void CodeGenerator::visit(Instruction_call *i) {
    vector<string> item_string;
    bool putBack=false;
    bool putBackPer=false;
    if(printAll){printg("call");}
    //printg(myOutput+to_string(currCont.size()));
    currCont.push_back(i);
    release();
    //printg(myOutput+to_string(currCont.size()));
    Item* args_items = i->getArgs();
    auto args = args_items->accept(&looker);

    Item* return_item = i->getCallee();//what is this
    string function_name = return_item->accept(&looker).front();
    string return_label;
    bool ingrainedFunction=false;
    if (function_name=="tensor-error"){
      return_label = ":tensor_error"+to_string(ret_add_counter)+"_ret";
    }
    else if(function_name.at(0)=='@'){
      function_name.erase(0,1);
      putBack=true;
      return_label = ":"+function_name+to_string(ret_add_counter)+"_ret";
    }
    else if(function_name.at(0)=='%'){
      function_name.erase(0,1);
      putBackPer=true;
      return_label = ":"+function_name+to_string(ret_add_counter)+"_ret";
    }
    else{
    return_label = ":"+function_name+to_string(ret_add_counter)+ "_ret";
    }
    
    myOutput += "    mem rsp -8 <- " + return_label + "\n";
    // Args
    for (int arg_index = stoi(args.front()); arg_index > 0; arg_index--) {
      if(printAll){printg("index is " + to_string(arg_index));}
      if (arg_index == 1) {
        myOutput += "    rdi <- " + args[1] + "\n";
      } else if (arg_index == 2) {
        myOutput += "    rsi <- " + args[2] + "\n";
      } else if (arg_index == 3) {
        myOutput += "    rdx <- " + args[3] + "\n";
      } else if (arg_index == 4) {
        myOutput += "    rcx <- " + args[4] + "\n";
      } else if (arg_index == 5) {
        myOutput += "    r8 <- " + args[5] + "\n";
      } else if (arg_index == 6) {
        myOutput += "    r9 <- " + args[6] + "\n";
      } else {
        myOutput += "    mem rsp -" + to_string((arg_index-5) * 8) + " <- " + args[arg_index] + "\n";
      }
    }

    myOutput += "    call "; 
    if(putBack){myOutput+="@";}
    if(putBackPer){myOutput+="%";}
    myOutput += function_name +  " " + args.front() + "\n";
    myOutput += "    " + return_label + "\n";
    ret_add_counter+=1;
    return;
  }

  void CodeGenerator::visit(Instruction_assignment_call *i) {
    vector<string> item_string;
    if(printAll){printg("Assignment call");}
    currCont.push_back(i);
    bool putBack=false;
    bool putBackPer=false;
    release();

    Item* args_items = i->getArgs();
    auto args = args_items->accept(&looker);

    Item* return_item = i->getCallee();
    string function_name = return_item->accept(&looker).front();
    //printg(function_name);
    string return_label;
    string dst = i->getDst()->accept(&looker).front();
    bool ingrainedFunction=false;
    if (function_name=="tensor-error"){
      return_label = ":tensor_error"+to_string(ret_add_counter)+"_ret";
    }
    else if(function_name.at(0)=='@' ){
      function_name.erase(0,1);
      putBack=true;
      return_label = ":"+function_name+to_string(ret_add_counter)+"_ret";
    }
    else if(function_name.at(0)=='%'){
      function_name.erase(0,1);
      putBackPer=true;
      return_label = ":"+function_name+to_string(ret_add_counter)+"_ret";
    }
    else{
    return_label = ":"+function_name+to_string(ret_add_counter)+ "_ret";
    }
    myOutput += "    mem rsp -8 <- " + return_label + "\n";
    // Args
    for (int arg_index = stoi(args.front()); arg_index > 0; arg_index--) {
      if(printAll){printg("index is " + to_string(arg_index));}
      if (arg_index == 1) {
        myOutput += "    rdi <- " + args[1] + "\n";
      } else if (arg_index == 2) {
        myOutput += "    rsi <- " + args[2] + "\n";
      } else if (arg_index == 3) {
        myOutput += "    rdx <- " + args[3] + "\n";
      } else if (arg_index == 4) {
        myOutput += "    rcx <- " + args[4] + "\n";
      } else if (arg_index == 5) {
        myOutput += "    r8 <- " + args[5] + "\n";
      } else if (arg_index == 6) {
        myOutput += "    r9 <- " + args[6] + "\n";
      } else {
        myOutput += "    mem rsp -" + to_string((arg_index-5) * 8) + " <- " + args[arg_index] + "\n";
      }
    }

    myOutput += "    call ";
    if(putBack){myOutput+="@";}
    if(putBackPer){myOutput+="%";}
    myOutput += function_name +  " " + args.front() + "\n";
    myOutput += "    " + return_label + "\n";
    myOutput += "    " + dst + " <- rax\n";
    ret_add_counter+=1;
    return;
  }

  void CodeGenerator::visit(Instruction_label *i) {
    vector<string> item_string;
    if(printAll){printg("label");}
    currCont.push_back(i);
    release();
    string label=i->get_label()->accept(&looker).front();
    int num=UseLabelMap(label);
    


    myOutput += "    " + LL + to_string(num) + "\n";

    
    return;
  }
}
