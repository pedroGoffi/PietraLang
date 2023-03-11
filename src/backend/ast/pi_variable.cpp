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
  bool                mut_;
  unique_ptr<pi_type> type_;
public:
  pi_variable(string name, unique_ptr<pi_type> type, unique_ptr<Expr> expr, bool mut = false)
  {

    if(!expr){
      printf("Error: can not initialza an instance of pi_variable with NULL expresions yet.\n");
      exit(1);
    }
    this->name_  = name;
    this->expr_  = make_unique<Expr>(std::move(*expr));   
    this->mut_   = mut;
    this->type_  = move(type);
    this->expr_  =  make_unique<Expr>(std::move(*expr));
  }
  void set_mut(bool state = true){
    this->mut_ = state;
  }
  void set_type(unique_ptr<pi_type> ty){
    this->type_  = make_unique<pi_type>(std::move(*ty));
  }
  bool is_mut() const {
    return this->mut_;
  }
  unique_ptr<Expr> expr() {
    return move(this->expr_);
  }
  void asign(Expr* expr){
    if(!this->mut_){
      printf("Error: can not reasign a non mutable variable: '%s'.\n",
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

