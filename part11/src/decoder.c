enum Op_Code
{
  OP_MOV = 0x22 // 0b100010
};
static_assert(sizeof(enum Op_Code) == 1);

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

struct Instr instr_decode(const uint8_t* instr_stream, int* index, int num_bytes)
{
  struct Instr instr = {};

  // ==== read instr ====
  ASSERT(*index + 1 <= num_bytes, "invalid instr stream: last instr incomplete");
  memcpy(&instr, instr_stream + *index, 1);
  ASSERT(instr.opcode == OP_MOV, "Unsupported obcode: 0x%02X (at index %i)", (uint32_t)instr.opcode, *index);
  *index += 1;

  ASSERT(*index + 1 <= num_bytes, "invalid instr stream: last instr incomplete");
  memcpy(1 + (uint8_t*)&instr, instr_stream + *index, 1);
  *index += 1;

  // if the mode was something else, I would need to decode more bytes
  ASSERT(instr.MOD == MOD_REGISTER, "Unsupported Mode: 0x%02X", (uint32_t)instr.MOD);

  return instr;
}

void instr_print(struct Instr instr)
{
  switch(instr.opcode)
  {
  case OP_MOV:
    printf("mov ");
    switch(instr.MOD)
    {
    case MOD_REGISTER:
      printf("X, Y\n");
      return;
    }
    abort();
  }
  abort();
}