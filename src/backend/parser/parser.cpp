#ifndef  __PARSER_CPP__
#define  __PARSER_CPP__
#include "../tokenizer/tokenizer.cpp"
#include "./expr.cpp"
class Parser{
  Tokenizer* tokenizer;
public:
  Parser(Tokenizer*);
  void  eat_string(const char* str);
  Expr* operand_expr();
  Expr* base_expr();
  Expr* unary_expr();
  Expr* mul_expr();
  Expr* add_expr();
  
  Expr* expr();
};
Parser::Parser(Tokenizer* tokenizer){
  this->tokenizer = tokenizer;  
}
void Parser::eat_string(const char* str){
  this->tokenizer = new Tokenizer(str);
}

Expr* Parser::operand_expr(){
  //assert(this->tokenizer->token.kind == TokenKind::TK_INT_LITERAL);
  if(this->tokenizer->is_tk(TK_INT_LITERAL)){
    int val = this->tokenizer->token.as_digit.INT;
    this->tokenizer->next();
    return new Expr(val);
  }
  else if(this->tokenizer->expect(TK_OR_PAREN)){
    auto* expr = Parser::expr();
    assert(this->tokenizer->expect(TK_CR_PAREN));
    return expr;
  }
  printf("Error: tryin to parse an operand_expr: %s.\n",
	 this->tokenizer->token.text);
  exit(1);
}
Expr* Parser::base_expr(){
  auto* lhs = this->operand_expr();
  if(this->tokenizer->expect(TK_OR_PAREN)){
    printf("Error: can not parse expr_list.\n");
    exit(1);
  }
  else if(this->tokenizer->expect(TK_OS_PAREN)){
    printf("Error: can not parse arrays.\n");
    exit(1);
  }
  else if(this->tokenizer->expect(TK_DOT)){
    printf("Error: can not parse DOT_ACCESS.\n");
    exit(1);
  }
  return lhs;
}
Expr* Parser::unary_expr(){
  return this->base_expr();
}
Expr* Parser::mul_expr(){
  auto* lhs = this->unary_expr();
  while(tokenizer->token.is_mul()){
    Token mul_op = this->tokenizer->token;
    this->tokenizer->next();
    auto* rhs = this->unary_expr();

    lhs = new Expr(lhs, rhs);
    lhs->token = mul_op;
    lhs->kind  = EXPR_BINARY;
  }
  return lhs;
}
Expr* Parser::add_expr(){
  auto* lhs = this->mul_expr();
  while(this->tokenizer->token.is_binary()){
    Token add_op = this->tokenizer->token;
    this->tokenizer->next();
    auto* rhs = this->mul_expr();

    lhs = new Expr(lhs, rhs);
    lhs->token = add_op;
    lhs->kind  = EXPR_BINARY;
  }
  return lhs;
}
Expr* Parser::expr(){
  return this->add_expr();
}



void parser_test(){ 
  auto  tk	= Tokenizer("1[420]");
  auto  parser	= Parser(&tk);  
  auto* expr	= parser.expr();
  
  Expr::print(stdout, expr);
}
#endif /*__PARSER_CPP__*/
