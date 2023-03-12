#ifndef  __INSTRUCTIONS_CPP__
#define  __INSTRUCTIONS_CPP__
#include <vector>
#include <iostream>
#include <cassert>

typedef struct Inst       Inst;
typedef int               Inst_Addr;
typedef int               Operand;
typedef int               Memory;
typedef int               Memory_Addr;
typedef bool              Flag;
typedef std::vector<Inst> InstList;


enum InstKind{
  // any type inst
  POP,
  PUSH,
  HALT,
  JMP,
  WRITE64,
  STORE64,
  NOT, 
  // i64 inst
  FAST_ADDI,
  FAST_SUBI,
  FAST_MULI,
  FAST_DIVI,
  FAST_DUMPI,
  JMP_IF,
  CMPI_FG,
  // ptr inst
  LOAD_PTR,
  // multiple args
  FAST_IO_WRITE // @stack: stream ptr
};
const char* Human_InstKind(InstKind k){
  switch(k){
    // NOTE:
    // -- return string must be human more readable than the CODE itself
  case HALT:            return "halt";
  case PUSH:		return "push";
  case POP:		return "pop";
  case FAST_ADDI:	return "fast_add_i64";
  case FAST_SUBI:	return "fast_sub_i64";
  case FAST_MULI:	return "fast_mul_i64";
  case FAST_DIVI:	return "fast_div_i64";
  case FAST_DUMPI:	return "fast_dump_i64";
  case FAST_IO_WRITE:   return "fast_io_write";
  case JMP:             return "jump";
  case WRITE64:		return "write_64bytes";
  case STORE64:		return "store_64bytes";
  case LOAD_PTR:        return "load_pointer";
  case NOT:             return "not";
  case JMP_IF:          return "conditional_jump";
  case CMPI_FG:         return "cmp_int_with_flag";
  default:
    assert(false && "unreachable");
  }
}

union Word{
  int64_t i64;
  void*   ptr;
};

inline Word Word_i64(int x){
  return {
    .i64 = (int64_t)x
  };
}

#define Arg_fmt "(Word = [%li, %p])"
#define Arg_word(w) (w).i64, (w).ptr
struct Inst{
  InstKind kind;
  Word  op_a;
  Word  op_b;
  Word  op_c;  
};
void Inst_print(Inst& inst){
  printf("[%s, " Arg_fmt Arg_fmt Arg_fmt "]",
	 Human_InstKind(inst.kind),
	 Arg_word(inst.op_a),
	 Arg_word(inst.op_b),
	 Arg_word(inst.op_c));

}

#endif /*__INSTRUCTIONS_CPP__*/
