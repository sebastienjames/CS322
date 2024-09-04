#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <string>

#include "L2.h"
#include "liveness.h"
#include "coloring.h"

using namespace std;

namespace L2 {
    Vertex* Graph::popVertex (const string& node) {
        // This is node actually removing the vertex, just removing it from the list of accessible nodes
        // print("node is " + node);
        // Get the Vertex of the node
        auto vertex = graph[node];
        // print("Beofre pop");
        // for (auto o : nodeList) {
        //     print(o);
        // }
        // Erase the node
        nodeList.erase(find(nodeList.begin(), nodeList.end(), node));
        // print("after pop");
        // for (auto o : nodeList) {
        //     print(o);
        // }

        // Remove it from its neighbor's adj list
        // for (auto v : vertex->adj) {
        //     v->adj.erase(vertex);
        // }
        // print("popped " + vertex->name);
        return vertex;
    }

    int64_t Vertex::numNeighbors() {
        return adj.size();
    }


    void Graph::color_node(const string& node, RegisterID color) {
        // print("coloring " + node + " with " + to_string(color));
        graph[node]->color = color;
    }

    int Graph::nextNodeToPop() {
        int nodeToPop = -1;
        for (int i=0; i < nodeList.size(); i++) {
            if (nodeList[i].at(0) == '%') {
                // print("NODE AT POS " + to_string(i) + " is a var: " + nodeList[i]);
                if (graph[nodeList[i]]->adj.size() < 15) {
                    return i;
                }
                nodeToPop = i;
            }
        }
        return nodeToPop;
    }




    Graph color (Graph g) {
        // print("Start coloring");
        // g.print_graph();

        // {rax, rbx, rbp, r10, r11, r12, r13, r14, r15, rdi, rsi, rdx, rcx, r8, r9, rsp};
        // Color the registers their own color
        g.color_node("rax", RegisterID::rax);
        g.color_node("rbx", RegisterID::rbx);
        g.color_node("rbp", RegisterID::rbp);
        g.color_node("r10", RegisterID::r10);
        g.color_node("r11", RegisterID::r11);
        g.color_node("r12", RegisterID::r12);
        g.color_node("r13", RegisterID::r13);
        g.color_node("r14", RegisterID::r14);
        g.color_node("r15", RegisterID::r15);
        g.color_node("rdi", RegisterID::rdi);
        g.color_node("rsi", RegisterID::rsi);
        g.color_node("rdx", RegisterID::rdx);
        g.color_node("rcx", RegisterID::rcx);
        g.color_node("r8", RegisterID::r8);
        g.color_node("r9", RegisterID::r9);
        g.color_node("rsp", RegisterID::rsp);


        // print(to_string( g.nodeList.size()));
        vector<Vertex*> stack;
        // for-in gives an extra empty for some reason
        // for doesn't work, keeps same index even after changing the vector
        // for (int i=0; i < g.nodeList.size(); i++) {
        int index = g.nextNodeToPop();
        while (index != -1) {
            // print("Start node" + g.nodeList[index]);
            auto popped = g.popVertex(g.nodeList[index]);
            stack.push_back(popped);
            // print("Popped vertex " + popped->name);
            // print("End node");
            index = g.nextNodeToPop();
        }


        // Make a new graph

        while(!stack.empty()) {
            Vertex* node = stack.back();
            stack.pop_back();

            // Find an available color for the node
            // print("Inserting " + node->name + " back into the graph");
            g.nodeList.push_back(node->name);

            set<RegisterID> available_colors = {
                // Maybe need to order these
                RegisterID::rdi,
                RegisterID::rax,  
                RegisterID::rsi, 
                RegisterID::rdx, 
                RegisterID::rcx, 
                RegisterID::r8, 
                RegisterID::r9,
                RegisterID::rbx, 
                RegisterID::rbp, 
                RegisterID::r10, 
                RegisterID::r11, 
                RegisterID::r12, 
                RegisterID::r13, 
                RegisterID::r14, 
                RegisterID::r15 
                
                };
            // print("Coloring node " + node->name);
            for (auto adjacentNode : node->adj) {
                // print("  Adjacent node " + adjacentNode->name + " is colored " + to_string(adjacentNode->color));
                if (available_colors.find(adjacentNode->color) != available_colors.end()) {
                    available_colors.erase(adjacentNode->color);
                    if (available_colors.empty()) {
                        // print("SPILL");
                    }
                }
            }

            if (available_colors.empty()) {
                // rsp is the placeholder for need to spill
                // g.color_node(node->name, RegisterID::rsp);
                // print("Need to spill " + node->name);
            } else {
                for (auto color : available_colors) {
                    // print("Color " + to_string(color) + " is available");
                }
                g.color_node(node->name, *(available_colors.begin()));
            }

        }
        
        string cg_registers[17]={"rax","rbx","rbp","r10","r11","r12","r13","r14","r15","rdi","rsi","rdx","rcx","r8","r9","rsp", "none"};
        for (auto node : g.nodeList) {
            auto vertex = g.graph[node];
            // cout << vertex->name << "----------" << cg_registers[vertex->color] << "\n";
        }

        return g;
    }
}