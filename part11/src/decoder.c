enum Op_Code
{
  OP_MOV = 042 // 0b100010
};
static_assert(sizeof(enum Op_Code) == 1);

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
    struct
    {
      // == byte 0 == 
      // 0: byte operation
      // 1: (16bit) word operation
      bool W : 1;

      // 0: src is specified in REG field.
      // 1: dest is specified in REG field.
      bool D : 1;
      
      enum Op_Code opcode : 6;

      // == byte 1 ==
      uint8_t R_M : 3;
      uint8_t REG : 3;
      uint8_t MOD : 2;
    };
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

  // ==== compatibility checks ====
  ASSERT(instr.opcode == OP_MOV, "Unsupported obcode: 0x%02X (at index %i)", (uint32_t)instr.opcode, *index);
  ASSERT(instr.MOD == MOD_REGISTER, "Unsupported Mode: 0x%02X", (uint32_t)instr.MOD);

  return instr;
}

void reg_print(uint8_t reg, bool W)
{
  reg |= (uint8_t)W << 3;

  char x, y;

  if(reg < 12)
  {
    const char NAME[] = "acdb";
    const char PART[] = "lhx";

    x = NAME[reg%4];
    y = PART[reg / 4];
  }else
  {
    const char REG[] = "spbpsidi";
    const int i = 2*(reg-12);
    x = REG[i];
    y = REG[i+1];
  }

  printf("%c%c", x, y);
}

void instr_print(struct Instr instr)
{
  uint8_t dest, src;
  if(instr.D)
  {
    dest = instr.REG;
    src = instr.R_M;
  }else
  {
    dest = instr.R_M;
    src = instr.REG;
  }

  switch(instr.opcode)
  {
  case OP_MOV:
    printf("mov ");
    switch(instr.MOD)
    {
    case MOD_REGISTER:
      printf("%s, %s\n", reg_to_str(reg_decode(instr.W, dest)), reg_to_str(reg_decode(instr.W, src)));
      return;
    }
    abort();
  }
  abort();
}

static int instr_size(struct Instr instr)
{
  switch(instr.opcode)
  {
  case OP_MOV: return 2;
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
}