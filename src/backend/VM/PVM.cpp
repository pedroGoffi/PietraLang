#ifndef  __RUNNER__
#define  __RUNNER__
#include "./PVM.hpp"

// Initialize the variable only
// Assignment will be done at VINST lvl

Err VM::Heap_alloc(std::string id, size_t size, Word val){
  if(!(this->HEAP_INFO_SIZE + 1 < VM_MEM_CAP)){
    return IlegalMemoryAccess;
  }
  
  this->heap[this->heap_size + 1] = val;
  
  this->HEAP_INFO[this->HEAP_INFO_SIZE++] = {
    .heap_allocated = true,
    .id             = id,
    .offset         = this->heap_size + 1,
    .size           = size
  };
  this->heap_size++;
  return OK;
}

Allocated_Obj* VM::Heap_find(std::string id){
  for(size_t i=0; i < this->HEAP_INFO_SIZE; i++){
    Allocated_Obj* al = &this->HEAP_INFO[i];
    printf("Heap_find = %s\n", al->id.c_str());
    if(al->id == id)      
      return al;
  }
  return NULL;
}
Err VM::next_inst(){
  Inst inst = this->program[this->ip];
  
  switch(inst.kind){
  case POP:
    this->stack[--this->stack_size] = {0};
    ip++;
    return OK;    
  case PUSH:
    this->stack[this->stack_size++] = (inst.op_a);
    ip++;
    return OK;    
  case LOAD_PTR: {
    auto data =  *(int64_t*)this->stack[this->stack_size - 1].ptr;    
    this->stack[this->stack_size - 1].i64 = data;
    ip++;
    return OK;
  }   
  case FAST_ADDI: {
    auto a = this->stack[--this->stack_size];
    auto b = this->stack[--this->stack_size];
    this->stack[this->stack_size++] = {
      .i64 = b.i64 + a.i64
    };
    ip++;
  } return OK;
  case FAST_SUBI:  {
    auto a = this->stack[--this->stack_size];
    auto b = this->stack[--this->stack_size];
    this->stack[this->stack_size++] = {
      .i64 =  b.i64 - a.i64
    };
    ip++;
  } return OK;
  case FAST_MULI: {
    auto a = this->stack[--this->stack_size];
    auto b = this->stack[--this->stack_size];
    this->stack[this->stack_size++] = {
      .i64 = b.i64 * a.i64
    };
    ip++;
  } return OK;
  case FAST_DIVI: {
    auto a = this->stack[--this->stack_size];
    auto b = this->stack[--this->stack_size];
    this->stack[this->stack_size++] = {
      .i64 = (int64_t)(b.i64 / a.i64)
    };
    ip++;
  } return OK;
  case FAST_DUMPI: {
    auto a = this->stack[--this->stack_size].i64;
    printf("%i\n", (int)a);
    ip++;
  } return OK;
  case NOT:
    this->stack[this->stack_size - 1].i64 =
      !this->stack[this->stack_size - 1].i64;
    ip++;
    return OK;
  case CMPI_FG: {
    auto cmp_flag  = inst.op_a.i64;     
    auto b = this->stack[--this->stack_size];
    auto a = this->stack[--this->stack_size];
    
#define TEST_CASE(__n__, __f__)						\
    case __n__:								\
      this->stack[this->stack_size++] = {.i64 = a.i64 __f__ b.i64};	\
      ip++;								\
      break;
    
    switch(cmp_flag){
      TEST_CASE(0, <);
      TEST_CASE(1, <=);
      TEST_CASE(2, ==);
      TEST_CASE(3, !=);
      TEST_CASE(4, >=);
      TEST_CASE(5, >);
    default:
      assert(false && "unreachable");
    }
    return OK;
  }
        
  case JMP:
    assert(inst.op_a.i64 > 0);
    assert(inst.op_a.i64 < (int)this->program.size());
    ip = (Inst_Addr) inst.op_a.i64;
    return OK;
  case JMP_IF: {
    assert(inst.op_a.i64 > 0);
    assert(inst.op_a.i64 < (int)this->program.size());
    auto      flag  = this->stack[--this->stack_size].i64;
    Inst_Addr iaddr = (Inst_Addr) inst.op_a.i64;
    
    ip++;

    if(flag != 0){
      ip = iaddr;
    }

  } return OK;
  case WRITE64: {
    // addr data
    Word addr = this->stack[this->stack_size - 2];
    Word data = this->stack[this->stack_size - 1];
    *(int64_t*)addr.ptr = (*(int64_t*)&data);
    this->stack_size -= 2;
    ip++;
    return OK;      
  }      
  case HALT: 
    this->halt = true;
    return OK;     
  default:
    assert(false && "unreachable");
  }
}
void VM::push0(InstKind ik){
  this->program.push_back({
      ik, {0}, {0}, {0}
    });
}
void VM::push1(InstKind ik, Word x1){
  this->program.push_back({
      ik, x1, {0}, {0}
    });
}  
void VM::push2(InstKind ik, Word x1, Word x2){
  this->program.push_back({
      ik, x1, x2, {0}
    });
}  
void VM::push3(InstKind ik, Word x1, Word x2, Word x3){
  this->program.push_back({
      ik, x1, x2, x3
    });
}  
  
void VM::print(){
  for(size_t i=0; i < this->program.size(); i++){
    printf("[%zu]: ", i);
    Inst_print(this->program[i]);
    printf("\n");
  }
}

void VM::exec(){
  ip = 0;
  while(!this->halt){
    Err err = this->next_inst();
    if(err != Err::OK){
      printf("ERROR: TODO HUMAN READABLE ERRORS.\n");
      exit(1);
    }   
  }
}


#endif  /*__RUNNER__*/
