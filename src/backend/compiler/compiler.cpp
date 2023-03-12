#ifndef  __COMPILER_CPP__
#define __COMPILER_CPP__
#include <iostream>
#include "./compiler.hpp"

bool load_vars = true;

unique_ptr<pi_type> Compiler::expr(unique_ptr<Expr> expr){
  switch(expr->kind){
  case EXPR_INT:
    this->vm.push1(PUSH, Word_i64(move(expr->INT)));
    return Type_i64();
  case EXPR_STRING: {
    static size_t string_count	= 0;
    std::string id		= std::to_string(string_count++);
    const char* str = expr->token.text.c_str();
    int str_len = (int)strlen(expr->token.text.c_str());

    Err err = this->vm.Heap_alloc(id,
				  sizeof(Word),
				  Word_i64(0));
    assert(err == OK);
    Allocated_Obj* str_alloc = this->vm.Heap_find(id);

    this->vm.heap[str_alloc->offset] = { .ptr = (void*) str };
    
    Compiled_string compiled_str = {
      .id         = id,
      .allocation = *str_alloc
    };
    Compiled_string::push(compiled_str);

    Word str_addr ={
      .ptr = (void*)(int64_t*)&this->vm.heap[compiled_str.allocation.offset]
    };
    
   
    this->vm.push1(PUSH, str_addr);
    this->vm.push1(PUSH, Word_i64(str_len));

    
    
    printf("WARNING! TODO STRING RETURN TYPE.\n");
    return Type_i64();
  }
  case EXPR_NAME_LITERAL: {

    if(Compiled_var* var = Compiled_var::find(compiled_vars, expr->token.text)){
      // get variable addr
      Word var_addr = { .ptr = (void*)(int64_t*)&this->vm.heap[var->allocation.offset] };

      // load in the stack as a c pointer
      this->vm.push1(PUSH, var_addr);
      if(load_vars) {
	// get the value of the pointer if not assign base-context
      	this->vm.push0(LOAD_PTR);
      }
      
      return move(var->type);
    }
    printf("Error: the name '%s' was not declared in this scope\n",
	   expr->token.text.c_str());
    exit(1);    
  } break;
    
  case EXPR_BINARY: {
    // FIXME LHS is comming NULL
    auto lhs = this->expr(move(expr->lhs));
    auto rhs = this->expr(move(expr->rhs));
    // TODO: binary type check    
    
    if(0){
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
    
    return rhs;
  }
  case EXPR_LIST: {
    unique_ptr<pi_type> last_expr;
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
      return NULL;
    }
    else if(expr->token.text == "write"){
      // @stack:
      // str_addr
      // TODO: get rid of str_len stack push when compiling strings
      this->vm.push1(PUSH, Word_i64(1));
      this->expr(move(expr->base));
      this->vm.push0(FAST_IO_WRITE);
      return NULL;
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
      printf("Err: TODO: Type_ptr(pi_type)\n");
      return ret;
    }
    else if(op == "!"){
      auto ret = this->expr(move(expr->rhs));
      this->vm.push0(NOT);
      return ret;
    }
    else if(op == "*"){
      /*
        // ptr struct:
        addr -> ptr -> addr -> value
	// *addr	= ptr
	// **addr	= *ptr
	// *ptr		= value
      */
      auto ret = this->expr(move(expr->rhs));
      printf("Err: TODO: pi_type->points_to\n");
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
    auto base = this->expr(move(expr->base));

    // addr
    load_vars = true;    
    auto rhs  = this->expr(move(expr->rhs));
    // addr value
    // ERROR AT GET VARIABLE NAME
    if(0){
      printf("Error: can not asign a void value into a variable.\n");
      exit(1);
    }
    this->vm.push0(WRITE64);
    printf("Err: TODO: Type_cmp(base, rhs)\n");
    return base;
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
    // bool := i8
    return Type_i64();
    
  };
  default:
    Expr::print(stderr, move(expr));
    assert(false && "Something went wrong!!");
  }
}
unique_ptr<pi_type> Compiler::stmt(unique_ptr<Stmt> stmt){
  switch(stmt->kind){
  case ST_EXPR: {
    auto e = this->expr(move(*stmt->expr));

    if(e)
      this->vm.push0(POP);
    
    return e;
  }
  case ST_BLOCK: {
    unique_ptr<pi_type> last_expr;
    for(size_t i=0; i < move(stmt->block->size()); ++i){
      last_expr = this->stmt(move((*stmt->block)[i]));
    }
    return last_expr;
  };
  case ST_IF: {
    auto iif =   move(*stmt->ifBlock);
    // if expr block
    this->expr(move(iif->expr));
    this->vm.push0(NOT);
    
    int jmp_pos = (int) this->vm.program.size();
    this->vm.push0(JMP_IF);
    this->stmt(move(iif->block));
    
    this->vm.program[jmp_pos].op_a.i64 = (int)this->vm.program.size();
    
  } return NULL;
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
    return NULL;
  }    
  default:
    assert(false && "Something went wrong!!");
  }
}
unique_ptr<pi_type> Compiler::decl(unique_ptr<Decl> decl){
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
    return move(proc->ret_type);
  } 
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
    Compiled_var comp_var = {
      .id               = name,
      .type		= move(var->type_),
      .allocation	= move(*this->vm.Heap_find(name))
    };

    Compiled_var::push(move(comp_var));
    // TODO: check if assignment
    if(var->expr()){
      printf("internal_error: global vars does not support expression assignment.\n");
    }
    return move(var->type_);
  } break;
  default:
    assert(false && "Something went wrong!!");
  }
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
