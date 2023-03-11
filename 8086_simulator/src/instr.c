void instr_print(struct Instr instr, FILE* file, const uint16_t* labels, size_t curr_pos)
{
  switch(instr.op)
  {
  case SEGMENT_OVERRIDE_PREFIX:
    fprintf(file, "%s:\n", fmt_operand(instr.src));
    return;
  case XLAT:
  case LAHF:
  case SAHF:
  case PUSHF:
  case POPF:
  case AAA:
  case DAA:
  case AAS:
  case DAS:
  case AAM:
  case AAD:
  case CBW:
  case CWD:
  case INTO:
  case IRET:
  case CLC:
  case CMC:
  case STC:
  case CLD:
  case STD:
  case CLI:
  case STI:
  case HLT:
  case WAIT:
  case LOCK:
    fprintf(file, "%s\n", instr_op_str(instr.op));
    return;
  case PUSH:
  case POP:
    // printing word directly feels like cheating
    fprintf(file, "%s word %s\n", instr_op_str(instr.op), fmt_operand(instr.src));
    return;
  case REP:
  case CALL:
  case JMP:
  case RET:
  case INT:
    fprintf(file, "%s %s\n", instr_op_str(instr.op), fmt_operand(instr.src));
    return;
  case MOV:
  case XCHG:
  case IN:
  case OUT:
  case ARITH_OP ... ARITH_OP|0b111:
  case LEA:
  case LES:
  case LDS:
  case SHL:
  case SHR:
  case SAR:
  case ROL:
  case ROR:
  case RCL:
  case RCR:
  case TEST:
    fprintf(file, "%s %s, %s\n", instr_op_str(instr.op), fmt_operand(instr.dest), fmt_operand(instr.src));
    return;
  case INC:
  case DEC:
  case NEG:
  case NOT:
  case MUL:
  case IMUL:
  case DIV:
  case IDIV:
    fprintf(file, "%s %s\n", instr_op_str(instr.op), fmt_operand(instr.src));
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
  
  case XLAT: return "xlat";
  case TEST: return "test";
  
  case LEA: return "lea";
  case LES: return "les";
  case LDS: return "lds";

  case LAHF: return "lahf";
  case SAHF: return "sahf";
  case PUSHF: return "pushf";
  case POPF: return "popf";
  
  case CBW: return "cbw";
  case CWD: return "cwd";

  case INC: return "inc";
  case DEC: return "dec";
  case NEG: return "neg";
  case NOT: return "not";
  
  case AAA: return "aaa";
  case DAA: return "daa";
  case AAS: return "aas";
  case DAS: return "das";
  case AAM: return "aam";
  case AAD: return "aad";
  
  case REP: return "rep";
  case CALL: return "call";
  case JMP: return "jmp";
  case RET: return "ret";
  case INT: return "int";
  
  case INTO: return "into";
  case IRET: return "iret";

  case CLC: return "clc";
  case CMC: return "cmc";
  case STC: return "stc";
  case CLD: return "cld";
  case STD: return "std";
  case CLI: return "cli";
  case STI: return "sti";
  case HLT: return "hlt";
  case WAIT: return "wait";

  case ADD: return "add";
  case ADC: return "adc";
  case SUB: return "sub";
  case SBB: return "sbb";
  case CMP: return "cmp";
  case MUL: return "mul";
  case IMUL: return "imul";
  case DIV: return "div";
  case IDIV: return "idiv";
  case AND: return "and";
  case OR: return "or";
  case XOR: return "xor";

  case SHL: return "shl";
  case SHR: return "shr";
  case SAR: return "sar";
  case ROL: return "rol";
  case ROR: return "ror";
  case RCL: return "rcl";
  case RCR: return "rcr";

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
  
  case LOCK: return "lock";
  case SEGMENT_OVERRIDE_PREFIX: UNREACHABLE();
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