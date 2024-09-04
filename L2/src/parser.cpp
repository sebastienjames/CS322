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

#include <L2.h>
#include <parser.h>

namespace pegtl = tao::TAO_PEGTL_NAMESPACE;



using namespace pegtl;

namespace L2 {
  bool printthing = false;
  bool actions = printthing;
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
  struct str_return : TAOCPP_PEGTL_STRING( "return" ) {};
  struct str_arrow : TAOCPP_PEGTL_STRING( "<-"/*"*/ ) {}; // For VSCode's color shenanigans
  struct str_rax : TAOCPP_PEGTL_STRING( "rax" ) {};
  // struct str_rbx : TAOCPP_PEGTL_STRING( "rbx" ) {};
  // struct str_rbp : TAOCPP_PEGTL_STRING( "rbp" ) {};
  // struct str_r10 : TAOCPP_PEGTL_STRING( "r10" ) {};
  // struct str_r11 : TAOCPP_PEGTL_STRING( "r11" ) {};
  // struct str_r12 : TAOCPP_PEGTL_STRING( "r12" ) {};
  // struct str_r13 : TAOCPP_PEGTL_STRING( "r13" ) {};
  // struct str_r14 : TAOCPP_PEGTL_STRING( "r14" ) {};
  // struct str_r15 : TAOCPP_PEGTL_STRING( "r15" ) {};
  struct str_rdi : TAOCPP_PEGTL_STRING( "rdi" ) {};
  struct str_rsi : TAOCPP_PEGTL_STRING( "rsi" ) {};
  struct str_rdx : TAOCPP_PEGTL_STRING( "rdx" ) {};
  struct str_rcx : TAOCPP_PEGTL_STRING( "rcx" ) {};
  struct str_r8 : TAOCPP_PEGTL_STRING( "r8" ) {};
  struct str_r9 : TAOCPP_PEGTL_STRING( "r9" ) {};
  struct str_rsp : TAOCPP_PEGTL_STRING( "rsp" ) {};
  struct str_mem : TAOCPP_PEGTL_STRING( "mem" ) {};
  struct str_plus_equals : TAOCPP_PEGTL_STRING( "+=" ) {};
  struct str_minus_equals : TAOCPP_PEGTL_STRING( "-=" ) {};
  struct str_times_equals : TAOCPP_PEGTL_STRING( "*=" ) {};
  struct str_and_equals : TAOCPP_PEGTL_STRING( "&=" ) {};
  struct str_shift_left : TAOCPP_PEGTL_STRING( "<<=" ) {};
  struct str_shift_right : TAOCPP_PEGTL_STRING( ">>=" ) {};
  struct str_less : TAOCPP_PEGTL_STRING( "<" ) {};
  struct str_less_equal : TAOCPP_PEGTL_STRING( "<=" ) {};
  struct str_equal : TAOCPP_PEGTL_STRING( "=" ) {};
  struct str_cjump : TAOCPP_PEGTL_STRING( "cjump" ) {};
  struct str_add_1 : TAOCPP_PEGTL_STRING( "++" ) {};
  struct str_sub_1 : TAOCPP_PEGTL_STRING( "--" ) {};
  struct str_goto : TAOCPP_PEGTL_STRING( "goto" ) {};
  struct str_call : TAOCPP_PEGTL_STRING( "call" ) {};
  struct str_print : TAOCPP_PEGTL_STRING( "print" ) {};
  struct str_input : TAOCPP_PEGTL_STRING( "input" ) {};
  struct str_allocate : TAOCPP_PEGTL_STRING( "allocate" ) {};
  struct str_tensor_error : TAOCPP_PEGTL_STRING( "tensor-error" ) {};
  struct str_stack_arg : TAOCPP_PEGTL_STRING( "stack-arg" ) {};
  


  struct label:
    pegtl::seq<
      pegtl::one<':'>,
      name
    > {};

  struct function_name_rule:
    pegtl::seq<
      pegtl::one<'@'>,
      name
    > {};

  

  struct l_rule:
    pegtl::seq<
      pegtl::one<'@'>,
      name
    > {};

  struct variable_rule:
    pegtl::seq<
      pegtl::one<'%'>,
      name
    > {};

  struct register_rax_rule:
      str_rax {};

  // struct register_rbx_rule:
  //     str_rbx {};

  // struct register_rbp_rule:
  //     str_rbp {};

  // struct register_r10_rule:
  //     str_r10 {};

  // struct register_r11_rule:
  //     str_r11 {};

  // struct register_r12_rule:
  //     str_r12 {};

  // struct register_r13_rule:
  //     str_r13 {};

  // struct register_r14_rule:
  //     str_r14 {};

  // struct register_r15_rule:
  //     str_r15 {};

  struct register_rdi_rule:
      str_rdi {};

  struct register_rsi_rule:
      str_rsi {};

  struct register_rdx_rule:
      str_rdx {};

  struct register_rcx_rule:
      str_rcx {};

  struct register_r8_rule:
      str_r8 {};

  struct register_r9_rule:
      str_r9 {};

  struct register_rsp_rule:
      str_rsp {};

  struct operator_add_rule:
    str_plus_equals {};

  struct operator_sub_rule:
    str_minus_equals {};

  struct operator_mult_rule:
    str_times_equals {};

  struct operator_band_rule:
    str_and_equals {};

  struct operator_shift_left_rule:
      str_shift_left {};

  struct operator_shift_right_rule:
      str_shift_right {};

    struct operator_l_rule:
        str_less {};

    struct operator_le_rule:
        str_less_equal {};

    struct operator_e_rule:
        str_equal {};
   
  struct w_rule:
    pegtl::sor<
      register_rax_rule,
      // register_rbx_rule,
      // register_rbp_rule,
      // register_r10_rule,
      // register_r11_rule,
      // register_r12_rule,
      // register_r13_rule,
      // register_r14_rule,
      // register_r15_rule,
      register_rdi_rule,
      register_rsi_rule,
      register_rdx_rule,
      register_rcx_rule,
      register_r8_rule,
      register_r9_rule,
      variable_rule
    > {};

  struct x_rule:
    pegtl::sor<
      w_rule,
      register_rsp_rule
    > {};



  struct number:
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

  struct t_rule:
      pegtl::sor<
        x_rule,
        number
      > {};

  struct s_rule:
      pegtl::sor<
        t_rule,
        l_rule,
        label
      > {};

  struct plus_minus_rule:
        pegtl::sor<
          operator_add_rule,
          operator_sub_rule
        > {};

        
  struct aop_rule:
    pegtl::sor<
        plus_minus_rule,
        operator_mult_rule,
        operator_band_rule
        > {};


    struct sop_rule:
        pegtl::sor<
            operator_shift_left_rule,
            operator_shift_right_rule
            > {} ;

    struct cmp_rule:
        pegtl::sor<
            pegtl::seq<pegtl::at<operator_le_rule>, operator_le_rule>,
            pegtl::seq<pegtl::at<operator_l_rule>, operator_l_rule>,
            pegtl::seq<pegtl::at<operator_e_rule>, operator_e_rule>
            > {} ;

    
        
  

          
  struct function_name:
    label {};

  struct argument_number:
    number {};

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

  struct mem_rule:
      pegtl::seq<
        str_mem,
        seps,
        x_rule,
        seps,
        number
      > {};

  struct Instruction_return_rule:
    pegtl::seq<
      str_return
    > {};

    struct Instruction_arithmetic_rule:
      pegtl::sor<
        pegtl::seq<
            w_rule,
            seps,
            aop_rule,
            seps,
            t_rule
            >,
        pegtl::seq<
            mem_rule,
            seps,
            plus_minus_rule,
            seps,
            t_rule
            >,
        pegtl::seq<
            w_rule,
            seps,
            plus_minus_rule,
            seps,
            mem_rule
            >
      > {};

  struct Instruction_shift_rule:
    pegtl::sor<
        pegtl::seq<
            w_rule,
            seps,
            sop_rule,
            seps,
            register_rcx_rule
            >,
        pegtl::seq<
            w_rule,
            seps,
            sop_rule,
            seps,
            number
            >,
        pegtl::seq<
            w_rule,
            seps,
            sop_rule,
            seps,
            variable_rule
            >
        
        > {} ;
            
  struct Compare_rule:
    pegtl::seq<
        t_rule,
        seps,
        cmp_rule,
        seps,
        t_rule
        > {} ;

  struct Instruction_assignment_rule:
    pegtl::sor<
        pegtl::seq<
            w_rule,
            seps,
            str_arrow,
            seps,
            s_rule
        >,
        pegtl::seq<
            mem_rule,
            seps,
            str_arrow,
            seps,
            s_rule
        >,
        pegtl::seq<
          w_rule,
          seps,
          str_arrow,
          seps,
          mem_rule
        >
    > {};
        
    
  struct Instruction_assignment_compare_rule:
        pegtl::seq<
                w_rule,
                seps,
                str_arrow,
                seps,
                Compare_rule
            > {} ;

  struct Instruction_assignment_stack_arg_rule:
        pegtl::seq<
                w_rule,
                seps,
                str_arrow,
                seps,
                str_stack_arg,
                seps,
                number
            > {} ;


  struct Instruction_inc_rule:
      pegtl::seq<
          w_rule,
          seps,
          str_add_1
      > {} ;
          
  struct Instruction_dec_rule:
      pegtl::seq<
          w_rule,
          seps,
          str_sub_1
      > {} ;

  struct Instruction_goto_rule:
      pegtl::seq<
          str_goto,
          seps,
          label
      > {} ;

  struct Instruction_cjump_rule:
      pegtl::seq<
          str_cjump,
          seps,
          Compare_rule,
          seps,
          label
      > {} ;

  struct Instruction_at_rule:
      pegtl::seq<
          w_rule,
          seps,
          pegtl::one<'@'>,
          seps,
          w_rule,
          seps,
          w_rule,
          seps,
          number
      > {} ;
      
  struct Instruction_call_rule:
          pegtl::seq<
              str_call,
              seps,
              pegtl::sor<
                  l_rule,
                  w_rule
              >,
              seps,
              number
          > {} ;

  struct Instruction_call_special_rule:
          pegtl::seq<
              str_call,
              seps,
              pegtl::sor<
                  str_print,
                  str_input,
                  str_allocate,
                  str_tensor_error
              >,
              seps,
              number
          > {} ;

  struct Instruction_label_rule:
      pegtl::seq<
          label
      > {} ; 
        

  struct Instruction_rule:
    pegtl::sor<
      pegtl::seq< pegtl::at<Instruction_return_rule>            , Instruction_return_rule             >,
      pegtl::seq< pegtl::at<Instruction_arithmetic_rule>        , Instruction_arithmetic_rule         >,
      pegtl::seq< pegtl::at<Instruction_shift_rule>             , Instruction_shift_rule              >,
      pegtl::seq< pegtl::at<Instruction_assignment_compare_rule>, Instruction_assignment_compare_rule >,
      pegtl::seq< pegtl::at<Instruction_assignment_rule>        , Instruction_assignment_rule         >,
      pegtl::seq< pegtl::at<Instruction_inc_rule>               , Instruction_inc_rule                >,
      pegtl::seq< pegtl::at<Instruction_dec_rule>               , Instruction_dec_rule                >,
      pegtl::seq< pegtl::at<Instruction_at_rule>                , Instruction_at_rule                 >,
      pegtl::seq< pegtl::at<Instruction_call_rule>              , Instruction_call_rule               >,
      pegtl::seq< pegtl::at<Instruction_goto_rule>              , Instruction_goto_rule               >,
      pegtl::seq< pegtl::at<Instruction_assignment_stack_arg_rule>         , Instruction_assignment_stack_arg_rule          >,
      pegtl::seq< pegtl::at<Instruction_call_special_rule>      , Instruction_call_special_rule       >,
      pegtl::seq< pegtl::at<Instruction_cjump_rule>             , Instruction_cjump_rule              >,
      pegtl::seq< pegtl::at<Instruction_call_special_rule>      , Instruction_call_special_rule       >,
      pegtl::seq< pegtl::at<Instruction_call_special_rule>      , Instruction_call_special_rule       >,
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
      seps,
      pegtl::one< '(' >,
      seps,
      function_name_rule,
      seps,
      argument_number,
      seps,
      Instructions_rule,
      seps,
      pegtl::one< ')' >
    > {};

  struct Functions_rule:
    pegtl::plus<
      seps,
      Function_rule,
      seps
    > {};

  struct entry_point_rule:
    pegtl::seq<
      seps,
      pegtl::one< '(' >,
      seps,
      function_name_rule,
      seps,
      Functions_rule,
      seps,
      pegtl::one< ')' >,
      seps
    > { };
  
  struct to_replace_rule:
    pegtl::seq<
      variable_rule
    > {};

  struct replace_by_rule:
    pegtl::seq<
      variable_rule
    > {};
  struct parameter_rule:
    pegtl::seq<
      to_replace_rule,
      seps,
      replace_by_rule
    > {};

  struct grammar : 
    pegtl::must< 
      entry_point_rule
    > {};

  struct function_grammar:
  	pegtl::must<
  		Function_rule
  		>{};

  struct spill_grammar:
  	pegtl::must<
  		Function_rule,
      seps,
      parameter_rule
  		>{};




  /* 
   * Actions attached to grammar rules.
   */
  template< typename Rule >
  struct action : pegtl::nothing< Rule > {};

  template<> struct action < function_name_rule > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      if (printthing) { print("function rule"); }
      	if (p.entryPointLabel.empty()){
        	p.entryPointLabel = in.string();
      	} else {
        auto newF = new Function();
        newF->name = in.string();
        p.functions.push_back(newF);
      }
    }
  };

  template<> struct action < number > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      auto number = new Number(std::stoll(in.string()));
      parsed_items.push_back(number);
      if (printthing) std::cerr << "Adding number token" << parsed_items.size() << std::endl;
    }
  };

  template<> struct action < label > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      auto label = new Label(in.string());
      parsed_items.push_back(label);
      if (printthing) std::cerr << "Adding label token" << parsed_items.size() << std::endl;

    }
  };

  template<> struct action < variable_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      auto label = new Variable(in.string());
      parsed_items.push_back(label);
      if (printthing) std::cerr << "Adding variable token" << parsed_items.size() << std::endl;

    }
  };
      
  template<> struct action < argument_number > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      auto currentF = p.functions.back();
      currentF->arguments = std::stoll(in.string());
    }
  };


  template<> struct action < Instruction_return_rule > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      auto currentF = p.functions.back();
      auto i = new Instruction_ret();
      currentF->instructions.push_back(i);
    }
  };


  // Register Action Rules

  template<> struct action < register_rax_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      auto r = new Register(RegisterID::rax);
      parsed_items.push_back(r);
      if (printthing) std::cerr << "Added rax token" << parsed_items.size();
      
    }
  };

  // template<> struct action < register_rbx_rule > {
  //   template< typename Input >
  //   static void apply( const Input & in, Program & p){
  //     auto r = new Register(RegisterID::rbx);
  //     parsed_items.push_back(r);
  //     if (printthing) std::cerr << "Added rbx token" << parsed_items.size() << std::endl;
      
  //   }
  // };

  // template<> struct action < register_rbp_rule > {
  //   template< typename Input >
  //   static void apply( const Input & in, Program & p){
  //     auto r = new Register(RegisterID::rbp);
  //     parsed_items.push_back(r);
  //     if (printthing) std::cerr << "Added rbp token" << parsed_items.size() << std::endl;
      
  //   }
  // };

  // template<> struct action < register_r10_rule > {
  //   template< typename Input >
  //   static void apply( const Input & in, Program & p){
  //     auto r = new Register(RegisterID::r10);
  //     parsed_items.push_back(r);
  //     if (printthing) std::cerr << "Added r10 token" << parsed_items.size() << std::endl;
      
  //   }
  // };

  // template<> struct action < register_r11_rule > {
  //   template< typename Input >
  //   static void apply( const Input & in, Program & p){
  //     auto r = new Register(RegisterID::r11);
  //     parsed_items.push_back(r);
  //     if (printthing) std::cerr << "Added r11 token" << parsed_items.size() << std::endl;
      
  //   }
  // };

  // template<> struct action < register_r12_rule > {
  //   template< typename Input >
  //   static void apply( const Input & in, Program & p){
  //     auto r = new Register(RegisterID::r12);
  //     parsed_items.push_back(r);
  //     if (printthing) std::cerr << "Added r12 token" << parsed_items.size() << std::endl;
      
  //   }
  // };

  // template<> struct action < register_r13_rule > {
  //   template< typename Input >
  //   static void apply( const Input & in, Program & p){
  //     auto r = new Register(RegisterID::r13);
  //     parsed_items.push_back(r);
  //     if (printthing) std::cerr << "Added r13 token" << parsed_items.size() << std::endl;
      
  //   }
  // };

  // template<> struct action < register_r14_rule > {
  //   template< typename Input >
  //   static void apply( const Input & in, Program & p){
  //     auto r = new Register(RegisterID::r14);
  //     parsed_items.push_back(r);
  //     if (printthing) std::cerr << "Added r14 token" << parsed_items.size() << std::endl;
      
  //   }
  // };

  // template<> struct action < register_r15_rule > {
  //   template< typename Input >
  //   static void apply( const Input & in, Program & p){
  //     auto r = new Register(RegisterID::r15);
  //     parsed_items.push_back(r);
  //     if (printthing) std::cerr << "Added r15 token" << parsed_items.size() << std::endl;
      
  //   }
  // };

  template<> struct action < register_rdi_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      auto r = new Register(RegisterID::rdi);
      parsed_items.push_back(r);
      if (printthing) std::cerr << "Added rdi token" << parsed_items.size() << std::endl;
      
    }
  };

  template<> struct action < register_rsi_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      auto r = new Register(RegisterID::rsi);
      parsed_items.push_back(r);
      if (printthing) std::cerr << "Added rsi token" << parsed_items.size() << std::endl;
      
    }
  };

  template<> struct action < register_rdx_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      auto r = new Register(RegisterID::rdx);
      parsed_items.push_back(r);
      if (printthing) std::cerr << "Added rdx token" << parsed_items.size() << std::endl;
      
    }
  };

  template<> struct action < register_rcx_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      auto r = new Register(RegisterID::rcx);
      parsed_items.push_back(r);
      if (printthing) std::cerr << "Added rcx token" << parsed_items.size() << std::endl;
      
    }
  };

  template<> struct action < register_r8_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      auto r = new Register(RegisterID::r8);
      parsed_items.push_back(r);
      if (printthing) std::cerr << "Added r8 token" << parsed_items.size() << std::endl;
      
    }
  };

  template<> struct action < register_r9_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      auto r = new Register(RegisterID::r9);
      parsed_items.push_back(r);
      if (printthing) std::cerr << "Added r9 token" << parsed_items.size() << std::endl;
      
    }
  };

  template<> struct action < register_rsp_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      auto r = new Register(RegisterID::rsp);
      parsed_items.push_back(r);
      if (printthing) std::cerr << "Added rsp token" << parsed_items.size() << std::endl;
      
    }
  };

  // Operator Rules

  template<> struct action < operator_add_rule > {
    template< typename Input >
      static void apply( const Input & in, Program & p){
      auto r = new Operator(OperatorID::addq);
      parsed_items.push_back(r);
      if (printthing) std::cerr << "Added += token" << parsed_items.size() << std::endl;
      
    }
  };

  template<> struct action < operator_sub_rule > {
    template< typename Input >
      static void apply( const Input & in, Program & p){
      auto r = new Operator(OperatorID::subq);
      parsed_items.push_back(r);
      if (printthing) std::cerr << "Added -= token" << parsed_items.size() << std::endl;
      
    }
  };

  template<> struct action < operator_mult_rule > {
    template< typename Input >
      static void apply( const Input & in, Program & p){
      auto r = new Operator(OperatorID::imulq);
      parsed_items.push_back(r);
      if (printthing) std::cerr << "Added *= token" << parsed_items.size() << std::endl;
      
    }
  };

  template<> struct action < operator_band_rule > {
    template< typename Input >
      static void apply( const Input & in, Program & p){
      auto r = new Operator(OperatorID::andq);
      parsed_items.push_back(r);
      if (printthing) std::cerr << "Added &= token" << parsed_items.size() << std::endl;
      
    }
  };

  template<> struct action < operator_shift_left_rule > {
    template< typename Input >
      static void apply( const Input & in, Program & p){
      auto r = new Operator(OperatorID::salq);
      parsed_items.push_back(r);
      if (printthing) std::cerr << "Added <<= token" << parsed_items.size() << std::endl;
      
    }
  };

  template<> struct action < operator_shift_right_rule > {
    template< typename Input >
      static void apply( const Input & in, Program & p){
      auto r = new Operator(OperatorID::sarq);
      parsed_items.push_back(r);
      if (printthing) std::cerr << "Added >>= token" << parsed_items.size() << std::endl;
      
    }
  };
  template<> struct action < operator_l_rule > {
    template< typename Input >
      static void apply( const Input & in, Program & p){
      auto r = new Operator(OperatorID::l);
      parsed_items.push_back(r);
      if (printthing) std::cerr << "Added < token" << parsed_items.size() << std::endl;
      
    }
  };

  template<> struct action < operator_le_rule > {
    template< typename Input >
      static void apply( const Input & in, Program & p){
      auto r = new Operator(OperatorID::le);
      parsed_items.push_back(r);
      if (printthing) std::cerr << "Added <= token" << parsed_items.size() << std::endl;
      
    }
  };
       
  template<> struct action < operator_e_rule > {
    template< typename Input >
      static void apply( const Input & in, Program & p){
      auto r = new Operator(OperatorID::e);
      parsed_items.push_back(r);
      if (printthing) std::cerr << "Added = token" << parsed_items.size() << std::endl;
    }
  };


  template<> struct action < mem_rule > {
    template< typename Input >
      static void apply( const Input & in, Program & p){
      if (actions) std::cerr << "Mem rule" << std::endl;

      
      auto M = parsed_items.back();
      parsed_items.pop_back();
      if (printthing) std::cerr << "Removed token (r)" << parsed_items.size() << std::endl;
      
      auto r = parsed_items.back();
      parsed_items.pop_back();
      if (printthing) std::cerr << "Removed token (M)" << parsed_items.size() << std::endl;

      auto mem = new Mem(r, M);
      parsed_items.push_back(mem);
      if (printthing) std::cerr << "Pushed Mem token" << parsed_items.size() << std::endl;
    }
  };



  // Instructions

  template<> struct action < Instruction_assignment_rule > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      if (actions) {
          std::cout << "Assignment\n";
      }
       
      // Fetch the current function.
        
      auto currentF = p.functions.back();

      
      //Fetch the last two tokens parsed.
      
      
      auto src = parsed_items.back();
      parsed_items.pop_back();
      if (printthing) std::cerr << "Removed token (src)" << parsed_items.size() << std::endl;
      auto dst = parsed_items.back();
      parsed_items.pop_back();
      if (printthing) std::cerr << "Removed token (dest)" << parsed_items.size() << std::endl;
      
      auto i = new Instruction_assignment(dst, src);

    
      currentF->instructions.push_back(i);
      if (printthing) std::cerr << std::endl;
    }
  };

  template<> struct action < Instruction_assignment_compare_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      if (actions) {
          std::cout << "Assignment Compare\n";
      }
      
      auto currentF = p.functions.back();

      
      auto src = parsed_items.back();
      parsed_items.pop_back();
      if (printthing) std::cerr << "Removed token (src)" << parsed_items.size() << std::endl;
      auto dst = parsed_items.back();
      parsed_items.pop_back();
      if (printthing) std::cerr << "Removed token (dst)" << parsed_items.size() << std::endl;

      
      auto i = new Instruction_assignment_compare(dst, src);

      
      currentF->instructions.push_back(i);
      if (printthing) std::cerr << std::endl;
    }
  };

  template<> struct action < Instruction_assignment_stack_arg_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      if (actions) {
          std::cout << "Assignment stack arg\n";
      }
      
      auto currentF = p.functions.back();

      
      auto src = parsed_items.back();
      parsed_items.pop_back();
      if (printthing) std::cerr << "Removed token (src)" << parsed_items.size() << std::endl;
      auto dst = parsed_items.back();
      parsed_items.pop_back();
      if (printthing) std::cerr << "Removed token (dst)" << parsed_items.size() << std::endl;

      
      auto i = new Instruction_assignment_stack_arg(dst, src);

      
      currentF->instructions.push_back(i);
      if (printthing) std::cerr << std::endl;
    }
  };

  template<> struct action < Instruction_arithmetic_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      if (actions) {
          std::cout << "Math\n";
      }
      
      auto currentF = p.functions.back();

      
      auto src = parsed_items.back();
      parsed_items.pop_back();
      if (printthing) std::cerr << "Removed token (src)" << parsed_items.size() << std::endl;
      auto op = parsed_items.back();
      parsed_items.pop_back();
      if (printthing) std::cerr << "Removed token (op)" << parsed_items.size() << std::endl;
      auto dst = parsed_items.back();
      parsed_items.pop_back();
      if (printthing) std::cerr << "Removed token (dst)" << parsed_items.size() << std::endl;

      
      auto i = new Instruction_arithmetic(dst, src, op);

      
      currentF->instructions.push_back(i);
      if (printthing) std::cerr << std::endl;
    }
  };

  template<> struct action < Instruction_shift_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      if (actions) {
          std::cout << "Shifting\n";
      }
      
      auto currentF = p.functions.back();

      
      auto src = parsed_items.back();
      parsed_items.pop_back();
      if (printthing) std::cerr << "Removed token (src)" << parsed_items.size() << std::endl;
      auto op = parsed_items.back();
      parsed_items.pop_back();
      if (printthing) std::cerr << "Removed token (op)" << parsed_items.size() << std::endl;
      auto dst = parsed_items.back();
      parsed_items.pop_back();
      if (printthing) std::cerr << "Removed token (dst)" << parsed_items.size() << std::endl;

      
      auto i = new Instruction_shift(dst, src, op);

      
      currentF->instructions.push_back(i);
      if (printthing) std::cerr << std::endl;
    }
  };

    template<> struct action < Instruction_goto_rule > {
      template< typename Input >
      static void apply( const Input & in, Program & p){
        if (printthing) {
            std::cout << "Gtot\n";
            
        }
        
        auto currentF = p.functions.back();
  
        
        auto dst = parsed_items.back();
        parsed_items.pop_back();
        
  
        
        auto i = new Instruction_goto(dst);
  
        
        currentF->instructions.push_back(i);
        if (printthing) std::cerr << std::endl;
      }
    };

  template<> struct action < Instruction_cjump_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
        if (actions) {
            std::cout << "Cjump\n";
            
        }
    
      auto currentF = p.functions.back();

    
      
      
      auto label = parsed_items.back();
      parsed_items.pop_back();
      if (printthing) std::cerr << "Removed token (label)" << parsed_items.size() << std::endl;
      auto cmp = parsed_items.back();
      parsed_items.pop_back();
      if (printthing) std::cerr << "Removed token (cmp)" << parsed_items.size() << std::endl;

      
      
      auto i = new Instruction_cjump(cmp, label);

      
      currentF->instructions.push_back(i);
      if (printthing) std::cerr << std::endl;
    }
  };

  template<> struct action < Instruction_inc_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      if (actions) {
          std::cout << "pp\n";
      }
      
      auto currentF = p.functions.back();

      
      auto dst = parsed_items.back();
      parsed_items.pop_back();
      
      auto i = new Instruction_inc(dst);

      
      currentF->instructions.push_back(i);
      if (printthing) std::cerr << std::endl;
    }
  };

  template<> struct action < Instruction_dec_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      if (actions) {
          std::cout << "mm\n";
          
      }
      
      auto currentF = p.functions.back();

      
      auto dst = parsed_items.back();
      parsed_items.pop_back();

      
      auto i = new Instruction_dec(dst);

      
      currentF->instructions.push_back(i);
      if (printthing) std::cerr << std::endl;
    }
  };


  template<> struct action < Instruction_at_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
    if (actions) {
        std::cout << "at\n";
        
    }
    
    auto currentF = p.functions.back();

    
    auto by = parsed_items.back();
    parsed_items.pop_back();
    auto mult = parsed_items.back();
    parsed_items.pop_back();
    auto add = parsed_items.back();
    parsed_items.pop_back();
    auto saveTo = parsed_items.back();
    parsed_items.pop_back();

    
    auto i = new Instruction_at(saveTo, add, mult, by);

    
    currentF->instructions.push_back(i);
    if (printthing) std::cerr << std::endl;
    }
  };

  template<> struct action < Instruction_call_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      if (actions) {
          std::cout << "call\n";
      }
      
      auto currentF = p.functions.back();

      
      auto params = parsed_items.back();
      parsed_items.pop_back();
      if (printthing) std::cerr << "Removed token (params)" << parsed_items.size() << std::endl;
      auto func = parsed_items.back();
      parsed_items.pop_back();
      if (printthing) std::cerr << "Removed token (func)" << parsed_items.size() << std::endl;

      
      auto i = new Instruction_call(func, params);

      
      currentF->instructions.push_back(i);
      if (printthing) std::cerr << std::endl;
      }
    };

  template<> struct action < Instruction_call_special_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      if (actions) {
        std::cout << "call special\n";
      }
      
      auto currentF = p.functions.back();

      
      auto params = parsed_items.back();
      parsed_items.pop_back();
      auto func = new Label(in.string());

      
      auto i = new Instruction_call(func, params);

      
      currentF->instructions.push_back(i);
      if (printthing) std::cerr << std::endl;
      }
    };

  template<> struct action < Instruction_label_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      if (actions) {
          std::cout << "labelling\n";
      }
      
      auto currentF = p.functions.back();

      
      auto label = parsed_items.back();
      parsed_items.pop_back();

      auto i = new Instruction_label(label);

      
      currentF->instructions.push_back(i);
      if (printthing) std::cerr << std::endl;
      }
    };



  template<> struct action < Compare_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      if (actions) {
          std::cout << "Compare\n";
      }

      auto src = parsed_items.back();
      parsed_items.pop_back();
      if (printthing) std::cerr << "Removed token (src)" << parsed_items.size() << std::endl;
      auto op = parsed_items.back();
      parsed_items.pop_back();
      if (printthing) std::cerr << "Removed token (op)" << parsed_items.size() << std::endl;
      auto dst = parsed_items.back();
      parsed_items.pop_back();
      if (printthing) std::cerr << "Removed token (dst)" << parsed_items.size() << std::endl;

      
      auto i = new Compare(dst, src, op);

      parsed_items.push_back(i);

      if (printthing) std::cerr << std::endl;

    }
  };

  template<> struct action < l_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      if (actions) {
          std::cout << "Function Name Call\n";
      }

      
      auto l = new L(in.string());

      parsed_items.push_back(l);
      
      if (printthing) std::cerr << "Added token (@thing)" << parsed_items.size() << std::endl;
      

    }
  };

  template<> struct action < replace_by_rule > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      if (printthing) { print("parameter rule"); }
        p.entryPointLabel = in.string();
    } 
  };

  template<> struct action < to_replace_rule > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      if (printthing) { print("parameter rule"); }
        auto toReplace = parsed_items.back();

        p.toReplace = in.string();
    } 
  };

  Program parse_file (char *fileName){

    /* 
     * Check the grammar for some possible issues.
     */
    if(printthing) std::cout <<"start parse File";
    pegtl::analyze< grammar >();

    /*
     * Parse.
     */
    file_input< > fileInput(fileName);
    Program p;
    parse< grammar, action >(fileInput, p);
    if (printthing) std::cout << p.entryPointLabel;
    return p;
  }

  Program parse_function_file (char *fileName){
  	if(printthing) std::cout <<"start parse Func";
  	Program p;
  	p.entryPointLabel="Empty";
  	pegtl::analyze< function_grammar >();
  	file_input< > fileInput(fileName);
  	//Function f;
  	parse< function_grammar, action >(fileInput,p);
  	if (printthing) std::cout <<"end parse Func";
  	return p;
  }

  Program parse_spill_file (char *fileName){
  	if(printthing) std::cout << "start parse spill";
  	Program p;
  	p.entryPointLabel="Empty";
  	pegtl::analyze< spill_grammar >();
  	file_input< > fileInput(fileName);
  	//Function f;
  	parse< spill_grammar, action >(fileInput,p);
  	if (printthing) std::cout <<"end parse spill";
  	return p;
  }

}
