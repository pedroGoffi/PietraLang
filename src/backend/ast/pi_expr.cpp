#ifndef  __EXPR_CPP__
#define  __EXPR_CPP__

#include <cassert>
#include <iostream>
#include <memory>
#include "../tokenizer/token.cpp"
#include "../allocator.cpp"
#include "./pi_types.cpp"

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
  EXPR_INT,
  EXPR_NAME_LITERAL,  
  EXPR_BINARY,
  EXPR_CMP,
  EXPR_AND,
  EXPR_UNARY,
  EXPR_TERNARY,
  EXPR_CAST,
  EXPR_LIST // '{' expr1 ',' expr2 ',' expr3 '}'
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
    int   INT;
    Cast* cast;
    //unique_ptr<Allocator<std::unique_ptr<Expr>>> expr_list;
  };
public:  

  //Expr(Allocator<std::unique_ptr<Expr>>* el) : expr_list(el){}
  //Expr(int val){
  //  this->kind = ExprKind::EXPR_INT;
  //  this->INT  = val;
  //}
  //Expr(std::unique_ptr<Expr> lhs, std::unique_ptr<Expr> rhs){
  //  assert(lhs);
  //  assert(rhs);
  //  this->lhs = lhs;
  //  this->rhs = rhs;
  //  this->kind = EXPR_BINARY;
  //}
  static void print(FILE* stream, std::unique_ptr<Expr> node){
    static int ident = 0;
    for(int i=0 ; i < ident; ++i)
      fprintf(stream, "%*.s", i, " ");

    switch(node->kind){
    case EXPR_INT:
      fprintf(stream, "%d", node->INT);
      break;
    case EXPR_NAME_LITERAL:
      fprintf(stream, "%s", node->token.text);
      break;
    case EXPR_BINARY:
      fprintf(stream, "(%s ", node->token.text);
      Expr::print(stream, move(node->lhs));
      fprintf(stream, " ");
      Expr::print(stream, move(node->rhs));
      fprintf(stream, ")");
      break;
    case EXPR_CMP:
    case EXPR_AND:
      fprintf(stream, "(%s ", node->token.text);
      Expr::print(stream, move(node->lhs));
      fprintf(stream, " ");
      Expr::print(stream, move(node->rhs));
      fprintf(stream, ")");
      break;
    case EXPR_UNARY: {
      fprintf(stream, "(%s ", node->token.text);
      Expr::print(stream, move(node->rhs));
      fprintf(stream, " )");
    } break;
    case EXPR_CAST: {
      fprintf(stream, "(CAST: ");
      Expr::print(stream, move(node->cast->expr));
      fprintf(stream, " -> %s", move(node->cast->type->name().c_str()));
      fprintf(stream, " )");
      
    } break;
    //case EXPR_TERNARY: {      
    //  Ternary ternary = move(node->ternary);
    //  fprintf(stream, "(");
    //  Expr::print(stream, move(ternary.or_expr));
    //  fprintf(stream, " if ");
    //  Expr::print(stream, move(ternary.if_expr));
    //  fprintf(stream, " else ");
    //  Expr::print(stream, move(ternary.else_expr));
    //  fprintf(stream, ")");
    //} break;
      
    //case EXPR_LIST: {
    //  assert(node->expr_list);
    //  int len = (int)node->expr_list->vec.size();
    //  fprintf(stream, "{");
    //  for(int i=0; i < len; ++i){
    //	std::unique_ptr<Expr> e = move(node->expr_list->vec[i]);
    //	Expr::print(stream, move(e));
    //	fprintf(stream, i + 1 < len? ", ": "");
    //  }
    //  fprintf(stream, "}");
    // 
    //}  break;
    default:
      printf("Error: undefined this->kind.\n");
      exit(1);
    }
  }
};

unique_ptr<Expr> Expr_int(int val){
  unique_ptr<Expr> ret = make_unique<Expr>(Expr());
  ret->kind = EXPR_INT;
  ret->INT  = val;
  return ret;
}
unique_ptr<Expr> Expr_unary(Token            token,
			    unique_ptr<Expr> rhs){
  unique_ptr<Expr> ret = make_unique<Expr>(Expr());
  ret->kind  = EXPR_UNARY;
  ret->token = token;
  ret->rhs   = move(rhs);
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
#endif /*__EXPR_CPP__*/
