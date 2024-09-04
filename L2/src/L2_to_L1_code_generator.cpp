#include <string>
#include <iostream>
#include <fstream>
#include <map>

#include "code_generator.h"
#include "liveness.h"
#include "interference.h"
#include "coloring.h"
#include "spiller.h"
#include "L2_to_L1_code_generator.h"

using namespace std;

namespace L2{

    string cg2_registers[16]={"rax","rbx","rbp","r10","r11","r12","r13","r14","r15","rdi","rsi","rdx","rcx","r8","r9","rsp"};
    string cg2_operators[9]={"+=","-=","*=","&=","<<=",">>=","<","<=","="};
    string myOutput2 = "";
    CG2_ItemLooker cg_looker2;
    Graph colored_graph;
    int64_t localsInThisFunction;

    void generate_code_to_L1(Program p){
        //CG_ItemLooker looker; 
        // Open the output file.
        std::ofstream outputFile;
        outputFile.open("prog.L1");
    
        /* 
        * Generate target code
        */ 
        string myEntry=p.entryPointLabel;
        auto function=p.functions;
        
        //INTRO 
        myOutput2+="("+myEntry+"\n";

        CodeGenerator2 codeGenerator;
        std::vector<Function*> functions = p.functions;

        for (auto f : functions) {
            generate_function_to_L1(f);
        }


        /* 
        * Close the output file.
        */ 
        myOutput2+=")\n";
        outputFile << myOutput2;
        outputFile.close();
    
        return ;
    }

    void generate_function_to_L1(Function* f){
        //Take Function and put it into Liveness which gives us output Vector<Lines>
        Function* currentFunction = f;
        Function* otherFunction = new Function();
        otherFunction->name = f->name;
        otherFunction->arguments = f->arguments;
        otherFunction->instructions = f->instructions;
        otherFunction->locals = f->locals;

       
        bool SpilledAVar=true;
        int num=0;
        while(SpilledAVar){
            SpilledAVar=false;
            vector<Line*> liveness_output=L2::liveness_analysis(currentFunction, false);
            //Take the Vector of Lines and input them into the interference graph
            Graph interference_output=L2::interference_graph(liveness_output);
            //Take the Graph and input them into the graph colorer
            colored_graph=L2::color(interference_output);

            //Anaylse the output to see there is a uncolored Node (create a list of vars)
            int64_t biggest_none = -1;
            string biggest_index;
            for(string i : colored_graph.nodeList){
                if (colored_graph.graph[i]->color==RegisterID::none && !(isdigit(i.at(1)))){
                    if (colored_graph.graph[i]->numNeighbors() > biggest_none) {
                        biggest_none = colored_graph.graph[i]->numNeighbors();
                        biggest_index = i;
                    }

                    
                }
            }
            
            if (biggest_none != -1) {
                //currentFunction=L2::spill(currentFunction, biggest_index, "%poopafdjajdfoijasoid_"+to_string(num));
                currentFunction=L2::spill(currentFunction, biggest_index, ("%"+to_string(num)));
                num+=1;
                SpilledAVar=true;
            }

        }
        
        
        for (string i : colored_graph.nodeList) {
            if(isdigit(i.at(1)) && i.at(0) == '%' && colored_graph.graph[i]->color == RegisterID::none) {
                currentFunction = otherFunction;
                vector<Line*> liveness_output=L2::liveness_analysis(currentFunction, false);
                Graph interference_output=L2::interference_graph(liveness_output);
                colored_graph=L2::color(interference_output);
                num = 0;
                for (auto i : colored_graph.nodeList) {
                    currentFunction = spill(currentFunction, i, "%" + to_string(num) + "SUPERSPILLER" + to_string(num) + "DONTFORGETPROTECTION");
                    num++;
                }
                
                liveness_output=L2::liveness_analysis(currentFunction, false);

                interference_output=L2::interference_graph(liveness_output);
                colored_graph=L2::color(interference_output);
                break;
                
            }
        }
        //Should end up with a finished function in L2;

        CodeGenerator2 codeGenerator;
        //print("Function Start");
        string fName = currentFunction->name;
        int64_t numLocals = currentFunction->locals;
        localsInThisFunction = numLocals;
        int64_t numArgs=currentFunction->arguments;
        string numA= to_string(numLocals);
        string numD= to_string(numArgs);
        myOutput2+="("+fName+"\n";
        myOutput2+="	"+numD+" "+numA+"\n";
        for (auto i : currentFunction->instructions) {
            i->accept(&codeGenerator);
        }
        myOutput2+=")";
    
        return ;
    }

    vector<string> CG2_ItemLooker::visit(Register *i) {
    	// print("Visited, Register");
    	vector<string> item_string;
    	RegisterID id = i->getID();
     	item_string.push_back(cg2_registers[id]);
        //item_string.push_back("%"+registers_8_bit[id]);
        item_string.push_back("register");
        return item_string;
        
    }

    vector<string> CG2_ItemLooker::visit(Number *i) {
        // print("Visited number");
        vector<string> item_string;
        int64_t numID=i->getNum();
        item_string.push_back(to_string(numID));
        item_string.push_back("number");
    	return item_string;
    }

    vector<string> CG2_ItemLooker::visit(Label *i) {
        vector<string> item_string;
  		string labelID=i->getLabel().erase(0,1);
  		item_string.push_back(":"+labelID); 
  		item_string.push_back("label");
        return item_string;
    }

    vector<string> CG2_ItemLooker::visit(L *i) {
       	vector<string> item_string;
       	string lID=i->getL().erase(0,1);
       	item_string.push_back("@"+lID);
       	item_string.push_back("l");
    	return item_string;
    }

    vector<string> CG2_ItemLooker::visit(Operator *i) {
        // print("OPerator");
       	vector<string> item_string;
       	OperatorID opID=i->getID();
       	item_string.push_back(cg2_operators[opID]);
       	item_string.push_back("operator");
    	return item_string;
    }

    vector<string> CG2_ItemLooker::visit(Compare *i) {
        vector<string> item_string;
        
        Item* a=i->getA();
        vector<string> aResult=a->accept(&cg_looker2);
        string a_str=aResult.front();
        item_string.push_back(a_str);
        
        Item* b=i->getB();
        vector<string> bResult=b->accept(&cg_looker2);
        string b_str=bResult.front();
        string bType=bResult.back();
        item_string.push_back(b_str);
        
        Item* op=i->getOp();
        string op_str=op->accept(&cg_looker2).front();
        item_string.push_back(op_str);
        

    	return item_string;
    }

    vector<string> CG2_ItemLooker::visit(Mem *i) {
        vector<string> item_string;
        Item* x_item=i->getX();
        string x_str=x_item->accept(&cg_looker2).front();
        Item* m_item=i->getM();
        string m_str=m_item->accept(&cg_looker2).front();
        item_string.push_back("mem "+x_str+" "+m_str);
    	return item_string;
    }

    vector<string> CG2_ItemLooker::visit(Variable *i) {
        // print("Call Variable");
        vector<string> item_string;
        string varName=i->getVar();
        RegisterID id=colored_graph.graph[varName]->color;
        if (id > 15) {
            print("Index out of range");
            print(to_string(id));
        }
        item_string.push_back(cg2_registers[id]);
        //item_string.push_back("%"+registers_8_bit[id]);
        item_string.push_back("register");
        //print(i->getVar());
    	return item_string;
    }



    void CodeGenerator2::visit(Instruction_ret *i) {
        //print("I'm a return");
        // myOutput2 += "r12 <- mem rsp 0\n";
        // myOutput2 += "r13 <- mem rsp 8\n";
        // myOutput2 += "r14 <- mem rsp 16\n";
        // myOutput2 += "r15 <- mem rsp 24\n";
        // myOutput2 += "rbp <- mem rsp 32\n";
        // myOutput2 += "rbx <- mem rsp 40\n";
        myOutput2+= "	return\n";
    }

    void CodeGenerator2::visit(Instruction_assignment *i) {
    	//print("I'm an assignment");
        Item* Item_source = i->getSource();
        vector<string> v_src=Item_source->accept(&cg_looker2);
        string src=v_src.front();
        //print(v_src.back());
        Item* Item_dest  = i->getDestination();
        vector<string> v_dst=Item_dest->accept(&cg_looker2);
        string dst=v_dst.front();
        myOutput2+="	"+dst+" <- "+src+"\n";
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
    void CodeGenerator2::visit(Instruction_arithmetic *i) {
        //print("I'm an arithmetic operation");
        Item* Item_source = i->getSource();
        vector<string> v_src=Item_source->accept(&cg_looker2);
        string src=v_src.front();
        Item* Item_dest  = i->getDestination();
        vector<string> v_dst=Item_dest->accept(&cg_looker2);
        string dst=v_dst.front();
        Item* Item_op  = i->getOperator();
        vector<string> v_op=Item_op->accept(&cg_looker2);
        string op=v_op.front();
        myOutput2+="	"+dst+" "+op+" "+src+"\n";
    }
    void CodeGenerator2::visit(Instruction_shift *i) {
        // print("I'm an shift operation");
        Item* Item_source = i->getSource();
        vector<string> v_src=Item_source->accept(&cg_looker2);
        string src=v_src.front();
        if(v_src.back()=="register"){
        	src=v_src[0]; // 1
        }
        Item* Item_dest  = i->getDestination();
        vector<string> v_dst=Item_dest->accept(&cg_looker2);
        string dst=v_dst.front();
        Item* Item_op  = i->getOperator();
        vector<string> v_op=Item_op->accept(&cg_looker2);
        string op=v_op.front();
        myOutput2+="	"+dst+" "+op+" "+src+"\n";
    }
   void CodeGenerator2::visit(Instruction_assignment_compare *i) {
       	// print("I'm an assignment compare");
       	Item* Item_source = i->getSource();
       	vector<string> v_src=Item_source->accept(&cg_looker2);
       	string a_str=v_src[0];
       	string b_str=v_src[1];
        string op_str=v_src[2];
       	Item* Item_dest  = i->getDestination();
       	vector<string> v_dst=Item_dest->accept(&cg_looker2);
       	string dst=v_dst.front();
       	myOutput2+="	"+dst+" <- "+b_str+" "+op_str+" "+a_str+"\n";
       	
    }

    void CodeGenerator2::visit(Instruction_assignment_stack_arg *i) {
        // print("I'm an assignement stack");
        Item* Item_dest = i->getDestination();
        string dst=Item_dest->accept(&cg_looker2).front();
        Item* Item_src = i->getSource();
        string src=Item_src->accept(&cg_looker2).front();
        myOutput2+="	"+dst+" <- mem rsp "+ to_string(stoi(src) + (localsInThisFunction * 8))+"\n";
    }

    void CodeGenerator2::visit(Instruction_inc *i) {
        // print("I'm an increment operation");
        Item* Item_dest  = i->getDestination();
        vector<string> v_dst=Item_dest->accept(&cg_looker2);
        string dst=v_dst.front();
        myOutput2+="	"+dst+"++\n";
    }

    void CodeGenerator2::visit(Instruction_dec *i) {
        // print("I'm an decriment operation");
        Item* Item_dest  = i->getDestination();
        vector<string> v_dst=Item_dest->accept(&cg_looker2);
        string dst=v_dst.front();
        myOutput2+="	"+dst+"--\n";
    }

    void CodeGenerator2::visit(Instruction_goto *i) {
        // print("I'm an goTo operation");
        Item* Item_dest  = i->getDestination();
        vector<string> v_dst=Item_dest->accept(&cg_looker2);
        string dst=v_dst.front().erase(0,1);
        myOutput2+="	goto :"+dst+"\n";
    }

    void CodeGenerator2::visit(Instruction_cjump *i) {
        // print("I'm a cjump");
       	Item* Item_source = i->getCompare();
       	vector<string> v_src=Item_source->accept(&cg_looker2);
       	string a_str=v_src[0];
       	string b_str=v_src[1];
        string op_str=v_src[2];
       	Item* Item_dest  = i->getLabel();
       	vector<string> v_dst=Item_dest->accept(&cg_looker2);
       	string dst=v_dst.front().erase(0,1);
        myOutput2+="	cjump "+b_str+" "+op_str+" "+a_str+" :"+dst+"\n";

        
    }

    void CodeGenerator2::visit(Instruction_call *i) {
        // print("I'm a call");
        Item* u_item=i->getFunc();
        Item* num_item=i->getParamNum();
        vector<string> hold=u_item->accept(&cg_looker2);
        string funcName=hold.front();
        string isRegister=hold.back();
        if(funcName==":all print 1"){myOutput2+=" call print 1\n";}
        else if(funcName==":all allocate 2"){myOutput2+=" call allocate 2\n";}
        else if(funcName==":all input 0"){myOutput2+=" call input 0\n";}
        else if(funcName==":all tensor-error 1"){myOutput2+=" call tensor-error 1\n";}
        else if(funcName==":all tensor-error 3"){myOutput2+=" call tensor-error 3\n";}
        else if(funcName==":all tensor-error 4"){myOutput2+=" call tensor-error 4\n";}
        else{
        string paramNum=num_item->accept(&cg_looker2).front();\
        //print(paramNum);
        int paramNum_int=stoi(paramNum);
        myOutput2+="	call "+funcName+" "+paramNum+"\n";
        }
    }

    void CodeGenerator2::visit(Instruction_at *i) {
		Item* add_item=i->getAdd();
		string add_str=add_item->accept(&cg_looker2).front();
		Item* mult_item=i->getMult();
		string mult_str=mult_item->accept(&cg_looker2).front();
		Item* by_item=i->getBy();
		string by_str=by_item->accept(&cg_looker2).front()/*.erase(0,1)*/;
        Item* saveTo_item=i->getSaveTo();
        string saveTo_str=saveTo_item->accept(&cg_looker2).front();
        myOutput2+="	"+saveTo_str+" @ "+add_str+" "+mult_str+" "+by_str+"\n";
    }

    void CodeGenerator2::visit(Instruction_label *i) {
    	//print("I'm an Label");
    	Item* Item_source = i->getLabel();
    	vector<string> v_src=Item_source->accept(&cg_looker2);
        string src=v_src.front();
		myOutput2+="	"+src+"\n";
    }
}
