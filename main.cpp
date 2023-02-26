// TODO: an Allocator
#include "./src/backend/tokenizer/tokenizer.cpp"
#include "./src/backend/parser/parser.cpp"
#include "./src/backend/ast/pi_variable.cpp"
#include "./src/frontend/cmd_manager.cpp"
#include "./src/frontend/reader.cpp"
#include <memory>


using namespace std;
int main(int argc, char** argv){
  auto man = CMD_Manager(&argc, &argv);
  {
    man.parse_flags();
  }
  auto reader = Reader(man.input_fp);
  {
    reader.load_file();
  }
  auto tk     = Tokenizer(reader.file_content);
  // TODO: TokenPos
  auto parser = Parser(make_unique<Tokenizer>(tk));
  // ok setted the parser with the tokenizer
  
  // for now we can create AST branchs
  printf("PIETRA_EXPRESSIONS_AST:\n");
  printf("------------------\n");
  
  do{
    auto st = parser.decl();
    if(!st) break;
    Decl::print(stdout, move(st));        
  } while(1);
}
