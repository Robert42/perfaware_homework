enum Op_Code
{
  MOV = 0x22 // 0b100010
};
static_assert(sizeof(enum Op_Code) == 1);

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

void decode_and_print(const uint8_t* instr, int* index, int num_bytes)
{
  ASSERT(*index + 1 <= num_bytes, "invalid instr stream: last instr incomplete");

  printf("%02x ", instr[*index]);
}