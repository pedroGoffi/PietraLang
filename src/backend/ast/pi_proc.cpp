#ifndef  __PROC_CPP__
#define  __PROC_CPP__
#include <string>
#include "./pi_stmt.cpp"

using namespace std;
class pi_proc{
  static int proc_count;
  string name_;
  bool   inlined_   = false;
  bool   externed_  = false;
public:
  unique_ptr<pi_type> ret_type;
  unique_ptr<Stmt>*  block_;

public:
  string& name();
  bool&   inlined();
  bool&   externed();
  
  unique_ptr<pi_type> type();
  void set_type(unique_ptr<pi_type> ret_type);
  void set_block(unique_ptr<Stmt> block);
  static void print(FILE* stream, unique_ptr<pi_proc> node);
};
// PROC IMPL
unique_ptr<pi_type> pi_proc::type(){
  return move(this->ret_type);
}
string& pi_proc::name(){
  return this->name_;
}
bool& pi_proc::inlined(){
  return this->inlined_;
}
bool& pi_proc::externed(){
  return this->externed_;
}
void pi_proc::set_block(unique_ptr<Stmt> block){
  if(!this->block_){
    this->block_ = new unique_ptr<Stmt>;
  }
  *this->block_ = move(block);
}
void pi_proc::set_type(unique_ptr<pi_type> ret_type){
  this->ret_type = move(ret_type);
}
void pi_proc::print(FILE* stream, unique_ptr<pi_proc> node){
  fprintf(stream, "(PROC %s %s %s",
	  node->inlined() ? "inlined": "",
	  node->externed()? "extern":  "",	  
	  move(node->name_.c_str()));
  
  // TODO: print args
  fprintf(stream, "()");
  if(node->ret_type){
    fprintf(stream, ": ");
    pi_type::print(stream, move(node->ret_type));
  }
  fprintf(stream, "\n");
  Stmt::print(stream, move(*node->block_));
  fprintf(stream, ")\n");
}

#endif /*__PROC_CPP__*/
