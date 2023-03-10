void instr_print(struct Instr instr, FILE* file)
{
  switch(instr.op)
  {
  case MOV:
    fprintf(file, "mov %s, %s\n", fmt_operand(instr.dest), fmt_operand(instr.src));
    return;
  case ADD:
    fprintf(file, "add %s, %s\n", fmt_operand(instr.dest), fmt_operand(instr.src));
    return;
  }

  abort();
}