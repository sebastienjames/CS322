#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <deque>
#include <algorithm>

#include <code_generator.h>

using namespace std;


namespace LA {
  void printg(string output) {
    if (true) {
      cout << output << "\n";
    }
  }

  string myOutput;



  void generate_code(Program p){
    
    std::ofstream outputFile;
    outputFile.open("prog.IR");

    

    // Value encoding 
    
    for (auto f : p.functions) {
      ValueEncoding decode;
      decode.f = f;
      decode.LL = f->longest_name;
      decode.instruction_index = 0;
      while (decode.instruction_index < f->instructions.size()) {
        f->instructions.at(decode.instruction_index)->print_instruction();
        f->instructions.at(decode.instruction_index)->accept(&decode);
        decode.instruction_index++;
      }

      // Variables used by array checking
      decode.newVars.insert(decode.LL + "_LineNumber");
      decode.newVars.insert(decode.LL + "_comp");
      decode.newVars.insert(decode.LL + "_length");
      decode.newVars.insert(decode.LL + "_length_decoded");
      decode.newVars.insert(decode.LL + "_encoded_index");

      for (auto newVar : decode.newVars) {
        auto type = new Type(TypeID::int64, 0);
        auto name = new Name(newVar);
        auto i = new Instruction_declaration(type, name);
        f->instructions.insert(f->instructions.begin(), i);
      }
    }
    // Array checking
    for (auto f : p.functions) {
      ArrayChecking checker;
      checker.instruction_index = 0;
      checker.f = f;
      checker.LL = f->longest_name;
      auto args = dynamic_cast<Vars*> (f->arguments);
      for (int i = 0; i < args->getNumVars(); i += 2) {
        auto type = args->getVars().at(i)->to_string();
        auto var = args->getVars().at(i + 1)->to_string();
        checker.types.insert({var, type});
      }
      
      // checker.types.insert({});
      while (checker.instruction_index < f->instructions.size()) {
        f->instructions.at(checker.instruction_index)->print_instruction();
        f->instructions.at(checker.instruction_index)->accept(&checker);
        checker.instruction_index++;
      }
    }

  
    // Basic block generation

    for (auto f : p.functions) {
      BBGenerator bb;
      bb.startBB = true;
      bb.instruction_index = 0;
      bb.LL = f->longest_name;
      bb.num_declarations = 1;
      while (bb.instruction_index < f->instructions.size()) {
        f->instructions.at(bb.instruction_index)->accept(&bb);
        bb.instruction_index++;
      }


      if (!bb.startBB) {
        if (f->return_type->to_string() == "void") {
          auto r = new Instruction_return();
          bb.new_instructions.push_back(r);
        } else {
          auto r = new Instruction_return_arg(new Number(0));
          bb.new_instructions.push_back(r);
        }
      }
      f->instructions = bb.new_instructions;
    }

    for (auto f : p.functions) {
      myOutput += "define " + f->return_type->to_string() +  " @" + f->name + " (" + f->arguments->to_string() + ") {\n";
      CodeGenerator generator;
      generator.function_names = p.function_names;
      for (auto i : f->instructions) {
        generator.code += "\t";
        i->accept(&generator);
        
      }
      myOutput += generator.code;
      myOutput += "}\n";
    }
    //printg(myOutput);
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
    return item_string;
  }

  vector<string> ItemLooker::visit(Type *i) {
    vector<string> item_string;
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

  string ValueEncoding::addVar(string var) {

    if (var.at(0) == '%') {
      var.erase(0, 1);
      string newVar = LL + var;

      // Add the definition to a set of vars to be added (removes duplicates)
      newVars.insert(newVar);
      
      // Add the decoding
    
      auto dst = new Name(newVar);
      
      auto thing1 = new Name(var);
      auto op = new Operator(OperatorID::sarq);
      auto thing2 = new Number(1);
    
      auto instruction = new Instruction_assignment_op(dst, thing1, op, thing2);
    

      f->instructions.insert(f->instructions.begin() + instruction_index, instruction);
      instruction_index++;

      return newVar;
    } 
    if (var.at(0) == ':') {
      return ":";
    }
    return "";
  }

  void ValueEncoding::visit(Instruction_declaration *i) {
    return;
  }

  void ValueEncoding::visit(Instruction_assignment *i) {
    auto src = i->getSource()->to_string();
    if (src.at(0) != '%') {
      auto newSrc = new Number((stol(src) << 1) + 1);
      f->instructions.at(instruction_index) = new Instruction_assignment(i->getDestination(), newSrc);
    }
    return;
  }

  void ValueEncoding::visit(Instruction_assignment_op *i) {
    auto t1 = i->getSource1()->to_string();
    auto newT1 = addVar(t1);
    auto t2 = i->getSource2()->to_string();
    auto newT2 = addVar(t2);


    if (newT1 != "" && newT2 == "") {
      auto newSource1 = new Name(newT1);
      // auto encoded2 = new Number((stol(i->getSource2()->to_string()) << 1) + 1);
      auto encoded2 = i->getSource2();
      f->instructions.at(instruction_index) = new Instruction_assignment_op(i->getDestination(), newSource1, i->getOperator(), encoded2);
    } else if (newT2 != "" && newT1 == "") {
      auto newSource2 = new Name(newT2);
      // auto encoded1 = new Number((stol(i->getSource1()->to_string()) << 1) + 1);
      auto encoded1 = i->getSource1();
      f->instructions.at(instruction_index) = new Instruction_assignment_op(i->getDestination(), encoded1, i->getOperator(), newSource2);
    } else if (newT1 != "" && newT2 != "") {
      auto newSource1 = new Name(newT1);
      auto newSource2 = new Name(newT2);
      f->instructions.at(instruction_index) = new Instruction_assignment_op(i->getDestination(), newSource1, i->getOperator(), newSource2);
    } else {
      // auto encoded1 = new Number((stol(i->getSource1()->to_string()) << 1) + 1);
      // auto encoded2 = new Number((stol(i->getSource2()->to_string()) << 1) + 1);
      // f->instructions.at(instruction_index) = new Instruction_assignment_op(i->getDestination(), encoded1, i->getOperator(), encoded2);
    }


    // Encode the dest
  
    auto dst = i->getDestination();
    
    auto thing1 = i->getDestination();
    auto op = new Operator(OperatorID::salq);
    auto thing2 = new Number(1);
    auto op2 = new Operator(OperatorID::addq);
  
    auto instruction = new Instruction_assignment_op(dst, thing1, op, thing2);
    auto instruction2 = new Instruction_assignment_op(dst, thing1, op2, thing2);
  

    f->instructions.insert(f->instructions.begin() + instruction_index+1, instruction2);
    f->instructions.insert(f->instructions.begin() + instruction_index+1, instruction);
    instruction_index += 2;


    return;
  }

  void ValueEncoding::visit(Instruction_read_tensor *i) {
    auto indices = i->getIndices();
    for (int i = 0; i < indices.size(); i++) {
      auto vari = indices[i]->to_string();
      auto newVari = addVar(vari);
      if (newVari != "") {
        indices[i] = new Name(newVari);
      }
      // } else {
      //   indices[i] = new Number((stol(vari) << 1) + 1);
      // }
    }

    auto newInstruction = new Instruction_read_tensor(i->getDestination(), i->getSource(), indices);
    newInstruction->line_number = i->line_number;
    f->instructions.at(instruction_index) = newInstruction;
    
    return;
  }

  void ValueEncoding::visit(Instruction_write_tensor *i) {
    auto indices = i->getIndices();
    for (int i = 0; i < indices.size(); i++) {
      auto vari = indices[i]->to_string();
      auto newVari = addVar(vari);
      if (newVari != "") {
        indices[i] = new Name(newVari);
      }
      // } else {
      //   indices[i] = new Number((stol(vari) << 1) + 1);
      // }
    }

    auto src = i->getSource()->to_string();
    auto newSrc = addVar(src);
    if (newSrc == "") {
      auto newInstruction = new Instruction_write_tensor(i->getDestination(), new Number((stol(src) << 1) + 1), indices);
      newInstruction->line_number = i->line_number;
      f->instructions.at(instruction_index) = newInstruction;
    } else {
      auto newInstruction = new Instruction_write_tensor(i->getDestination(), i->getSource(), indices);
      newInstruction->line_number = i->line_number;
      f->instructions.at(instruction_index) = newInstruction;
    }
    return;
  }

  void ValueEncoding::visit(Instruction_length *i) {
    auto var3 = i->getIndex()->to_string();
    auto newVar3 = addVar(var3);
    if (newVar3 != "") {
      auto newIndex = new Name(newVar3);
      f->instructions.at(instruction_index) = new Instruction_length(i->getDestination(), i->getSource(), newIndex);
    }
    return;
  }

  void ValueEncoding::visit(Instruction_new_array *i) {
    auto args = dynamic_cast<Args*> (i->getSizes())->getArgs();
    for (int i = 0; i < args.size(); i++) {
      auto arg = args[i]->to_string();
      if (arg.at(0) != '%') {
        args[i] = new Number((stol(arg) << 1) + 1);
      }
    }
    auto newArgs = new Args(args);
    auto newInstruction = new Instruction_new_array(i->getDestination(), newArgs);
    newInstruction->line_number = i->line_number;
    newInstruction->print_instruction();
    f->instructions.at(instruction_index) = newInstruction;
    return;
    return;
  }

  void ValueEncoding::visit(Instruction_new_tuple *i) {
    auto arg = i->getSize()->to_string();
    if (arg.at(0) != '%') {
      auto newArg = new Number((stol(arg) << 1) + 1);
      auto newInstruction = new Instruction_new_tuple(i->getDestination(), newArg);
      newInstruction->line_number = i->line_number;
      newInstruction->print_instruction();
      f->instructions.at(instruction_index) = newInstruction;
    }
    return;
  }

  void ValueEncoding::visit(Instruction_return *i) {
    
    return;
  }

  void ValueEncoding::visit(Instruction_return_arg *i) {
    if (i->get_value()->to_string().at(0) != '%') {
      auto instr = new Instruction_return_arg(new Number((stol(i->get_value()->to_string()) << 1) + 1));
      f->instructions.at(instruction_index) = instr;
    }
    return;
  }

  void ValueEncoding::visit(Instruction_branch *i) {
    return;
  }

  void ValueEncoding::visit(Instruction_branch_conditional *i) {
    auto t = i->getCondition()->to_string();
    auto newVar = addVar(t);
    if (newVar != "") {
      auto newCondition = new Name(newVar);
      f->instructions.at(instruction_index) = new Instruction_branch_conditional(newCondition, i->getDestination1(), i->getDestination2());
    }
    return;
  }

  void ValueEncoding::visit(Instruction_call *i) {
    auto args = dynamic_cast<Args*> (i->getArgs())->getArgs();
    for (int i = 0; i < args.size(); i++) {
      auto arg = args[i]->to_string();
      if (arg.at(0) != '%') {
        args[i] = new Number((stol(arg) << 1) + 1);
      }
    }
    auto newArgs = new Args(args);
    auto newInstruction = new Instruction_call(i->getCallee(), newArgs);
    newInstruction->line_number = i->line_number;
    newInstruction->print_instruction();
    f->instructions.at(instruction_index) = newInstruction;
    return;
  }

  void ValueEncoding::visit(Instruction_assignment_call *i) {
    auto args = dynamic_cast<Args*> (i->getArgs())->getArgs();
    for (int i = 0; i < args.size(); i++) {
      auto arg = args[i]->to_string();
      if (arg.at(0) != '%') {
        args[i] = new Number((stol(arg) << 1) + 1);
      }
    }
    auto newArgs = new Args(args);
    auto newInstruction = new Instruction_assignment_call(i->getDst(), i->getCallee(), newArgs);
    newInstruction->line_number = i->line_number;
    f->instructions.at(instruction_index) = newInstruction;
    return;
  }

  void ValueEncoding::visit(Instruction_label *i) {

    
    return;
  }


  void ArrayChecking::visit(Instruction_declaration *i) {
    types[i->get_var()->to_string()] = i->get_type()->to_string();
    return;
  }

  void ArrayChecking::visit(Instruction_assignment *i) {
    return;
  }

  void ArrayChecking::visit(Instruction_assignment_op *i) {
    return;
  }

  

  void ArrayChecking::visit(Instruction_read_tensor *i) {
    //printg("start read");
    auto comp = new Name(LL + "_comp");
    auto correct = new Label(":" + LL + "_CORRECT_" + to_string(instruction_index));
    auto error = new Label(":" + LL + "_ERROR_" + to_string(instruction_index));
    auto linenumber = new Name(LL + "_LineNumber");
    //printg("setup");
    auto i1 = new Instruction_assignment(linenumber, new Number((i->line_number << 1) + 1));
    // auto i2 = new Instruction_declaration(new Type(TypeID::int64, 0), newV);
    auto i3 = new Instruction_assignment_op(comp, i->getSource(), new Operator(OperatorID::e), new Number(0));
    auto i4 = new Instruction_branch_conditional(comp, error, correct);
    auto i5 = new Instruction_label(error);
    vector<Item*> new_args;
    new_args.push_back(linenumber);
    auto i6 = new Instruction_call(new Name("tensor-error"), new Args(new_args));
    auto i7 = new Instruction_label(correct);
    f->instructions.insert(f->instructions.begin() + instruction_index, i7);
    // f->instructions.insert(instruction_index, i2);
    f->instructions.insert(f->instructions.begin() + instruction_index, i6);
    f->instructions.insert(f->instructions.begin() + instruction_index, i5);
    f->instructions.insert(f->instructions.begin() + instruction_index, i4);
    f->instructions.insert(f->instructions.begin() + instruction_index, i3);
    f->instructions.insert(f->instructions.begin() + instruction_index, i1);
    instruction_index += 6;
    // Check array access 
    // Check if tuple
    if (types[i->getSource()->to_string()] != "tuple") {
      // Single-dimension array
      if (i->getIndices().size() == 1) {
        auto length = new Name(LL + "_length");
        auto good = new Label(":" + LL + "_GOOD_" + to_string(instruction_index));
        auto bad = new Label(":" + LL + "_BAD_" + to_string(instruction_index));
        Item* encoded_index;
        if (i->getIndices().front()->to_string().at(0) == '%') {
          encoded_index = i->getIndices().front();
        } else {
          encoded_index = new Number((stol(i->getIndices().front()->to_string()) << 1) + 1);
        }

        auto a1 = new Instruction_length(length, i->getSource(), new Number(0));
        auto a2 = new Instruction_assignment_op(comp, encoded_index, new Operator(OperatorID::l), length);
        auto a3 = new Instruction_branch_conditional(comp, good, bad);
        auto a4 = new Instruction_label(bad);
        vector<Item*> new_args2 = {linenumber, length, encoded_index};
        auto a5 = new Instruction_call(new Name("tensor-error"), new Args(new_args2));
        auto a6 = new Instruction_label(good);
        f->instructions.insert(f->instructions.begin() + instruction_index, a6);
        f->instructions.insert(f->instructions.begin() + instruction_index, a5);
        f->instructions.insert(f->instructions.begin() + instruction_index, a4);
        f->instructions.insert(f->instructions.begin() + instruction_index, a3);
        f->instructions.insert(f->instructions.begin() + instruction_index, a2);
        f->instructions.insert(f->instructions.begin() + instruction_index, a1);
        instruction_index += 6;
      } else {
        Instruction* a1;
        Instruction* a2;
        Instruction* a3;
        Instruction* a4;
        Instruction* a5;
        Instruction* a6;
        vector<Item*> new_args2;
        for (int arg_index = 0; arg_index < i->getIndices().size(); arg_index++) {
          //cout << i->getIndices().at(arg_index)->to_string() << "\n";
          auto length = new Name(LL + "_length");
          auto good = new Label(":" + LL + "_GOOD_" + to_string(instruction_index) + "_" + to_string(arg_index));
          auto bad = new Label(":" + LL + "_BAD_" + to_string(instruction_index) + "_" + to_string(arg_index));
          Item* encoded_index;
          if (i->getIndices().at(arg_index)->to_string().at(0) == '%') {
            
            encoded_index = i->getIndices().at(arg_index);
          } else {
            
            encoded_index = new Number((stol(i->getIndices().at(arg_index)->to_string()) << 1) + 1);
          }

          a1 = new Instruction_length(length, i->getSource(), new Number(arg_index));
          a2 = new Instruction_assignment_op(comp, encoded_index, new Operator(OperatorID::l), length);
          a3 = new Instruction_branch_conditional(comp, good, bad);
          a4 = new Instruction_label(bad);
          new_args2 = {linenumber, new Number((arg_index << 1) + 1), length, encoded_index};
          a5 = new Instruction_call(new Name("tensor-error"), new Args(new_args2));
          a6 = new Instruction_label(good);
          f->instructions.insert(f->instructions.begin() + instruction_index, a6);
          f->instructions.insert(f->instructions.begin() + instruction_index, a5);
          f->instructions.insert(f->instructions.begin() + instruction_index, a4);
          f->instructions.insert(f->instructions.begin() + instruction_index, a3);
          f->instructions.insert(f->instructions.begin() + instruction_index, a2);
          f->instructions.insert(f->instructions.begin() + instruction_index, a1);
          
          instruction_index += 6;
        }
      }
    }


    return;
  }

  void ArrayChecking::visit(Instruction_write_tensor *i) {
    //cout << "start write";
    auto comp = new Name(LL + "_comp");
    auto correct = new Label(":" + LL + "_CORRECT_" + to_string(instruction_index));
    auto error = new Label(":" + LL + "_ERROR_" + to_string(instruction_index));
    auto linenumber = new Name(LL + "_LineNumber");
    auto i1 = new Instruction_assignment(linenumber, new Number((i->line_number << 1) + 1));
    // auto i2 = new Instruction_declaration(new Type(TypeID::int64, 0), newV);
    auto i3 = new Instruction_assignment_op(comp, i->getDestination(), new Operator(OperatorID::e), new Number(0));
    auto i4 = new Instruction_branch_conditional(comp, error, correct);
    auto i5 = new Instruction_label(error);
    vector<Item*> new_args;
    new_args.push_back(linenumber);
    auto i6 = new Instruction_call(new Name("tensor-error"), new Args(new_args));
    auto i7 = new Instruction_label(correct);
    f->instructions.insert(f->instructions.begin() + instruction_index, i7);
    // f->instructions.insert(instruction_index, i2);
    f->instructions.insert(f->instructions.begin() + instruction_index, i6);
    f->instructions.insert(f->instructions.begin() + instruction_index, i5);
    f->instructions.insert(f->instructions.begin() + instruction_index, i4);
    f->instructions.insert(f->instructions.begin() + instruction_index, i3);
    f->instructions.insert(f->instructions.begin() + instruction_index, i1);
    instruction_index += 6;

    // Check array access 
    // Check if tuple
    

    if (types[i->getDestination()->to_string()] != "tuple") {
      // Single-dimension array
      if (i->getIndices().size() == 1) {
        auto length = new Name(LL + "_length");
        auto decoded_length = new Name(LL + "_length_decoded");
        auto good = new Label(":" + LL + "_GOOD_" + to_string(instruction_index));
        auto bad = new Label(":" + LL + "_BAD_" + to_string(instruction_index));
        Item* encoded_index;
        Item* decoded_index;
        if (i->getIndices().front()->to_string().at(0) == '%') {
          decoded_index = i->getIndices().front();
          encoded_index = new Name(LL + "_encoded_index");
          auto aencode = new Instruction_assignment_op(encoded_index, decoded_index, new Operator(OperatorID::salq), new Number(1));
          auto aencode2 = new Instruction_assignment_op(encoded_index, encoded_index, new Operator(OperatorID::addq), new Number(1));
          f->instructions.insert(f->instructions.begin() + instruction_index, aencode2);
          f->instructions.insert(f->instructions.begin() + instruction_index, aencode);
          instruction_index += 2;
        } else {
          encoded_index = new Number((stol(i->getIndices().front()->to_string()) << 1) + 1);
          decoded_index = i->getIndices().front();
        }

        auto a1 = new Instruction_length(length, i->getDestination(), new Number(0));
        auto atest = new Instruction_assignment_op(decoded_length, length, new Operator(OperatorID::sarq), new Number(1));
        auto a2 = new Instruction_assignment_op(comp, decoded_index, new Operator(OperatorID::l), decoded_length);
        auto a3 = new Instruction_branch_conditional(comp, good, bad);
        auto a4 = new Instruction_label(bad);
        vector<Item*> new_args2 = {linenumber, length, encoded_index};
        auto a5 = new Instruction_call(new Name("tensor-error"), new Args(new_args2));
        auto a6 = new Instruction_label(good);
        f->instructions.insert(f->instructions.begin() + instruction_index, a6);
        f->instructions.insert(f->instructions.begin() + instruction_index, a5);
        f->instructions.insert(f->instructions.begin() + instruction_index, a4);
        f->instructions.insert(f->instructions.begin() + instruction_index, a3);
        f->instructions.insert(f->instructions.begin() + instruction_index, a2);

        f->instructions.insert(f->instructions.begin() + instruction_index, atest);
        f->instructions.insert(f->instructions.begin() + instruction_index, a1);
        instruction_index += 7;
      } else {
        Instruction* a1;
        Instruction* a2;
        Instruction* a3;
        Instruction* a4;
        Instruction* a5;
        Instruction* a6;
        vector<Item*> new_args2;
        for (int arg_index = 0; arg_index < i->getIndices().size(); arg_index++) {
          auto length = new Name(LL + "_length");
          auto good = new Label(":" + LL + "_GOOD_" + to_string(instruction_index) + "_" + to_string(arg_index));
          auto bad = new Label(":" + LL + "_BAD_" + to_string(instruction_index) + "_" + to_string(arg_index));
          Item* encoded_index;
          if (i->getIndices().at(arg_index)->to_string().at(0) == '%') {
            encoded_index = i->getIndices().at(arg_index);
          } else {
            encoded_index = new Number((stol(i->getIndices().at(arg_index)->to_string()) << 1) + 1);
          }

          a1 = new Instruction_length(length, i->getDestination(), new Number(arg_index));
          a2 = new Instruction_assignment_op(comp, encoded_index, new Operator(OperatorID::l), length);
          a3 = new Instruction_branch_conditional(comp, good, bad);
          a4 = new Instruction_label(bad);
          new_args2 = {linenumber, new Number((arg_index << 1) + 1), length, encoded_index};
          a5 = new Instruction_call(new Name("tensor-error"), new Args(new_args2));
          a6 = new Instruction_label(good);
          f->instructions.insert(f->instructions.begin() + instruction_index, a6);
          f->instructions.insert(f->instructions.begin() + instruction_index, a5);
          f->instructions.insert(f->instructions.begin() + instruction_index, a4);
          f->instructions.insert(f->instructions.begin() + instruction_index, a3);
          f->instructions.insert(f->instructions.begin() + instruction_index, a2);
          f->instructions.insert(f->instructions.begin() + instruction_index, a1);
          
          instruction_index += 6;
        }
      }
    }


    return;
  }

  void ArrayChecking::visit(Instruction_length *i) {
    return;
  }

  void ArrayChecking::visit(Instruction_new_array *i) {
    return;
  }

  void ArrayChecking::visit(Instruction_new_tuple *i) {
    return;
  }

  void ArrayChecking::visit(Instruction_return *i) {
    
    return;
  }

  void ArrayChecking::visit(Instruction_return_arg *i) {
    return;
  }

  void ArrayChecking::visit(Instruction_branch *i) {
    return;
  }

  void ArrayChecking::visit(Instruction_branch_conditional *i) {
    return;
  }

  void ArrayChecking::visit(Instruction_call *i) {
    return;
  }

  void ArrayChecking::visit(Instruction_assignment_call *i) {
    return;
  }

  void ArrayChecking::visit(Instruction_label *i) {
    return;
  }

  void BBGenerator::visit(Instruction_declaration *i) {
    if (startBB) {
      auto l = new Instruction_label(new Label(":" + LL + "_label_" + to_string(instruction_index)));
      new_instructions.push_back(l);
      startBB = false;
    }
    new_instructions.insert(new_instructions.begin() + 1, i);
    num_declarations++;
    if (i->get_type()->to_string() == "int64") {
      new_instructions.insert(new_instructions.begin() + num_declarations, new Instruction_assignment(i->get_var(), new Number(1)));
    } else {
      new_instructions.insert(new_instructions.begin() + num_declarations, new Instruction_assignment(i->get_var(), new Number(0)));
    
    }
    

    return;
  }

  void BBGenerator::visit(Instruction_assignment *i) {
    if (startBB) {
      auto l = new Instruction_label(new Label(":" + LL + "_label_" + to_string(instruction_index)));
      new_instructions.push_back(l);
      startBB = false;
    }
    new_instructions.push_back(i);
    return;
  }

  void BBGenerator::visit(Instruction_assignment_op *i) {
    if (startBB) {
      auto l = new Instruction_label(new Label(":" + LL + "_label_" + to_string(instruction_index)));
      new_instructions.push_back(l);
      startBB = false;
    }
    new_instructions.push_back(i);
    return;
  }

  void BBGenerator::visit(Instruction_read_tensor *i) {
    if (startBB) {
      auto l = new Instruction_label(new Label(":" + LL + "_label_" + to_string(instruction_index)));
      new_instructions.push_back(l);
      startBB = false;
    }
    new_instructions.push_back(i);
    return;
  }

  void BBGenerator::visit(Instruction_write_tensor *i) {
    if (startBB) {
      auto l = new Instruction_label(new Label(":" + LL + "_label_" + to_string(instruction_index)));
      new_instructions.push_back(l);
      startBB = false;
    }
    new_instructions.push_back(i);
    return;
  }

  void BBGenerator::visit(Instruction_length *i) {
    if (startBB) {
      auto l = new Instruction_label(new Label(":" + LL + "_label_" + to_string(instruction_index)));
      new_instructions.push_back(l);
      startBB = false;
    }
    new_instructions.push_back(i);
    return;
  }

  void BBGenerator::visit(Instruction_new_array *i) {
    if (startBB) {
      auto l = new Instruction_label(new Label(":" + LL + "_label_" + to_string(instruction_index)));
      new_instructions.push_back(l);
      startBB = false;
    }
    new_instructions.push_back(i);
    return;
  }

  void BBGenerator::visit(Instruction_new_tuple *i) {
    if (startBB) {
      auto l = new Instruction_label(new Label(":" + LL + "_label_" + to_string(instruction_index)));
      new_instructions.push_back(l);
      startBB = false;
    }
    new_instructions.push_back(i);
    return;
  }

  void BBGenerator::visit(Instruction_return *i) {
    if (startBB) {
      auto l = new Instruction_label(new Label(":" + LL + "_label_" + to_string(instruction_index)));
      new_instructions.push_back(l);
      startBB = false;
    }
    new_instructions.push_back(i);
    startBB = true;
    return;
  }

  void BBGenerator::visit(Instruction_return_arg *i) {
    if (startBB) {
      auto l = new Instruction_label(new Label(":" + LL + "_label_" + to_string(instruction_index)));
      new_instructions.push_back(l);
      startBB = false;
    }
    new_instructions.push_back(i);
    startBB = true;
    return;
  }

  void BBGenerator::visit(Instruction_branch *i) {
    if (startBB) {
      auto l = new Instruction_label(new Label(":" + LL + "_label_" + to_string(instruction_index)));
      new_instructions.push_back(l);
      startBB = false;
    }
    new_instructions.push_back(i);
    startBB = true;
    return;
  }

  void BBGenerator::visit(Instruction_branch_conditional *i) {
    if (startBB) {
      auto l = new Instruction_label(new Label(":" + LL + "_label_" + to_string(instruction_index)));
      new_instructions.push_back(l);
      startBB = false;
    }
    new_instructions.push_back(i);
    startBB = true;
    return;
  }

  void BBGenerator::visit(Instruction_call *i) {
    if (startBB) {
      auto l = new Instruction_label(new Label(":" + LL + "_label_" + to_string(instruction_index)));
      new_instructions.push_back(l);
      startBB = false;
    }
    new_instructions.push_back(i);
    return;
  }

  void BBGenerator::visit(Instruction_assignment_call *i) {
    if (startBB) {
      auto l = new Instruction_label(new Label(":" + LL + "_label_" + to_string(instruction_index)));
      new_instructions.push_back(l);
      startBB = false;
    }
    new_instructions.push_back(i);
    return;
  }

  void BBGenerator::visit(Instruction_label *i) {
    if (!startBB) {
      auto g = new Instruction_branch(i->get_label());
      new_instructions.push_back(g);
    }
    startBB = false;
    new_instructions.push_back(i);
    return;
  }

  void CodeGenerator::visit(Instruction_declaration *i) {
    code += i->get_type()->to_string() + " " + i->get_var()->to_string() + "\n"; 
    return;
  }

  void CodeGenerator::visit(Instruction_assignment *i) {
    auto source = i->getSource()->to_string();
    if (source.at(0) == '%') {
      source.erase(0, 1);
      if (std::find(function_names.begin(), function_names.end(), source) != function_names.end()) {
        source.insert(0, ":");
      } else {
        source = i->getSource()->to_string();
      }
    }
    code += i->getDestination()->to_string() + " <- " + source + "\n";
    return;
  }

  void CodeGenerator::visit(Instruction_assignment_op *i) {
    code += i->getDestination()->to_string() + " <- " + i->getSource1()->to_string() + " " + i->getOperator()->to_string() + " " + i->getSource2()->to_string() + "\n";
    return;
  }

  

  void CodeGenerator::visit(Instruction_read_tensor *i) {
    code += i->getDestination()->to_string() + " <- " + i->getSource()->to_string();
    for (auto elem : i->getIndices()) {
      code += "[" + elem->to_string() + "]";
    }
    code += "\n";
    return;
  }

  void CodeGenerator::visit(Instruction_write_tensor *i) {
    code += i->getDestination()->to_string();
    for (auto elem : i->getIndices()) {
      code += "[" + elem->to_string() + "]";
    }
    code += " <- " + i->getSource()->to_string() + "\n";
    return;
  }

  void CodeGenerator::visit(Instruction_length *i) {
    code += i->getDestination()->to_string() + " <- length " + i->getSource()->to_string() + " " + i->getIndex()->to_string() + "\n";
    return;
  }

  void CodeGenerator::visit(Instruction_new_array *i) {
    code += i->getDestination()->to_string() + " <- new Array(" + i->getSizes()->to_string() + ")\n";
    return;
  }

  void CodeGenerator::visit(Instruction_new_tuple *i) {
    code += i->getDestination()->to_string() + " <- new Tuple(" + i->getSize()->to_string() + ")\n";
    return;
  }

  void CodeGenerator::visit(Instruction_return *i) {
    code += "return\n";
    return;
  }

  void CodeGenerator::visit(Instruction_return_arg *i) {
    code += "return " + i->get_value()->to_string() + "\n";
    return;
  }

  void CodeGenerator::visit(Instruction_branch *i) {
    code += "br "+ i->getDestination()->to_string()+"\n";
    return;
  }

  void CodeGenerator::visit(Instruction_branch_conditional *i) {
    code += "br " + i->getCondition()->to_string() + " " + i->getDestination1()->to_string() + " " + i->getDestination2()->to_string() + "\n";
    return;
  }

  string CodeGenerator::clean_func_name(string func_name) {
    string clean_name = func_name;
    clean_name.erase(0, 1);
    if (std::find(function_names.begin(), function_names.end(), clean_name) != function_names.end()) {
      func_name.at(0) = '@';
    } 
    if (func_name == "%print") {
      func_name = "print";
    } else if (func_name == "%tensor-error") {
      func_name = "tensor-error";
    } else if (func_name == "%input") {
      func_name = "input";
    }
    return func_name;
  }

  void CodeGenerator::visit(Instruction_call *i) {
    string func_name = i->getCallee()->to_string();
    func_name = clean_func_name(func_name);
    code += "call " + func_name + "(" + i->getArgs()->to_string() + ")\n";
    return;
  }

  void CodeGenerator::visit(Instruction_assignment_call *i) {
    string func_name = i->getCallee()->to_string();
    func_name = clean_func_name(func_name);
    code += i->getDst()->to_string() + " <- call " + func_name + "(" + i->getArgs()->to_string() + ")\n";
    return;
  }

  void CodeGenerator::visit(Instruction_label *i) {
    code += i->get_label()->to_string()+"\n";
    return;
  }
}
