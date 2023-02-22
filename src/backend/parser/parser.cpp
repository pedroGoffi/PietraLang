#ifndef  __PARSER_CPP__
#define  __PARSER_CPP__

#include <memory>
#include "../tokenizer/tokenizer.cpp"
#include "../ast/pi_expr.cpp"
#include "../ast/pi_stmt.cpp"
#include "../ast/pi_types.cpp"
using namespace std;

typedef std::unique_ptr<Stmt> (*STMT_FUNC)();

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
  
  std::unique_ptr<Stmt> stmt();
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
  //else if(this->tokenizer->is_tk(TK_NAME)){
  //  Token tk = this->tokenizer->token;
  //  this->tokenizer->next();
  //  auto* expr = new Expr();
  //  expr->kind = EXPR_NAME_LITERAL;
  //  expr->token = tk;
  //  return expr;
  //}
  //else if(this->tokenizer->expect(TK_OR_PAREN)){
  //  // '(' expr ')'
  //  auto* expr = Parser::expr();
  //  assert(this->tokenizer->expect(TK_CR_PAREN));
  //  return expr;
  //}
  //else if(this->tokenizer->expect(TK_OC_PAREN)){
  //  // '{' expr_list '}'
  //  auto expr_list = new Allocator<std::unique_ptr<Expr>>();
  //  do {
  //    std::unique_ptr<Expr> expr = Parser::expr();
  //    expr_list->vec.push_back(expr);
  //    
  //  } while (this->tokenizer->expect(TK_COMMA));
  //
  //  assert(this->tokenizer->expect(TK_CC_PAREN));
  //
  //  auto* ret = new Expr();
  //  ret->expr_list = expr_list;
  //  ret->kind = EXPR_LIST;
  //  return ret;
  //}
  printf("Error: expected an expression, got: %s.\n",
  	 this->tokenizer->token.text);
  exit(1);
}
std::unique_ptr<Expr> Parser::base_expr(){
  // operand_expr ('(' expr_list ')' | '[' expr ']' | '.' base_expr)*
  auto lhs = this->operand_expr();
  if(this->tokenizer->expect(TK_OR_PAREN)){
    printf("Error: can not parse expr_list.\n");
    exit(1);
  }
  else if(this->tokenizer->expect(TK_OS_PAREN)){
    printf("Error: can not parse arrays.\n");
    exit(1);
  }
  else if(this->tokenizer->expect(TK_DOT)){
    printf("Error: field of name is not implemented yet.\n");
    exit(1);
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
  if(!strcmp(this->tokenizer->token.text, "if")){
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
std::unique_ptr<Stmt> parse_where(){
  printf("Error: %s is not implemented yet.\n",
	 __FUNCTION__);
  exit(1);
  
}
std::unique_ptr<Stmt> parse_if(){
  printf("Error: %s is not implemented yet.\n",
	 __FUNCTION__);
  exit(1);
  
}

std::unique_ptr<Stmt> parse_for(){
  printf("Error: %s is not implemented yet.\n",
	 __FUNCTION__);
  exit(1);
  
}

std::unique_ptr<pi_type> Parser::type(){
  // type<type> | type 
  auto type = std::make_unique<pi_type>(pi_type());
  if(!strcmp(this->tokenizer->token.text, "const")){
    this->tokenizer->next();
    type->set_cte(true);
  }
  if(!strcmp(this->tokenizer->token.text, "static")){
    this->tokenizer->next();
    printf("Error: static variables are not defined yet.\n ");
    exit(1);
  }
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
  static vector<pair<const char*, STMT_FUNC>> stmt_funcs = {
    {"where",  parse_where},
    {"if",     parse_if},
    {"for",    parse_for},
    // TODOS:
    //  {"return", parse_return}
  };
  for(auto& stf: stmt_funcs){
    if(!strcmp(this->tokenizer->token.text, stf.first)){
      return (*stf.second)();
    }
  }
  printf("Error: statements are not implemented yet.\n");
  exit(1);
  //auto* expr    = this->expr();
  //auto* as_expr = new Stmt(expr);
  //as_expr->kind = ST_EXPR;
  //return as_expr;
}

std::unique_ptr<Expr> Parser::from_string(const char* str){
  this->tokenizer.reset();
  this->tokenizer = make_unique<Tokenizer>(Tokenizer(str));
  if(*this->tokenizer->stream)
    return this->expr();
      
  return nullptr;
}

void parser_test(){
  const char* strings[] = {
    "where {1, 2}{"
    "  _ => 1"
    "}"
    //"where {a,b}{"
    //"  a, _ => ERR;"
    //"  _ => OK;"
    //"}"
  };
  for(auto& str : strings){
    auto  tk		= Tokenizer(str);
    auto  parser	= Parser(make_unique<Tokenizer>(tk));  
    auto  stmt		= parser.stmt();
    //Stmt::print(stdout, stmt);
  }
  
 
}
#endif /*__PARSER_CPP__*/
