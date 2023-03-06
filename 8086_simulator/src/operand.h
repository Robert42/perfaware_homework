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
  OPERAND_ADDR_EXPR,
};
struct Operand
{
  enum Operand_Variant variant : 1;
  union
  {
    enum Reg reg : 4;
    enum Addr_Expr addr_expr : 3;
  };
  union Payload addr_displacement;
  union Payload value;
};