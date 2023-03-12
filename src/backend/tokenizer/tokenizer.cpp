// TODO: calculate the position in the file
#ifndef  __TOKENIZER_CPP__
#define  __TOKENIZER_CPP__



#include <cassert>
#include <string.h>
#include <climits>
#include "../allocator.cpp"
#include "./token.cpp"
using namespace std;
namespace TokenizerRules {
  bool is_name(const char* str){
    return (('A' <= *str and *str <= 'Z') or
	    ('a' <= *str and *str <= 'z') or
	    *str == '_');
  }
  bool is_digit(char chr){
    return ('0' <= chr and chr <= '9');
  }
}
namespace Scanner{
  int chr_to_int(char c){
    switch(c){
    case '0': return 0;
    case '1': return 1;
    case '2': return 2;
    case '3': return 3;
    case '4': return 4;
    case '5': return 5;
    case '6': return 6;
    case '7': return 7;
    case '8': return 8;
    case '9': return 9;
    case 'a': case 'A': return 10;
    case 'b': case 'B': return 11;
    case 'c': case 'C': return 12;
    case 'd': case 'D': return 13;
    case 'e': case 'E': return 14;
    case 'f': case 'F': return 15;
    default:  return 0;
    }
  }
  int scan_int(const char* stream){
    int number_base = 10;
    if(*stream == '0'){
      stream++;
      if(tolower(*stream) == 'x'){
	stream++;
	number_base = 16;
      }
      else if (isdigit(*stream)){
	number_base = 8;
      }   
    }  
    int acc = 0;
    for(;;){      
      int digit = Scanner::chr_to_int(*stream);      
      if(digit == 0 && *stream != '0'){
	break;
      }
      if(digit >= number_base){
	printf("Sytnax ERROR: digit '%c' out of range for base %d\n", *stream, number_base);
	exit(1);
      }

      if(acc > (INT_MAX - digit)/number_base){
	printf("Sytnax ERROR: Integer literal overflow\n");
	exit(1);
      }
      
      acc = acc*number_base + digit;
      stream++;
    }
    return acc;
  }
  vector<string> intern_strings;
  const char* str_ptr_range(const char* bgin, const char* end){
    size_t len = end - bgin;
    for(int i=0; i < (int)intern_strings.size(); ++i){
      auto str = intern_strings[i];
      if(str.size() == len and strncmp(str.c_str(), bgin, len) == 0){
	return intern_strings[i].c_str();
      }
    }
    char *str = (char*)std::malloc(len + 1);
    memcpy(str, bgin, len);
    str[len] = 0;    
    intern_strings.push_back(string(str));
    return (const char*)str;
  }
}
// A tokeinzer will handle the stream and the current token
class Tokenizer{
private:
  Token* is_simple_keyword();
  Token* is_duo_keyword();
  Token* is_complex_keyword();
public:
  Token       token;
  const char* stream;
  
  Tokenizer(const char*);
  Token* next();
  bool   expect(TokenKind kind);
  bool   is_tk(TokenKind kind);
  
};

Tokenizer::Tokenizer(const char* stream){
  this->token = Token();
  this->stream = stream;
  this->next();
}

Token* Tokenizer::next(){
  int col_diff  = 0;
  int line_diff = 0;
 _TOKENIZER_NEXT:
  const char* strBegin = this->stream;
  
  if(*this->stream == ' ' or *this->stream == '\t'){
    col_diff++;
    
    this->stream++;
    goto _TOKENIZER_NEXT;    
  }
  if(*this->stream == '\n'){
    col_diff   = 0;        
    line_diff += 1;

    
    this->token.pos.col = 1;
    this->stream++;    
    goto _TOKENIZER_NEXT;
  }
  
  if(isspace(*this->stream)){
    printf("UNHANDLED SPACE PONCTUATION: %i.\n",
	   (int)*this->stream);
    exit(1);
    
  }
  if(TokenizerRules::is_name(this->stream)){
    while(std::isalnum(*this->stream) or *this->stream == '_'){
      col_diff++;
      this->stream++;
    }
    this->token.kind = TokenKind::TK_NAME;
    this->token.text = Scanner::str_ptr_range(strBegin, this->stream);
    this->token.kind = TokenKind::TK_NAME;

    this->token.pos.line += line_diff;
    this->token.pos.col  += col_diff;
    return &this->token;
  }
  // DIGITS
  else if(TokenizerRules::is_digit(*this->stream)){
    while(TokenizerRules::is_digit(*this->stream)){
      col_diff++;
      this->stream++;
    }
    // FLOATS
    if(*this->stream == '.'){
      printf("Error: FLOATS are not implemented yet.\n");
      exit(1);
    }
    // INTS
    else {      
      const char* str		= Scanner::str_ptr_range(strBegin, this->stream);
      this->token.as_digit.INT	= Scanner::scan_int(str);
      this->token.text		= str;
      this->token.kind		= TokenKind::TK_INT_LITERAL;
      // TOOD :this->token	= Scanner::scan_int(this);
    }
    assert(*this->stream != '.');
    this->token.pos.line += line_diff;
    this->token.pos.col  += col_diff;
    return &this->token;
  }
  // STRINGS
  else if(*this->stream == '"'){
    strBegin = this->stream;

    this->stream++;
    while(*this->stream != '"' and *this->stream != '\0'){
      this->stream++;
    }


    if(*this->stream != '"'){
      printf("Syntax error: expected '\"' at the end of a string but got: %s\n",
	     this->stream);
      exit(1);
    }
    this->stream++;
    
    this->token.kind = TokenKind::TK_STRING_LITERAL;
    assert(strBegin != this->stream);
    this->token.text = Scanner::str_ptr_range(strBegin + 1, this->stream - 1);
    this->token.pos.line += line_diff;
    this->token.pos.col  += col_diff;
    return &this->token;
  }
  // KEYWORDS
  // KW = ''
  else if(Token* tk = this->is_simple_keyword()){
    tk->pos.line += line_diff;
    tk->pos.col  += col_diff;
    return tk;
  }
  else if(Token* tk = this->is_duo_keyword()){
    tk->pos.line += line_diff;
    tk->pos.col  += col_diff;
    return tk;
  }
  else if(Token* tk = this->is_complex_keyword()){
    tk->pos.line += line_diff;
    tk->pos.col  += col_diff;
    return tk;
  }
  
  // Custom poctuations
  else if(*this->stream == '-'){
    this->token.kind = TK_TAKEAWAY;
    col_diff++;
    this->stream++;
  
    if(*this->stream == '-'){
      this->token.kind = TK_DTAKEAWAY;
      col_diff++;
      this->stream++;      
    }
    else if(*this->stream == '>'){  
      this->token.kind = TK_ARROW;
      col_diff++;
      this->stream++;
    }
    else if(*this->stream == '='){
      this->token.kind = TK_MINUS_ASIGN;
      col_diff++;
      this->stream++;
    }
    this->token.text = Scanner::str_ptr_range(strBegin, this->stream);
    this->token.pos.line += line_diff;
    this->token.pos.col  += col_diff;
    return &this->token;
    
  }
  //{'-', TK_TAKEAWAY, '-', TK_DTAKEAWAY, '=', TK_MINUS_ASIGN}
  
  
  else {   
    return NULL;
  }
}
Token* Tokenizer::is_simple_keyword(){
  using namespace std;
  static vector<pair<char, TokenKind>> simple_kws = {
    {'#', TokenKind::TK_HASHTAG},
    {'$', TokenKind::TK_DOLLAR},
    {'&', TokenKind::TK_AMPERSAND},
    {'*', TokenKind::TK_STAR},
    {',', TokenKind::TK_COMMA},
    {'.', TokenKind::TK_DOT},
    {'[', TokenKind::TK_OS_PAREN},
    {']', TokenKind::TK_CS_PAREN},
    {';', TokenKind::TK_DCOMMA},        
    {'?', TokenKind::TK_QMARK},
    {'(', TokenKind::TK_OR_PAREN},
    {')', TokenKind::TK_CR_PAREN},
    {'{', TokenKind::TK_OC_PAREN},
    {'}', TokenKind::TK_CC_PAREN},    
    {'\0',TokenKind::TK_EOF}
  };
  const char* begin = this->stream;
  for(pair<char, TokenKind>& kw : simple_kws){
    if(*this->stream == kw.first){
      this->token.kind = kw.second;
      this->stream++;
      this->token.text = Scanner::str_ptr_range(begin, this->stream);

      this->token.pos.col  += 1;
      return &this->token;
    }
  }
  return NULL;
}

struct __duo_kws__{
  char      chr1;
  TokenKind kind1;
  char      chr2;
  TokenKind kind2;
};
Token* Tokenizer::is_duo_keyword(){
  using namespace std;
  static vector<__duo_kws__> duo_kws = {
    {':', TK_DDOT,  ':', TK_DDDOT},
    {'=', TK_ASIGN, '=', TK_EQ},
    {'!', TK_BANG,  '=', TK_NEQ}
  };
  const char* begin = this->stream;
  for(__duo_kws__& dk: duo_kws){
    if(*this->stream == dk.chr1){
      this->token.kind = dk.kind1;
      this->token.pos.col++;
      this->stream++;
      if(*this->stream == dk.chr2){
	this->token.kind  = dk.kind2;
	this->token.pos.col++;
	this->stream++;
      }
      this->token.text = Scanner::str_ptr_range(begin, this->stream);      
      return &this->token;
    }
  }
  return NULL;
}
struct __complex_kws__ {
  char chr1;
  TokenKind kind1;
  char chr2;
  TokenKind kind2;
  char chr3;
  TokenKind kind3;
  
};
Token* Tokenizer::is_complex_keyword(){
  using namespace std;
  vector<__complex_kws__> complex_kws = {
    {'<', TK_LESS, '<', TK_SHL, '=', TK_LEQ},
    {'>', TK_GT,   '>', TK_SHR, '=', TK_GEQ},
    {'+', TK_PLUS,      '+', TK_DPLUS, '=', TK_PLUS_ASIGN}
  };
  const char* begin = this->stream;
  for(auto& ckw : complex_kws){
    if(*this->stream == ckw.chr1){
      this->token.kind = ckw.kind1;
      this->token.pos.col++;
      this->stream++;
      
      if(*this->stream == ckw.chr2){
	this->token.kind = ckw.kind2;
	this->token.pos.col++;
	this->stream++;
      }
      else if(*this->stream == ckw.chr3){
	this->token.kind = ckw.kind3;
	this->token.pos.col++;
	this->stream++;
      }
      this->token.text = Scanner::str_ptr_range(begin, this->stream);
      return &this->token;      
    }
  }
  return NULL;
}
bool Tokenizer::expect(TokenKind kind){
  if(this->token.kind == kind){
    this->next();
    return true;
  }
  return false;
}
bool Tokenizer::is_tk(TokenKind kind){
  if(this->token.kind == kind){
    return true;
  }
  return false;
}
#endif /*__TOKENIZER_CPP__*/
