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
#include <code_generator.h>
#include <LA.h>


void print_help (char *progName){
  std::cerr << "Usage: " << progName << " [-v] [-g 0|1] [-O 0|1|2] SOURCE" << std::endl;
  return ;
}
//update
int main(
  int argc, 
  char **argv
  ){
  //cout<<"Start Compiler"<<endl;
  auto enable_code_generator = true;
  int32_t optLevel = 0;
  bool verbose= false;

  /* 
   * Check the compiler arguments.
   */
  if( argc < 2 ) {
    print_help(argv[0]);
    return 1;
  }
  int32_t opt;
  while ((opt = getopt(argc, argv, "vg:O:")) != -1) {
    switch (opt){
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
        print_help(argv[0]);
        return 1;
    }
  }

  /*
   * Parse the input file.
   */
  // cout<<"Parse File"<<endl;
  auto p = LA::parse_file(argv[optind]);
  // cout << "Done parsing\n";

  /*
   * Code optimizations (optional)
   */

  /* 
   * Print the source program.
   */
  if (verbose){
    for (auto f : p.functions) {
      cout << f->return_type->to_string() << " " << f->name << " (" << f->arguments->to_string() << ") {\n";
      for (auto i : f->instructions) {
        cout << i->line_number << "\t" << i->instruction_to_IR();
      }
      cout << "}\n";
    }
    
  }
  /*
   * Generate x86_64 assembly.
   */
  if (enable_code_generator){
    LA::generate_code(p);
  }

  return 0;
}
