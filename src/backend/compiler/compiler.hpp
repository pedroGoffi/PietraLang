#ifndef  __COMPILER_H__
#define  __COMPILER_H__
#include "../ast/pi_module.cpp"
#include "../VM/PVM.cpp"


enum CT_Types{
  VOID = 0,
  DATA,
  PTR

};

class Compiler{
  FILE* out = stdout;
public:
  VM    vm;
  
public:
  void codegen(Module*);
  
  CT_Types expr(unique_ptr<Expr>);
  CT_Types stmt(unique_ptr<Stmt>);
  CT_Types decl(unique_ptr<Decl>);
};

#endif /*__COMPILER_H__*/
