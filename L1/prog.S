.text
.globl go
go: pushq %rbx
 pushq %rbp
 pushq %r12
 pushq %r13
 pushq %r14
 pushq %r15
call _main
 popq %r15
 popq %r14
 popq %r13
 popq %r12
 popq %rbp
 popq %rbx
retq
_main:
 subq $0, %rsp
_new_label0_global_2:
 movq $1, %rax
 movq $_fib0_ret, -8(%rsp)
 movq $19, %rdi
 subq $8, %rsp
 jmp _fib
_fib0_ret:
 movq %rax, %rax
 movq $_print1_ret, -8(%rsp)
 movq %rax, %rdi
 call print
_print1_ret:
 addq $0, %rsp
retq
_fib:
 subq $16, %rsp
 movq %rdi, %rax
_new_label0_global_3:
 movq $1, %r10
 movq $1, %r10
 movq %r10, 0(%rsp)
 movq $1, %r10
 movq %r10, 8(%rsp)
 movq $1, %r11
 movq $1, %r10
 movq $1, %r11
 movq $1, %r11
 movq %rax, %r11
 sarq $1, %r11
 movq $1, %r10
 movq $1, %r10
 sarq $1, %r10
 cmpq %r10, %r11
 sete %r11b
 movzbq %r11b, %r11
 salq $1, %r11
 addq $1, %r11
 movq $1, %r10
 movq %r11, %r10
 sarq $1, %r10
 cmpq $1, %r10
 je _new_label0_global_4
 jmp _new_label0_global_5
_new_label0_global_5:
 movq $1, %r11
 movq $1, %r11
 movq %rax, %r11
 sarq $1, %r11
 movq $1, %r10
 movq $3, %r10
 sarq $1, %r10
 cmpq %r10, %r11
 sete %r11b
 movzbq %r11b, %r11
 salq $1, %r11
 addq $1, %r11
 movq $1, %r10
 movq %r11, %r10
 sarq $1, %r10
 cmpq $1, %r10
 je _new_label0_global_6
 jmp _new_label0_global_7
_new_label0_global_7:
 movq $1, %r10
 movq %rax, %r10
 sarq $1, %r10
 movq $1, %r11
 movq $5, %r11
 sarq $1, %r11
 movq %r10, %r10
 subq %r11, %r10
 salq $1, %r10
 addq $1, %r10
 movq $1, %rdi
 movq %rax, %rdi
 sarq $1, %rdi
 movq $1, %r11
 movq $3, %r11
 sarq $1, %r11
 movq %rdi, %rax
 movq %rax, 0(%rsp)
 movq 0(%rsp), %rax
 subq %r11, %rax
 movq %rax, 0(%rsp)
 movq 0(%rsp), %rax
 salq $1, %rax
 movq %rax, 0(%rsp)
 movq 0(%rsp), %rax
 addq $1, %rax
 movq %rax, 0(%rsp)
 movq $_fib2_ret, -8(%rsp)
 movq %r10, %rdi
 subq $8, %rsp
 jmp _fib
_fib2_ret:
 movq %rax, %rax
 movq %rax, 8(%rsp)
 movq $_fib3_ret, -8(%rsp)
 movq 0(%rsp), %rax
 movq %rax, %rdi
 subq $8, %rsp
 jmp _fib
_fib3_ret:
 movq %rax, %r11
 movq $1, %r10
 movq 8(%rsp), %rax
 movq %rax, %r10
 sarq $1, %r10
 movq $1, %rax
 movq %r11, %rax
 sarq $1, %rax
 movq %r10, %r10
 addq %rax, %r10
 salq $1, %r10
 addq $1, %r10
 movq %r10, %rax
 addq $16, %rsp
retq
_new_label0_global_6:
 movq $3, %rax
 addq $16, %rsp
retq
_new_label0_global_4:
 movq $1, %rax
 addq $16, %rsp
retq
