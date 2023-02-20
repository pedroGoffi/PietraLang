#ifndef  __CMD_MANAGER_CPP__
#define  __CMD_MANAGER_CPP__
#include <cassert>


namespace Flags{
  const char* const FL_HELP = "-h";
};
class CMD_Manager{
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
};

CMD_Manager::CMD_Manager(int* argc, char*** argv){
    this->argc		= argc;
    this->argv		= argv;
    this->program	= this->shift();
  }
void CMD_Manager::parse_flags(){
  while(*this->argc){
    this->flag = this->shift();
    if(*this->flag == '-'){
      this->flag++;
      if(*this->flag == '-'){
	// complex flag
	printf("Error: complex flag is not implemented yet.\n");
	exit(1);
      }
      else {
	// simple flag
	if(!strcmp(this->flag, "h")){
	  this->show_help(stdout);
	  exit(1);
	}
	else {
	  printf("Error: undefined flag '-%s'.\n", this->flag);
	  exit(1);
	}
      }
     
    }

    else {
      this->input_fp = this->flag;
    }
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

#endif /*__CMD_MANAGER_CPP__*/
