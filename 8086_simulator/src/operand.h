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
  struct
  {
    uint8_t lo;
    uint8_t hi;
  };
  uint16_t wide;
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
  union Payload payload;
};
const char* fmt_operand(struct Operand op);

void op_swap(struct Operand* x, struct Operand* y);
struct Operand op_reg(bool W, uint8_t REG);
struct Operand op_im(bool W, union Payload payload);
struct Operand op_addr_direct(uint16_t addr);
struct Operand op_addr_expr(enum Addr_Expr addr_expr);
struct Operand op_addr_expr_with_displacement(enum Addr_Expr addr_expr, bool W, union Payload displacement);