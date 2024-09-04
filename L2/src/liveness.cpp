#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <set>

#include "liveness.h"
#include <algorithm>

using namespace std;

namespace L2 {

    string registers[16]={"rax","rbx","rbp","r10","r11","r12","r13","r14","r15","rdi","rsi","rdx","rcx","r8","r9","rsp"};
    // string operators[9]={"addq","subq","imulq","andq","salq","sarq","l","le","e"};
    // string registers_8_bit[16]={"al","bl","bpl","r10b","r11b","r12b","r13b","r14b","r15b","dil","sil","dl","cl","r8b","r9b","rsp"};
    
    ItemLooker looker;
    vector<Line*> lines;
    // Line class imported from python program
    Line::Line(std::string name, set<string> gen, set<string> kill, vector<string> fs)
      : name { name },
      	gen { gen },
        kill { kill },
        future_successors {fs} {
            sopName = "";
        return ;
    }

    bool Line::updateSelf() {
    	// cout<<"UpdateSelf Called"<<endl;
        bool didUpdate = false;

        set<string> result;
        set<string> result2;
        
        set_difference(out.begin(), out.end(), kill.begin(), kill.end(), inserter(result, result.end()));
        set_union(gen.begin(), gen.end(), result.begin(), result.end(), inserter(result2, result2.end()));
        // print("A");
        // auto a = kill.begin();
        // print("Begin");
        // auto b = kill.end();
        // print("End");
        // outCopy.erase(a, b);
        // print("A");
        // genCopy.insert(out.begin(), out.end());
        // print("A");
        if (result2 != in) {
            didUpdate = true;
            in = result2;
        }
        
        // print("Done in");

        // vector<string> keep;
        // for (auto item : out) {
        //     bool hold = true;
        //     for (auto item2 : kill) {
        //         if (item == item2) {
        //             hold = false;
        //         }
        //     }
        //     if (hold) {
        //         keep.push_back(item);
        //     }
        // }
        
        // Update IN
        // set<string> newIn = gen;
        // newIn.insert(newIn.end(), keep.begin(), keep.end());
        //sort(newIn.begin(),newIn.end());
       /* if(name=="cjump"){
        	print("Loop,newIn:");
        	for(string i : newIn){
        		cout<<i<<endl;
        	}
        	print("Loop,oldIn:");
        	for(string i : in){
           		cout<<i<<endl;
        	}
        	if(newIn!=in){
        		print("Would Update");
        	}
      		print("");
        }*/

        // if (newIn != in) {
        //   	//cout<<"InUpdate"<<endl;
        //     in = newIn;
        //     //print("NewIn:"+name);
        //     didUpdate = true;
        // }
        // else{
        // 	//cout<<"InDone"<<endl;
        // 	int i=0;
        // }

        // Update OUT
        set<string> newOut;
        
        for (auto successor : successors) {
        	set<string> curIn=successor->in;
            set_union(newOut.begin(), newOut.end(), curIn.begin(), curIn.end(), inserter(newOut, newOut.end()));
            // newOut.insert(curIn.begin(), curIn.end());
        	// for(auto j : curIn){
        	// 	if(find(allIn.begin(),allIn.end(),j)==allIn.end()){
        	// 		allIn.push_back(j);
        	// 	}
            // }
       	}
        // print("Done out");
       	// sort(allIn.begin(),allIn.end());
        
        // for(auto i : allIn){
        // 	bool hold=true;
        // 	for(auto e : newOut){
        // 		if(e==i){
        // 			hold=false;
        // 		}
        // 	}
        // 	if(hold){newOut.push_back(i);}         
        // }

		/*
        if(name=="cjump"){
        	print("Loop,newOut:");
        	for(string i : newOut){
        		cout<<i<<endl;
        	}
        	print("Loop,oldOut:");
        	for(string i : out){
           		cout<<i<<endl;
        	}
        	if(newOut!=out){
        		print("Would Update");
        	}
      		print("");
        }
        */
        
        if (out != newOut) {
            // print("TRUE");
            //cout<<"OutUpdate"<<endl;
           	out=newOut;
           	
            didUpdate = true;
        }
        // print("Done");
        return didUpdate;
    }

	void printAll(){
		// Print the lines
		std::cout<<"-----------------"<<endl;
		for (auto line : lines) {
		   	std::cout<< "-NewLine: ";
		   	std::cout<< line->name+"\n";
		   	std::cout<< "Gen: ";
		for (auto i : line->gen) {
		    std::cout << i << " ";
		}
		    std::cout << "\n";
		    std::cout<< "Kill: ";
		for (auto i : line->kill) {
		    std::cout << i << " ";
		}
		    std::cout << "\n";
		    std::cout<< "In: ";
		for (auto i : line->in) {
		    std::cout << i << " ";
		}
		    std::cout << "\n";
		    std::cout<< "Out: ";
		for (auto i : line->out) {
		    std::cout << i << " ";
		}
		    std::cout << "\n";
		    std::cout<< "Successors: ";
		for (auto i : line->successors) {
		    std::cout << i->name << " ";
		}
		if(line->successors.size()==0){std::cout<<"None";}
		    std::cout << "\n";
		}
	}
    
    // Renamed code_generator to liveness_analysis
    vector<Line*> liveness_analysis(Function* f, bool printOutput){
        lines.clear();
        string myOutput = "";
        //print("Liveness Analysis Start");
        //ItemLooker looker; 
        // Open the output file.
        //std::ofstream outputFile;
        //outputFile.open("prog.txt");

        // Setup
        /*
        auto function=p.functions;
        std::vector<Function*> functions = p.functions;
        */
        Liveness liveness;

        // Loop through instructions
        //for (auto f : functions) {
        //Populate Lines
        // print("Populate lines");
            for (auto i : f->instructions) {
                i->accept(&liveness);
                //print("Looping in populate");
            }
        // }
        // print("Populate successors");
		//Populate Successors
        for (int curr=0;curr<lines.size();curr++){
        	//print("Looping in Successors");
        	for(auto fs : lines[curr]->future_successors){
        		if (fs=="Next"){
        			lines[curr]->successors.push_back({lines[curr+1]});
        		}
        		else if (fs=="None"){
        			lines[curr]->successors={};
        			break;
        		}
        		else{
        			for(auto line : lines){
        			    if (line->name==fs){
        			    	//print("GoTo Filled");
        			    	lines[curr]->successors.push_back({line});
        			    	break;
        			    }
        			}
        		}
        		if(lines[curr]->successors.size()>1){
        			//print("ok");
        			for(auto s : lines[curr]->successors){
        				//print(s->name);
        			}
        		}
			}
        }

        //Update Loop
        bool isUpdated=true;
        while(isUpdated){
        	isUpdated=false;
        	// print("Looping in Update");
        	int curr=lines.size()-1;
        	while (curr>=0){
        		if(isUpdated){
        			lines[curr]->updateSelf();
        		}	
        		else{
        			isUpdated=lines[curr]->updateSelf();
        		}
        		curr--;
        	}
            
        }

        // Print if liveness testing
        if (printOutput) {
            string in_txt="(\n(in\n";
            string out_txt=")\n\n(out\n";
            for(auto line : lines){
                in_txt+="(";
                for (auto i:line->in){
                    in_txt+=i+" ";
                }
                in_txt+=")\n";

                out_txt+="(";
                for (auto i:line->out){
                    out_txt+=i+" ";
                }
                out_txt+=")\n";
            }
            out_txt+=")\n\n)";
            myOutput=in_txt+out_txt;

            print(myOutput);
        }
    
        return lines;
    }
    // ItemLooker visitor methods
    // Registers start with r, variables with %, labels with _
    // Need to add functions somehow
    vector<string> ItemLooker::visit(Register *i) {
    	vector<string> item_string;
        string r = registers[i->getID()];
        //print(r);
        item_string.push_back(r);
        return item_string;
        
    }

    vector<string> ItemLooker::visit(Number *i) {
        vector<string> item_string;
        item_string.push_back("number");
        item_string.push_back(to_string(i->getNum()));
    	return item_string;
    }

    vector<string> ItemLooker::visit(Label *i) {
        vector<string> item_string;
        item_string.push_back(i->getLabel());
    	return item_string;
    }

    vector<string> ItemLooker::visit(Variable *i) {
        vector<string> item_string;
        item_string.push_back(i->getVar());
    	return item_string;
    }

    vector<string> ItemLooker::visit(L *i) {
       	vector<string> item_string;
        item_string.push_back(i->getL());
    	return item_string;
    }

    vector<string> ItemLooker::visit(Operator *i) {
       	vector<string> item_string;
        //item_string.push_back("operator");
    	return item_string;
    }

    // Compare can return 2 registers or variables 

    vector<string> ItemLooker::visit(Compare *i) {
        ItemLooker looker;
        vector<string> item_string;
        // No need to check a and b, since they are always r or %
        auto a = i->getA()->accept(&looker);
        auto b = i->getB()->accept(&looker);
        if (!a.empty()) {
            item_string.push_back(a.front());
        }
        if (!b.empty()) {
            item_string.push_back(b.front());
        }
    	return item_string;
    }

    vector<string> ItemLooker::visit(Mem *i) {
        ItemLooker looker;
        vector<string> item_string;
        // Same here
        item_string.push_back(i->getX()->accept(&looker).front());
        item_string.push_back("mem");
    	return item_string;
    }

    void Liveness::visit(Instruction_ret *i) {
        set<string> gen;
        set<string> kill;
        // Extra generation things, need to fill callee with the actual ones
        gen.insert("rax");
        gen.insert("r12");
        gen.insert("r13");
        gen.insert("r14");
        gen.insert("r15");
        gen.insert("rbp");
        gen.insert("rbx");
        auto line = new Line("return", gen, kill, {});
        lines.push_back(line);
    }

    void Liveness::visit(Instruction_assignment *i) {
        set<string> gen;
        set<string> kill;
    	auto src = i->getSource()->accept(&looker);
        // Need to check the type of src
        if (src.front().at(0) == 'r' || src.front().at(0) == '%'){
            gen.insert(src.front());
        }
        auto dst = i->getDestination()->accept(&looker);
        if(dst.back()=="mem"){
        	if(dst.front()!="rsp"){
        		gen.insert(dst.front());
        	}
        	auto line = new Line("mem",gen, kill,{"Next"});
        	lines.push_back(line);
        }
        else{
          	kill.insert(dst.front());
            // kill.front() -> dst.front()
          	auto line = new Line(dst.front(), gen, kill,{"Next"});
          	lines.push_back(line);
        }
        
    }

    void Liveness::visit(Instruction_arithmetic *i) {
        set<string> gen;
        set<string> kill;
    	auto src = i->getSource()->accept(&looker);
        // Check the type again
        if (src.front().at(0) == 'r' || src.front().at(0) == '%') {
            gen.insert(src.front());
        }
        auto dst = i->getDestination()->accept(&looker);
        gen.insert(dst.front());
        kill.insert(dst.front());

        auto line = new Line("arithmetic", gen, kill, {"Next"});
        lines.push_back(line);
    }

    void Liveness::visit(Instruction_shift *i) {
        set<string> gen;
        set<string> kill;
    	auto src = i->getSource()->accept(&looker);
        string name = "";
        // Why the empty check?
        if (!src.empty()) {
            if (src.front().at(0) == 'r' || src.front().at(0) == '%') {
                gen.insert(src.front());
                // need to test this
                name = src.front();
                //print(src.front());
            }
        }
        auto dst = i->getDestination()->accept(&looker);

		gen.insert(dst.front());
        kill.insert(dst.front());


        auto line = new Line("shift",gen, kill,{"Next"});
        line->sopName = name;
        lines.push_back(line);
    }

    void Liveness::visit(Instruction_assignment_compare *i) {
    	//print("comare_ass ran");
        set<string> gen;
        set<string> kill;
    	auto src = i->getSource()->accept(&looker);
    	
        // Compare can return 0, 1, or 2 elements
        //print(src.back());
        //print(src.front());
        if (src.front().at(0) == 'r' || src.front().at(0) == '%') {
        	//print(src.back()+"ok");
            gen.insert(src.front());
        }
        if(src.back()!=src.front()){
        	if (src.back().at(0) == 'r' || src.back().at(0) == '%') {
        		//print(src.front()+"ok");
            	gen.insert(src.back());
        	}
        }
        auto dst = i->getDestination()->accept(&looker);
        //print(dst.front());
        kill.insert(dst.front());

        auto line = new Line("assignment_compare", gen, kill,{"Next"});
        lines.push_back(line);
    }

    void Liveness::visit(Instruction_assignment_stack_arg *i) {
        set<string> gen;
        set<string> kill;
    	
        auto dst = i->getDestination()->accept(&looker);
        
        kill.insert(dst.front());

        auto line = new Line("assignment_stack_arg", gen, kill,{"Next"});
        lines.push_back(line);
    }

    void Liveness::visit(Instruction_inc *i) {
        set<string> gen;
        set<string> kill;
        auto dst = i->getDestination()->accept(&looker);
        gen.insert(dst.front());
        kill.insert(dst.front());

        auto line = new Line("inc", gen, kill,{"Next"});
        lines.push_back(line);
    }

    void Liveness::visit(Instruction_dec *i) {
        set<string> gen;
        set<string> kill;
        auto dst = i->getDestination()->accept(&looker);
        gen.insert(dst.front());
        kill.insert(dst.front());

        auto line = new Line("dec", gen, kill,{"Next"});
        lines.push_back(line);
    }

    void Liveness::visit(Instruction_goto *i) {
        set<string> gen;
        set<string> kill;
        auto dst = i->getDestination()->accept(&looker);

        auto line = new Line("goto", gen, kill,{dst.front()});
        lines.push_back(line);        
    }

    void Liveness::visit(Instruction_cjump *i) {
        set<string> gen;
        set<string> kill;

        auto compare = i->getCompare()->accept(&looker);
        auto dst = i->getLabel()->accept(&looker);
        // Compare can return 0, 1, or 2 elements
        if (compare.front().at(0) == 'r' || compare.front().at(0) == '%') {
            gen.insert(compare.front());
        }
        if(compare.front()!=compare.back()){
        	if (compare.back().at(0) == 'r' || compare.back().at(0) == '%') {
            	gen.insert(compare.back());
        	}
        }

        auto line = new Line("cjump",gen, kill,{dst.front(),"Next"});
        lines.push_back(line);
    }

    void Liveness::visit(Instruction_at *i) {
        set<string> gen;
        set<string> kill;
        auto saveTo = i->getSaveTo()->accept(&looker);
        if (saveTo.front().at(0) == 'r' || saveTo.front().at(0) == '%') {
            kill.insert(saveTo.front());
        }

        auto add = i->getAdd()->accept(&looker);
        if (add.front().at(0) == 'r' || add.front().at(0) == '%') {
            gen.insert(add.front());
        }

        auto mult = i->getMult()->accept(&looker);
        if (mult.front().at(0) == 'r' || mult.front().at(0) == '%') {
            gen.insert(mult.front());
        }

        auto line = new Line("at", gen, kill, {"Next"});
        lines.push_back(line);
    }

    void Liveness::visit(Instruction_call *i) {
        set<string> gen;
        set<string> kill;
        auto u_item = i->getFunc();
        auto num_item=i->getParamNum();
        vector<string> hold=u_item->accept(&looker);
        string funcName=hold.front();
        //print(funcName);
        string paramNum=num_item->accept(&looker).back();
        int paramNum_int=stoi(paramNum);
        vector<string>args={"rdi","rsi","rdx","rcx","r8","r9"};
        for(int i=0; (i<paramNum_int&&i<6);i++){
        	gen.insert(args[i]);
        }
        //CALLER
        kill.insert("r10");
        kill.insert("r11");
        kill.insert("r8");
        kill.insert("r9");
        kill.insert("rax");
        kill.insert("rcx");
        kill.insert("rdi");
        kill.insert("rdx");
        kill.insert("rsi");
        if((funcName=="call tensor-error 1")||(funcName=="call tensor-error 3") || (funcName=="call tensor-error 4")){
        	auto line = new Line("call",gen, kill,{});
        	lines.push_back(line);
        }
        else if((funcName=="call print 1")||(funcName=="call allocate 2") || (funcName=="call input 0")){
                	auto line = new Line("call",gen, kill,{"Next"});
                	lines.push_back(line);
        }
        else{
        	if (hold.front().at(0) == 'r' || hold.front().at(0) == '%') {
        		//print(funcName);
            	gen.insert(hold.front());
            }
        	auto line = new Line("call",gen, kill,{"Next"});
        	lines.push_back(line);
        }
    }

    void Liveness::visit(Instruction_label *i) {
        set<string> gen;
        set<string> kill;
        string label = i->getLabel()->accept(&looker).front();

        auto line = new Line(label, gen, kill, {"Next"});
        lines.push_back(line);
    }
}
