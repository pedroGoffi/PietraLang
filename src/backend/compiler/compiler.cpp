#ifndef  __COMPILER_CPP__
#define __COMPILER_CPP__
#include <iostream>
#include "./compiler.hpp"

bool load_vars = true;


CT_Types Compiler::expr(unique_ptr<Expr> expr){
  switch(expr->kind){
  case EXPR_INT:
    this->vm.push1(PUSH, Word_i64(move(expr->INT)));
    return DATA;
    
  case EXPR_NAME_LITERAL: {
    if(Allocated_Obj* al = this->vm.Heap_find(expr->token.text)){
      // load the heap obj in stack
      printf("OKASODKAISDIOAS\n");
      Word var_addr = {
	.ptr = (void*)(int64_t*)&this->vm.heap[al->offset]
      };
      
      this->vm.push1(PUSH, var_addr);
      if(load_vars) {
	this->vm.push0(LOAD_PTR);
      }	
      return PTR;
    }
    printf("Err: EXPR_NAME_LITERAL: %s\n",
	   expr->token.text.c_str());
    exit(1);
  } break;
  case EXPR_BINARY: {
    auto lhs = this->expr(move(expr->lhs));
    auto rhs = this->expr(move(expr->rhs));
    if(lhs == VOID or rhs == VOID){
      assert(false && "INTERNAL ERROR WHILE TRING TO ADD TWO VOID DATA TYPE");
    }
    string op = move(expr->token.text);
    if(op == "+")
      this->vm.push0(FAST_ADDI);
    
    else if(op == "-")
      this->vm.push0(FAST_SUBI);
    
    else if(op == "*")
      this->vm.push0(FAST_MULI);
    
    else if(op == "/")
      this->vm.push0(FAST_DIVI);
    
    else
      assert(false && "unreachable");
    
    return lhs;
  }
  case EXPR_LIST: {
    CT_Types last_expr = VOID;
    for(size_t i=0; i < move(expr->expr_list->size()); ++i){
      last_expr = this->expr(move((*expr->expr_list)[i]));
    }
    return last_expr;
  }
  case EXPR_CALL: {
    std::cout << "call: " << move(expr->token.text) << "\n";
    if(expr->token.text == "dump"){
      this->expr(move(expr->base));
      this->vm.push0(FAST_DUMPI);
      return VOID;
    }
    else
      assert(false && "unreachable");
  };
  case EXPR_UNARY: {
    std::string op = expr->token.text;
    if(op == "&"){
      load_vars = false;
      auto ret = this->expr(move(expr->rhs));
      load_vars = true;
      
      return ret;
    }
    if(op == "*"){
      /*
        // ptr struct:
        addr -> ptr -> addr -> value
	// *addr	= ptr
	// **addr	= *ptr
	// *ptr		= value
      */
      auto ret = this->expr(move(expr->rhs));
      this->vm.push0(LOAD_PTR);
      return ret;
    }
    else {
      printf("Internal_error: no suport for: '%s' unary op.\n",
	     op.c_str());
      exit(1);
    }

    exit(1);
  }
  case EXPR_ASSIGN: {
    assert(expr->base->kind == EXPR_NAME_LITERAL);
    load_vars = false;
    this->expr(move(expr->base));
    load_vars = true;
    // addr
    this->expr(move(expr->rhs));
    // addr value
    this->vm.push0(WRITE64);
    return VOID;
  };
  case EXPR_CMP: {
    std::string op = expr->token.text;
    this->expr(move(expr->lhs));
    this->expr(move(expr->rhs));
    /*
      <  = 0
      <= = 1
      == = 2
      != = 3
      >= = 4
      >  = 5
     */
    if(op == "<"){
      this->vm.push1(CMPI_FG, Word_i64(0));
    }
    else if(op == "<="){
      this->vm.push1(CMPI_FG, Word_i64(1));
    }
    else if(op == "=="){
      this->vm.push1(CMPI_FG, Word_i64(2));
    }
    else if(op == "!="){
      this->vm.push1(CMPI_FG, Word_i64(3));
    }
    else if(op == ">="){
      this->vm.push1(CMPI_FG, Word_i64(4));
    }
    else if(op == ">"){
      this->vm.push1(CMPI_FG, Word_i64(5));
    }
    else {
      printf("Internal_error: unreachable cmp id: '%s'\n",
	     op.c_str());
      exit(1);
    }
    
    return DATA;
  };
  default:
    Expr::print(stderr, move(expr));
    assert(false && "Something went wrong!!");
  }
}
CT_Types Compiler::stmt(unique_ptr<Stmt> stmt){
  switch(stmt->kind){
  case ST_EXPR: {
    auto e = this->expr(move(*stmt->expr));
    if(e != VOID)
      this->vm.push0(POP);
    
    return e;
  }
  case ST_BLOCK: {
    CT_Types last_expr = VOID;
    for(size_t i=0; i < move(stmt->block->size()); ++i){
      last_expr = this->stmt(move((*stmt->block)[i]));
    }
    return last_expr;
  };
  case ST_WHILE: {
    auto whl = move(*stmt->whileLoop);

    
    int bgin_pos = (int)this->vm.program.size();
    this->expr(move(whl->expr));
    this->vm.push0(NOT);
    int jmp_pos = (int)this->vm.program.size();
    this->vm.push0(JMP_IF);
    this->stmt(move(whl->block));
    this->vm.push1(JMP, Word_i64(bgin_pos));
    
    this->vm.program[jmp_pos].op_a.i64 = (int)this->vm.program.size();
    return VOID;
  }    
  default:
    assert(false && "Something went wrong!!");
  }
}
CT_Types Compiler::decl(unique_ptr<Decl> decl){
  switch(decl->kind){
  case DeclKind::DC_PROC: {
    assert(decl->proc);
    unique_ptr<pi_proc> proc = move(*decl->proc);
    
    if(proc->name() == "main"){
      this->vm.entry = (Inst_Addr)this->vm.program.size();
    }
    
    if(proc->inlined()){
      printf("ERROR: can not compile inline procedures yet.\n");
      exit(1);
    }

    this->stmt(move(*proc->block_));
  } break;
  case DC_VAR: {
    auto  var = move(*decl->var);
    // check: var already declared?
    std::string name = move(var->name());

    if(0){      
      printf("internal_error: %s already declared.\n",
	     name.c_str());
      exit(1);
    }
    Err err = this->vm.Heap_alloc(name,
				  sizeof(Word),
				  Word_i64(0));
    assert(err == OK);
    // TODO: check if assignment
    if(var->expr()){
      printf("internal_error: global vars does not support expression assignment.\n");
    }
  } break;
  default:
    assert(false && "Something went wrong!!");
  }
  return VOID;
}

void Compiler::codegen(Module* mod){
  this->vm.push0(JMP); // program[0]
  for(size_t i=0; i < mod->AST.size(); i++){   
    this->decl(move(mod->AST[i]));
  }
  
  this->vm.program[0].op_a = Word_i64(this->vm.entry);
  this->vm.push0(HALT);
}
#endif /*__COMPILER_CPP__*/
