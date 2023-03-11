#ifndef  __PVM_CPP__
#define  __PVM_CPP__
#include <iostream>
#include "./instructions.cpp"
#define VM_MEM_CAP 1024

struct Allocated_Obj{
  bool        heap_allocated;
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





#endif /*__PVM_CPP__*/
