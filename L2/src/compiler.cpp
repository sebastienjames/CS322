#include <string>
#include <vector>
#include <utility>
#include <algorithm>
#include <set>
#include <iterator>
#include <iostream>
#include <cstring>
#include <cctype>
#include <cstdlib>
#include <stdint.h>
#include <unistd.h>
#include <iostream>
#include <assert.h>

#include <parser.h>
#include <liveness.h>
#include <interference.h>
#include <coloring.h>
#include <spiller.h>
#include <code_generator.h>
#include <L2_to_L1_code_generator.h>

void print_help (char *progName){
  std::cerr << "Usage: " << progName << " [-v] [-g 0|1] [-O 0|1|2] [-s] [-l] [-i] SOURCE" << std::endl;
  return ;
}

int main(
  int argc, 
  char **argv
  ){
  auto enable_code_generator = true;
  auto spill_only = false;
  auto interference_only = false;
  auto liveness_only = false;
  auto graph_coloring = false;
  int32_t optLevel = 3;
  /* 
   * Check the compiler arguments.
   */
  auto verbose = false;
  if( argc < 2 ) {
    print_help(argv[0]);
    return 1;
  }
  int32_t opt;
  while ((opt = getopt(argc, argv, "vg:O:slic")) != -1) {
    switch (opt){
      case 'l':
      	//cout<<"hitLiveness"<<endl;
       	liveness_only = true ;
       	break;
      case 'i':
       	interference_only=true ;
       	break;

      case 's':
       	spill_only=true ;
       	break;

      case 'c':
       	graph_coloring=true ;
       	break;
       	
      case 'O':
        optLevel = strtoul(optarg, NULL, 0);
        break ;

      case 'g':
        enable_code_generator = (strtoul(optarg, NULL, 0) == 0) ? false : true ;
        break ;

      case 'v':
        verbose = true;
        break ;
       
      default:
      	cout<<opt;
        print_help(argv[0]);
        return 1;
    }
  }

  //std::cout << "Start\n";

   /*
   * Parse the input file.
   */
  L2::Program p;
  if (spill_only){

    /* 
     * Parse an L2 function and the spill arguments.
     */
    p = L2::parse_spill_file(argv[optind]);
 
  } else if (liveness_only){

    /*
     * Parse an L2 function.
     */
    p = L2::parse_function_file(argv[optind]);
    // need to print the output to pass the tests

  } else if (interference_only){

    /*
     * Parse an L2 function.
     */
    p = L2::parse_function_file(argv[optind]);

  } else if (graph_coloring){

    /*
     * Parse an L2 function.
     */
    p = L2::parse_function_file(argv[optind]);

  } else {

    /* 
     * Parse the L2 program.
     */
     ///CHANGE THIS EVENTURAL
    //cout<<("Currently only parsing a function");
    p = L2::parse_file(argv[optind]);
  }

  

  //std::cout << "Done parsing\n";
  /*
   * Code optimizations (optional)
   */

  /* 
   * Print the source program.
   */
  if (verbose){
    
  }

  /*
   * Generate liveness
   */

  /*
   * Liveness test.
   */
  if (liveness_only){
    L2::liveness_analysis(p.functions.front(), true);
    return 0;
  }

  /*
   * Interference graph test.
   */
  if (interference_only){
    auto lines = L2::liveness_analysis(p.functions.front(), false);
    auto g = L2::interference_graph(lines);
    g.print_graph();
    return 0;
  }

  if (graph_coloring){
    auto lines = L2::liveness_analysis(p.functions.front(), false);
    auto g = L2::interference_graph(lines);
    L2::color(g);
    return 0;
  }

  if (spill_only){
    string var = p.entryPointLabel;
    string replace = p.toReplace;
    auto func = L2::spill(p.functions.front(), replace, var);
    // L2::generate_function(func);
    return 0;
  }

  /*
   * Generate the target code.
   */
  if (enable_code_generator){
    //cout << "START";
    L2::generate_code_to_L1(p);
  }

  return 0;
}
