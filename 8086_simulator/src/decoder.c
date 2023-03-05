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
  MOD_MEMORY_NO_DISPLACEMENT = 0, // except if R/M==6
  MOD_MEMORY_8BIT_DISPLACEMENT = 1, // 
  MOD_MEMORY_16BIT_DISPLACEMENT = 2, // 
  MOD_REGISTER = 3, // 0b11 // Register mode (no displacement)
};
static_assert(sizeof(enum Mod_Encoding) == 1);

struct Instr
{
  union
  {
    struct
    {
      uint8_t bytes[3];
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
      
      union
      {
        uint8_t displacement_u8;
        uint16_t displacement_u16;
      };
    } mov_rm_r;
    struct __attribute__((packed)) Mov_Im_R
    {
      uint8_t REG : 3;
      bool W : 1;
      uint8_t _opcode : 4; // OP_MOV_IM_R

      // == byte 1 ==
      union
      {
        struct
        {
          uint8_t data_lo;
          uint8_t data_hi;
        };
        uint16_t data;
      };
    } mov_im_r;
  };
};
static_assert(sizeof(struct Mov_Rm_R) == 4);
static_assert(sizeof(struct Mov_Im_R) == 3);

static int instr_size(struct Instr instr);

struct Instr instr_decode(const uint8_t* instr_stream, int* index, int num_bytes)
{
  ASSERT(*index + 1 <= num_bytes, "no more instructions to decode!");

  if(LOG)
    fprintf(stderr, "%4i | %02X ", *index, instr_stream[0]);

  // ==== read instr ====
  struct Instr instr = {};
  memcpy(&instr, instr_stream + *index, sizeof(struct Instr)); // I've made the buffer sizeof(struct Instr) bytes bigger tan necessary, so I can always copy sizeof(struct Instr) bytes without further checks

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

  return instr;
}

void instr_print(struct Instr instr)
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
      char buf[256];
      static const char* addr_TABLE[] = {
        "BX + SI", "BX + DI", "BP + SI", "BP + DI",
        "SI", "DI", "BP", "BX",
      };

      const char* addr = addr_TABLE[instr.mov_rm_r.R_M];
      switch(mov.MOD)
      {
      case MOD_MEMORY_NO_DISPLACEMENT:
        if(instr.mov_rm_r.R_M==6)
          snprintf(buf, ARRAY_LEN(buf), "[%" PRIu16 "]", mov.displacement_u16);
        else
          snprintf(buf, ARRAY_LEN(buf), "[%s]", addr);
        break;
      case MOD_MEMORY_8BIT_DISPLACEMENT:
      case MOD_MEMORY_16BIT_DISPLACEMENT:
        uint32_t displacement = mov.MOD==MOD_MEMORY_16BIT_DISPLACEMENT ? mov.displacement_u16 : mov.displacement_u8;
        snprintf(buf, ARRAY_LEN(buf), "[%s + %u]", addr, displacement);
        break;
      }
      addr = buf;
      const char* reg = reg_to_str(reg_decode(mov.W, mov.REG));
      if(mov.D)
        printf("mov %s, %s\n", reg, addr);
      else
        printf("mov %s, %s\n", addr, reg);
      return;
    }
    case MOD_REGISTER:
    {
      const char* REG = reg_to_str(reg_decode(mov.W, mov.REG));
      const char* R_M = reg_to_str(reg_decode(mov.W, mov.R_M));
      if(mov.D)
        printf("mov %s, %s\n", REG, R_M);
      else
        printf("mov %s, %s\n", R_M, REG);
      return;
    }
    }
    abort();
  }
  case OP_MOV_IM_RM: UNIMPLEMENTED();
  case OP_MOV_IM_R:
  {
    const struct Mov_Im_R mov = instr.mov_im_r;
    const enum Reg reg = reg_decode(mov.W, mov.REG);
    const uint32_t data = mov.W ? mov.data : mov.data_lo;
    printf("mov %s, %u\n", reg_to_str(reg), (uint32_t)data);
    return;
  }
  }
  abort();
}

static int instr_size(struct Instr instr)
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
  case OP_MOV_IM_RM: UNIMPLEMENTED();
  case OP_MOV_IM_R: return instr.mov_im_r.W ? 3 : 2;
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

  ASSERT(false, "Could not decode opcode from byte: 0o%03o  0x%02X", (uint32_t)instr.bytes[0], (uint32_t)instr.bytes[0]);
#undef CASE
}