static enum Reg reg_decode(bool W, uint8_t REG)
{
  ASSERT(REG < 8);
  ASSERT((uint8_t)W < 2);
  return ((uint8_t)W << 3) | REG;
}

static const char* reg_to_str(enum Reg reg)
{
#define CASE(X) case X: return #X
  switch(reg)
  {
    CASE(AL);
    CASE(CL);
    CASE(DL);
    CASE(BL);
    CASE(AH);
    CASE(CH);
    CASE(DH);
    CASE(BH);
    CASE(AX);
    CASE(CX);
    CASE(DX);
    CASE(BX);
    CASE(SP);
    CASE(BP);
    CASE(SI);
    CASE(DI);
  }
#undef CASE
  abort();
}

static const char* addr_expr_to_str(enum Addr_Expr addr_expr)
{
  static const char* const addr_TABLE[] = {
    "BX + SI", "BX + DI", "BP + SI", "BP + DI",
    "SI", "DI", "BP", "BX",
  };

  ASSERT(0 <= addr_expr && addr_expr < 8);

  return addr_TABLE[addr_expr];
}

#define MAX_LINE_LEN 256
const char* fmt_operand(struct Operand op)
{
  ASSERT(op.variant < OPERAND_COUNT);

  static char RING_BUFFER[MAX_LINE_LEN*4] = {};
  static int START = 0;

  if(START > ARRAY_LEN(RING_BUFFER) - MAX_LINE_LEN)
    START = 0;

  char* text = RING_BUFFER + START;
  
  switch(op.variant)
  {
  case OPERAND_REG:
    return reg_to_str(op.reg);
  case OPERAND_ADDR_DIRECT:
  case OPERAND_ADDR_EXPR: UNIMPLEMENTED();
  case OPERAND_ADDR_EXPR_WITH_DISPLACEMENT_8: UNIMPLEMENTED();
  case OPERAND_ADDR_EXPR_WITH_DISPLACEMENT_16: UNIMPLEMENTED();
  case OPERAND_IMMEDIATE_8: sprintf(text, "%" PRIu8, op.value.lo); return text;
  case OPERAND_IMMEDIATE_16: sprintf(text, "%" PRIu16, op.value.wide); return text;
  case OPERAND_COUNT: abort();
  }

  abort();
}

void op_swap(struct Operand* x, struct Operand* y)
{
  struct Operand tmp = *x;
  *x = *y;
  *y = tmp;
}

struct Operand op_reg(bool W, uint8_t REG)
{
  return (struct Operand){
    .variant = OPERAND_REG,
    .reg = reg_decode(W, REG),
  };
}

struct Operand op_im(bool W, union Payload payload)
{
  return (struct Operand){
    .variant = W ? OPERAND_IMMEDIATE_16 : OPERAND_IMMEDIATE_8,
    .value = payload,
  };
}