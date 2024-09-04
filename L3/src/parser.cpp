/*
 * SUGGESTION FROM THE CC TEAM:
 * double check the order of actions that are fired.
 * You can do this in (at least) two ways:
 * 1) by using gdb adding breakpoints to actions
 * 2) by adding printing statements in each action
 *
 * For 2), we suggest writing the code to make it straightforward to enable/disable all of them 
 * (e.g., assuming shouldIPrint is a global variable
 *    if (shouldIPrint) std::cerr << "MY OUTPUT" << std::endl;
 * )
 */
#include <sched.h>
#include <string>
#include <vector>
#include <utility>
#include <algorithm>
#include <set>
#include <iterator>
#include <cstring>
#include <cctype>
#include <cstdlib>
#include <stdint.h>
#include <assert.h>

#include <tao/pegtl.hpp>
#include <tao/pegtl/analyze.hpp>
#include <tao/pegtl/contrib/raw_string.hpp>

#include <L3.h>
#include <parser.h>

namespace pegtl = tao::TAO_PEGTL_NAMESPACE;

using namespace pegtl;

namespace L3 {

  /* 
   * Tokens parsed
   */ 
  std::vector<Item *> parsed_items;

  /* 
   * Grammar rules from now on.
   */
  struct name:
    pegtl::seq<
      pegtl::plus< 
        pegtl::sor<
          pegtl::alpha,
          pegtl::one< '_' >
        >
      >,
      pegtl::star<
        pegtl::sor<
          pegtl::alpha,
          pegtl::one< '_' >,
          pegtl::digit
        >
      >
    > {};

  /* 
   * Keywords.
   */
  struct str_arrow : TAOCPP_PEGTL_STRING( "<-"/*"*/ ) {};
  struct str_return : TAOCPP_PEGTL_STRING( "return" ) {};
  struct str_define : TAOCPP_PEGTL_STRING( "define" ) {};
  struct str_less_than : TAOCPP_PEGTL_STRING( "<" ) {};
  struct str_less_than_equals : TAOCPP_PEGTL_STRING( "<=" ) {};
  struct str_equals : TAOCPP_PEGTL_STRING( "=" ) {};
  struct str_greater_than_equals : TAOCPP_PEGTL_STRING( ">=" ) {};
  struct str_greater_than : TAOCPP_PEGTL_STRING( ">" ) {};
  struct str_add : TAOCPP_PEGTL_STRING( "+" ) {};
  struct str_sub : TAOCPP_PEGTL_STRING( "-" ) {};
  struct str_imult : TAOCPP_PEGTL_STRING( "*" ) {};
  struct str_and : TAOCPP_PEGTL_STRING( "&" ) {};
  struct str_sal : TAOCPP_PEGTL_STRING( "<<" ) {};
  struct str_sar : TAOCPP_PEGTL_STRING( ">>" ) {};
  struct str_print : TAOCPP_PEGTL_STRING( "print" ) {};
  struct str_allocate : TAOCPP_PEGTL_STRING( "allocate" ) {};
  struct str_input : TAOCPP_PEGTL_STRING( "input" ) {};
  struct str_tensor_error : TAOCPP_PEGTL_STRING( "tensor-error" ) {};
  struct str_load : TAOCPP_PEGTL_STRING( "load" ) {};
  struct str_store : TAOCPP_PEGTL_STRING( "store" ) {};
  struct str_br : TAOCPP_PEGTL_STRING( "br" ) {};
  struct str_call : TAOCPP_PEGTL_STRING( "call" ) {};
  struct str_empty : TAOCPP_PEGTL_STRING( "" ) {};

  struct comment: 
    pegtl::disable< 
      TAOCPP_PEGTL_STRING( "//" ), 
      pegtl::until< pegtl::eolf > 
    > {};

  struct seps: 
    pegtl::star< 
      pegtl::sor< 
        pegtl::ascii::space, 
        comment 
      > 
    > {};

  struct var:
    pegtl::seq<
      pegtl::one<'%'>,
      name
    > {};

  struct label:
    pegtl::seq<
      pegtl::one<':'>,
      name
    > {};

  struct l_rule:
    pegtl::seq<
      pegtl::one<'@'>,
      name
    > {};

  struct N:
    pegtl::seq<
      pegtl::opt<
        pegtl::sor<
          pegtl::one< '-' >,
          pegtl::one< '+' >
        >
      >,
      pegtl::plus< 
        pegtl::digit
      >
    >{};

  struct cmp:
    pegtl::sor<
      str_less_than_equals,
      str_less_than,
      str_equals,
      str_greater_than_equals,
      str_greater_than
    > {};

  struct op:
    pegtl::sor<
      str_add,
      str_sub,
      str_imult,
      str_and,
      str_sal,
      str_sar
    > {};

  struct u:
    pegtl::sor<
      var,
      l_rule
    > {};
  
  struct t:
    pegtl::sor<
      var,
      N
    > {};
  
  // l is not on the slides, but the tests have it
  struct s:
    pegtl::sor<
      t,
      label,
      l_rule
    > {};

  struct args:
    pegtl::sor<
      
      pegtl::seq< pegtl::at<pegtl::seq<
        t,
        pegtl::star<
          seps,
          pegtl::one<','>,
          seps,
          t
        >
      >
      >, 
      pegtl::seq<
        t,
        pegtl::star<
          seps,
          pegtl::one<','>,
          seps,
          t
        >
      >
      >,
      pegtl::seq< pegtl::at<str_empty>, str_empty>
    > {};

  struct vars:
    pegtl::sor<
      // Either var (, var)* or nothing
      pegtl::seq< pegtl::at<pegtl::seq<
        var,
        pegtl::star<
          seps,
          pegtl::one<','>,
          seps,
          var
        >
      >
      >, 
      pegtl::seq<
        var,
        pegtl::star<
          seps,
          pegtl::one<','>,
          seps,
          var
        >
      >
      >,
      pegtl::seq< pegtl::at<str_empty>, str_empty>
    > {};

  struct callee:
    pegtl::sor<
      u,
      str_print,
      str_allocate,
      str_input,
      str_tensor_error
    > {};

  struct function_name:
    l_rule {};

  struct argument_number:
    N {};

  struct local_number:
    N {} ;


  

  

  // Instructions

  struct Instruction_assignment_rule:
    pegtl::seq<
      var,
      seps,
      str_arrow,
      seps,
      s
    > {};

  struct Instruction_assignment_op_rule:
    pegtl::seq<
      var,
      seps,
      str_arrow,
      seps,
      t,
      seps,
      op,
      seps,
      t
    > {};

  struct Instruction_assignment_cmp_rule:
    pegtl::seq<
      var,
      seps,
      str_arrow,
      seps,
      t,
      seps,
      cmp,
      seps,
      t
    > {};

  struct Instruction_load_rule:
    pegtl::seq<
      var,
      seps,
      str_arrow,
      seps,
      str_load,
      seps,
      var
    > {};

  struct Instruction_store_rule:
    pegtl::seq<
      str_store,
      seps,
      var,
      seps,
      str_arrow,
      seps,
      s
    > {};

  struct Instruction_return_rule:
    pegtl::seq<
      str_return
    > {};

  struct Instruction_return_arg_rule:
    pegtl::seq<
      str_return,
      seps,
      t
    > {};

  struct Instruction_label_rule:
    label {};
  
  struct Instruction_branch_rule:
    pegtl::seq<
      str_br,
      seps,
      label
    > {};

  struct Instruction_branch_conditional_rule:
    pegtl::seq<
      str_br,
      seps,
      t,
      seps,
      label
    > {};  

  struct Instruction_call_rule:
    pegtl::seq<
      str_call,
      seps,
      callee,
      seps,
      pegtl::one<'('>,
      seps,
      args,
      seps,
      pegtl::one<')'>
    > {};

  struct Instruction_assignment_call_rule:
    pegtl::seq<
      var,
      seps,
      str_arrow,
      seps,
      str_call,
      seps,
      callee,
      seps,
      pegtl::one<'('>,
      seps,
      args,
      seps,
      pegtl::one<')'>
    > {};

 

  struct Instruction_rule:
    pegtl::sor<
      pegtl::seq< pegtl::at<Instruction_assignment_op_rule>     , Instruction_assignment_op_rule      >,
      pegtl::seq< pegtl::at<Instruction_assignment_cmp_rule>    , Instruction_assignment_cmp_rule     >,
      pegtl::seq< pegtl::at<Instruction_assignment_call_rule>   , Instruction_assignment_call_rule    >,
      pegtl::seq< pegtl::at<Instruction_assignment_rule>        , Instruction_assignment_rule         >,
      pegtl::seq< pegtl::at<Instruction_return_arg_rule>        , Instruction_return_arg_rule         >,
      pegtl::seq< pegtl::at<Instruction_load_rule>              , Instruction_load_rule               >,
      pegtl::seq< pegtl::at<Instruction_store_rule>             , Instruction_store_rule              >,
      pegtl::seq< pegtl::at<Instruction_return_rule>            , Instruction_return_rule             >,
      pegtl::seq< pegtl::at<Instruction_branch_conditional_rule>, Instruction_branch_conditional_rule >,
      pegtl::seq< pegtl::at<Instruction_branch_rule>            , Instruction_branch_rule             >,
      pegtl::seq< pegtl::at<Instruction_call_rule>              , Instruction_call_rule               >,
      pegtl::seq< pegtl::at<Instruction_label_rule>             , Instruction_label_rule              >
      
    > { };

  struct Instructions_rule:
    pegtl::plus<
      pegtl::seq<
        seps,
        Instruction_rule,
        seps
      >
    > { };

  struct Function_rule:
    pegtl::seq<
      str_define,
      seps,
      function_name,
      seps,
      pegtl::one< '(' >,
      seps,
      vars,
      seps,
      pegtl::one< ')' >,
      seps,
      pegtl::one< '{' >,
      seps,
      Instructions_rule,
      seps,
      pegtl::one< '}' >
    > {};

  struct Functions_rule:
    pegtl::plus<
      seps,
      Function_rule,
      seps
    > {};

  struct grammar : 
    pegtl::must< 
      Functions_rule
    > {};

  /* 
   * Actions attached to grammar rules.
   */

  template< typename Rule >
  struct action : pegtl::nothing< Rule > {};


  template<> struct action < function_name > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      print("Adding a function name item");
      auto function = new Function();
      function->name = in.string();
      
      p.functions.push_back(function);
    }
  };

  template<> struct action < var > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      print("Adding a var item");
      auto i = new Variable(in.string());
      parsed_items.push_back(i);
    }
  };

  template<> struct action < label > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      print("Adding a label item");
      auto i = new Label(in.string());
      parsed_items.push_back(i);
    }
  };

  template<> struct action < l_rule > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      print("Adding an l item");
      auto i = new L(in.string());
      parsed_items.push_back(i);
    }
  };

  template<> struct action < N > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      print("Adding an N item");
      //cout<<"Tried-";
      auto i = new Number(stol(in.string()));
      //cout<<"Succeeded";
      parsed_items.push_back(i);
    }
  };

  template<> struct action < cmp > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      print("Adding a cmp item");

      auto op = in.string();

      if (op == "<") {
        auto i = new Compare_Operator(Compare_OperatorID::l);
        parsed_items.push_back(i);
      } else if (op == "<=") {
        auto i = new Compare_Operator(Compare_OperatorID::le);
        parsed_items.push_back(i);
      } else if (op == "=") {
        auto i = new Compare_Operator(Compare_OperatorID::e);
        parsed_items.push_back(i);
      } else if (op == ">=") {
        auto i = new Compare_Operator(Compare_OperatorID::ge);
        parsed_items.push_back(i);
      } else if (op == ">") {
        auto i = new Compare_Operator(Compare_OperatorID::g);
        parsed_items.push_back(i);
      } else {
        std::cerr << "Compare isn't correct";
      }
    }
  };

  template<> struct action < op > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      print("Adding an op item");

      auto op = in.string();

      if (op == "+") {
        auto i = new Arithmetic_Operator(Arithmetic_OperatorID::addq);
        parsed_items.push_back(i);
      } else if (op == "-") {
        auto i = new Arithmetic_Operator(Arithmetic_OperatorID::subq);
        parsed_items.push_back(i);
      } else if (op == "*") {
        auto i = new Arithmetic_Operator(Arithmetic_OperatorID::imultq);
        parsed_items.push_back(i);
      } else if (op == "&") {
        auto i = new Arithmetic_Operator(Arithmetic_OperatorID::andq);
        parsed_items.push_back(i);
      } else if (op == "<<") {
        auto i = new Arithmetic_Operator(Arithmetic_OperatorID::salq);
        parsed_items.push_back(i);
      } else if (op == ">>") {
        auto i = new Arithmetic_Operator(Arithmetic_OperatorID::sarq);
        parsed_items.push_back(i);
      } else {
        std::cerr << "Arithmetic isn't correct";
      }
    }
  };

  template<> struct action < callee > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      print("Adding a callee item");
      auto callee_string = in.string();
      Item* callee;
      if (callee_string.at(0) != '%' && callee_string.at(0) != '@') {
        callee = new L(callee_string);
      } else {
        callee = parsed_items.back();
        parsed_items.pop_back();
      }
      
      parsed_items.push_back(callee);
    }
  };

  template<> struct action < vars > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      print("Adding a vars item");
      auto vars_string = in.string();
      std::vector<Item*> vars;
      //count number of vars
      auto n = std::count(vars_string.begin(), vars_string.end(), ',');

      if (parsed_items.size() > 0) {
        for (int i = 0; i <= n; i++) {
          vars.insert(vars.begin(), parsed_items.back());
          parsed_items.pop_back();
        }
      } 

      auto varsItem = new Vars(vars);
        
      parsed_items.push_back(varsItem);

     
    }
  };

  template<> struct action < args > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      print("Adding a args item");

      print("Before starting, " + to_string(parsed_items.size()));
      print("THE LAST ITEM IS" + parsed_items.back()->to_string());
      
      auto arg_string = in.string();
      print(arg_string);
      std::vector<Item*> args;
      //count number of args
      auto n = std::count(arg_string.begin(), arg_string.end(), ',');

      
      // if no args
      if (arg_string.size() != 0) {
        for (int i = 0; i <= n; i++) {
          args.insert(args.begin(), parsed_items.back());
          parsed_items.pop_back();
        }
      }

      auto argsItem = new Args(args);

      print("THERE ARE " + to_string(parsed_items.size()) + " items left");
      parsed_items.push_back(argsItem);
      print("THERE ARE " + to_string(parsed_items.size()) + " items left");
      print("THE LAST ITEM IS" + parsed_items.back()->to_string());
     
    }
  };

/*
  template<> struct action < str_return > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      auto currentF = p.functions.back();
      auto i = new Instruction_ret();
      currentF->instructions.push_back(i);
    }
  };
*/
  template<> struct action < Instruction_assignment_rule > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      print("Adding a assignment instruction");
      
      auto currentF = p.functions.back();

      auto src = parsed_items.back();
      parsed_items.pop_back();
      auto dst = parsed_items.back();
      parsed_items.pop_back();

      
      auto i = new Instruction_assignment(dst, src);

  
      currentF->instructions.push_back(i);
      
    }
    
  };

  template<> struct action < Instruction_assignment_op_rule > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      print("Adding a op assignment instruction");
      auto currentF = p.functions.back();
      auto t2 = parsed_items.back();
      parsed_items.pop_back();
      auto op = parsed_items.back();
      parsed_items.pop_back();
      auto t1 = parsed_items.back();
      parsed_items.pop_back();
      auto dst = parsed_items.back();
      parsed_items.pop_back();

      
      auto i = new Instruction_assignment_op(dst, t1, op, t2);

  
      currentF->instructions.push_back(i);
    }
  };

  template<> struct action < Instruction_assignment_cmp_rule > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      print("Adding a cmp assignment instruction");
      /*
      auto currentF = p.functions.back();
      auto dst = parsed_items.back();
      parsed_items.pop_back();
      auto t1 = parsed_items.back();
      parsed_items.pop_back();
      auto cmp = parsed_items.back();
      parsed_items.pop_back();
      auto t2 = parsed_items.back();
      parsed_items.pop_back();
      */ //I'm Going to kill Sebastian for this

      auto currentF = p.functions.back();
      auto t2 = parsed_items.back();
      parsed_items.pop_back();
      auto cmp = parsed_items.back();
      parsed_items.pop_back();
      auto t1 = parsed_items.back();
      parsed_items.pop_back();
      auto dst = parsed_items.back();
      parsed_items.pop_back();

      
      auto i = new Instruction_assignment_cmp(dst, t1, cmp, t2);

  
      currentF->instructions.push_back(i);
    }
  };

  template<> struct action < Instruction_load_rule > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      print("Adding a load instruction");
      auto currentF = p.functions.back();
      auto src = parsed_items.back();
      parsed_items.pop_back();
      auto dst = parsed_items.back();
      parsed_items.pop_back();

      
      auto i = new Instruction_load(dst, src);

  
      currentF->instructions.push_back(i);
    }
  };

  template<> struct action < Instruction_store_rule > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      print("Adding a store instruction");
      auto currentF = p.functions.back();
      auto src = parsed_items.back();
      parsed_items.pop_back();
      auto dst = parsed_items.back();
      parsed_items.pop_back();

      
      auto i = new Instruction_store(dst, src);

  
      currentF->instructions.push_back(i);
    }
  };

  template<> struct action < Instruction_return_rule > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      print("Adding a return instruction");
      auto currentF = p.functions.back();

      auto i = new Instruction_return();

      currentF->instructions.push_back(i);
    }
  };

  template<> struct action < Instruction_return_arg_rule > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      print("Adding a return arg instruction");
      auto currentF = p.functions.back();
      auto src = parsed_items.back();
      parsed_items.pop_back();

      
      auto i = new Instruction_return_arg(src);

  
      currentF->instructions.push_back(i);
    }
  };

  template<> struct action < Instruction_label_rule > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      print("Adding a label instruction");
      std::string label_string = in.string();
      print(label_string);
      // For some reason, this doesn't call label rule, meaning we have to process the label here
      auto currentF = p.functions.back();
      print("got func");
      
      // Make the label
      print("Adding a label item (workaround)");
      auto label = new Label(label_string);
      print(label_string);
      if (label_string.length() > p.longest_label.length()) {
        print("new LL!");
        print(label_string);
        p.longest_label = label_string;
        print(p.longest_label);
      }
      

      // If the label rule fired, this would be used
      // auto dst = parsed_items.back();
      // print("got item");
      // print(dst->to_string());
      // parsed_items.pop_back();
      // print("popped item");
      
      auto i = new Instruction_label(label);

  
      currentF->instructions.push_back(i);
    }
  };

  template<> struct action < Instruction_branch_rule > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      print("Adding a br instruction");
      auto currentF = p.functions.back();
      auto dst = parsed_items.back();
      parsed_items.pop_back();

      
      auto i = new Instruction_branch(dst);

  
      currentF->instructions.push_back(i);
    }
  };

  template<> struct action < Instruction_branch_conditional_rule > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      print("Adding a br conditional instruction");
      auto currentF = p.functions.back();
      auto dst = parsed_items.back();
      parsed_items.pop_back();
      auto cond = parsed_items.back();
      parsed_items.pop_back();

      
      auto i = new Instruction_branch_conditional(cond, dst);

  
      currentF->instructions.push_back(i);
    }
  };

  template<> struct action < Instruction_call_rule > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      print("Adding a call instruction");
      
      auto currentF = p.functions.back();
      auto args = parsed_items.back();
      parsed_items.pop_back();
      auto callee = parsed_items.back();
      parsed_items.pop_back();

      print("the callee is " + callee->to_string());
      print("the args are " + args->to_string());

      
      
      auto i = new Instruction_call(callee, args);

  
      currentF->instructions.push_back(i);
    }
  };

  template<> struct action < Instruction_assignment_call_rule > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      print("Adding a assignment call instruction");

      auto currentF = p.functions.back();
      auto args = parsed_items.back();
      parsed_items.pop_back();
      auto callee = parsed_items.back();
      parsed_items.pop_back();
      auto dst = parsed_items.back();
      parsed_items.pop_back();
      
      auto i = new Instruction_assignment_call(dst, callee, args);

  
      currentF->instructions.push_back(i);
    }
  };

  template<> struct action < Function_rule > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      print("Adding a function instruction");
      auto currentF = p.functions.back();
      auto vars = parsed_items.back();
      parsed_items.pop_back();

      print(vars->to_string());

      currentF->arguments = vars;
    
    }
  };

  Program parse_file (char *fileName){

    /* 
     * Check the grammar for some possible issues.
     */
    pegtl::analyze< grammar >();
    print("analysed");
    /*
     * Parse.
     */
    file_input< > fileInput(fileName);
    Program p;
    parse< grammar, action >(fileInput, p);
    print("parsed");

    print("Parsed_items is " + to_string(parsed_items.size()) + " leftover\n");
    print("LL is " + p.longest_label);
    return p;
  }

}
