#ifndef  __COMPILER_CPP__
#define __COMPILER_CPP__
#include "../ast/pi_module.cpp"
#include "./codegen.cpp"


class Compiler{
  FILE* out = stdout;
  
public:
  Compiler(){}
  void open_file(const char* fp);
  void codegen(Module* mod);
};

void Compiler::open_file(const char* fp){
  this->out = fopen(fp, "w");
  assert(this->out);
}
void Compiler::codegen(Module* mod){
  for(size_t i=0; i < mod->AST.size(); ++i){
    unique_ptr<Decl> node = move(mod->AST[i]);
    switch(node->kind){
    case DC_PROC:
      assert(node->proc);
      printf("ok\n");
      Emit::Decl::Proc(this->out, move(*node->proc));
      break;
    default:
      fprintf(stderr, "Error: undefined node->kind.\n");
      exit(1);
    }
  }
}
#endif /*__COMPILER_CPP__*/
