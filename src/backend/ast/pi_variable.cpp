#ifndef  __PI_VARIABLES_CPP__
#define  __PI_VARIABLES_CPP__
#include <string>
#include <memory>
#include "./pi_expr.cpp"
#include "./pi_entities.cpp"
#include "./pi_types.cpp"

using namespace std;

class pi_variable{
private:  
  string              name_;
  unique_ptr<Expr>    expr_;
  bool                const_;
  unique_ptr<pi_type> type_;
public:
  pi_variable(string name, unique_ptr<Expr> expr, bool cte = false)
  {
    if(!expr){
      printf("Error: can not initialza an instance of pi_variable with NULL expresions yet.\n");
      exit(1);
    }
    this->name_  = name;
    this->expr_  = make_unique<Expr>(std::move(*expr));   
    this->const_ = cte;
    this->type_  = NULL;
  }
  void set_const(bool state = true){
    this->const_ = state;
  }
  void set_type(unique_ptr<pi_type> ty){
    this->type_  = make_unique<pi_type>(std::move(*ty));
  }
  bool is_const() const {
    return this->const_;
  }
  unique_ptr<Expr> expr() {
    return move(this->expr_);
  }
  void asign(Expr* expr){
    if(this->const_){
      printf("Error: can not (re)asign a const variable '%s'.\n",
  	     this->name_.c_str());
      exit(1);
    }
    if(!this->type_){
      printf("Error: can not (re)asign the variable %s without specify a type.\n",
	     this->name_.c_str());
      exit(1);
    }
    this->expr_.reset();
    assert(expr);
    this->expr_ = make_unique<Expr>(std::move(*expr));
  }
  string& name() {
    return this->name_;
  }
  
};
#endif /*__PI_VARIABLES_CPP__*/
