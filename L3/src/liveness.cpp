#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <set>

#include "liveness.h"
#include <algorithm>

using namespace std;

namespace L3 {

    //string registers[16]={"rax","rbx","rbp","r10","r11","r12","r13","r14","r15","rdi","rsi","rdx","rcx","r8","r9","rsp"};
    // string operators[9]={"addq","subq","imulq","andq","salq","sarq","l","le","e"};
    // string registers_8_bit[16]={"al","bl","bpl","r10b","r11b","r12b","r13b","r14b","r15b","dil","sil","dl","cl","r8b","r9b","rsp"};
    
    ItemLookerLiveness looker_liveness;
    vector<Line*> lines;
    bool printAll_LIVENESS=false;
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
    vector<Line*> liveness_analysis(Function* f){
        lines.clear();
        string myOutput = "";
        //print("Liveness Analysis Start");
        //ItemLooker looker_liveness; 
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
        //Output File Fill

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

        // Print the lines
        // printAll();
        // print("Writing to file");
        /* 
        * Close the output file.
        */ 
        //print(myOutput);
        //outputFile << myOutput;
        //outputFile.close();
    
        return lines;
    }
    // ItemLooker visitor methods
    // Registers start with r, variables with %, labels with _
    // Need to add functions somehow
 vector<string> ItemLookerLiveness::visit(Number *i) {
    vector<string> item_string;
    if(printAll_LIVENESS){printe("Number");}
    item_string.push_back("Number");
    item_string.push_back(to_string(i->get_number()));
    return item_string;
  }

  vector<string> ItemLookerLiveness::visit(Label *i) {
    vector<string> item_string;
    if(printAll_LIVENESS){printe("Label");}
    item_string.push_back("Label");
    item_string.push_back(i->get_name());
    return item_string;
  }

  vector<string> ItemLookerLiveness::visit(L *i) {
    vector<string> item_string;
    if(printAll_LIVENESS){printe("L");}
    item_string.push_back("L");
    item_string.push_back(i->get_name());
    return item_string;
  }

  vector<string> ItemLookerLiveness::visit(Variable *i) {
    vector<string> item_string;
    if(printAll_LIVENESS){printe("Variable");}
    item_string.push_back("Variable");
    item_string.push_back(i->get_name());
    return item_string;
  }

  vector<string> ItemLookerLiveness::visit(Compare_Operator *i) {
    vector<string> item_string;
    if(printAll_LIVENESS){printe("Compare_Operator");}
    item_string.push_back("Compare_Operator");
    item_string.push_back(to_string(i->get_op()));
    return item_string;
  }

  vector<string> ItemLookerLiveness::visit(Arithmetic_Operator *i) {
    vector<string> item_string;
    if(printAll_LIVENESS){printe("Arithmetic_Operator");}
    item_string.push_back("Arithmetic_Operator");
    item_string.push_back(to_string(i->get_op()));
    return item_string;
  }

  vector<string> ItemLookerLiveness::visit(Vars *i) {
    vector<string> item_string;
    if(printAll_LIVENESS){printe("new vars");}
    item_string.push_back("Variables");
    item_string.push_back(to_string(i->getNumVars()));
    for (auto arg : i->getVars()) {
      string arg_string = arg->accept(&looker_liveness).front();
      item_string.push_back(arg_string);
    }
    return item_string;
  }

  vector<string> ItemLookerLiveness::visit(Args *i) {
    vector<string> item_string;
    if(printAll_LIVENESS){printe("Args");}
    item_string.push_back("Args");
    item_string.push_back(to_string(i->getNumArgs()));
    for (auto arg : i->getArgs()) {
      string arg_type = arg->accept(&looker_liveness).front();
      string arg_string = arg->accept(&looker_liveness).back();
      if(arg_type=="Variable"){
        item_string.push_back(arg_string);
      }
    }

    return item_string;
  }


  void Liveness::visit(Instruction_assignment *i) {
    set<string> gen;
    set<string> kill;
    auto src = i->getSource()->accept(&looker_liveness);
    auto dst = i->getDestination()->accept(&looker_liveness);
    // Need to check the type of src
    if (src.front()=="Variable"){
        gen.insert(src.back());
    }
    if(dst.front()=="Variable"){
        kill.insert(dst.back());
    }
    auto line = new Line(dst.front(), gen, kill,{"Next"});
    lines.push_back(line);
    
  }

  void Liveness::visit(Instruction_assignment_op *i) {
    set<string> gen;
    set<string> kill;
    if(printAll_LIVENESS){printe("Assignment op");}
    vector<Item*> sources = i->getSources();
    Item* dest = i->getDestination();
    auto dst=dest->accept(&looker_liveness);
    //Item* op = i->getOperator();
    for(auto source : sources){
        auto src=source->accept(&looker_liveness);
        if (src.front()=="Variable"){
            gen.insert(src.back());
        }
    }
    if(dst.front()=="Variable"){
        //gen.insert(dst.back())
        kill.insert(dst.back());
    }
    auto line = new Line(dst.front(), gen, kill,{"Next"});
    lines.push_back(line);
    return;
}

  void Liveness::visit(Instruction_assignment_cmp *i) {
    set<string> gen;
    set<string> kill;
    if(printAll_LIVENESS){printe("Assignment compare");}
    vector<Item*> sources = i->getSources();
    Item* dest = i->getDestination();
    auto dst=dest->accept(&looker_liveness);
    //Item* op = i->getOperator();
    for(auto source : sources){
        auto src=source->accept(&looker_liveness);
        if (src.front()=="Variable"){
            gen.insert(src.back());
        }
    }
    if(dst.front()=="Variable"){
        //gen.insert(dst.back())
        kill.insert(dst.back());
    }
    auto line = new Line(dst.front(), gen, kill,{"Next"});
    lines.push_back(line);
    return;
  }

  void Liveness::visit(Instruction_return *i) {
    if(printAll_LIVENESS){printe("return");} 
    set<string> gen;
    set<string> kill;
    auto line = new Line("return", gen, kill, {});
    lines.push_back(line);
    return;
  }

  void Liveness::visit(Instruction_return_arg *i) {
    if(printAll_LIVENESS){printe("return arg");}
    set<string> gen;
    set<string> kill;
    Item* value = i->get_value();
    auto val = value->accept(&looker_liveness);
    if(val.front()=="Variable"){
        //gen.insert(dst.back())
        kill.insert(val.back());
    }
    auto line = new Line("return", gen, kill, {});
    lines.push_back(line);
    return;
  }

  void Liveness::visit(Instruction_load *i) {
    set<string> gen;
    set<string> kill;
    if(printAll_LIVENESS){printe("load");}
    auto src = i->getSource()->accept(&looker_liveness);
    auto dst = i->getDestination()->accept(&looker_liveness);
    // Need to check the type of src
    if (src.front()=="Variable"){
        gen.insert(src.back());
    }
    if(dst.front()=="Variable"){
        kill.insert(dst.back());
    }
    auto line = new Line(dst.front(), gen, kill,{"Next"});
    lines.push_back(line);
  }

  void Liveness::visit(Instruction_store *i) {
    set<string> gen;
    set<string> kill;
    if(printAll_LIVENESS){printe("store");}
    auto src = i->getSource()->accept(&looker_liveness);
    auto dst = i->getDestination()->accept(&looker_liveness);
    // Need to check the type of src
    if (src.front()=="Variable"){
        gen.insert(src.back());
    }
    if(dst.front()=="Variable"){
        kill.insert(dst.back());
    }
    auto line = new Line(dst.front(), gen, kill,{"Next"});
    lines.push_back(line);
  }

  void Liveness::visit(Instruction_branch *i) {
    set<string> gen;
    set<string> kill;
    if(printAll_LIVENESS){printe("branch");}
    Item* dest = i->getDestination();
    auto dst = dest->accept(&looker_liveness);
    auto line = new Line(dst.back(), gen, kill,{dst.back()});
    lines.push_back(line);
    return;
  }

  void Liveness::visit(Instruction_branch_conditional *i) {
    set<string> gen;
    set<string> kill;
    if(printAll_LIVENESS){printe("branch conditional");}
    Item* conditional = i->getCondition();
    Item* dest = i->getDestination();
    auto dst = dest->accept(&looker_liveness);
    auto con = conditional->accept(&looker_liveness);


    // Need to check the type of src
    if (con.front()=="Variable"){
        gen.insert(con.back());
    }
    auto line = new Line(dst.front(), gen, kill,{dst.back(),"Next"});
    lines.push_back(line);
    return;
  }

  void Liveness::visit(Instruction_call *i) {
    set<string> gen;
    set<string> kill;
    Item* args_items = i->getArgs();
    auto args = args_items->accept(&looker_liveness);

    Item* return_item = i->getCallee();//what is this
    string function_type = return_item->accept(&looker_liveness).front();
    string function_name = return_item->accept(&looker_liveness).back();
    if(function_type=="Variable"){kill.insert(function_name);}
    for (auto arg : args){
        if(arg!="Args"){
            kill.insert(arg);
        }
    }
    auto line = new Line("call", gen, kill,{"Next"});
    lines.push_back(line);
    return;
  }

  void Liveness::visit(Instruction_assignment_call *i) {
    set<string> gen;
    set<string> kill;
    Item* args_items = i->getArgs();
    auto args = args_items->accept(&looker_liveness);

    Item* return_item = i->getCallee();//what is this
    string function_type = return_item->accept(&looker_liveness).front();
    string function_name = return_item->accept(&looker_liveness).back();

    Item* dest = i->getDst();
    auto dst = dest->accept(&looker_liveness);
    if(dst.front()=="Variable"){
        gen.insert(dst.back());
    }


    if(function_type=="Variable"){kill.insert(function_name);}
    for (auto arg : args){
        if(arg!="Args"){
            kill.insert(arg);
        }
    }
    auto line = new Line("call_assignment", gen, kill,{"Next"});
    lines.push_back(line);
    return;
  }

  void Liveness::visit(Instruction_label *i) {
    set<string> gen;
    set<string> kill;
    string label = i->get_label()->accept(&looker_liveness).back();

    auto line = new Line(label, gen, kill, {"Next"});
    lines.push_back(line);
    return;
  }
  
}
