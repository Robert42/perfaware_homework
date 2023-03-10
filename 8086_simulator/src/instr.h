#define ARITH 0x80

enum Instr_Op
{
  MOV,

  // arith
  ADD = ARITH | 0b000,

};

#undef ARITH

struct Instr
{
  enum Instr_Op op;
  struct Operand dest;
  struct Operand src;
};
void instr_print(struct Instr instr, FILE* file);
const char* instr_op_str(enum Instr_Op op);