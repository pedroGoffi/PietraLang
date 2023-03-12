#ifndef  __PVM_CPP__
#define  __PVM_CPP__
#include <iostream>
#include "./instructions.cpp"
#define VM_MEM_CAP 1024

struct Allocated_Obj{
  bool        heap_allocated = true;
  std::string id;
  size_t      offset;
  size_t      size;
};

enum Err{
  OK,
  StackOverflow,
  StackUnderflow,
  IlegalInst,
  IlegalOperand,
  IlegalMemoryAccess,
  DivisionByZero,
  IlegalInstructionAccess,
};
struct VM {
  InstList  program;
  Inst_Addr ip;
  Inst_Addr entry;
  
  Flag      halt;
  
  Word    stack[VM_MEM_CAP];
  size_t  stack_size;

  Word    heap[VM_MEM_CAP];
  size_t  heap_size;

  Allocated_Obj HEAP_INFO[VM_MEM_CAP];
  size_t        HEAP_INFO_SIZE;

  void push0(InstKind ik);
  void push1(InstKind ik, Word x1);
  void push2(InstKind ik, Word x1, Word x2);
  void push3(InstKind ik, Word x1, Word x2, Word x3);
  
  void print();
  Err  next_inst();
  void exec();

  Err Heap_alloc(std::string id, size_t size, Word val);
  Allocated_Obj* Heap_find(std::string id);
  
};

struct Compiled_string {
  std::string   id; 
  Allocated_Obj allocation;
  static Compiled_string* find(std::string id);
  static void push(Compiled_string compstr);
};

Compiled_string compiled_string[VM_MEM_CAP];
size_t          compiled_string_size;


Compiled_string* Compiled_string::find(std::string id){
  for(size_t i=0; i < compiled_string_size; ++i){
    if(compiled_string[i].id == id)
      return &compiled_string[i];
  }
  return NULL;
}
void             Compiled_string::push(Compiled_string compiled_str){
  assert(compiled_string_size + 1 < VM_MEM_CAP);  
  compiled_string[compiled_string_size++] = compiled_str;
}
#endif /*__PVM_CPP__*/
