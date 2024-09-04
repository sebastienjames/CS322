#include <string>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <map>

#include "liveness.h"
#include "interference.h"


using namespace std;

namespace L2 {

    Vertex::Vertex (string s)  
    : name(s) {
        color = RegisterID::none ;
    }
    
    // Graph DST
    // Change type to bool for when is dupe
    bool Graph::addvertex(const string &name) {
        // print("Adding vertex " + name);
        vmap::iterator itr = graph.find(name);
        if (itr == graph.end())
        {
            Vertex *v;
            v = new Vertex(name);
            graph[name] = v;
            nodeList.push_back(name);
            // print("New vertex " + name);
            return true;
        }
        // print("Vertex " + name + " already exists!");
        return false;
    }

    void Graph::addedge(const string& from, const string& to) {
        // print("Adding edge from " + from + " to " + to);
        // Vertex *f = (graph.find(from)->second);
        // Vertex *t = (graph.find(to)->second);
        // print("do f");
        Vertex *f = graph[from];
        // print("f is " + f->name);
        // print("do t");
        Vertex *t = graph[to];
        // print("t is " + t->name);
        // print("do loop");
        for (auto vert : f->adj) {
            // print("start loop");
            if (vert->name == to) {
                // print(from + " already has edge to " + to);
                return;
            }
            // print("end loop");
        }
        // print("done loop");
        f->adj.insert(t);
        t->adj.insert(f);
    }

    void Graph::print_graph() {
        // print("START ADJ LIST");
        vector<string> regs = {"rax","rbx","rbp","r10","r11","r12","r13","r14","r15","rdi","rsi","rdx","rcx","r8","r9","rsp", "none"};
        for (auto node : nodeList) {
            if (node != "rsp") {
                auto vertex = graph[node];
                cout << vertex->name << " ";
                for (auto adj : vertex->adj) {
                    if (adj->name != "rsp") {
                        cout << adj->name << " ";
                    }
                }
                // cout << " Colored with " << regs[vertex->color];
                cout << "\n";
            }
        }
        // print("END");
    }
    
    // Renamed code_generator to liveness_analysis
    Graph interference_graph(vector<Line*> lines){
        // print("Interference Start");
        //ItemLooker looker; 
        // Open the output file.
        std::ofstream outputFile;
        outputFile.open("prog.txt");

        // Setup
        /*
        auto function=p.functions;
        std::vector<Function*> functions = p.functions;
        */
        Liveness liveness;
        Graph interference;

        // print("Start interference");
        // Make the interference graph
        vector<string> seen;
        vector<string> regs = {"rax","rbx","rbp","r10","r11","r12","r13","r14","r15","rdi","rsi","rdx","rcx","r8","r9","rsp"};
        vector<string> sop_regs = {"rax","rbx","rbp","r10","r11","r12","r13","r14","r15","rdi","rsi","rdx","r8","r9","rsp"};


        // Add the kill vertices

        for (auto line : lines) {
            // print(r);
            for (auto kill : line->kill) {
                interference.addvertex(kill);
            }
        }

        // Set GP registers and fill their edges
        // print("Setting GP registers");

        for (auto r : regs) {
            // print(r);
            interference.addvertex(r);
        }

        

        for (auto from : regs) {
            for (auto to : regs) {
                if (from != to) {
                    interference.addedge(from, to);
                }
            }
        }

        // print("Set vertices");
        // Set vertices
        // Put all graph vertices first, otherwise we try to create edges to nodes that don't exist yet
        for (auto line : lines) {
            for (string node : line->in) {
                // If the node doesn't exist in the graph
                if(interference.addvertex(node)) {
                    // interference.nodeList.push_back(node);
                }
                
            }
        }
        
        // Set edges
        for (auto line : lines) {
            for (string from : line->in) {
                // This is dumb, 3 nested for loops
                for (string to : line->in) {
                    // If not a path to itself
                    if (from != to) {
                        interference.addedge(from, to);
                    }
                }
            }
            
            for (string from : line->kill) {
                // This is dumb, 3 nested for loops
                for (string to : line->out) {
                    // If not a path to itself
                    if (from != to) {
                        interference.addedge(from, to);
                    }
                }
            }

            // Shift operation, need to connect to all registers except rcx
            if (line->name == "shift") {
                // print("SHIFT");
                // print(line->sopName);
                // print(to_string(line->gen.size()));
                
                // if (line->gen.size() == 2) {
                if (line->sopName != "") {
                    // print(line->sopName);

                    for (auto to : sop_regs) {
                        // print("added edge from " + line->sopName + " to " + to);
                        interference.addedge(line->sopName, to);
                    }
                }
            }
        }

        //Output File Fill
        string myOutput;
        // Print the lines
        // print("Writing to file");
        auto g = interference.graph;
        for (auto vect : interference.nodeList) {
            if (vect != "rsp") {
                myOutput += vect;
                for (auto adjacent : g[vect]->adj) {
                    if (adjacent->name != "rsp") {
                        myOutput += " " + adjacent->name;
                    }
                }
                myOutput += "\n";
            }
        }
        
        /* 
        * Close the output file.
        */ 
        
        // print(myOutput);
        
        outputFile << myOutput;
        outputFile.close();
    
        return interference;
    }
}
