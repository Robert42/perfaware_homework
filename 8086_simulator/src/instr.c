void instr_print(struct Instr instr, FILE* file)
{
  switch(instr.op)
  {
  case MOV:
  case ADD:
  case SUB:
    fprintf(file, "%s %s, %s\n", instr_op_str(instr.op), fmt_operand(instr.dest), fmt_operand(instr.src));
    return;
  }

  abort();
}

const char* instr_op_str(enum Instr_Op op)
{
  switch(op)
  {
  case MOV: return "mov";
  case ADD: return "add";
  case SUB: return "sub";
  }

  abort();
}