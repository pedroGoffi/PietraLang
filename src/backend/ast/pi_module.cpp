#ifndef  __MODULE_CPP__
#define  __MODULE_CPP__

#include <vector>
#include "../parser/parser.cpp"
#include "../tokenizer/tokenizer.cpp"
#include "./pi_decl.cpp"
#include "./pi_stmt.cpp"
#include "./pi_expr.cpp"
#include "./pi_types.cpp"

using namespace std;
class Module{
  Parser* parser_;
public:
  vector<unique_ptr<Decl>> AST;

  Module(Tokenizer);
  void parse_file();
  void dump_ast(const char*);
};
Module::Module(Tokenizer tk){
  this->parser_ = new Parser(make_unique<Tokenizer>(tk));
}
void Module::parse_file(){
  this->AST = this->parser_->AST();

}
void Module::dump_ast(const char* fp){
  auto fd = fopen(fp, "w");
  assert(fd);
  for(size_t i=0; i < this->AST.size(); ++i){
    Decl::print(fd, move(this->AST[i]));
  }
}
#endif /*__MODULE_CPP__*/
