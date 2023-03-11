#define ARITH_OP 0xD0
#define JMP_OP 0xE0
#define LOOP_OP 0xF0

enum Instr_Op
{
  MOV,
  PUSH,
  POP,
  XCHG,
  IN, // read from port
  OUT, // write to port
  XLAT,
  TEST,
  
  LEA, // https://stackoverflow.com/a/1665570/2301866, basically mov is `x[i+offsetof(y)]` and lea is `&x[i+offsetof(y)]`
  LDS,
  LES,

  LAHF,
  SAHF,
  PUSHF,
  POPF,
  
  CBW,
  CWD,

  INC,
  DEC,
  NEG,
  MUL,
  IMUL,
  DIV,
  IDIV,
  NOT,

  SHL,
  SHR,
  SAR,
  ROL,
  ROR,
  RCL,
  RCR,
  
  // https://www.pcjs.org/documents/manuals/intel/8086/ops/AAA/
  // https://en.wikipedia.org/wiki/Binary-coded_decimal
  AAA,
  DAA,
  AAS,
  DAS,
  AAM,
  AAD,

  // arith
  ADD = ARITH_OP | 0b000,
  ADC = ARITH_OP | 0b010, // add with carry
  SUB = ARITH_OP | 0b101,
  CMP = ARITH_OP | 0b111,
  SBB = ARITH_OP | 0b011,
  AND = ARITH_OP | 0b100,
  OR  = ARITH_OP | 0b001,
  XOR = ARITH_OP | 0b110,

  // jump
  JZ  = JMP_OP | 0b0100,
  JL  = JMP_OP | 0b1100,
  JLE = JMP_OP | 0b1110,
  JB  = JMP_OP | 0b0010,
  JBE = JMP_OP | 0b0110,
  JPE = JMP_OP | 0b1010,
  JO  = JMP_OP | 0b0000,
  JS  = JMP_OP | 0b1000,
  JNZ = JMP_OP | 0b0101,
  JGE = JMP_OP | 0b1101,
  JG  = JMP_OP | 0b1111,
  JAE = JMP_OP | 0b0011,
  JA  = JMP_OP | 0b0111,
  JPO = JMP_OP | 0b1011,
  JNO = JMP_OP | 0b0001,
  JNS = JMP_OP | 0b1001,

  LOOP   = LOOP_OP | 2,
  LOOPZ  = LOOP_OP | 1,
  LOOPNZ = LOOP_OP | 0,
  JCXZ   = LOOP_OP | 3,
};

struct Instr
{
  enum Instr_Op op;
  union
  {
    struct{
      struct Operand dest;
      struct Operand src;
    };
    int8_t ip_incr;
  };
};
void instr_print(struct Instr instr, FILE* file, const uint16_t* labels, size_t curr_pos);
const char* instr_op_str(enum Instr_Op op);
bool has_label(enum Instr_Op op);