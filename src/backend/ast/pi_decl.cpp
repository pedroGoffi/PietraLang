#ifndef  __DECL_CPP__
#define  __DECL_CPP__
#include "./pi_proc.cpp"
#include "./pi_stmt.cpp"
#include <memory>

enum DeclKind{
  DC_PROC,
  DC_VAR
};
class Decl{
public:
  DeclKind kind;
  union{
    unique_ptr<class pi_proc>*      proc;
    unique_ptr<class pi_variable>*  var;
  };

  static void print(FILE* stream, unique_ptr<Decl> decl);
};

void Decl::print(FILE* stream, unique_ptr<Decl> decl){
  switch(decl->kind){
  case DC_PROC:
    pi_proc::print(stream, move(*decl->proc));
    break;
  default:
    printf("Error: unexpected Decl::print decl->kind.\n");
    exit(1);
  }
}
#endif /*__DECL_CPP__*/
