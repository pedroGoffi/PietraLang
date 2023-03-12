#ifndef  __COMPILER_H__
#define  __COMPILER_H__
#include "../ast/pi_module.cpp"
#include "../VM/PVM.cpp"


class Compiler{
  FILE* out = stdout;
public:
  VM    vm;
  
public:
  void codegen(Module*);
  
  unique_ptr<pi_type> expr(unique_ptr<Expr>);
  unique_ptr<pi_type> stmt(unique_ptr<Stmt>);
  unique_ptr<pi_type> decl(unique_ptr<Decl>);
};

struct Compiled_var{
  std::string         id;
  unique_ptr<pi_type> type;
  Allocated_Obj       allocation;
  
  static Compiled_var* find(Compiled_var* table, std::string name);
  static void          push(Compiled_var item);
};

Compiled_var compiled_vars[VM_MEM_CAP];
size_t       compiled_vars_size;

Compiled_var* Compiled_var::find(Compiled_var* table, std::string name){
  for(size_t i=0; i < compiled_vars_size; ++i){
    if(table[i].id == name){
      return &compiled_vars[i];
    }
  }
  return NULL;
}
void Compiled_var::push(Compiled_var item){
  assert(compiled_vars_size < VM_MEM_CAP);
  compiled_vars[compiled_vars_size++] = move(item);
}
#endif /*__COMPILER_H__*/
