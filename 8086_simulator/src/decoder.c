struct Decoder;

// Using octals because of this article https://gist.github.com/seanjensengrey/f971c20d05d4d0efc0781f2f3c0353da suggested by x13pixels [in the comments of the course](https://www.computerenhance.com/p/instruction-decoding-on-the-8086/comment/13235714)
enum Op_Code
{
  OP_MOV_RM_R  = 0210, // 0b10_001_0xx - Register/memory to/from register
  OP_MOV_IM_RM = 0306, // 0b11_000_11x - Immediate to register/memory
  OP_MOV_IM_R  = 0260, // 0b10_11x_xxx - Immediate to register
};
static_assert(sizeof(enum Op_Code) == 1);

enum Op_Code op(struct Decoder);

enum Mod_Encoding
{
  MOD_MEMORY_NO_DISPLACEMENT = 0, // except if R/M==6
  MOD_MEMORY_8BIT_DISPLACEMENT = 1, // 
  MOD_MEMORY_16BIT_DISPLACEMENT = 2, // 
  MOD_REGISTER = 3, // 0b11 // Register mode (no displacement)
};
static_assert(sizeof(enum Mod_Encoding) == 1);

struct Decoder
{
  union
  {
    struct
    {
      uint8_t bytes[6];
    };
    struct __attribute__((packed)) Mov_Rm_R
    {
      // == byte 0 == 
      // 0: byte operation
      // 1: (16bit) word operation
      bool W : 1;

      // 0: src is specified in REG field.
      // 1: dest is specified in REG field.
      bool D : 1;

      uint8_t _opcode : 6; // OP_MOV_RM_R

      // == byte 1 ==
      uint8_t R_M : 3;
      uint8_t REG : 3;
      uint8_t MOD : 2;
      
      union Payload displacement;
    } mov_rm_r;
    struct __attribute__((packed)) Mov_Im_Rm
    {
      bool W : 1;
      uint8_t _opcode : 7; // OP_MOV_IM_RM

      uint8_t R_M : 3;
      uint8_t _padding : 3;
      uint8_t MOD : 2;

      union Payload p1;
      union Payload p2;
    } mov_im_rm;
    struct __attribute__((packed)) Mov_Im_R
    {
      uint8_t REG : 3;
      bool W : 1;
      uint8_t _opcode : 4; // OP_MOV_IM_R

      // == byte 1 ==
      union Payload data;
    } mov_im_r;
  };
};
static_assert(sizeof(struct Mov_Rm_R) == 4);
static_assert(sizeof(struct Mov_Im_Rm) == 6);
static_assert(sizeof(struct Mov_Im_R) == 3);

#define MAX_INSTR_LEN sizeof(struct Decoder)

static int instr_size(struct Decoder instr);
static struct Instr to_instr(struct Decoder instr);

struct Instr instr_decode(const uint8_t* instr_stream, int* index, int num_bytes)
{
  ASSERT(*index + 1 <= num_bytes, "no more instructions to decode!");

  if(LOG)
    fprintf(stderr, "%4i | %02X ", *index, instr_stream[0]);

  // ==== read instr ====
  struct Decoder instr = {};
  memcpy(&instr, instr_stream + *index, MAX_INSTR_LEN); // I've made the buffer is MAX_INSTR_LEN bytes bigger than necessary, so I can always copy MAX_INSTR_LEN bytes without further checks

  // ==== increment the index ====
  {
    const int size = instr_size(instr);

    if(LOG)
    {
      for(int i=1; i<size; ++i)
        fprintf(stderr, "%02X ", instr_stream[*index+i]);
      fprintf(stderr, "\n");
    }

    ASSERT(*index + size <= num_bytes, "invalid instr stream: last instr incomplete!");
    *index += size;
  }

  return to_instr(instr);
}

struct Operand op_addr(enum Mod_Encoding mod, uint8_t R_M, union Payload displacement)
{
  ASSERT(mod != MOD_REGISTER);

  switch(mod)
  {
  case MOD_MEMORY_NO_DISPLACEMENT:
    if(R_M==6)
      return op_addr_direct(displacement.wide);
    else
      return op_addr_expr(R_M);
  case MOD_MEMORY_8BIT_DISPLACEMENT:
    return op_addr_expr_with_displacement(R_M, false, displacement);
  case MOD_MEMORY_16BIT_DISPLACEMENT:
    return op_addr_expr_with_displacement(R_M, true, displacement);
  case MOD_REGISTER:
    abort();
  }

  abort();
}

static struct Instr to_instr(struct Decoder instr)
{
  switch(op(instr))
  {
  case OP_MOV_RM_R:
  {
    const struct Mov_Rm_R mov = instr.mov_rm_r;
    switch(mov.MOD)
    {
    case MOD_MEMORY_NO_DISPLACEMENT:
    case MOD_MEMORY_8BIT_DISPLACEMENT:
    case MOD_MEMORY_16BIT_DISPLACEMENT:
    {
      struct Operand dest = op_addr(mov.MOD, mov.R_M, mov.displacement);
      struct Operand src = op_reg(mov.W, mov.REG);
      if(mov.D)
        op_swap(&dest, &src);
      return (struct Instr){.op=MOV, .dest=dest, .src=src};
    }
    case MOD_REGISTER:
    {
      struct Operand dest = op_reg(mov.W, mov.R_M);
      struct Operand src = op_reg(mov.W, mov.REG);
      if(mov.D)
        op_swap(&dest, &src);
      return (struct Instr){.op=MOV, .dest=dest, .src=src};
    }
    }
    abort();
  }
  case OP_MOV_IM_RM:
  {
    const struct Mov_Im_Rm mov = instr.mov_im_rm;
    switch(mov.MOD)
    {
    case MOD_MEMORY_NO_DISPLACEMENT:
    case MOD_MEMORY_8BIT_DISPLACEMENT:
    case MOD_MEMORY_16BIT_DISPLACEMENT:
    {
      UNIMPLEMENTED();
    }
    case MOD_REGISTER:
      UNIMPLEMENTED();
    }
    abort();
  }
  case OP_MOV_IM_R:
  {
    const struct Mov_Im_R mov = instr.mov_im_r;
    const struct Operand dest = op_reg(mov.W, mov.REG);
    const struct Operand src = op_im(mov.W, mov.data);
    return (struct Instr){.op=MOV, .dest=dest, .src=src};
  }
  }
  abort();
}

static int instr_size(struct Decoder instr)
{
  switch(op(instr))
  {
  case OP_MOV_RM_R:
    switch(instr.mov_rm_r.MOD)
    {
    case MOD_MEMORY_NO_DISPLACEMENT: return 2 + 2*(instr.mov_rm_r.R_M==6);
    case MOD_MEMORY_8BIT_DISPLACEMENT: return 3;
    case MOD_MEMORY_16BIT_DISPLACEMENT: return 4;
    case MOD_REGISTER: return 2;
    }
    abort();
  case OP_MOV_IM_RM: return 4 + 2*(instr.mov_im_rm.R_M==6);
  case OP_MOV_IM_R: return instr.mov_im_r.W ? 3 : 2;
  }
  ASSERT(false, "Unknown opcode!");
}

enum Op_Code op(struct Decoder instr)
{
#define CASE(X) case X: return X
  switch(instr.bytes[0] & 0xfe)
  {
  CASE(OP_MOV_IM_RM);
  }

  switch(instr.bytes[0] & 0xfc)
  {
  CASE(OP_MOV_RM_R);
  }

  switch(instr.bytes[0] & 0xf0)
  {
  CASE(OP_MOV_IM_R);
  }

  ASSERT(false, "Could not decode opcode from byte: 0o%03o  0x%02X", (uint32_t)instr.bytes[0], (uint32_t)instr.bytes[0]);
#undef CASE
}