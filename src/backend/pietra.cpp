#ifndef  __PIETRA_CPP__
#define  __PIETRA_CPP__
#include "./parser/parser.cpp"

#include "./ast/pi_ast.cpp"

#include "./compiler/compiler.cpp"
#include "../frontend/cmd_manager.cpp"
#include "../frontend/reader.cpp"

namespace Pietra{
  void Main(int argc, char** argv);
  Parser Parser_from_CMD(CMD_Manager man);
}
// PIETRA IMPL
void   Pietra::Main(int argc, char** argv){
  auto cmd_man		= CMD_Manager(&argc, &argv);  
  auto reader		= Reader(cmd_man.input_fp);
  Tokenizer tokenizer	= Tokenizer(reader.file_content);

  tokenizer.token.pos = {
    .fd		= reader.fd,
    .fname	= cmd_man.input_fp,
    .col	= 1,
    .line	= 1
  };


  //auto parser = Parser(make_unique<Tokenizer>(tk));
  auto parser = Parser(make_unique<Tokenizer>(tokenizer));
  auto mod    = Module();
  
  mod.AST = parser.AST();

  do {
    if(cmd_man.dump_ast){
      mod.dump_ast(cmd_man.output_fp);
      break;
    }
    // TODO: check flags
    // type check the program
    // compile onto llvm intermediate representation
    auto compiler = Compiler();
    
    compiler.codegen(&mod);
    compiler.vm.print();
    compiler.vm.exec();
    //compiler.open_file(cmd_man.output_fp);

  } while(0);
}
#endif /*__PIETRA_CPP__*/
