enum Instr_Op
{
  // arith
  ADD = 0b000,

  MOV = 0x80,
};

struct Instr
{
  enum Instr_Op op;
  struct Operand dest;
  struct Operand src;
};
void instr_print(struct Instr instr, FILE* file);
const char* instr_op_str(enum Instr_Op op);