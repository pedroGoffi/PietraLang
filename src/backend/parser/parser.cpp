#ifndef  __PARSER_CPP__
#define  __PARSER_CPP__

#include <memory>
#include "../tokenizer/tokenizer.cpp"
#include "../ast/pi_expr.cpp"
#include "../ast/pi_stmt.cpp"
#include "../ast/pi_types.cpp"
#include "../ast/pi_decl.cpp"
using namespace std;


#define SKIP_OPTIONAL_EOL_TK while(this->tokenizer->expect(TK_DCOMMA));



class Parser{
public:
  std::unique_ptr<Tokenizer> tokenizer;

  Parser(std::unique_ptr<Tokenizer>);
  void  eat_string(const char* str);
  std::unique_ptr<Expr> operand_expr();
  std::unique_ptr<Expr> base_expr();
  std::unique_ptr<Expr> unary_expr();
  std::unique_ptr<Expr> mul_expr();
  std::unique_ptr<Expr> add_expr();
  std::unique_ptr<Expr> cmp_expr();
  std::unique_ptr<Expr> and_expr();
  std::unique_ptr<Expr> ternary();
  std::unique_ptr<Expr> expr();
  
  std::unique_ptr<pi_type> type();
  
  std::unique_ptr<Stmt>  PWhile();
  std::unique_ptr<Stmt>  PIf();
  std::unique_ptr<Stmt>  block();
  
  std::unique_ptr<Stmt> stmt();
  
  std::unique_ptr<Decl> decl();
  std::unique_ptr<Expr> from_string(const char* str);
};

Parser::Parser(std::unique_ptr<Tokenizer> tokenizer){
  this->tokenizer = move(tokenizer);
  
}
void Parser::eat_string(const char* str){  
  this->tokenizer
    = std::make_unique<Tokenizer>(Tokenizer(str));
}

std::unique_ptr<Expr> Parser::operand_expr(){  
  if(this->tokenizer->is_tk(TK_INT_LITERAL)){
    // INT
    int val = this->tokenizer->token.as_digit.INT;
    this->tokenizer->next();   
    return Expr_int(val);
  }
  //// NAME LITERAL
  else if(this->tokenizer->is_tk(TK_NAME)){
    Token tk = this->tokenizer->token;
    this->tokenizer->next();

    auto expr = make_unique<Expr>(Expr());
    expr->kind = EXPR_NAME_LITERAL;
    expr->token = tk;
    return expr;
  }
  else if(this->tokenizer->expect(TK_OR_PAREN)){
    // '(' expr ')'
    auto expr = Parser::expr();
    assert(this->tokenizer->expect(TK_CR_PAREN));
    return expr;
  }
  else if(this->tokenizer->expect(TK_OC_PAREN)){
    // '{' expr_list '}'
    auto el = vector<std::unique_ptr<Expr>>();
    do {
      std::unique_ptr<Expr> expr = Parser::expr();

      el.push_back(move(expr));
    } while (this->tokenizer->expect(TK_COMMA));
    
    assert(this->tokenizer->expect(TK_CC_PAREN));
       
    return Expr_list(move(el));
    
  }
  printf("Error: expected an expression, got: %s.\n",	 
  	 this->tokenizer->token.text);
  exit(1);
}
std::unique_ptr<Expr> Parser::base_expr(){
  // operand_expr ('(' expr_list ')' | '[' expr ']' | '.' base_expr)*
  auto lhs = this->operand_expr();
  if(this->tokenizer->expect(TK_OR_PAREN)){

    auto el = vector<std::unique_ptr<Expr>>();
    // Scenarios:
    // Empty args:    ()
    // One arg:       (a)
    // Multiple args: (a, b)
    lhs->kind = ExprKind::EXPR_CALL;
    if(!this->tokenizer->expect(TK_CR_PAREN)){

      // Parse the args
      // args_list = expr (',' args_list)*
      do {
	std::unique_ptr<Expr> expr = Parser::expr();

	el.push_back(move(expr));
      } while (this->tokenizer->expect(TK_COMMA));

      // Check correct syntax
      // call ending with ')' token
      if(!this->tokenizer->expect(TK_CR_PAREN)){
	printf("Error: expected ')', after an expression list, got '%s'.\n",
	       this->tokenizer->token.text);
	exit(1);
      }
      // Exrpression OK
      lhs->rhs  = Expr_list(move(el));
    }
    return lhs;
  }
  else if(this->tokenizer->expect(TK_OS_PAREN)){
    printf("Error: can not parse arrays.\n");
    exit(1);
  }
  else if(this->tokenizer->expect(TK_DOT)){
    printf("Error: field of name is not implemented yet.\n");
    exit(1);
  }

  else if(this->tokenizer->expect(TK_DDOT)){

    if (lhs->kind != EXPR_NAME_LITERAL){
      // Verify if the VAR NAME is OK
      fprintf(stderr, "Error: var declaraton expects a NAME instead of an expression.\n");
      fprintf(stderr, "Note: the ast note is: ");
      
      fprintf(stderr, "[");
      Expr::print(stderr, move(lhs));
      fprintf(stderr, "]\n");
      exit(1);
    } // var name OK
    unique_ptr<pi_type> type;
    if(this->tokenizer->is_tk(TK_ASIGN)){
      type = make_unique<pi_type>(pi_type("auto", 8));
      printf("auto.\n");
    }
    else {
      type = this->type();
    }

    unique_ptr<Expr> rhs;
    if(this->tokenizer->expect(TK_ASIGN)){
      // NOTE: var declaration only accept '=' as token      
      rhs = this->expr();
    }
    SKIP_OPTIONAL_EOL_TK
    return Expr_var(move(type),
		    move(lhs),
		    move(rhs));
  }
  return lhs;
}
std::unique_ptr<Expr> Parser::unary_expr(){
  // [+-&*!] unary_expr | base_expr
  if(this->tokenizer->token.is_unary()){
    Token unary_op = this->tokenizer->token;
    this->tokenizer->next();
    auto expr		= this->unary_expr();
    return Expr_unary(unary_op, move(expr));
  }
  return this->base_expr();
}
std::unique_ptr<Expr> Parser::mul_expr(){
  auto lhs = this->unary_expr();
  while(tokenizer->token.is_mul()){
    Token mul_op = this->tokenizer->token;
    this->tokenizer->next();
    auto rhs = this->unary_expr();
  

    lhs = Expr_binary_op(mul_op, move(lhs), move(rhs));
  }
  return lhs;
}
std::unique_ptr<Expr> Parser::add_expr(){
  auto lhs = this->mul_expr();
  while(this->tokenizer->token.is_binary()){
    Token add_op = this->tokenizer->token;
    this->tokenizer->next();
    
    auto rhs = this->mul_expr();
  
    lhs = Expr_binary_op(add_op, move(lhs), move(rhs));
  }
  return lhs;
}
std::unique_ptr<Expr> Parser::cmp_expr(){
  auto lhs = this->add_expr();
  while(this->tokenizer->token.is_cmp()){
    Token cmp_op = this->tokenizer->token;
    this->tokenizer->next();
    auto rhs = this->add_expr();
  

    lhs = Expr_binary_op(cmp_op, move(lhs), move(rhs));
    lhs->kind = EXPR_CMP;
  }
  return lhs;
}
std::unique_ptr<Expr> Parser::and_expr(){
  auto lhs = this->cmp_expr();
  while(this->tokenizer->token.is_and()){
    Token and_op = this->tokenizer->token;
    this->tokenizer->next();
    auto rhs = this->cmp_expr();
    

    lhs = Expr_binary_op(and_op, move(lhs), move(rhs));
    lhs->kind  = EXPR_AND;
  }
  return lhs;
}
std::unique_ptr<Expr> Parser::ternary(){
  // or_expr ('if' ternary_expr else' ternary_expr)*
  auto and_expr = this->and_expr();
  if(!strcmp(this->tokenizer->token.text, "test")){
    this->tokenizer->next();
    // ternary struct
    auto if_expr = this->ternary();
    
    assert(!strcmp(this->tokenizer->token.text, "else"));
    this->tokenizer->next();
    
    auto else_expr = this->ternary();
    
    return Expr_ternary(move(and_expr),
			move(if_expr),
			move(else_expr));
  }
  return and_expr;
}
std::unique_ptr<Expr> Parser::expr(){
  auto lhs = this->ternary();
  if(!strcmp(this->tokenizer->token.text, "as")){
    this->tokenizer->next();
    auto type  = this->type();
    auto* cast = new Cast(move(lhs), move(type));
    return Expr_cast(cast);
  }
  return lhs;
}


std::unique_ptr<Stmt>  Parser::PWhile(){
  // while expr stmt
  assert(!strcmp(this->tokenizer->token.text, "while"));  
  this->tokenizer->next();
  
  auto expr      = this->expr();
  auto opt_block = this->block();
  return Stmt_while(move(expr), move(opt_block));
}

std::unique_ptr<Stmt>  Parser::PIf(){
  assert(!strcmp(this->tokenizer->token.text, "if"));
  
  this->tokenizer->next();  
  auto expr      = this->expr();
  auto opt_block = this->block();
  return Stmt_if(move(expr), move(opt_block));  
}
std::unique_ptr<Stmt>  Parser::block(){
  // cases:
  /*
    One st: stmt
    
    Poli st: {stmt1; stmt2; stmt3; ... , stmtn;}
    NOTE: ';' token is optional
   */

  // TODO: auto block = make_unique(vector<unique_ptr<Stmt>>(vector<unique_ptr<Stmt>()));l

  if(this->tokenizer->expect(TK_OC_PAREN)){

    auto block = new vector<unique_ptr<Stmt>>();
    while(!this->tokenizer->is_tk(TK_CC_PAREN)){
      unique_ptr<Stmt> stmt_node = this->stmt();
      block->push_back(move(stmt_node));
      SKIP_OPTIONAL_EOL_TK;
    }
    assert(this->tokenizer->is_tk(TK_CC_PAREN));
    this->tokenizer->next();
    
    return Stmt_block(move(*block));
  }
  else {
    auto expr = this->expr();
    SKIP_OPTIONAL_EOL_TK;
    return Stmt_expr(move(expr));
  }
  // return block;
}
std::unique_ptr<pi_type> Parser::type(){
  // type<type> | type 
  auto type = std::make_unique<pi_type>(pi_type());

  // check for pi_keywords
  if(!strcmp(this->tokenizer->token.text, "const")){
    this->tokenizer->next();
    type->set_cte(true);
  }
  if(!strcmp(this->tokenizer->token.text, "static")){
    this->tokenizer->next();
    printf("Error: static variables are not defined yet.\n ");
    exit(1);
  }


  // check for pointers, name_literals, templates
  if(this->tokenizer->is_tk(TK_STAR)){
    printf("Error: can not parse pointers yet.\n");
    exit(1);
  }
  if(this->tokenizer->is_tk(TK_NAME)){
    // type_name
    type->name() = string(this->tokenizer->token.text);
    this->tokenizer->next();

  }

  if(this->tokenizer->is_tk(TK_LESS)){
    // template<type>
    printf("Error: can not parse templates yet.\n");
    exit(1);
    
  }
  return type;
}
std::unique_ptr<Stmt> Parser::stmt(){

  if(!strcmp(this->tokenizer->token.text, "if")){
    return this->PIf();
  }
  if(!strcmp(this->tokenizer->token.text, "while")){
    return this->PWhile();
  }
  else {
    // Expr
    auto e = this->expr();
    return Stmt_expr(move(e));
  }

}

std::unique_ptr<Decl> Parser::decl(){
  if(!this->tokenizer->stream) return NULL;
  const char* str = this->tokenizer->token.text;
  this->tokenizer->next();
  bool pre_processed_entity = false;


  
  if(this->tokenizer->expect(TK_DDDOT)){    
    pre_processed_entity = true;
    auto proc = make_unique<pi_proc>(pi_proc());
    
    proc->name() = string(str);
    /*
      Syntax:
        NAME :: (inline)* (extern)* '(' ARGS ')' STMT
    */
    if(!strcmp(this->tokenizer->token.text, "inline")){
      this->tokenizer->next();
      proc->inlined() = true;
    }
    if(!strcmp(this->tokenizer->token.text, "extern")){
      this->tokenizer->next();
      proc->externed() = true;
    }
    
    if(this->tokenizer->expect(TK_OR_PAREN)){
      if(!this->tokenizer->expect(TK_CR_PAREN)){
	printf("Error: can not parse procedure arguments yet.\n");
	exit(1);
      }

      std::unique_ptr<pi_type> ty;
      if(this->tokenizer->expect(TK_ARROW)){
	ty = this->type();
      }
      else {
	ty = NULL;
      }
      proc->set_type(move(ty));
      proc->set_block(this->block());
      return Decl_proc(move(proc));
    }
    else {
      // TODO: syntax for aggregates decl (enum, union, struct, ...)
      printf("Error: unexpected pre-processed expression at '%s' definition.\n",
	     str);
      exit(1);
    }
    exit(1);
  }
  else {
    printf("Error: unexpected name at global declaration: '%s'\n",
	   str);
    exit(1);
  }
}
std::unique_ptr<Expr> Parser::from_string(const char* str){
  this->tokenizer.reset();
  this->tokenizer = make_unique<Tokenizer>(Tokenizer(str));
  return
    !this->tokenizer->is_tk(TK_EOF) and
    *this->tokenizer->stream != '\0'
    ? this->expr()
    : nullptr;
    
}
#endif /*__PARSER_CPP__*/
