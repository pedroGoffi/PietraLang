#ifndef  __DECL_CPP__
#define  __DECL_CPP__
#include "./pi_proc.cpp"
#include "./pi_stmt.cpp"
#include <memory>
enum DeclKind{
  DC_PROC
};
class Decl{
public:
  DeclKind kind;
  union{
    unique_ptr<pi_proc>* proc;
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
unique_ptr<Decl> Decl_proc(unique_ptr<pi_proc> proc){
  auto dc	= make_unique<Decl>(Decl());
  dc->kind	= DC_PROC;
  dc->proc	= new unique_ptr<pi_proc>;
  *dc->proc	= move(proc);
  return dc;
}
#endif /*__DECL_CPP__*/
