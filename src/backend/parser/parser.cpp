#ifndef  __PARSER_CPP__
#define  __PARSER_CPP__

#include <memory>
#include <cstdarg>
#include "../../frontend/reader.cpp"
#include "../tokenizer/tokenizer.cpp"

#include "../ast/pi_ast.cpp"



using namespace std;


#define SKIP_OPTIONAL_EOL_TK while(this->tokenizer->expect(TK_DCOMMA)){}


class Parser{
public:
  bool err = false;
  CursorPos pos;
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

  std::vector<unique_ptr<Decl>> AST();
  void syntax_error(const char* fmt, ...);  
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
  else if(this->tokenizer->token.kind == TK_STRING_LITERAL){
    Token tk = this->tokenizer->token;
    this->tokenizer->next();
    return Expr_str(tk);
  }
  //// NAME LITERAL
  else if(this->tokenizer->is_tk(TK_NAME)){
    Token tk = this->tokenizer->token;
    this->tokenizer->next();
    if(tk.text == "let" or tk.text == "const"){
      // let NAME (':' TYPE)* ('=' EXPR)*
      // var declaration
      bool var_mutable = false;
      bool cte = tk.text == "const";
      
      if(this->tokenizer->token.text == "mut"){
	var_mutable = true;
	this->tokenizer->next();
      }
      
      if(this->tokenizer->token.kind != TK_NAME){
	this->syntax_error("Expected NAME LITERAL in variable definition, got: %s.\n",
			   this->tokenizer->token.text.c_str());
	exit(1);
      }
      Token var_name_def_tk		= this->tokenizer->token;
      this->tokenizer->next();
      

      auto   var_type		= make_unique<pi_type>(pi_type());
      if(this->tokenizer->expect(TK_DDOT)){
	var_type = this->type();
	var_type->undefined() = false;
      }

      auto   var_expr		= make_unique<Expr>();
      if(this->tokenizer->expect(TK_ASIGN)){
	var_expr = this->expr();
      }
      // Filter bad syntax
      if(cte and var_mutable ){
	this->syntax_error("Can not use CONST with MUT in variable: %s.\n",
			   var_name_def_tk.text.c_str());
	exit(1);
      }
      if(cte and var_type->undefined()){
	this->syntax_error("Expected typed variable while using const in variable: %s.\n",
			   var_name_def_tk.text.c_str());
	exit(1);	
      }
      return Expr_var(var_name_def_tk,
		      var_mutable,
		      move(var_type),
		      move(var_expr));
    }
    else {
      // EXPR_NAME
      auto expr = make_unique<Expr>(Expr());
      expr->kind = EXPR_NAME_LITERAL;
      expr->token = tk;
      return expr;
    }
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
  this->syntax_error("expected an expression, got: %s.\n",	 
		     this->tokenizer->token.text.c_str());
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
	this->syntax_error("expected ')', after an expression list, got '%s'.\n",
			   this->tokenizer->token.text.c_str());
	exit(1);
	
      }
      // Exrpression OK
      lhs->base  = Expr_list(move(el));

      if(this->tokenizer->expect(TK_DOT)){	
	lhs->rhs = this->base_expr();
	
      }      
    }
    return lhs;
  }
  else if(this->tokenizer->expect(TK_OS_PAREN)){
    lhs->rhs = this->base_expr();
    if(!this->tokenizer->expect(TK_CS_PAREN)){
      this->syntax_error("expected ']' after array access.\n");
      exit(1);
    }
    return lhs;
    
  }
  else if(this->tokenizer->expect(TK_ASIGN)){
    // TODO: differend types of ASSING -= ^= *= /= ...
    printf("ASSIGN\n");
    auto rhs = this->expr();
    printf("ok\n");
    return Expr_assign(move(lhs),
		       move(rhs));
  }
  else if(this->tokenizer->expect(TK_DOT)){
    auto rhs = this->base_expr();
    lhs->rhs = move(rhs);
    return lhs;    
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
  if(this->tokenizer->token.text == "teste"){
    this->tokenizer->next();
    // ternary struct
    auto if_expr = this->ternary();
    
    if(this->tokenizer->token.text != "senao"){
      this->syntax_error("expected the keyword 'else' after test expression.\n");
      exit(1);
    }
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
  if(this->tokenizer->token.text == "as"){
    this->tokenizer->next();
    auto type  = this->type();
    auto* cast = new Cast(move(lhs), move(type));
    return Expr_cast(cast);
  }
  return lhs;
}


std::unique_ptr<Stmt>  Parser::PWhile(){
  // while expr stmt
  assert(this->tokenizer->token.text == "enquanto");  
  this->tokenizer->next();
  
  auto expr      = this->expr();
  auto opt_block = this->block();
  return Stmt_while(move(expr), move(opt_block));
}

std::unique_ptr<Stmt>  Parser::PIf(){
  assert(this->tokenizer->token.text == "se");
  
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

  // check for pointers, name_literals, templates
  if(this->tokenizer->is_tk(TK_STAR)){
    this->syntax_error("can not parse pointers yet.\n");
    exit(1);    
  }
  
  if(this->tokenizer->is_tk(TK_NAME)){
    // type_name
    type->name() = this->tokenizer->token.text;
    this->tokenizer->next();
  }

  if(this->tokenizer->is_tk(TK_LESS)){
    // template<type>
    this->syntax_error("can not parse templates yet.\n");
    exit(1);       
  }
  
  return type;
}
std::unique_ptr<Stmt> Parser::stmt(){
  if(this->tokenizer->token.text == "se"){
    return this->PIf();
  }
  
  if(this->tokenizer->token.text == "enquanto"){
    return this->PWhile();
  }
   
  auto e = this->expr();
  return Stmt_expr(move(e)); 
}

std::unique_ptr<Decl> Parser::decl(){
  string str = this->tokenizer->token.text;
  this->tokenizer->next();
  bool pre_processed_entity = false;


  if (str == "decl" or str == "const"){
    printf("OK GLOBAL VAR.\n");
    bool var_mutable = false;
    bool cte = str == "const";
      
    if(this->tokenizer->token.text == "mut"){
      var_mutable = true;
      this->tokenizer->next();
    }

    if(this->tokenizer->token.kind != TK_NAME){
      this->syntax_error("Expected NAME LITERAL in variable definition, got: %s.\n",
			 this->tokenizer->token.text.c_str());
      exit(1);
    }
    Token var_name_def_tk		= this->tokenizer->token;
    this->tokenizer->next();
      

    auto   var_type		= make_unique<pi_type>(pi_type());
    if(this->tokenizer->expect(TK_DDOT)){
      var_type = this->type();
      var_type->undefined() = false;
    }

    auto   var_expr		= Expr_int(0);
    if(this->tokenizer->expect(TK_ASIGN)){
      var_expr = this->expr();
    }
    // Filter bad syntax
    if(cte and var_mutable ){
      this->syntax_error("Can not use CONST with MUT in variable: %s.\n",
			 var_name_def_tk.text.c_str());
      exit(1);
    }
    if(cte and var_type->undefined()){
      this->syntax_error("Expected typed variable while using const in variable: %s.\n",
			 var_name_def_tk.text.c_str());
      exit(1);	
    }
    auto var = pi_variable(var_name_def_tk.text,
			   move(var_type),
			   move(var_expr),
			   var_mutable);

    return Decl_var(make_unique<pi_variable>(move(var)));
      
  }
  else if(this->tokenizer->expect(TK_DDDOT)){    
    pre_processed_entity = true;
    auto proc = make_unique<pi_proc>(pi_proc());
    
    proc->name() = str;
    /*
      Syntax:
        NAME :: (inline)* (extern)* '(' ARGS ')' STMT
    */
    if(this->tokenizer->token.text == "alinhada"){
      this->tokenizer->next();
      proc->inlined() = true;
    }
    if(this->tokenizer->token.text == "externa"){
      this->tokenizer->next();
      proc->externed() = true;
    }
    
    if(this->tokenizer->expect(TK_OR_PAREN)){
      if(!this->tokenizer->expect(TK_CR_PAREN)){
        this->syntax_error("can not parse procedure arguments yet.\n");
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
      this->syntax_error("unexpected pre-processed expression at '%s' definition.\n",
			 str.c_str());
      exit(1);
      
    }
    
  }

  this->syntax_error("unexpected name at global declaration: '%s'\n",
		     str.c_str());
  exit(1);
    
  
}
std::vector<unique_ptr<Decl>> Parser::AST(){
  auto ast = vector<unique_ptr<Decl>>();
  do {
    unique_ptr<Decl> decl_node = this->decl();
    ast.push_back(move(decl_node));
  } while(this->tokenizer->next()->kind != TK_EOF);
  
  return ast;
}
void Parser::syntax_error(const char* fmt, ...){
  va_list args;
  va_start(args, fmt);
  fprintf(stderr, FMT_CPos ": Syntax Error: ",
	  ARG_CPos(this->tokenizer->token.pos));
  fprintf(stderr, fmt, args);
  va_end(args);
  

  exit(1);
}
#endif /*__PARSER_CPP__*/
