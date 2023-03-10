#define ARITH_OP 0x80

enum Instr_Op
{
  MOV,

  // arith
  ADD = ARITH_OP | 0b000,
  SUB = ARITH_OP | 0b101,
  CMP = ARITH_OP | 0b111,

};

struct Instr
{
  enum Instr_Op op;
  struct Operand dest;
  struct Operand src;
};
void instr_print(struct Instr instr, FILE* file);
const char* instr_op_str(enum Instr_Op op);