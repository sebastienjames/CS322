#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include "L2.h"
#include "liveness.h"

namespace L2{

  struct Vertex {
    set<Vertex*> adj; 
    string name;
    RegisterID color;
    Vertex(string s);
    int64_t numNeighbors();
  };

  class Graph
  {
  public:
    typedef map<string, Vertex *> vmap;
    vmap graph;
    bool addvertex(const string&);
    void addedge(const string& from, const string& to);
    Vertex* popVertex(const string& node);
    void color_node(const string& node, RegisterID color);
    void print_graph();
    int nextNodeToPop();
    vector<string> nodeList;
  };

  Graph interference_graph(vector<Line*> lines);
}
