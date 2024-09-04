#include <string>
#include <iostream>
#include <fstream>
#include <map>

#include "code_generator.h"

using namespace std;

namespace L1{

    string registers[16]={"rax","rbx","rbp","r10","r11","r12","r13","r14","r15","rdi","rsi","rdx","rcx","r8","r9","rsp"};
    string operators[9]={"addq","subq","imulq","andq","salq","sarq","l","le","e"};
    string registers_8_bit[16]={"al","bl","bpl","r10b","r11b","r12b","r13b","r14b","r15b","dil","sil","dl","cl","r8b","r9b","rsp"};
    string myOutput = "";
    string param = "";
    string  numD="";
    ItemLooker looker;

    void generate_code(Program p){
        //ItemLooker looker; 
        // Open the output file.
        std::ofstream outputFile;
        outputFile.open("prog.S");
    
        /* 
        * Generate target code
        */ 
        string myEntry=p.entryPointLabel;
        myEntry.erase(0,1);
        auto function=p.functions;
        
        //INTRO 
        myOutput+=".text\n";
        myOutput+=".globl go\ngo:";
        myOutput+=" pushq %rbx\n pushq %rbp\n pushq %r12\n pushq %r13\n pushq %r14\n pushq %r15\n";
        myOutput+="call _"+myEntry+"\n";
        myOutput+=" popq %r15\n popq %r14\n popq %r13\n popq %r12\n popq %rbp\n popq %rbx\n";
        myOutput+="retq\n";

        CodeGenerator codeGenerator;
        std::vector<Function*> functions = p.functions;

        for (auto f : functions) {
            //print("Function Start");
            string fName = f->name;
            int64_t numAlocate = (f->locals) * 8;
            int64_t numDealocate = (f->locals) * 8;
            if (f->arguments>6) { numDealocate += (f->arguments-6) * 8; }
            fName.erase(0,1);
            string numA= to_string(numAlocate);
            numD= to_string(numDealocate);
            myOutput+="_"+fName+":\n";
            myOutput+=" subq $"+numA+", %rsp\n";

            for (auto i : f->instructions) {
            	//print("reached");
                i->accept(&codeGenerator);
            }
        }


        /* 
        * Close the output file.
        */ 
        //print(myOutput);
        outputFile << myOutput;
        outputFile.close();
    
        return ;
    }

    vector<string> ItemLooker::visit(Register *i) {
    	//print("Visited, Register");
    	vector<string> item_string;
    	RegisterID id = i->getID();
     	item_string.push_back("%"+registers[id]);
        item_string.push_back("%"+registers_8_bit[id]);
        item_string.push_back("register");
        return item_string;
        
    }

    vector<string> ItemLooker::visit(Number *i) {
        vector<string> item_string;
        int64_t numID=i->getNum();
        item_string.push_back("$"+to_string(numID));
        item_string.push_back("number");
    	return item_string;
    }

    vector<string> ItemLooker::visit(Label *i) {
        vector<string> item_string;
  		string labelID=i->getLabel().erase(0,1);
  		item_string.push_back("$_"+labelID); 
  		item_string.push_back("label");
        return item_string;
    }

    vector<string> ItemLooker::visit(L *i) {
       	vector<string> item_string;
       	string lID=i->getL().erase(0,1);
       	item_string.push_back("$_"+lID);
       	item_string.push_back("l");
    	return item_string;
    }

    vector<string> ItemLooker::visit(Operator *i) {
       	vector<string> item_string;
       	OperatorID opID=i->getID();
       	item_string.push_back(operators[opID]);
       	item_string.push_back("operator");
    	return item_string;
    }

    vector<string> ItemLooker::visit(Compare *i) {
        vector<string> item_string;
        
        Item* a=i->getA();
        vector<string> aResult=a->accept(&looker);
        string a_str=aResult.front();
        string aType=aResult.back();
        
        Item* b=i->getB();
        vector<string> bResult=b->accept(&looker);
        string b_str=bResult.front();
        string bType=bResult.back();
        
        Item* op=i->getOp();
        string op_str=op->accept(&looker).front();
        if(bType=="register"){
        	item_string.push_back(" cmpq "+a_str+", "+b_str+"\n");
        	item_string.push_back(op_str);
        }
        if(bType=="number" && aType=="register"){
            item_string.push_back(" cmpq "+b_str+", "+a_str+"\n");
            if(op_str=="l"){item_string.push_back("g");}
            else if(op_str=="le"){item_string.push_back("ge");}
            else if(op_str=="e"){item_string.push_back("e");}
        }
        if(bType=="number" && aType=="number"){
        	int a_int=stoi(a_str.erase(0,1));
        	int b_int=stoi(b_str.erase(0,1));
            item_string.push_back("-BothNumbers-");
            if(op_str=="l" && b_int<a_int){item_string.push_back("$1");}
            else if(op_str=="le" && b_int <= a_int){item_string.push_back("$1");}
            else if(op_str=="e" && b_int == a_int){item_string.push_back("$1");}
            else{item_string.push_back("$0");}
        }
    	return item_string;;
    }

    vector<string> ItemLooker::visit(Mem *i) {
        vector<string> item_string;
        Item* x_item=i->getX();
        string x_str=x_item->accept(&looker).front();
        Item* m_item=i->getM();
        string m_str=m_item->accept(&looker).front();
        item_string.push_back(m_str.erase(0,1)+"("+x_str+")");
    	return item_string;
    }

    void CodeGenerator::visit(Instruction_ret *i) {
        // print("I'm a return");
        myOutput+= " addq $"+numD+", %rsp\nretq\n";
    }

    void CodeGenerator::visit(Instruction_assignment *i) {
    	// print("I'm an assignment");
        Item* Item_source = i->getSource();
        vector<string> v_src=Item_source->accept(&looker);
        string src=v_src.front();
        Item* Item_dest  = i->getDestination();
        vector<string> v_dst=Item_dest->accept(&looker);
        string dst=v_dst.front();
        myOutput+=" movq "+src+", "+dst+"\n";
        // return "";

    }

    /*OLD CODE
    void CodeGenerator::visit(Instruction_operation *i) {
        ItemLooker looker;

        // if (op == "salq" || op == "sarq") {
        //     source = eight_bit_registers[source];
        // }
        // return " ";
    }
    */
    void CodeGenerator::visit(Instruction_arithmetic *i) {
        // print("I'm an arithmetic operation");
        Item* Item_source = i->getSource();
        vector<string> v_src=Item_source->accept(&looker);
        string src=v_src.front();
        Item* Item_dest  = i->getDestination();
        vector<string> v_dst=Item_dest->accept(&looker);
        string dst=v_dst.front();
        Item* Item_op  = i->getOperator();
        vector<string> v_op=Item_op->accept(&looker);
        string op=v_op.front();
        myOutput+=" "+op+" "+src+", "+dst+"\n";
    }
    void CodeGenerator::visit(Instruction_shift *i) {
        // print("I'm an shift operation");
        Item* Item_source = i->getSource();
        vector<string> v_src=Item_source->accept(&looker);
        string src=v_src.front();
        if(v_src.back()=="register"){
        	src=v_src[1];
        }
        Item* Item_dest  = i->getDestination();
        vector<string> v_dst=Item_dest->accept(&looker);
        string dst=v_dst.front();
        Item* Item_op  = i->getOperator();
        vector<string> v_op=Item_op->accept(&looker);
        string op=v_op.front();
        myOutput+=" "+op+" "+src+", "+dst+"\n";
    }
   void CodeGenerator::visit(Instruction_assignment_compare *i) {
       	// print("I'm an assignment compare");
       	Item* Item_source = i->getSource();
       	vector<string> v_src=Item_source->accept(&looker);
       	string src=v_src.front();
       	string back=v_src.back();
       	Item* Item_dest  = i->getDestination();
       	vector<string> v_dst=Item_dest->accept(&looker);
       	string dst=v_dst.front();
       	if (src=="-BothNumbers-"){
       		myOutput+=" movq "+back+", "+dst+"\n";
       	}
       	else{
       		string dst8=v_dst[1];
       		myOutput+=src;
       		myOutput+=" set"+back+" "+dst8+"\n";
       		myOutput+=" movzbq "+dst8+", "+dst+"\n";
       	}
    }

    void CodeGenerator::visit(Instruction_inc *i) {
        // print("I'm an increment operation");
        Item* Item_dest  = i->getDestination();
        vector<string> v_dst=Item_dest->accept(&looker);
        string dst=v_dst.front();
        myOutput+=" inc "+dst+"\n";
    }

    void CodeGenerator::visit(Instruction_dec *i) {
        // print("I'm an decriment operation");
        Item* Item_dest  = i->getDestination();
        vector<string> v_dst=Item_dest->accept(&looker);
        string dst=v_dst.front();
        myOutput+=" dec "+dst+"\n";
    }

    void CodeGenerator::visit(Instruction_goto *i) {
        // print("I'm an goTo operation");
        Item* Item_dest  = i->getDestination();
        vector<string> v_dst=Item_dest->accept(&looker);
        string dst=v_dst.front().erase(0,1);
        myOutput+=" jmp "+dst+"\n";
    }

    void CodeGenerator::visit(Instruction_cjump *i) {
        // print("I'm a cjump");
       	Item* Item_source = i->getCompare();
       	vector<string> v_src=Item_source->accept(&looker);
       	string src=v_src.front();
       	string back=v_src.back();
       	Item* Item_dest  = i->getLabel();
       	vector<string> v_dst=Item_dest->accept(&looker);
       	string dst=v_dst.front().erase(0,1);
       	if (src=="-BothNumbers-"){;
       		if(back=="$1"){
       			myOutput+=" jmp "+dst+"\n";	
       		}
       	}
       	else{
       		myOutput+=src;
       		myOutput+=" j"+back+" "+dst+"\n";
       	}
        
    }

    void CodeGenerator::visit(Instruction_call *i) {
        // print("I'm a call");
        Item* u_item=i->getFunc();
        Item* num_item=i->getParamNum();
        vector<string> hold=u_item->accept(&looker);
        string funcName=hold.front().erase(0,1);
        string isRegister=hold.back();
        string paramNum=num_item->accept(&looker).front();
        int paramNum_int=stoi(paramNum.erase(0,1));
        // print(funcName);
        if(funcName=="_all print 1"){myOutput+=" call print\n";}
        else if(funcName=="_all allocate 2"){myOutput+=" call allocate\n";}
        else if(funcName=="_all input 0"){myOutput+=" call input\n";}
        else if(funcName=="_all tensor-error 1"){myOutput+=" call array_tensor_error_null\n";}
        else if(funcName=="_all tensor-error 3"){myOutput+=" call array_error\n";}
        else if(funcName=="_all tensor-error 4"){myOutput+=" call tensor_error\n";}
        else{
        	int mod=paramNum_int-6;
        	if(mod>0){mod=(mod*8)+8;}
        	else{mod=8;}
        	myOutput+=" subq $"+to_string(mod)+", %rsp\n";
        	if(hold.back()=="register"){funcName="*%"+funcName;}
        	else{funcName=funcName;}
        	myOutput+=" jmp "+funcName+"\n";
        }
    }

    void CodeGenerator::visit(Instruction_at *i) {
		Item* add_item=i->getAdd();
		string add_str=add_item->accept(&looker).front();
		Item* mult_item=i->getMult();
		string mult_str=mult_item->accept(&looker).front();
		Item* by_item=i->getBy();
		string by_str=by_item->accept(&looker).front().erase(0,1);
        Item* saveTo_item=i->getSaveTo();
        string saveTo_str=saveTo_item->accept(&looker).front();
        myOutput+=" lea ("+add_str+", "+mult_str+", "+by_str+"),"+saveTo_str+"\n";
    }

    void CodeGenerator::visit(Instruction_label *i) {
    	// print("I'm an Label");
    	Item* Item_source = i->getLabel();
    	vector<string> v_src=Item_source->accept(&looker);
        string src=v_src.front();
		myOutput+=src.erase(0,1)+":\n";
    }
}
