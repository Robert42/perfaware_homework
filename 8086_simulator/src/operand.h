enum Reg
{
  AL, CL, DL, BL, AH, CH, DH, BH,
  AX, CX, DX, BX, SP, BP, SI, DI,
};
static enum Reg reg_decode(bool W, uint8_t REG);
static const char* reg_to_str(enum Reg reg);

enum Seg_Reg
{
  CS = 1,
  DS = 3,
};
static enum Seg_Reg seg_reg_decode(uint8_t REG);
static const char* seg_reg_to_str(enum Seg_Reg reg);

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
  OPERAND_SEG_REG,
  OPERAND_ADDR_DIRECT,
  OPERAND_ADDR_EXPR,
  OPERAND_ADDR_EXPR_WITH_DISPLACEMENT,
  OPERAND_DATA_8,
  OPERAND_DATA_16,
  
  OPERAND_COUNT,
};

enum Operand_Expl_Size
{
  OP_EXPL_SIZE_NONE,
  OP_EXPL_SIZE_U8,
  OP_EXPL_SIZE_U16,
};

struct Operand
{
  enum Operand_Variant variant : 6;
  enum Operand_Expl_Size expl_size : 2;
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
struct Operand op_seg_reg(uint8_t REG);
struct Operand op_im(bool W, union Payload payload);
struct Operand op_data8(uint8_t payload);
struct Operand op_addr_direct(uint16_t addr);
struct Operand op_addr_expr(enum Addr_Expr addr_expr);
struct Operand op_addr_expr_with_displacement(enum Addr_Expr addr_expr, uint16_t displacement);