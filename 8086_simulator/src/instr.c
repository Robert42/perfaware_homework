void instr_print(struct Instr instr, FILE* file, const uint16_t* labels, size_t curr_pos)
{
  switch(instr.op)
  {
  case PUSH:
  case POP:
    // printing word directly feels like cheating
    fprintf(file, "%s word %s\n", instr_op_str(instr.op), fmt_operand(instr.src));
    return;
  case MOV:
  case XCHG:
  case IN:
  case OUT:
  case ADD:
  case SUB:
  case CMP:
    fprintf(file, "%s %s, %s\n", instr_op_str(instr.op), fmt_operand(instr.dest), fmt_operand(instr.src));
    return;
  case JMP_OP ... JMP_OP | 0b1111:
  case LOOP_OP ... LOOP_OP | 0b0011:
    if(labels)
    {
      ASSERT(labels[curr_pos+instr.ip_incr] > 0);
      fprintf(file, "%s label%" PRIu16 "\n", instr_op_str(instr.op), labels[curr_pos+instr.ip_incr]-1);
    }else
    {
      fprintf(file, "%s %" PRIi8 "\n", instr_op_str(instr.op), instr.ip_incr);
    }
    return;
  }

  UNREACHABLE("instr.op: 0x%02" PRIx8 "\n", instr.op);
}

const char* instr_op_str(enum Instr_Op op)
{
  switch(op)
  {
  case MOV: return "mov";
  
  case PUSH: return "push";
  case POP: return "pop";
  
  case XCHG: return "xchg";
  
  case IN: return "in";
  case OUT: return "out";

  case ADD: return "add";
  case SUB: return "sub";
  case CMP: return "cmp";

  case JZ: return "jz";
  case JL: return "jl";
  case JLE: return "jle";
  case JB: return "jb";
  case JBE: return "jbe";
  case JPE: return "jpe";
  case JO: return "jo";
  case JS: return "js";
  case JNZ: return "jnz";
  case JGE: return "jge";
  case JG: return "jg";
  case JAE: return "jae";
  case JA: return "ja";
  case JPO: return "jpo";
  case JNO: return "jno";
  case JNS: return "jns";
  case LOOP: return "loop";
  case LOOPZ: return "loopz";
  case LOOPNZ: return "loopnz";
  case JCXZ: return "jcxz";
  }

  UNREACHABLE();
}

bool has_label(enum Instr_Op op)
{
  switch(op)
  {
  case JMP_OP ... JMP_OP | 0b1111:
  case LOOP_OP ... LOOP_OP | 0b0011:
    return true;
  default:
    return false;
  }
}