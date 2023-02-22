#ifndef  __READER_CPP__
#define  __READER_CPP__
#include <iostream>
class Reader{
public:
  const char* file_path;
  const char* file_content;
  std::FILE* fd;
  
  Reader(const char* fp){
    this->file_path = fp;
    this->reach_file();
  }
  void reach_file();
  const char* load_file();
};
void Reader::reach_file(){
  this->fd = std::fopen(this->file_path, "r");
}
const char* Reader::load_file(){
  if(!this->fd){
    printf("Error: this->fd is a nullptr.\n");
    exit(1);
  }
  std::fseek(this->fd, 0, SEEK_END);
  auto fsize = std::ftell(this->fd);
  std::fseek(this->fd, 0, SEEK_SET);

  this->file_content
    = (char*)std::malloc(sizeof(char) * fsize + 1);
  std::fread((void*)this->file_content, 1, fsize, this->fd);
  std::fclose(this->fd);
  return this->file_content;
}
#endif /*__READER_CPP__*/
