enum Reg
{
  AL, CL, DL, BL, AH, CH, DH, BH,
  AX, CX, DX, BX, SP, BP, SI, DI,
};
static enum Reg reg_decode(bool W, uint8_t REG);
static const char* reg_to_str(enum Reg reg);

enum Addr_Expr
{
  ADDREXPR_BX_plus_SI, ADDREXPR_BX_plus_DI, ADDREXPR_BP_plus_SI, ADDREXPR_BP_plus_DI,
  ADDREXPR_SI, ADDREXPR_DI, ADDREXPR_BP, ADDREXPR_BX,
};
static const char* addr_expr_to_str(enum Addr_Expr addr_expr);

union Payload
{
  uint8_t u8;
  uint16_t u16;
};

enum Operand_Variant
{
  OPERAND_REG,
  OPERAND_ADDR_DIRECT,
  OPERAND_ADDR_EXPR,
  OPERAND_ADDR_EXPR_WITH_DISPLACEMENT_8,
  OPERAND_ADDR_EXPR_WITH_DISPLACEMENT_16,
  OPERAND_IMMEDIATE_8,
  OPERAND_IMMEDIATE_16,
  
  OPERAND_COUNT,
};
struct Operand
{
  enum Operand_Variant variant;
  union
  {
    enum Reg reg;
    enum Addr_Expr addr_expr;
  };
  union Payload value;
  union Payload addr_displacement;
};
const char* fmt_operand(struct Operand op);

void op_swap(struct Operand* x, struct Operand* y);
struct Operand op_reg(bool W, uint8_t REG);