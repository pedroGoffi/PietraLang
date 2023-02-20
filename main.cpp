#include "./src/backend/tokenizer/tokenizer.cpp"
#include "./src/backend/parser/parser.cpp"

#include "./src/frontend/cmd_manager.cpp"
int main(int argc, char** argv){
  auto manager = CMD_Manager(&argc, &argv);
  manager.parse_flags();

  assert(manager.input_fp);
  parser_test();
}
