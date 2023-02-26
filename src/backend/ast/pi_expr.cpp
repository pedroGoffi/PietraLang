#ifndef  __EXPR_CPP__
#define  __EXPR_CPP__

#include <cassert>
#include <iostream>
#include <memory>
#include <vector>
#include "../tokenizer/token.cpp"
#include "./pi_types.cpp"
#include "../allocator.cpp"

typedef class Cast Cast;
typedef class Expr Expr;

class Cast{
public:
  std::unique_ptr<Expr>    expr;
  std::unique_ptr<pi_type> type;
public:
  Cast(std::unique_ptr<Expr>    expr_,
       std::unique_ptr<pi_type> type_)
    : expr(move(expr_)),
      type(move(type_))
  {}
};


enum ExprKind {
  EXPR_INT,		// INT
  EXPR_NAME_LITERAL,	// NAME
  EXPR_BINARY,		// expr OP expr
  EXPR_CMP,		// expr OP expr
  EXPR_AND,		// expr OP expr
  EXPR_UNARY,		// OP expr
  EXPR_TERNARY,		// expr if expr else expr
  EXPR_CAST,		// expr as type
  EXPR_LIST,		// '{' expr1 ',' expr2 ',' expr3 '}'
  EXPR_CALL,		// [NAME]'(' ARGS* ')'
  EXPR_VAR              // NAME ':' type ('=' expr)*
};

class Expr{
public:
  ExprKind kind;
  Token    token;
  std::unique_ptr<Expr>    base;
  std::unique_ptr<Expr>    lhs;
  std::unique_ptr<Expr>    rhs;
  std::unique_ptr<pi_type> type;
  union{
    int                       INT;
    Cast*                     cast;
    vector<unique_ptr<Expr>>* expr_list;
  };
public:  
  static void print(FILE* stream, std::unique_ptr<Expr> node){
    static int ident = 0;
    for(int i=0 ; i < ident; ++i)
      fprintf(stream, "%*.s", i, " ");

    switch(node->kind){
    case EXPR_INT:
      fprintf(stream, "%d", node->INT);
      break;
    case EXPR_NAME_LITERAL:
      fprintf(stream, "%s", node->token.text.c_str());
      break;
    case EXPR_BINARY:
      fprintf(stream, "(%s ", node->token.text.c_str());
      Expr::print(stream, move(node->lhs));
      fprintf(stream, " ");
      Expr::print(stream, move(node->rhs));
      fprintf(stream, ")");
      break;
    case EXPR_CMP:
    case EXPR_AND:
      fprintf(stream, "(%s ", node->token.text.c_str());
      Expr::print(stream, move(node->lhs));
      fprintf(stream, " ");
      Expr::print(stream, move(node->rhs));
      fprintf(stream, ")");
      break;
    case EXPR_UNARY: {
      fprintf(stream, "(%s ", node->token.text.c_str());
      Expr::print(stream, move(node->rhs));
      fprintf(stream, " )");
    } break;
    case EXPR_CAST: {
      fprintf(stream, "(CAST: ");
      Expr::print(stream, move(node->cast->expr));
      fprintf(stream, " -> %s", move(node->cast->type->name().c_str()));
      fprintf(stream, " )");
      
    } break;
    case EXPR_TERNARY: {      
   
      fprintf(stream, "(");
      Expr::print(stream, move(node->base));
      fprintf(stream, " if ");
      Expr::print(stream, move(node->lhs));
      fprintf(stream, " else ");
      Expr::print(stream, move(node->rhs));
      fprintf(stream, ")");
    } break;
      
    case EXPR_LIST: {
      assert(node->expr_list);
      int len = (int)node->expr_list->size();

      fprintf(stream, "{ ");
      for(int i=0; i < len; ++i){
	unique_ptr<Expr> e = move((*node->expr_list)[i]);
	Expr::print(stream, move(e));
    	fprintf(stream, i + 1 < len? ", ": "");
      }
      fprintf(stream, " }");

    }  break;
    case EXPR_CALL: {
      fprintf(stream, "(CALL %s(", move(node->token.text.c_str()));      
      if(node->base){
        Expr::print(stream, move(node->base));
      }
      fprintf(stream, "))");
      
    } break;
    case EXPR_VAR: {
      assert(node->base->kind == EXPR_NAME_LITERAL);
      fprintf(stream, "(VAR %s", move(node->base->token.text.c_str()));

      if(node->type){
	fprintf(stream, " : ");
	pi_type::print(stream, move(node->type));

      }
      if(node->rhs) {
	fprintf(stream, " = ");
	Expr::print(stream, move(node->rhs));
      }
      fprintf(stream, " )");           
    } break;
    default:
      printf("Error: undefined node->kind: %s.\n",
	     node->token.text.c_str());
      exit(1);
    }
    if(node->rhs){
      fprintf(stream, " -> ");
      Expr::print(stream, move(node->rhs));
    }
  }

};

unique_ptr<Expr> Expr_int(int val){
  unique_ptr<Expr> ret	= make_unique<Expr>(Expr());
  ret->kind		= EXPR_INT;
  ret->INT		= val;
  return ret;
}
unique_ptr<Expr> Expr_unary(Token            token,
			    unique_ptr<Expr> rhs){
  unique_ptr<Expr> ret	= make_unique<Expr>(Expr());
  ret->kind		= EXPR_UNARY;
  ret->token		= token;
  ret->rhs		= move(rhs);
  return ret;
}
unique_ptr<Expr> Expr_binary_op(
				Token            token,
				unique_ptr<Expr> lhs,
				unique_ptr<Expr> rhs){
  unique_ptr<Expr> ret	= make_unique<Expr>(Expr());
  ret->kind		= EXPR_BINARY;;
  ret->token		= token;
  ret->lhs		= move(lhs);
  ret->rhs		= move(rhs);
  return ret;  
}
unique_ptr<Expr> Expr_ternary(unique_ptr<Expr> base,
			      unique_ptr<Expr> lhs,
			      unique_ptr<Expr> rhs){
  
  unique_ptr<Expr> ret	= make_unique<Expr>(Expr());
  ret->kind		= EXPR_TERNARY;
  ret->base             = move(base);
  ret->lhs		= move(lhs);
  ret->rhs		= move(rhs);
  return ret;  
}
unique_ptr<Expr> Expr_cast(Cast* cast){
  unique_ptr<Expr> ret	= make_unique<Expr>(Expr());
  ret->kind		= EXPR_CAST;
  ret->cast             = cast;
  return ret;    
}
unique_ptr<Expr> Expr_var(
			  unique_ptr<pi_type> type,
			  unique_ptr<Expr>    base,
			  unique_ptr<Expr>    rhs){
  unique_ptr<Expr> ret	= make_unique<Expr>(Expr());
  ret->kind		= EXPR_VAR;
  ret->type             = move(type);
  ret->base		= move(base);
  ret->rhs		= move(rhs);
  return ret;    
}
unique_ptr<Expr> Expr_list(vector<unique_ptr<Expr>> expr_list){
                         
  unique_ptr<Expr> ret	= make_unique<Expr>(Expr());
  ret->kind		= EXPR_LIST;
  ret->expr_list	= new vector<unique_ptr<Expr>>;
  *ret->expr_list	= vector_copy<unique_ptr<Expr>>(move(expr_list));
  return ret;    
}
#endif /*__EXPR_CPP__*/
