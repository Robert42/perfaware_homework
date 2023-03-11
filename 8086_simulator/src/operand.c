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
  UNREACHABLE();
}

enum Seg_Reg seg_reg_decode(uint8_t REG)
{
  ASSERT(REG < 4);
  ASSERT(REG == 1 || REG == 3, "%" PRIu8, REG);
  return REG;
}

const char* seg_reg_to_str(enum Seg_Reg reg)
{
  switch(reg)
  {
    CASE(CS);
    CASE(DS);
  }
  UNREACHABLE();
}
#undef CASE

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
  case OPERAND_REG: return reg_to_str(op.reg);
  case OPERAND_SEG_REG: return seg_reg_to_str(op.reg);
  case OPERAND_ADDR_DIRECT: sprintf(text, "[%" PRIu16 "]", op.payload.wide); goto done;
  case OPERAND_ADDR_EXPR: sprintf(text, "[%s]", addr_expr_to_str(op.addr_expr)); goto done;
  case OPERAND_ADDR_EXPR_WITH_DISPLACEMENT: sprintf(text, "[%s + %i]", addr_expr_to_str(op.addr_expr), (int)(int16_t)op.payload.wide); goto done;
  case OPERAND_IMMEDIATE_8: sprintf(text, "byte %" PRIu8, op.payload.lo); goto done;
  case OPERAND_IMMEDIATE_16: sprintf(text, "word %" PRIu16, op.payload.wide); goto done;
  case OPERAND_DATA_8: sprintf(text, "%" PRIu8, op.payload.lo); goto done;
  case OPERAND_COUNT: UNREACHABLE();
  }

  UNREACHABLE();

done:
  START += strlen(text)+1;
  ASSERT(START < ARRAY_LEN(RING_BUFFER));
  return text;
}

void op_swap(struct Operand* x, struct Operand* y)
{
  struct Operand tmp = *x;
  *x = *y;
  *y = tmp;
}

struct Operand op_seg_reg(uint8_t REG)
{
  return (struct Operand){
    .variant = OPERAND_SEG_REG,
    .reg = seg_reg_decode(REG),
  };
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
    .payload = payload,
  };
}

struct Operand op_data8(uint8_t payload)
{
  return (struct Operand){
    .variant = OPERAND_DATA_8,
    .payload = {.lo = payload},
  };
}

struct Operand op_addr_direct(uint16_t addr)
{
  return (struct Operand){
    .variant = OPERAND_ADDR_DIRECT,
    .payload.wide = addr,
  };
}

struct Operand op_addr_expr(enum Addr_Expr addr_expr)
{
  return (struct Operand){
    .variant = OPERAND_ADDR_EXPR,
    .addr_expr = addr_expr,
  };
}

struct Operand op_addr_expr_with_displacement(enum Addr_Expr addr_expr, uint16_t displacement)
{
  return (struct Operand){
    .variant = OPERAND_ADDR_EXPR_WITH_DISPLACEMENT,
    .addr_expr = addr_expr,
    .payload.wide = displacement,
  };
}