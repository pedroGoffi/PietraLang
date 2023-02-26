#ifndef  __CODEGEN_CPP
#define  __CODEGEN_CPP
#include <iostream>
#include <memory>
#include "../ast/pi_stmt.cpp"
#include "../ast/pi_module.cpp"

// NOTE: for now we emit to C code

namespace Emit{
  void expr(FILE*, unique_ptr<class Expr> expr);

  void stmt(FILE* fd, unique_ptr<class Stmt> st);
  
  namespace Decl{
    void Proc(FILE*, unique_ptr<class pi_proc>);
  };
};
// EMIT EXPR IMPL
void Emit::expr(FILE* fd, unique_ptr<class Expr> expr){
  switch(expr->kind){
  case EXPR_INT:
    fprintf(fd, "%d", move(expr->INT));
    break;
  case EXPR_UNARY: {
    fprintf(fd, "(");
    fprintf(fd, "%s", move(expr->token.text.c_str()));
    Emit::expr(fd, move(expr->rhs));
    fprintf(fd, ")");
  } break;
  case EXPR_BINARY: {
    fprintf(fd, "(");
    Emit::expr(fd, move(expr->lhs));
    fprintf(fd, " %s ",
	    move(expr->token.text.c_str()));
    Emit::expr(fd, move(expr->rhs));
    fprintf(fd, ")");    
  } break;
  case EXPR_NAME_LITERAL:
    fprintf(fd, "%s", move(expr->token.text.c_str()));
    break;
  case EXPR_VAR:
    if(expr->type){
      fprintf(fd, "%s ",
	      move(expr->type->name().c_str()));
    }
    else {
      fprintf(fd, "auto ");
    }
    Emit::expr(fd, move(expr->base));
    if(expr->rhs){
      fprintf(fd, " = ");
      Emit::expr(fd, move(expr->rhs));
    }
    break;
  case EXPR_TERNARY:
    fprintf(fd, "(");
    Emit::expr(fd, move(expr->lhs));
    fprintf(fd, ")? ");
    Emit::expr(fd, move(expr->base));
    fprintf(fd, ": ");
    Emit::expr(fd, move(expr->rhs));
    
    break;
  default:
    fprintf(stderr,
	    "Error: EMIT_EXPR.\n");
    exit(1);
  }
}
int count(bool reset = false){
  static int C = 1;
  if(reset)  C = 1;
  return C++;
}
// EMIT STMT IMPL
void Emit::stmt(FILE* fd, unique_ptr<class Stmt> st){
  static int ilvl = 1;


  auto ident = [](FILE* fd, int lvl){
    for(int i=0; i < lvl; ++i){
      fprintf(fd, " ");
    }
  };



  switch(st->kind){
  case ST_EXPR:
    fprintf(fd, ".L%i:\n", count());
    ident(fd, ilvl);
    Emit::expr(fd, move(*st->expr));
    fprintf(fd, ";\n");
    break;
    
  case ST_BLOCK:
    ident(fd, ilvl);
    ilvl++;
    fprintf(fd, "{\n");
    
    for(size_t i=0; i < move(st->block->size()); ++i){
      Emit::stmt(fd, move((*st->block)[i]));
    }
    ilvl--;
    ident(fd, ilvl);
    fprintf(fd, "}\n");    
    break;
  case ST_IF: {
    unique_ptr<If> IF = move(*st->ifBlock);   
    int end = count();
    /*
      if a {...}
      if !a jump end
      {...}
    */
    ident(fd, ilvl);
    fprintf(fd, "if (!(");
    Emit::expr(fd, move(IF->expr));
    fprintf(fd, ")) goto L%i;\n", end);
    Emit::stmt(fd, move(IF->block));
    fprintf(fd, ".L%i:\n", end);
  } break;
  case ST_WHILE: {
    unique_ptr<class While> wl = move(*st->whileLoop);
    int bgin = count();
    fprintf(fd, ".L%i:\n", bgin);

    Emit::stmt(fd, move(wl->block));
    
    ident(fd, ilvl);        
    fprintf(fd, "if(!(");
    Emit::expr(fd, move(wl->expr));
    fprintf(fd, ")) goto L%i;\n", bgin);    
    
  } break;
  default:
    fprintf(stderr,
	    "Error: EMIT_STMT.\n");
    exit(1);
  }
}
// EMIT DECL IMPL
void Emit::Decl::Proc(FILE* fd, unique_ptr<pi_proc> proc){
  fprintf(fd, "%s %s(){\n",	  
	  move(proc->ret_type->name().c_str()),	 
	  proc->name().c_str());
  Emit::stmt(fd, move(*proc->block_));
  fprintf(fd, "}\n");
}
#endif /*__CODEGEN_CPP*/
