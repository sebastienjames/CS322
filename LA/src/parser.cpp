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

#include <LA.h>
#include <parser.h>

namespace pegtl = tao::TAO_PEGTL_NAMESPACE;

using namespace pegtl;

namespace LA {

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
  struct str_br : TAOCPP_PEGTL_STRING( "br" ) {};
  struct str_length : TAOCPP_PEGTL_STRING( "length" ) {};
  struct str_new : TAOCPP_PEGTL_STRING( "new" ) {};
  struct str_Array : TAOCPP_PEGTL_STRING( "Array" ) {};
  struct str_Tuple : TAOCPP_PEGTL_STRING( "Tuple" ) {};
  struct str_void : TAOCPP_PEGTL_STRING( "void" ) {};
  struct str_int64 : TAOCPP_PEGTL_STRING( "int64" ) {};
  struct str_tuple : TAOCPP_PEGTL_STRING( "tuple" ) {};
  struct str_code : TAOCPP_PEGTL_STRING( "code" ) {};
  struct str_empty : TAOCPP_PEGTL_STRING( "" ) {};
  struct str_brackets : TAOCPP_PEGTL_STRING( "[]" ) {};
  

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

  struct label:
    pegtl::seq<
      pegtl::one<':'>,
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

  struct op:
    pegtl::sor<
      str_add,
      str_sub,
      str_imult,
      str_and,
      str_sal,
      str_sar,
      str_less_than_equals,
      str_less_than,
      str_equals,
      str_greater_than_equals,
      str_greater_than
    > {};
  
  struct t:
    pegtl::sor<
      name,
      N
    > {};
  
  struct s:
    pegtl::sor<
      t,
      label
    > {};
  
  struct type:
    pegtl::sor<
      pegtl::seq<
        str_int64,
        pegtl::star<
          seps,
          str_brackets
        >
      >,
      str_tuple,
      str_code
    > {};

  struct T:
    pegtl::sor<
      type,
      str_void
    > {};

  struct args:
    pegtl::sor<
      pegtl::seq<
        pegtl::at<
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
      // Either name (, name)* or nothing
      pegtl::seq<
        pegtl::at<
          pegtl::seq<
            type,
            seps,
            name,
            pegtl::star<
              seps,
              pegtl::one<','>,
              seps,
              type,
              seps,
              name
            >
          >
        >, 
        pegtl::seq<
          type,
          seps,
          name,
          pegtl::star<
            seps,
            pegtl::one<','>,
            seps,
            type,
            seps,
            name
          >
        >
      >,
      pegtl::seq< pegtl::at<str_empty>, str_empty>
    > {};


  

  


  struct function_name:
    name {};

  struct argument_number:
    N {};

  struct local_number:
    N {} ;


  

  

  // Instructions

  // struct Instruction_label_rule:
  //   label {};

  struct Instruction_declaration_rule:
    pegtl::seq<
      type,
      seps,
      name
    > {};

  struct Instruction_assignment_rule:
    pegtl::seq<
      name,
      seps,
      str_arrow,
      seps,
      s
    > {};

  struct Instruction_assignment_op_rule:
    pegtl::seq<
      name,
      seps,
      str_arrow,
      seps,
      t,
      seps,
      op,
      seps,
      t
    > {};

  struct Instruction_read_tensor_rule:
    pegtl::seq<
      name,
      seps,
      str_arrow,
      seps,
      name,
      pegtl::plus<
        seps,
        pegtl::one<'['>,
        seps,
        t,
        seps,
        pegtl::one<']'>
      >
    > {};
    
  struct Instruction_write_tensor_rule:
    pegtl::seq<
      name,
      pegtl::plus<
        seps,
        pegtl::one<'['>,
        seps,
        t,
        seps,
        pegtl::one<']'>
      >,
      seps,
      str_arrow,
      seps,
      s
    > {};

  struct Instruction_length_rule:
    pegtl::seq<
      name,
      seps,
      str_arrow,
      seps,
      str_length,
      seps,
      name,
      seps,
      t
    > {};

  struct Instruction_call_rule:
    pegtl::seq<
      name,
      seps,
      pegtl::one<'('>,
      seps,
      args,
      seps,
      pegtl::one<')'>
    > {};

  struct Instruction_assignment_call_rule:
    pegtl::seq<
      name,
      seps,
      str_arrow,
      seps,
      name,
      seps,
      pegtl::one<'('>,
      seps,
      args,
      seps,
      pegtl::one<')'>
    > {};

  struct Instruction_new_array_rule:
    pegtl::seq<
      name,
      seps,
      str_arrow,
      seps,
      str_new,
      seps,
      str_Array,
      seps,
      pegtl::one<'('>,
      seps,
      args,
      seps,
      pegtl::one<')'>
    > {};

    struct Instruction_new_tuple_rule:
    pegtl::seq<
      name,
      seps,
      str_arrow,
      seps,
      str_new,
      seps,
      str_Tuple,
      seps,
      pegtl::one<'('>,
      seps,
      t,
      seps,
      pegtl::one<')'>
    > {};

  
  struct Instruction_br_rule:
    pegtl::seq<
          str_br,
          seps,
          label
    > {};

  struct Instruction_br_conditional_rule:
    pegtl::seq<
          str_br,
          seps,
          t,
          seps,
          label,
          seps,
          label
    > {};

  struct Instruction_return_arg_rule:
    pegtl::seq<
      str_return,
      seps,
      t
    > {};

  struct Instruction_return_rule:
    str_return {};

  struct Instruction_label_rule:
    label {};
 

  struct Instruction_rule:
    pegtl::sor<
      
      pegtl::seq< pegtl::at<Instruction_declaration_rule>     , Instruction_declaration_rule      >,
      pegtl::seq< pegtl::at<Instruction_read_tensor_rule>     , Instruction_read_tensor_rule      >,
      pegtl::seq< pegtl::at<Instruction_write_tensor_rule>     , Instruction_write_tensor_rule      >,
      pegtl::seq< pegtl::at<Instruction_length_rule>     , Instruction_length_rule      >,
      pegtl::seq< pegtl::at<Instruction_new_tuple_rule>     , Instruction_new_tuple_rule      >,
      pegtl::seq< pegtl::at<Instruction_new_array_rule>     , Instruction_new_array_rule      >,
      pegtl::seq< pegtl::at<Instruction_assignment_op_rule>     , Instruction_assignment_op_rule      >,
      pegtl::seq< pegtl::at<Instruction_assignment_call_rule>   , Instruction_assignment_call_rule    >,
      pegtl::seq< pegtl::at<Instruction_assignment_rule>        , Instruction_assignment_rule         >,
      pegtl::seq< pegtl::at<Instruction_call_rule>              , Instruction_call_rule               >,
      pegtl::seq< pegtl::at<Instruction_br_rule>              , Instruction_br_rule               >,
      pegtl::seq< pegtl::at<Instruction_br_conditional_rule>              , Instruction_br_conditional_rule               >,
      pegtl::seq< pegtl::at<Instruction_return_arg_rule>              , Instruction_return_arg_rule               >,
      pegtl::seq< pegtl::at<Instruction_return_rule>              , Instruction_return_rule               >,
      pegtl::seq< pegtl::at<Instruction_label_rule>              , Instruction_label_rule               >
    > {};


        


  struct Function_rule:
    pegtl::seq<
      T,
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
      pegtl::star<
        Instruction_rule,
        seps
      >,
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
      
      function->return_type = parsed_items.back();
      parsed_items.pop_back();

      
      p.functions.push_back(function);
    }
  };

  template<> struct action < name > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      print("Adding a name item: " + in.string());
      auto i = new Name(in.string());
      parsed_items.push_back(i);

      auto currentF = p.functions.back();

      if (currentF->longest_name.size() < in.string().size()) {
        currentF->longest_name = in.string();
      }
    }
  };

  template<> struct action < type > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      print("Adding a type item");
      auto t = in.string();
      if (t == "code") {
        auto i = new Type(TypeID::code, 0);
        parsed_items.push_back(i);
      } else if (t == "tuple") {
        auto i = new Type(TypeID::tuple, 0);
        parsed_items.push_back(i);
      } else {
        // only int64 types left
        auto n = std::count(t.begin(), t.end(), '[');
        auto i = new Type(TypeID::int64, n);
        parsed_items.push_back(i);
      }
      
    }
  };

  template<> struct action < T > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      print("Adding a T item");
      // Remove duplicates from type rule]
      if (parsed_items.size() != 0) {
        parsed_items.pop_back();
      }
      auto t = in.string();
      if (t == "code") {
        auto i = new Type(TypeID::code, 0);
        parsed_items.push_back(i);
      } else if (t == "tuple") {
        auto i = new Type(TypeID::tuple, 0);
        parsed_items.push_back(i);
      } else if (t == "void") {
        auto i = new Type(TypeID::void_type, 0);
        parsed_items.push_back(i);
      } else {
        // only int64 types left
        auto n = std::count(t.begin(), t.end(), '[');
        auto i = new Type(TypeID::int64, n);
        parsed_items.push_back(i);
      }
      print("oops2");
    }
  };

  template<> struct action < label > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      print("Adding a label item");
      parsed_items.pop_back();

      auto i = new Label(in.string());
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

  template<> struct action < op > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      print("Adding an op item");

      auto op = in.string();

      if (op == "+") {
        auto i = new Operator(OperatorID::addq);
        parsed_items.push_back(i);
      } else if (op == "-") {
        auto i = new Operator(OperatorID::subq);
        parsed_items.push_back(i);
      } else if (op == "*") {
        auto i = new Operator(OperatorID::imultq);
        parsed_items.push_back(i);
      } else if (op == "&") {
        auto i = new Operator(OperatorID::andq);
        parsed_items.push_back(i);
      } else if (op == "<<") {
        auto i = new Operator(OperatorID::salq);
        parsed_items.push_back(i);
      } else if (op == ">>") {
        auto i = new Operator(OperatorID::sarq);
        parsed_items.push_back(i);
      } else if (op == "<") {
        auto i = new Operator(OperatorID::l);
        parsed_items.push_back(i);
      } else if (op == "<=") {
        auto i = new Operator(OperatorID::le);
        parsed_items.push_back(i);
      } else if (op == "=") {
        auto i = new Operator(OperatorID::e);
        parsed_items.push_back(i);
      } else if (op == ">=") {
        auto i = new Operator(OperatorID::ge);
        parsed_items.push_back(i);
      } else if (op == ">") {
        auto i = new Operator(OperatorID::g);
        parsed_items.push_back(i);
      } else {
        std::cerr << "Operator isn't correct";
      }
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

      if (parsed_items.size() > 1) {
        //cout<<"why am i here";
        for (int i = 0; i <= n && parsed_items.size()>=2; i++) {
          //vars.insert(vars.begin(), parsed_items.back());
          //parsed_items.pop_back();
          //parsed_items.pop_back();
          //if(parsed_items.back()->to_string().front()=='%'){}
          vars.insert(vars.begin(), parsed_items.back());
          parsed_items.pop_back();
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


  template<> struct action < Instruction_declaration_rule > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      print("Adding a declaration instruction");

      auto currentF = p.functions.back();

      auto name = parsed_items.back();
      parsed_items.pop_back();
      auto type = parsed_items.back();
      parsed_items.pop_back();

      
      
      auto i = new Instruction_declaration(type, name);
      i->line_number = in.position().line;

  
      currentF->instructions.push_back(i);
    }
    
  };

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
      i->line_number = in.position().line;

  
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
      i->line_number = in.position().line;
  
      currentF->instructions.push_back(i);
    }
  };

  template<> struct action < Instruction_read_tensor_rule > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      print("Adding a read tensor instruction");
      auto currentF = p.functions.back();
      auto instr = in.string();
      auto num_dims = std::count(instr.begin(), instr.end(), '[');

      vector<Item*> args;

      for (int i = 0; i < num_dims; i++) {
        args.insert(args.begin(), parsed_items.back());
        parsed_items.pop_back();
      }

      auto src = parsed_items.back();
      parsed_items.pop_back();
      auto dst = parsed_items.back();
      parsed_items.pop_back();

      auto i = new Instruction_read_tensor(dst, src, args);
      i->line_number = in.position().line;
      currentF->instructions.push_back(i);
    }
    
  };

  template<> struct action < Instruction_write_tensor_rule > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      print("Adding a write tensor instruction");
      auto currentF = p.functions.back();

      auto src = parsed_items.back();
      parsed_items.pop_back();

      auto instr = in.string();
      auto num_dims = std::count(instr.begin(), instr.end(), '[');

      vector<Item*> args;

      for (int i = 0; i < num_dims; i++) {
        args.insert(args.begin(), parsed_items.back());
        parsed_items.pop_back();
      }
      
      auto dst = parsed_items.back();
      parsed_items.pop_back();

      auto i = new Instruction_write_tensor(dst, src, args);
      i->line_number = in.position().line;
      currentF->instructions.push_back(i);
    }
    
  };



  template<> struct action < Instruction_length_rule > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      print("Adding a length t instruction");
      auto currentF = p.functions.back();

      auto index = parsed_items.back();
      parsed_items.pop_back();

      auto dst = parsed_items.back();
      parsed_items.pop_back();

      auto src = parsed_items.back();
      parsed_items.pop_back();

      auto i = new Instruction_length(src, dst, index);
      i->line_number = in.position().line;
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
      i->line_number = in.position().line;
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
      i->line_number = in.position().line;
  
      currentF->instructions.push_back(i);
    }
  };

  template<> struct action < Instruction_new_array_rule > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      print("Adding a new array instruction");

      auto currentF = p.functions.back();
      auto sizes = parsed_items.back();
      parsed_items.pop_back();

      auto dst = parsed_items.back();
      parsed_items.pop_back();

      auto i = new Instruction_new_array(dst, sizes);
      i->line_number = in.position().line;
      currentF->instructions.push_back(i);

    }
    
  };

  template<> struct action < Instruction_new_tuple_rule > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      print("Adding a new tuple instruction");
      auto currentF = p.functions.back();

      auto size = parsed_items.back();
      parsed_items.pop_back();

      auto dst = parsed_items.back();
      parsed_items.pop_back();

      auto i = new Instruction_new_tuple(dst, size);
      i->line_number = in.position().line;
      currentF->instructions.push_back(i);
    }
    
  };

  template<> struct action < Instruction_label_rule > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      print("added a label");
      auto currentF = p.functions.back();

      parsed_items.pop_back();

      auto dst = new Label(in.string());

      auto i = new Instruction_label(dst);
      i->line_number = in.position().line;
      currentF->instructions.push_back(i);
    }
  };


// Terminators

  template<> struct action < Instruction_return_rule > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      print("added a return");
      auto currentF = p.functions.back();
      auto i = new Instruction_return();
      i->line_number = in.position().line;
      currentF->instructions.push_back(i);
    }
  };

  template<> struct action < Instruction_return_arg_rule > {
    template< typename Input >
	  static void apply( const Input & in, Program & p) {
      print("added a return_arg");
      auto currentF = p.functions.back();
      auto arg = parsed_items.back();
      parsed_items.pop_back();
      auto i = new Instruction_return_arg(arg);
      i->line_number = in.position().line;
      currentF->instructions.push_back(i);
    }
  };

  template<> struct action < Instruction_br_rule > {
    template< typename Input >
	  static void apply( const Input & in, Program & p) {
      print("Added a br");
      auto currentF = p.functions.back();
      auto dst = parsed_items.back();
      parsed_items.pop_back();
      auto i = new Instruction_branch(dst);
      i->line_number = in.position().line;
      currentF->instructions.push_back(i);
    }
  };

  template<> struct action < Instruction_br_conditional_rule > {
    template< typename Input >
	  static void apply( const Input & in, Program & p) {
      print("added a br conditional");
      auto currentF = p.functions.back();
      auto dst2 = parsed_items.back();
      parsed_items.pop_back();
      auto dst1 = parsed_items.back();
      parsed_items.pop_back();
      auto cond = parsed_items.back();
      parsed_items.pop_back();
      auto i = new Instruction_branch_conditional(cond, dst1, dst2);
      i->line_number = in.position().line;
      currentF->instructions.push_back(i);
    }
  };

  template<> struct action < Function_rule > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      print("Adding a function instruction");
      auto currentF = p.functions.back();
      p.function_names.push_back(currentF->name);
      auto vars = parsed_items.back();
      currentF->arguments=vars;
      parsed_items.pop_back();
      // Make the longest name
      currentF->longest_name += "LONGEST";
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
    for (auto i : parsed_items) {
      print(i->to_string());
    }
    return p;
  }

}
