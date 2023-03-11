#ifndef  __PI_AST_CPP__
#define  __PI_AST_CPP__
#include <memory>
#include "./pi_decl.cpp"
#include "./pi_variable.cpp"

using namespace std;
unique_ptr<class Expr> Expr_int(int val){
  unique_ptr<class Expr> ret	= make_unique<class Expr>(Expr());
  ret->kind		= EXPR_INT;
  ret->INT		= val;
  return ret;
}
unique_ptr<class Expr> Expr_unary(Token            token,
			    unique_ptr<class Expr> rhs){
  unique_ptr<class Expr> ret	= make_unique<class Expr>(Expr());
  ret->kind		= EXPR_UNARY;
  ret->token		= token;
  ret->rhs		= move(rhs);
  return ret;
}
unique_ptr<class Expr> Expr_binary_op(
				Token            token,
				unique_ptr<class Expr> lhs,
				unique_ptr<class Expr> rhs){
  unique_ptr<class Expr> ret	= make_unique<class Expr>(Expr());
  ret->kind		= EXPR_BINARY;;
  ret->token		= token;
  ret->lhs		= move(lhs);
  ret->rhs		= move(rhs);
  return ret;  
}
unique_ptr<class Expr> Expr_ternary(unique_ptr<class Expr> base,
			      unique_ptr<class Expr> lhs,
			      unique_ptr<class Expr> rhs){
  
  unique_ptr<class Expr> ret	= make_unique<class Expr>(Expr());
  ret->kind		= EXPR_TERNARY;
  ret->base             = move(base);
  ret->lhs		= move(lhs);
  ret->rhs		= move(rhs);
  return ret;  
}
unique_ptr<class Expr> Expr_cast(Cast* cast){
  unique_ptr<class Expr> ret	= make_unique<class Expr>(Expr());
  ret->kind		= EXPR_CAST;
  ret->cast             = cast;
  return ret;    
}
unique_ptr<class Expr> Expr_var(
			  unique_ptr<pi_type> type,
			  unique_ptr<class Expr>    base,
			  unique_ptr<class Expr>    rhs){
  unique_ptr<class Expr> ret	= make_unique<class Expr>(Expr());
  ret->kind		= EXPR_VAR;
  ret->type             = move(type);
  ret->base		= move(base);
  ret->rhs		= move(rhs);
  return ret;    
}
  unique_ptr<class Expr> Expr_list(vector<unique_ptr<class Expr>> expr_list){
                         
    unique_ptr<class Expr> ret	= make_unique<class Expr>(Expr());
    ret->kind		= EXPR_LIST;
    ret->expr_list	= new vector<unique_ptr<class Expr>>;
    *ret->expr_list	= vector_copy<unique_ptr<class Expr>>(move(expr_list));
    return ret;    
  }
    unique_ptr<class Expr> Expr_var(Token               name_def_tk,
			      bool                mut,
			      unique_ptr<pi_type> type,
			      unique_ptr<class Expr>    expr){
  
      unique_ptr<class Expr> ret	= make_unique<class Expr>(Expr());
      ret->kind		= EXPR_VAR;

      ret->token            = name_def_tk;
      ret->type		= move(type);
      ret->rhs		= move(expr);
      ret->mut              = mut;
      return ret;      
    }
unique_ptr<class Expr> Expr_assign(unique_ptr<Expr> lhs,
				   unique_ptr<Expr> rhs){
  auto ret = make_unique<Expr>(Expr());
  ret->kind = EXPR_ASSIGN;

  ret->base = move(lhs);
  ret->rhs = move(rhs);
  return ret;
}
unique_ptr<Stmt> Stmt_expr(unique_ptr<class Expr> e){
  auto st	= make_unique<Stmt>(Stmt());
  st->kind	= ST_EXPR;
  st->expr	= new unique_ptr<class Expr>; 
  *st->expr	= move(e);
  return st;
}
unique_ptr<Stmt> Stmt_block(vector<unique_ptr<Stmt>> block){
  auto st	= make_unique<Stmt>(Stmt());
  st->kind	= ST_BLOCK;  
  st->block	= new vector<unique_ptr<Stmt>>;  
  *st->block	= vector_copy<unique_ptr<Stmt>>(move(block));

  return st;
}
unique_ptr<Stmt> Stmt_while(unique_ptr<class Expr> expr,
			    unique_ptr<Stmt> block){
  
  auto st		= make_unique<Stmt>(Stmt());
  st->kind		= ST_WHILE;
  st->whileLoop		= new unique_ptr<While>;
  *st->whileLoop	= make_unique<While>(While(move(expr), move(block)));
  
  return st;
  
}
unique_ptr<Stmt> Stmt_if(unique_ptr<class Expr> expr,
			 unique_ptr<Stmt> block){
  
  auto st	= make_unique<Stmt>(Stmt());
  st->kind	= ST_IF;
  st->ifBlock	= new unique_ptr<If>;
  *st->ifBlock	= make_unique<If>(If(move(expr), move(block)));
   
  return st;
  
}
std::unique_ptr<class Decl> Decl_proc(std::unique_ptr<pi_proc> proc){
  auto dc	= make_unique<class Decl>(Decl());
  dc->kind	= DC_PROC;
  dc->proc	= new unique_ptr<pi_proc>;
  *dc->proc	= move(proc);
  return dc;
}

std::unique_ptr<class Decl> Decl_var(std::unique_ptr<pi_variable> var){

  auto dc	= make_unique<class Decl>(Decl());
  dc->kind	= DC_VAR;
  dc->var	= new unique_ptr<pi_variable>;
  *dc->var	= move(var);
  return dc;
}
#endif /*__PI_AST_CPP__*/
