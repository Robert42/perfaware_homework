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
  JNZ = JMP_OP | 0b0101,
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