#define ARITH_OP 0x10
#define JMP_OP 0x20

enum Instr_Op
{
  MOV,

  // arith
  ADD = ARITH_OP | 0b000,
  SUB = ARITH_OP | 0b101,
  CMP = ARITH_OP | 0b111,

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
bool is_jmp(enum Instr_Op op);