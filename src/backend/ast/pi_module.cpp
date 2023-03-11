#ifndef  __MODULE_CPP__
#define  __MODULE_CPP__

#include "../parser/parser.cpp"
#include <vector>
#include "../tokenizer/tokenizer.cpp"
#include "./pi_decl.cpp"
#include "./pi_stmt.cpp"
#include "./pi_expr.cpp"
#include "./pi_types.cpp"

using namespace std;
class Module{
public:
  vector<unique_ptr<class Decl>> AST;
public:
  void dump_ast(const char*);
};

void Module::dump_ast(const char* fp){
  auto fd = fopen(fp, "w");
  assert(fd);
  for(size_t i=0; i < this->AST.size(); ++i){
    Decl::print(fd, move(this->AST[i]));
  }
}
#endif /*__MODULE_CPP__*/
