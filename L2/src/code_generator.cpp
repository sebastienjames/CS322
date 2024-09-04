#include <string>
#include <iostream>
#include <fstream>
#include <map>

#include "code_generator.h"

using namespace std;

namespace L2{

    string cg_registers[16]={"rax","rbx","rbp","r10","r11","r12","r13","r14","r15","rdi","rsi","rdx","rcx","r8","r9","rsp"};
    string cg_operators[9]={"+=","-=","*=","&=","<<=",">>=","<","<=","="};
    //string registers_8_bit[16]={"al","bl","bpl","r10b","r11b","r12b","r13b","r14b","r15b","dil","sil","dl","cl","r8b","r9b","rsp"};
    string myOutput = "";
    //string param = "";
    //string  numD="";
    CG_ItemLooker cg_looker;

    void generate_code(Program p){
        //CG_ItemLooker looker; 
        // Open the output file.
        std::ofstream outputFile;
        //outputFile.open("prog.S");
    
        /* 
        * Generate target code
        */ 
        string myEntry=p.entryPointLabel;
        auto function=p.functions;
        
        //INTRO 
        myOutput="("+myEntry+"\n";

        CodeGenerator codeGenerator;
        std::vector<Function*> functions = p.functions;

        for (auto f : functions) {
            //print("Function Start");
            string fName = f->name;
            int64_t numLocals = f->locals;
            int64_t numArgs=f->arguments;
            string numA= to_string(numLocals);
            string numD= to_string(numArgs);
            myOutput+="("+fName+"\n";
            myOutput+="	"+numD+" "+numA+"\n";

            for (auto i : f->instructions) {
            	//print("reached");
                i->accept(&codeGenerator);
            }
            myOutput+=")\n";
        }


        /* 
        * Close the output file.
        */ 
        //print(myOutput);
        myOutput+=")\n";
        //outputFile << myOutput;
        //outputFile.close();
    
        return ;
    }

    void generate_function(Function* f){
        //CG_ItemLooker looker; 
        // Open the output file.
        //std::ofstream outputFile;
        //outputFile.open("prog.S");

        CodeGenerator codeGenerator;
            //print("Function Start");
        string fName = f->name;
        int64_t numLocals = f->locals;
        int64_t numArgs=f->arguments;
        string numA= to_string(numLocals);
        string numD= to_string(numArgs);
        myOutput="("+fName+"\n";
        myOutput+="	"+numD+" "+numA+"\n";

        for (auto i : f->instructions) {
            	//print("reached");
            i->accept(&codeGenerator);
        }
            myOutput+=")";
        /* 
        * Close the output file.
        */ 
        
        print(myOutput);
        //outputFile << myOutput;
        //outputFile.close();
    
        return ;
    }

    vector<string> CG_ItemLooker::visit(Register *i) {
    	//print("Visited, Register");
    	vector<string> item_string;
    	RegisterID id = i->getID();
     	item_string.push_back(cg_registers[id]);
        //item_string.push_back("%"+registers_8_bit[id]);
        item_string.push_back("register");
        return item_string;
        
    }

    vector<string> CG_ItemLooker::visit(Number *i) {
        vector<string> item_string;
        int64_t numID=i->getNum();
        item_string.push_back(to_string(numID));
        item_string.push_back("number");
    	return item_string;
    }

    vector<string> CG_ItemLooker::visit(Label *i) {
        vector<string> item_string;
  		string labelID=i->getLabel().erase(0,1);
  		item_string.push_back(":"+labelID); 
  		item_string.push_back("label");
        return item_string;
    }

    vector<string> CG_ItemLooker::visit(L *i) {
       	vector<string> item_string;
       	string lID=i->getL().erase(0,1);
       	item_string.push_back("@"+lID);
       	item_string.push_back("l");
    	return item_string;
    }

    vector<string> CG_ItemLooker::visit(Operator *i) {
       	vector<string> item_string;
       	OperatorID opID=i->getID();
       	item_string.push_back(cg_operators[opID]);
       	item_string.push_back("operator");
    	return item_string;
    }

    vector<string> CG_ItemLooker::visit(Compare *i) {
        // print("Compare");
        vector<string> item_string;
        
        Item* a=i->getA();
        vector<string> aResult=a->accept(&cg_looker);
        string a_str=aResult.front();
        // print(a_str);
        item_string.push_back(a_str);
        
        Item* b=i->getB();
        vector<string> bResult=b->accept(&cg_looker);
        string b_str=bResult.front();
        string bType=bResult.back();
        // print(b_str);
        item_string.push_back(b_str);
        
        Item* op=i->getOp();
        string op_str=op->accept(&cg_looker).front();
        // print(op_str);
        item_string.push_back(op_str);
        

    	return item_string;
    }

    vector<string> CG_ItemLooker::visit(Mem *i) {
        vector<string> item_string;
        Item* x_item=i->getX();
        string x_str=x_item->accept(&cg_looker).front();
        Item* m_item=i->getM();
        string m_str=m_item->accept(&cg_looker).front();
        item_string.push_back("mem "+x_str+" "+m_str);
    	return item_string;
    }

    vector<string> CG_ItemLooker::visit(Variable *i) {
        //print("Call Variable");
        vector<string> item_string;
        item_string.push_back(i->getVar());
        //print(i->getVar());
    	return item_string;
    }



    void CodeGenerator::visit(Instruction_ret *i) {
        //print("I'm a return");
        myOutput+= "	return\n";
    }

    void CodeGenerator::visit(Instruction_assignment *i) {
    	//print("I'm an assignment");
        Item* Item_source = i->getSource();
        vector<string> v_src=Item_source->accept(&cg_looker);
        string src=v_src.front();
        //print(v_src.back());
        Item* Item_dest  = i->getDestination();
        vector<string> v_dst=Item_dest->accept(&cg_looker);
        string dst=v_dst.front();
        myOutput+="	"+dst+" <- "+src+"\n";
        // return "";

    }

    /*OLD CODE
    void CodeGenerator::visit(Instruction_operation *i) {
        CG_ItemLooker looker;

        // if (op == "salq" || op == "sarq") {
        //     source = eight_bit_registers[source];
        // }
        // return " ";
    }
    */
    void CodeGenerator::visit(Instruction_arithmetic *i) {
        //print("I'm an arithmetic operation");
        Item* Item_source = i->getSource();
        vector<string> v_src=Item_source->accept(&cg_looker);
        string src=v_src.front();
        Item* Item_dest  = i->getDestination();
        vector<string> v_dst=Item_dest->accept(&cg_looker);
        string dst=v_dst.front();
        Item* Item_op  = i->getOperator();
        vector<string> v_op=Item_op->accept(&cg_looker);
        string op=v_op.front();
        myOutput+="	"+dst+" "+op+" "+src+"\n";
    }
    void CodeGenerator::visit(Instruction_shift *i) {
        //print("I'm an shift operation");
        Item* Item_source = i->getSource();
        vector<string> v_src=Item_source->accept(&cg_looker);
        string src=v_src.front();
        if(v_src.back()=="register"){
        	src=v_src[1];
        }
        Item* Item_dest  = i->getDestination();
        vector<string> v_dst=Item_dest->accept(&cg_looker);
        string dst=v_dst.front();
        Item* Item_op  = i->getOperator();
        vector<string> v_op=Item_op->accept(&cg_looker);
        string op=v_op.front();
        myOutput+="	"+dst+" "+op+" "+src+"\n";
    }
   void CodeGenerator::visit(Instruction_assignment_compare *i) {
       	// print("I'm an assignment compare");
       	Item* Item_source = i->getSource();
       	vector<string> v_src=Item_source->accept(&cg_looker);
       	string a_str=v_src[0];
       	string b_str=v_src[1];
        string op_str=v_src[2];
        // print(a_str);
        // print(b_str);
        // print(op_str);
       	Item* Item_dest  = i->getDestination();
       	vector<string> v_dst=Item_dest->accept(&cg_looker);
       	string dst=v_dst.front();
       	myOutput+="	"+dst+" <- "+b_str+" "+op_str+" "+a_str+"\n";
       	
    }

    void CodeGenerator::visit(Instruction_assignment_stack_arg *i) {
        //print("I'm an assignement stack");
        Item* Item_dest = i->getDestination();
        string dst=Item_dest->accept(&cg_looker).front();
        Item* Item_src = i->getSource();
        string src=Item_src->accept(&cg_looker).front();
        myOutput+="	"+dst+" <- stack-arg "+src+"\n";
    }

    void CodeGenerator::visit(Instruction_inc *i) {
        //print("I'm an increment operation");
        Item* Item_dest  = i->getDestination();
        vector<string> v_dst=Item_dest->accept(&cg_looker);
        string dst=v_dst.front();
        myOutput+="	"+dst+"++\n";
    }

    void CodeGenerator::visit(Instruction_dec *i) {
        //print("I'm an decriment operation");
        Item* Item_dest  = i->getDestination();
        vector<string> v_dst=Item_dest->accept(&cg_looker);
        string dst=v_dst.front();
        myOutput+="	"+dst+"--\n";
    }

    void CodeGenerator::visit(Instruction_goto *i) {
        //print("I'm an goTo operation");
        Item* Item_dest  = i->getDestination();
        vector<string> v_dst=Item_dest->accept(&cg_looker);
        string dst=v_dst.front().erase(0,1);
        myOutput+="	goto :"+dst+"\n";
    }

    void CodeGenerator::visit(Instruction_cjump *i) {
        // print("I'm a cjump");
       	Item* Item_source = i->getCompare();
       	vector<string> v_src=Item_source->accept(&cg_looker);
       	string a_str=v_src[0];
       	string b_str=v_src[1];
        string op_str=v_src[2];
        // print(a_str);
        // print(b_str);
       	Item* Item_dest  = i->getLabel();
       	vector<string> v_dst=Item_dest->accept(&cg_looker);
       	string dst=v_dst.front().erase(0,1);
        myOutput+="	cjump "+b_str+" "+op_str+" "+a_str+" :"+dst+"\n";

        
    }

    void CodeGenerator::visit(Instruction_call *i) {
        //print("I'm a call");
        Item* u_item=i->getFunc();
        Item* num_item=i->getParamNum();
        vector<string> hold=u_item->accept(&cg_looker);
        string funcName=hold.front();
        string isRegister=hold.back();
        string paramNum=num_item->accept(&cg_looker).front();\
        //print(paramNum);
        int paramNum_int=stoi(paramNum);
        myOutput+="	call "+funcName+" "+paramNum+"\n";
    }

    void CodeGenerator::visit(Instruction_at *i) {
		Item* add_item=i->getAdd();
		string add_str=add_item->accept(&cg_looker).front();
		Item* mult_item=i->getMult();
		string mult_str=mult_item->accept(&cg_looker).front();
		Item* by_item=i->getBy();
		string by_str=by_item->accept(&cg_looker).front().erase(0,1);
        Item* saveTo_item=i->getSaveTo();
        string saveTo_str=saveTo_item->accept(&cg_looker).front();
        myOutput+="	"+saveTo_str+" @ "+add_str+" "+mult_str+" "+by_str+"\n";
    }

    void CodeGenerator::visit(Instruction_label *i) {
    	//print("I'm an Label");
    	Item* Item_source = i->getLabel();
    	vector<string> v_src=Item_source->accept(&cg_looker);
        string src=v_src.front();
		myOutput+="	"+src+"\n";
    }
}
