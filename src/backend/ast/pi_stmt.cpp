#ifndef  __STMT_CPP__
#define  __STMT_CPP__
#include <iostream>
#include "../allocator.cpp"
#include "./pi_expr.cpp"

typedef class Stmt  Stmt;


enum StmtKind{
  ST_EXPR
};
class Stmt{
public:
  StmtKind kind;

  union{
    Expr* expr;
  };
  Stmt(Expr* expr) : expr(expr){}    
};
#endif /*__STMT_CPP__*/
