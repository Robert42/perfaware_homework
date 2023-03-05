struct Instr;

// Using octals because of this article https://gist.github.com/seanjensengrey/f971c20d05d4d0efc0781f2f3c0353da suggested by x13pixels [in the comments of the course](https://www.computerenhance.com/p/instruction-decoding-on-the-8086/comment/13235714)
enum Op_Code
{
  OP_MOV_RM_R  = 0210, // 0b10_001_0xx - Register/memory to/from register
  OP_MOV_IM_RM = 0306, // 0b11_000_11x - Immediate to register/memory
  OP_MOV_IM_R  = 0260, // 0b10_11x_xxx - Immediate to register
};
static_assert(sizeof(enum Op_Code) == 1);

enum Op_Code op(struct Instr);

enum Reg
{
  AL, CL, DL, BL, AH, CH, DH, BH,
  AX, CX, DX, BX, SP, BP, SI, DI,
};
static enum Reg reg_decode(bool W, uint8_t REG);
static const char* reg_to_str(enum Reg reg);

enum Mod_Encoding
{
  MOD_REGISTER = 3, // 0b11 // Register mode (no displacement)
};
static_assert(sizeof(enum Mod_Encoding) == 1);

struct Instr
{
  union
  {
    struct
    {
      uint8_t bytes[2];
    };
    struct Mov_Rm_R
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
    } mov_rm_r;
  };
};
static_assert(sizeof(struct Instr) == 2);

static int instr_size(struct Instr instr);

struct Instr instr_decode(const uint8_t* instr_stream, int* index, int num_bytes)
{
  ASSERT(*index + 1 <= num_bytes, "no more instructions to decode!");

  // ==== read instr ====
  struct Instr instr = {};
  memcpy(&instr, instr_stream + *index, sizeof(struct Instr)); // I've made the buffer sizeof(struct Instr) bytes bigger tan necessary, so I can always copy sizeof(struct Instr) bytes without further checks

  // ==== increment the index ====
  {
    const int size = instr_size(instr);
    ASSERT(*index + size <= num_bytes, "invalid instr stream: last instr incomplete!");
    *index += size;
  }

  return instr;
}

void instr_print(struct Instr instr)
{
  switch(op(instr))
  {
  case OP_MOV_RM_R:
  {
    const struct Mov_Rm_R mov = instr.mov_rm_r;
    ASSERT(mov.MOD == MOD_REGISTER, "Unsupported Mode: 0x%02X", (uint32_t)mov.MOD);
    switch(mov.MOD)
    {
    case MOD_REGISTER:
    {
      enum Reg dest, src;
      if(mov.D)
      {
        dest = reg_decode(mov.W, mov.REG);
        src = reg_decode(mov.W, mov.R_M);
      }else
      {
        dest = reg_decode(mov.W, mov.R_M);
        src = reg_decode(mov.W, mov.REG);
      }
      printf("mov %s, %s\n", reg_to_str(dest), reg_to_str(src));
      return;
    }
    }
    abort();
  }
  case OP_MOV_IM_RM: ASSERT(false, "UNIMPLEMENTED!");
  case OP_MOV_IM_R: ASSERT(false, "UNIMPLEMENTED!");
  }
  abort();
}

static int instr_size(struct Instr instr)
{
  switch(op(instr))
  {
  case OP_MOV_RM_R: return 2;
  case OP_MOV_IM_RM: ASSERT(false, "UNIMPLEMENTED!");
  case OP_MOV_IM_R: ASSERT(false, "UNIMPLEMENTED!");
  }
  ASSERT(false, "Unknown opcode!");
}

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

enum Op_Code op(struct Instr instr)
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

  ASSERT(false, "Could not decode opcode from byte: 0o%03o", (uint32_t)instr.bytes[0]);
#undef CASE
}