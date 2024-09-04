#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <string>

#include "L2.h"
#include "liveness.h"
#include "coloring.h"
#include "spiller.h"
#include "code_generator.h"

namespace L2 {
    Function* newFunction;
    int64_t numSpilled = 0;
    int64_t currentVar=0;
    bool didIUpdate=false;
    bool doAll=false;
    SpillerVisitor spiller;
    SpillerItemLooker lookspill;
    string prefix;
    string lookingForVar;
    vector<OperatorID> opIDs = {OperatorID::addq, OperatorID::subq, OperatorID::imulq, OperatorID::andq, OperatorID::salq, OperatorID::sarq, OperatorID::l, OperatorID::le, OperatorID::e};
    vector<RegisterID> registerIDs = {RegisterID::rax, RegisterID::rbx, RegisterID::rbp, RegisterID::r10, RegisterID::r11, RegisterID::r12, RegisterID::r13, RegisterID::r14, RegisterID::r15, RegisterID::rdi, RegisterID::rsi, RegisterID::rdx, RegisterID::rcx, RegisterID::r8, RegisterID::r9, RegisterID::rsp, RegisterID::none};

    Function* spill(Function* f, string var, string pre) {
        // print("Start spilling " + var + " into " + pre);
        prefix = pre;
        lookingForVar=var;
        didIUpdate=false;
        doAll=false;
        numSpilled=0; //changed
        newFunction=new Function();
        
        //print("Get name and args");
        newFunction->name = f->name;
        newFunction->arguments = f->arguments;
        currentVar=f->locals*8;
        //print("Start for loop");;
        for (auto instruction : f->instructions) {
            // Replace the original instruction with new variable
            //print("Looking at");
            instruction->accept(&spiller);

        }
        if(didIUpdate){
            newFunction->locals=f->locals+1;
        }
        else{
            newFunction->locals=f->locals;
        }


        // Add loads/stores around the new instruction
        //print("Done"); 
        L2::generate_function(newFunction);
        return newFunction;
    }

    Function* spill_all(Function* f,int varCount){
                //print("Start");
        prefix = "%s";
        lookingForVar = "any";
        didIUpdate = true;
        doAll=true;
        newFunction = new Function();
        numSpilled=0;
        
        //print("Get name and args");
        newFunction->name = f->name;
        newFunction->arguments = f->arguments;
        currentVar=f->locals*8;
        //print("Start for loop");;
        for (auto instruction : f->instructions) {
            // Replace the original instruction with new variable
            //print("Looking at");
            instruction->accept(&spiller);

        }
        if(didIUpdate){
            newFunction->locals=f->locals+varCount;
        }
        else{
            newFunction->locals=f->locals;
        }
        print("LOOK HERE");
        L2::generate_function(newFunction);
        return newFunction;
    }

    // Instruction* create_load(Item* dst) {
    //     Item* rsp_register = new Register(RegisterID::rsp);
    //     Item* num_on_stack = new Number(8*numSpilled);
    //     Item* memory_instruction = new Mem(rsp_register, num_on_stack);
        
    //     Instruction_assignment(memory_instruction, dst);
    // }

    // ItemLooker visitor methods
    vector<string> SpillerItemLooker::visit(Register *i) {
    	vector<string> item_string;
        item_string.push_back("Register");
        item_string.push_back(to_string(i->getID()));
        return item_string;
        
    }

    vector<string> SpillerItemLooker::visit(Number *i) {
        vector<string> item_string;
        item_string.push_back("Number");
        item_string.push_back(to_string(i->getNum()));
    	return item_string;
    }

    vector<string> SpillerItemLooker::visit(Label *i) {
        vector<string> item_string;
        item_string.push_back("Label");
        item_string.push_back(i->getLabel());
    	return item_string;
    }

    vector<string> SpillerItemLooker::visit(Variable *i) {
        vector<string> item_string;
        string maybe=i->getVar();
        if(maybe==lookingForVar || doAll){
            //maybe=(prefix+to_string(numSpilled));
            item_string.push_back("NewVar");
        }
        else{
            item_string.push_back("OldVar");
        }
        item_string.push_back(maybe);
    	return item_string;
    }

    vector<string> SpillerItemLooker::visit(L *i) {
       	vector<string> item_string;
        item_string.push_back("L");
        item_string.push_back(i->getL());
    	return item_string;
    }

    vector<string> SpillerItemLooker::visit(Operator *i) {
        
       	vector<string> item_string;
        item_string.push_back("Operator");
        item_string.push_back(to_string(i->getID()));
    	return item_string;
    }

    vector<string> SpillerItemLooker::visit(Compare *i) {
        vector<string> item_string;
        auto a = i->getA()->accept(&lookspill);
        // print(
        //     "ASDASD"
        // );
        // print(a.front());
        // print(a.back());
        if (!a.empty()) {
            item_string.push_back(a.front());
            item_string.push_back(a.back());
        }
        auto op = i->getOp()->accept(&lookspill).back();
        item_string.push_back(op);
        auto b = i->getB()->accept(&lookspill);
        if (!b.empty()) {
            item_string.push_back(b.front());
            item_string.push_back(b.back());
        }
    	return item_string;
    }

    vector<string> SpillerItemLooker::visit(Mem *i) {
        vector<string> item_string;
        // print("Visiting a mem");
        // i->print_item();
        // print("\n");
        item_string.push_back("Mem");
        Item* x=i->getX();
        vector<string> x_result =x->accept(&lookspill);
        string x_type_str=x_result.front();
        string x_result_str=x_result.back();
        Item* m=i->getM();
        vector<string> m_result=m->accept(&lookspill);
        string m_type_str=m_result.front();
        string m_result_str=m_result.back();
        item_string.push_back(x_type_str);
        item_string.push_back(x_result_str);
        item_string.push_back(m_type_str);
        item_string.push_back(m_result_str);
        return item_string;
    }

    Item* MemCreator(vector<string> input){
        string x_type=input[1];
        string x_result=input[2];
        string m_type=input[3];
        string m_result=input[4];
        
        bool foundNewVar=false;
        if(x_type=="Register"){
            auto x=new Register(registerIDs[stoi(x_result)]);
            auto m=new Number(stoi(m_result));
            auto mem=new Mem(x,m);
            return mem;
        }
        if(x_type=="NewVar"||x_type=="OldVar"){
            if(x_type=="NewVar"){x_result=(prefix+to_string(numSpilled));}
            auto x=new Variable(x_result);
            auto m=new Number(stoi(m_result));
            auto mem=new Mem(x,m);
            // print("-----");
            // mem->print_item();
            // print("\n");
            return mem;
        }
        return NULL;
    }

    void SpillerVisitor::visit(Instruction_ret *i) {
        newFunction->instructions.push_back(i);
        //print("return");
    }

    void TryPushUsed(bool used){
        if(used){
            auto src= new Variable(prefix+to_string(numSpilled-1));
            auto usedInstruction = new Instruction_assignment(src,new Mem(new Register(RegisterID::rsp),(new Number(currentVar))));
            newFunction->instructions.push_back(usedInstruction);
            didIUpdate=true;
            // print("USED");
            // usedInstruction->print_instruction();
        }
    }

    void TryPushLoad(bool load){
        if(load){
            auto dst= new Variable(prefix+to_string(numSpilled-1));
            auto loadInstruction = new Instruction_assignment(new Mem(new Register(RegisterID::rsp),(new Number(currentVar))),dst);
            newFunction->instructions.push_back(loadInstruction);
            didIUpdate=true;
            // print("LOAD");
            // loadInstruction->print_instruction();
        }
    }

    void SpillerVisitor::visit(Instruction_assignment *i) {
        auto dst = i->getDestination();
        auto dst_data = dst->accept(&lookspill);
        auto src = i->getSource();
        auto src_data = src->accept(&lookspill);
        bool loadedVar=false;
        bool usedVar=false;
        // print("Got both");
        if (dst_data.front()=="Mem"){
            dst= MemCreator(dst_data);
            if(dst_data[1]=="NewVar"){
                usedVar=true;
            }
        }
        if (dst_data.front()=="NewVar") {
            auto var = new Variable(prefix+to_string(numSpilled));
            dst = var; loadedVar=true;
        } 
    ;
        if (src_data.front()=="Mem"){
            if(src_data[1]=="NewVar"){usedVar=true;}
            src= MemCreator(src_data);
        }
        if (src_data.front()=="NewVar") {
            auto var = new Variable(prefix+to_string(numSpilled));
            src = var; usedVar=true;
        } 
        // print("Got source");
        auto newInstruction = new Instruction_assignment(dst, src);
        if(usedVar || loadedVar){numSpilled+=1;}
        TryPushUsed(usedVar);
        newFunction->instructions.push_back(newInstruction);
        TryPushLoad(loadedVar);
    }

    void SpillerVisitor::visit(Instruction_arithmetic *i) {
        auto dst = i->getDestination();
        auto dst_data = dst->accept(&lookspill);
        auto src = i->getSource();
        auto src_data = src->accept(&lookspill);
        bool loadedVar=false;
        bool usedVar=false;
        // print("Got both");
        if (dst_data.front()=="Mem"){
            if(dst_data[1]=="NewVar"){usedVar=true;}
            dst= MemCreator(dst_data);
        }
        if (dst_data.front()=="NewVar") {
            auto var = new Variable(prefix+to_string(numSpilled));
            dst = var; loadedVar=true;  usedVar=true;
        } 
    ;
        if (src_data.front()=="Mem"){
            if(src_data[1]=="NewVar"){usedVar=true; loadedVar=true;}
            src= MemCreator(src_data);
        }
        if (src_data.front()=="NewVar") {
            auto var = new Variable(prefix+to_string(numSpilled));
            src = var; usedVar=true;
        } 
        // print("Got source");
        auto newInstruction = new Instruction_arithmetic(dst, src, i->getOperator());
        if(usedVar || loadedVar){numSpilled+=1;}
        TryPushUsed(usedVar);
        newFunction->instructions.push_back(newInstruction);
        TryPushLoad(loadedVar);
    }

    void SpillerVisitor::visit(Instruction_shift *i) {
        auto dst = i->getDestination();
        auto dst_data = dst->accept(&lookspill);
        auto src = i->getSource();
        auto src_data = src->accept(&lookspill);
        bool loadedVar=false;
        bool usedVar=false;
        // print("Got both");
        if (dst_data.front()=="Mem"){
            if(dst_data[1]=="NewVar"){loadedVar=true;}
            dst= MemCreator(dst_data);
        }
        if (dst_data.front()=="NewVar") {
            auto var = new Variable(prefix+to_string(numSpilled));
            dst = var; loadedVar=true; usedVar=true;
        } 
    ;
        if (src_data.front()=="Mem"){
            if(src_data[1]=="NewVar"){usedVar=true;}
            src= MemCreator(src_data);
        }
        if (src_data.front()=="NewVar") {
            auto var = new Variable(prefix+to_string(numSpilled));
            src = var; usedVar=true;
        } 
        // print("Got source");
        auto newInstruction = new Instruction_shift(dst, src, i->getOperator());
        if(usedVar || loadedVar){numSpilled+=1;}
        TryPushUsed(usedVar);
        newFunction->instructions.push_back(newInstruction);
        TryPushLoad(loadedVar);
    }

    void SpillerVisitor::visit(Instruction_assignment_compare *i) {
        auto dst = i->getDestination();
        auto src = i->getSource();
        bool usedVar=false;
        bool loadedVar=false;
        bool didAlready=false;
        if (dst->accept(&lookspill).front()=="NewVar") {
            auto var = new Variable(prefix + to_string(numSpilled));
            dst = var; loadedVar=true; //numSpilled+=1;
            // print("OEO");
        } 

        // Compare
        Item* a;
        Item* b;
        auto compare = i->getSource()->accept(&lookspill);
        if (compare.size() != 1) {
            // print("LOOK");
            // print(compare.front());
            // print(compare.back());
            if (compare[0] == "NewVar" || compare[0] == "OldVar") {
                if(compare[1] == lookingForVar || doAll){
                    a = new Variable(prefix + to_string(numSpilled));
                    usedVar=true; 
                    didAlready=true;
                }
                else{
                    a= new Variable(compare[1]);
                }
            } else {
                if (compare[0] == "Register") {
                    a = new Register(registerIDs[stoi(compare[1])]);
                } else {
                    a = new Number(stoi(compare[1]));
                }
            }
            if (compare[3] == "NewVar" || compare[3] == "OldVar") {
                if(compare[4] ==lookingForVar || doAll){
                    b = new Variable(prefix + to_string(numSpilled));
                    usedVar=true;
                }
                else{
                    b= new Variable(compare[4]); // 0
                }
            } else {
                if (compare[3] == "Register") {
                    b = new Register(registerIDs[stoi(compare[4])]);
                } else {
                    b = new Number(stoi(compare[4]));
                }
            }
        } 
        //print("Got source");
        auto op = new Operator(opIDs[stoi(compare[2])]);
        auto newCompare = new Compare(b, a, op);
        if(usedVar || loadedVar){numSpilled+=1;}
        auto newInstruction = new Instruction_assignment_compare(dst, newCompare);
        TryPushUsed(usedVar);
        newFunction->instructions.push_back(newInstruction);
        TryPushLoad(loadedVar);
    }

    void SpillerVisitor::visit(Instruction_assignment_stack_arg *i) {
        auto dst = i->getDestination();
        auto dst_data = dst->accept(&lookspill);
        auto src = i->getSource();
        auto src_data = src->accept(&lookspill);
        bool loadedVar=false;
        bool usedVar=false;
        // print("Got both");
        if (dst_data.front()=="Mem"){
            if(dst_data[1]=="NewVar"){loadedVar=true;}
            dst= MemCreator(dst_data);
        }
        if (dst_data.front()=="NewVar") {
            auto var = new Variable(prefix+to_string(numSpilled));
            dst = var; loadedVar=true;
        } 
    ;
        if (src_data.front()=="Mem"){
            if(src_data[1]=="NewVar"){loadedVar=true;}
            src= MemCreator(src_data);
        }
        if (src_data.front()=="NewVar") {
            auto var = new Variable(prefix+to_string(numSpilled));
            src = var; usedVar=true;
        } 
        // print("Got source");
        auto newInstruction = new Instruction_assignment_stack_arg(dst, src);
        if(usedVar || loadedVar){numSpilled+=1;}
        TryPushUsed(usedVar);
        newFunction->instructions.push_back(newInstruction);
        TryPushLoad(loadedVar);
    }

    void SpillerVisitor::visit(Instruction_inc *i) {
        auto dst = i->getDestination();
        auto dst_data = dst->accept(&lookspill);
        bool loadedVar=false;
        bool usedVar = false;
        // print("Got both");
        if (dst_data.front()=="Mem"){
            if(dst_data[1]=="NewVar"){loadedVar=true;}
            dst= MemCreator(dst_data);
        }
        if (dst_data.front()=="NewVar") {
            auto var = new Variable(prefix+to_string(numSpilled));
            dst = var; loadedVar=true; usedVar=true;
        } 
        auto newInstruction = new Instruction_inc(dst);
        if(usedVar || loadedVar){numSpilled+=1;}
        TryPushUsed(usedVar);
        newFunction->instructions.push_back(newInstruction);
        TryPushLoad(loadedVar);
    }

    void SpillerVisitor::visit(Instruction_dec *i) {
        auto dst = i->getDestination();
        auto dst_data = dst->accept(&lookspill);
        bool loadedVar=false;
        bool usedVar = false;
        // print("Got both");
        if (dst_data.front()=="Mem"){
            if(dst_data[1]=="NewVar"){loadedVar=true;}
            dst= MemCreator(dst_data);
        }
        if (dst_data.front()=="NewVar") {
            auto var = new Variable(prefix+to_string(numSpilled));
            dst = var; loadedVar=true; usedVar=true;
        } 
        auto newInstruction = new Instruction_dec(dst);
        if(usedVar || loadedVar){numSpilled+=1;}
        TryPushUsed(usedVar);
        newFunction->instructions.push_back(newInstruction);
        TryPushLoad(loadedVar);
    }

    void SpillerVisitor::visit(Instruction_goto *i) {
        auto newInstruction = new Instruction_goto(i->getDestination());
        newFunction->instructions.push_back(newInstruction);
        //print("Pushed a goto");
    }

    void SpillerVisitor::visit(Instruction_cjump *i) {
        auto dst = i->getLabel();
        //auto src = i->getCompare();
        bool usedVar=false;
        bool loadedVar=false;
        bool didAlready=false;
        if (dst->accept(&lookspill).front()=="NewVar") {
            auto var = new Variable(prefix + to_string(numSpilled));
            dst = var; loadedVar=true;
        } 

        // Compare
        Item* a;
        Item* b;
        auto compare = i->getCompare()->accept(&lookspill);
        if (compare.size() != 1) {
            // print("LOOK");
            // print(compare.front());
            // print(compare.back());
            if (compare[0] == "NewVar" || compare[0] == "OldVar") {
                if(compare[1] == lookingForVar || doAll){
                    a = new Variable(prefix + to_string(numSpilled));
                    usedVar=true; 
                    didAlready=true;
                }
                else{
                    a= new Variable(compare[1]);
                }
            } else {
                if (compare[0] == "Register") {
                    a = new Register(registerIDs[stoi(compare[1])]);
                } else {
                    a = new Number(stoi(compare[1]));
                }
            }
            if (compare[3] == "NewVar" || compare[3] == "OldVar") {
                if(compare[4] ==lookingForVar || doAll){
                    b = new Variable(prefix + to_string(numSpilled));
                    usedVar=true;
                }
                else{
                    b= new Variable(compare[4]); // 0
                }
            } else {
                if (compare[3] == "Register") {
                    b = new Register(registerIDs[stoi(compare[4])]);
                } else {
                    b = new Number(stoi(compare[4]));
                }
            }
        } 
        //print("Got source");
        auto op = new Operator(opIDs[stoi(compare[2])]);
        auto newCompare = new Compare(b, a, op);
        auto newInstruction = new Instruction_cjump(newCompare, i->getLabel());
        if(usedVar || loadedVar){numSpilled+=1;}
        TryPushUsed(usedVar);
        newFunction->instructions.push_back(newInstruction);
        TryPushLoad(loadedVar);
        //print("Pushed a cjump");
    }

    void SpillerVisitor::visit(Instruction_at *i) {
        auto add = i->getAdd();
        auto mult = i->getMult();
        auto sav = i->getSaveTo();
        bool usedVar=false;
        bool loadedVar=false;
        if (add->accept(&lookspill).front()=="NewVar") {
            auto var = new Variable(prefix + to_string(numSpilled));
            add = var;
            usedVar=true;
        } 
       // print("Got dest");
        if (mult->accept(&lookspill).front()=="NewVar") {
            auto var = new Variable(prefix + to_string(numSpilled));
            mult = var;
            usedVar=true;
        } 
        //print("Got dest");
        if (sav->accept(&lookspill).front()=="NewVar") {
            auto var = new Variable(prefix + to_string(numSpilled));
            sav = var;
            loadedVar=true; 
        } 
        auto newInstruction = new Instruction_at(sav, add, mult, i->getBy());
        if(usedVar || loadedVar){numSpilled+=1;}
        TryPushUsed(usedVar);
        newFunction->instructions.push_back(newInstruction);
        TryPushLoad(loadedVar);
    }

    void SpillerVisitor::visit(Instruction_call *i) {
        auto func = i->getFunc();
        bool usedVar=false;
        bool loadedVar=false;
        //print("Got both");
        if (func->accept(&lookspill).front()=="NewVar") {
            auto var = new Variable(prefix + to_string(numSpilled));
            func = var;
            usedVar=true;
        } 
       
        //print("Got source");
        auto newInstruction = new Instruction_call(func, i->getParamNum());
        if(usedVar || loadedVar){numSpilled+=1;}
        TryPushUsed(usedVar);
        newFunction->instructions.push_back(newInstruction);
        TryPushLoad(loadedVar);
        //print("Pushed a call");
    }

    void SpillerVisitor::visit(Instruction_label *i) {
        auto newInstruction = new Instruction_label(i->getLabel());
        newFunction->instructions.push_back(newInstruction);
        //print("Pushed a label");
    }
}