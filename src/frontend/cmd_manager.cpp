#ifndef  __CMD_MANAGER_CPP__
#define  __CMD_MANAGER_CPP__

#include <cassert>
#include <iostream>


class CMD_Manager{
public:
  // flags  
  bool dump_ast = false;
  //
  int*        argc;
  const char* flag;  
  char***     argv;
public:
  const char* program;
  const char* input_fp;
  const char* output_fp = "out.c";
  CMD_Manager(int* argc, char*** argv);
  void parse_flags();
  const char* shift();
  void show_help(std::FILE*);
  void simple_flag();
};

CMD_Manager::CMD_Manager(int* argc, char*** argv){
    this->argc		= argc;
    this->argv		= argv;
    this->program	= this->shift();
    this->parse_flags();
}
void CMD_Manager::parse_flags(){
  while(*this->argc){
    
    this->flag = this->shift();   
    if(*this->flag == '-'){
      this->flag++;
      this->simple_flag();
      continue;
    }
    this->input_fp = this->flag;    
  }
}

const char* CMD_Manager::shift(){
  assert(*this->argc > 0);
  const char* ret = **this->argv;
  *this->argv += 1;
  *this->argc -= 1;
  return ret;

};

void CMD_Manager::show_help(std::FILE* file){
  fprintf(file, "|| USAGE:\n");
  fprintf(file, "||     %s <SUBCOMMAND> [OPTION] <file>\n", this->program);
  fprintf(file, "|| SUBCOMMANDS:\n");
  fprintf(file, "||     -h         will print this message to the standart output\n");
  
  
}
void CMD_Manager::simple_flag(){
  const char* flag_begin = this->flag - 1;
  if(*this->flag == '-'){
    // -[key] or --[word]
    this->flag++;
  }
  // using string to make the code easier to read (for me)
  std::string str = std::string(this->flag);
  
  if(str == "h" or str == "help"){
    this->show_help(stdout);
    exit(1);
  }

  if(str == "ast"){
    this->dump_ast = true;
  }
  else {
    fprintf(stderr,
	    "Error: unknown flag: '%s'\n",
	    flag_begin);
    exit(1);
  }
}
#endif /*__CMD_MANAGER_CPP__*/
