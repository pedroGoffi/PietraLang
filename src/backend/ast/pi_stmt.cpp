#ifndef  __STMT_CPP__
#define  __STMT_CPP__
#include <iostream>
#include "../allocator.cpp"
#include "./pi_expr.cpp"


typedef class While While;
typedef class If If;

typedef class Stmt  Stmt;


enum StmtKind{
  ST_EXPR,
  ST_WHILE,
  ST_IF,
  ST_BLOCK
};
class While{
public:
  unique_ptr<Expr> expr;
  unique_ptr<Stmt> block;
  While(unique_ptr<Expr> e, unique_ptr<Stmt> st)
    : expr(move(e)), block(move(st))
  {}
};
class If{
public:
  unique_ptr<Expr> expr;
  unique_ptr<Stmt> block;
  If(unique_ptr<Expr> e, unique_ptr<Stmt> st)
    : expr(move(e)), block(move(st))
  {}
};

class Stmt{
public:
  StmtKind kind;

  union{
    unique_ptr<Expr>*         expr;
    unique_ptr<While>*        whileLoop;
    unique_ptr<If>*           ifBlock;
    vector<unique_ptr<Stmt>>* block;
  };
  static void print(FILE* stream, std::unique_ptr<Stmt> node);

};
// STMT IMPL
void Stmt::print(FILE* stream, std::unique_ptr<Stmt> node){
  static int identlvl = 0;
  auto ident = [](FILE* stream, int lvl) {
    for(int i=0; i < lvl; i++){
      fprintf(stream, " ");
    }
  };

  // TODO: identation
  switch(node->kind){
  case ST_WHILE:
    fprintf(stream, "(WHILE (");
    
    Expr::print(stream, move((*node->whileLoop)->expr));
    fprintf(stream, ")\n");
    Stmt::print(stream, move((*node->whileLoop)->block));
    fprintf(stream, ")\n");
    break;
  case ST_IF:
    fprintf(stream, "(IF (");
    Expr::print(stream, move((*node->ifBlock)->expr));
    fprintf(stream, ")\n");
    Stmt::print(stream, move((*node->ifBlock)->block));
    fprintf(stream, ")\n");
    break;
  case ST_BLOCK:
    fprintf(stream, "{\n");
       
    for(size_t i=0; i < move(node->block->size()); ++i){    
      Stmt::print(stream, move((*node->block)[i]));
      fprintf(stream, "\n");
    }
    fprintf(stream, "}\n");    
    break;
  case ST_EXPR:
    Expr::print(stream, move(*node->expr));
    break;
  default:
    printf("Error: undefined node->kind.\n");
    exit(1);
  }
}
//unique_ptr<Stmt>
unique_ptr<Stmt> Stmt_expr(unique_ptr<Expr> e){
  auto st	= make_unique<Stmt>(Stmt());
  st->kind	= ST_EXPR;
  st->expr	= new unique_ptr<Expr>; 
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
unique_ptr<Stmt> Stmt_while(unique_ptr<Expr> expr,
			    unique_ptr<Stmt> block){
  
  auto st		= make_unique<Stmt>(Stmt());
  st->kind		= ST_WHILE;
  st->whileLoop		= new unique_ptr<While>;
  *st->whileLoop	= make_unique<While>(While(move(expr), move(block)));
  
  return st;
  
}
unique_ptr<Stmt> Stmt_if(unique_ptr<Expr> expr,
			    unique_ptr<Stmt> block){
  
  auto st	= make_unique<Stmt>(Stmt());
  st->kind	= ST_IF;
  st->ifBlock	= new unique_ptr<If>;
  *st->ifBlock	= make_unique<If>(If(move(expr), move(block)));
   
  return st;
  
}
#endif /*__STMT_CPP__*/
