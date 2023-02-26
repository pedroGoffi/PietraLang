#ifndef  __TOKEN_CPP__
#define  __TOKEN_CPP__
// TODO: optimize this file

#include <cassert>
#include <string.h>
#include <iostream>
using namespace std;

struct CursorPos{
  FILE*  fd;
  string fname;
  int    col;
  int    line;
};

#define FMT_CPos "%s: %d: %d"
#define ARG_CPos(x)				\
  (x).fname.c_str(),				\
    (x).line,					\
    (x).col

enum TokenKind {
  TK_HASHTAG,		// #
  TK_DOLLAR,		// $
  TK_STRING_LITERAL,    // '"' [*] '"'
  TK_NAME,		// identifier
  TK_AMPERSAND,		// &
  TK_STAR,		// *
  TK_COMMA,		// ,
  TK_DOT,		// .
  TK_OS_PAREN,		// (
  TK_CS_PAREN,		// )
  TK_DCOMMA,		// ; 
  TK_QMARK,		// ?
  TK_OR_PAREN,		// [
  TK_CR_PAREN,		// ]
  TK_OC_PAREN,		// {
  TK_CC_PAREN,		// }
  TK_ASIGN,             // =
  TK_LESS,              // <
  TK_LEQ,		// <=
  TK_GT,		// >
  TK_GEQ,		// >=
  TK_TAKEAWAY,		// -
  TK_ARROW,             // ->
  TK_DTAKEAWAY,		// --
  TK_MINUS_ASIGN,	// -=
  TK_PLUS,		// +
  TK_DPLUS,		// ++
  TK_PLUS_ASIGN,	// +=
  TK_EQ,                // == 
  TK_NEQ,               // !=
  TK_SHL,		// <<
  TK_SHR,		// >>  
  TK_BANG,              // !
  TK_INT_LITERAL,	// [INT]
  TK_DDOT,		// :
  TK_DDDOT,		// ::
  TK_EOF,		// '\0'
};

//class TokenPos{
//public:
//  FILE*  fd;
//  std::string fname;
//  size_t line;
//  size_t col;
//};

 
class Token{
public:
  TokenKind   kind;
  string      text;
  CursorPos   pos;
  union {
    int   INT;
    float FLOAT;
  } as_digit;
  bool is_binary();
  bool is_unary();
  bool is_mul();
  bool is_cmp();
  bool is_and();
  
};
bool Token::is_binary(){
  return
    this->kind == TK_PLUS or
    this->kind == TK_TAKEAWAY
    ;
}
bool Token::is_unary(){
  return
    this->text == "+" or
    this->text == "-" or
    this->text == "&" or
    this->text == "*" or
    this->text == "!"        
    ;
}
bool Token::is_mul(){
  return
    this->kind == TK_STAR
    ;
}
bool Token::is_cmp(){ 
  return
    this->text == "<"  or
    this->text == "<=" or
    this->text == "==" or
    this->text == "!=" or
    this->text == ">"  or
    this->text == ">="    
    ;
}
bool Token::is_and(){
  return
    this->text =="and" or
    this->text =="or"  or
    this->text =="||"  or
    this->text =="&&" 
    ;
}
#endif/*__TOKEN_CPP__*/
