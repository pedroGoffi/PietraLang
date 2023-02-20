#ifndef  __EXPR_CPP__
#define  __EXPR_CPP__

#include <cassert>
#include "../tokenizer/token.cpp"

enum ExprKind {
  EXPR_INT,
  EXPR_BINARY
};
class Expr{
public:
  ExprKind kind;
  Token    token;
  Expr*    lhs;
  Expr*    rhs;
  union{
    int INT;
    
  };
public:
  Expr(int val){
    this->kind = ExprKind::EXPR_INT;
    this->INT  = val;
  }
  Expr(Expr* lhs, Expr* rhs){
    assert(lhs);
    assert(rhs);
    this->lhs = lhs;
    this->rhs = rhs;
    this->kind = EXPR_BINARY;
  }
  static void print(FILE* stream, Expr* node){
    static int ident = 0;
    for(int i=0 ; i < ident; ++i)
      fprintf(stream, " ");

    switch(node->kind){
    case EXPR_INT:
      fprintf(stream, "%d", node->INT);
      break;
    case EXPR_BINARY:
      fprintf(stream, "(%s ", node->token.text);
      node->print(stream, node->lhs);
      fprintf(stream, " ");
      node->print(stream, node->rhs);
      fprintf(stream, ")");
      break;
      
    default:
      printf("Error: undefined this->kind.\n");
      exit(1);
    }
  }
};

#endif /*__EXPR_CPP__*/
