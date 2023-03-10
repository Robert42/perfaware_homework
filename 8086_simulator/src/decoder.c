// Using octals because of this article https://gist.github.com/seanjensengrey/f971c20d05d4d0efc0781f2f3c0353da suggested by x13pixels [in the comments of the course](https://www.computerenhance.com/p/instruction-decoding-on-the-8086/comment/13235714)

struct Operand decode_addr_expr(bool W, uint8_t mode, uint8_t r_m, struct Byte_Stream* byte_stream);
struct Instr decode_instr_rm2rm(enum Instr_Op op, uint8_t* bytes, struct Byte_Stream* byte_stream);
struct Instr decode_instr_im2rm(enum Instr_Op op, bool wide_im, uint8_t* bytes, struct Byte_Stream* byte_stream);
struct Instr decode_instr_im2r(enum Instr_Op op, uint8_t* bytes, struct Byte_Stream* byte_stream);
struct Instr decode_instr_mem_acc(enum Instr_Op op, uint8_t* bytes, struct Byte_Stream* byte_stream);

enum Instr_Op arith_op(uint8_t encoded, uint8_t rshift){return (7 & (encoded >> rshift)) | ARITH_OP;}

struct Instr instr_decode(struct Byte_Stream* byte_stream)
{
  uint8_t bytes[6] = {};
  
  bytes[0] = read_u8(byte_stream);
  
  switch(bytes[0] & 0xfe)
  {
  case 0306: // Immediate to register/memory
    const bool W = bytes[0] & 1;
    bytes[1] = read_u8(byte_stream);
    return decode_instr_im2rm(MOV, W, bytes, byte_stream);
  }

  switch(bytes[0] & 0xfc)
  {
  // Immediate to register/memory
  case 0200:
    const bool wide_im = (bytes[0] & 3) == 1;
    bytes[1] = read_u8(byte_stream);
    return decode_instr_im2rm(arith_op(bytes[1], 3), wide_im, bytes, byte_stream);

  case 0210: // Register/memory to/from register
    return decode_instr_rm2rm(MOV, bytes, byte_stream);
  case 0240: // Memory to/from accumulator
    return decode_instr_mem_acc(MOV, bytes, byte_stream);
  }
  
  switch(bytes[0] & 0xf0)
  {
  case 0260: // Immediate to register
    return decode_instr_im2r(MOV, bytes, byte_stream);
  }

  switch(bytes[0] & 0300)
  {
  case 0100:
    bytes[1] = read_u8(byte_stream);
    return (struct Instr){.op=JNZ, .ip_incr=bytes[1]};
  }

  // Immediate to accumulator
  if((bytes[0] & 0b11000100) == 0b100)
  {
    struct Instr instr = {.op=arith_op(bytes[0], 3)};
    const bool W = bytes[0] & 1;
    const union Payload data = read_payload(W, byte_stream);
    instr.dest = op_reg(W, AL);
    instr.src = op_im(W, data);
    return instr;
  }

  if((bytes[0] & 0b11000100) == 0) // Arithmetic --   Reg/memory and register to either
    return decode_instr_rm2rm(arith_op(bytes[0], 3), bytes, byte_stream);

  UNIMPLEMENTED("%03o", bytes[0]);
}

struct Operand decode_addr_expr(bool W, uint8_t mod, uint8_t r_m, struct Byte_Stream* byte_stream)
{
  switch(mod)
  {
  case 0:
    if(r_m == 6)
      return op_addr_direct(read_u16(byte_stream));
    else
      return op_addr_expr(r_m);
  case 1 ... 2:
  {
    const bool wide_displacement = mod == 2;
    const uint16_t displacement = read_s16_with_sign_extension(wide_displacement, byte_stream);
    return op_addr_expr_with_displacement(r_m, displacement);
  }
  case 3:
    return op_reg(W, r_m);
  }

  abort();
}

struct Instr decode_instr_rm2rm(enum Instr_Op op, uint8_t* bytes, struct Byte_Stream* byte_stream)
{
    struct Instr instr = {.op=op};

    const bool D = bytes[0] & 2;
    const bool W = bytes[0] & 1;

    bytes[1] = read_u8(byte_stream);
    const uint8_t mod = (bytes[1] & 0300) >> 6;
    const uint8_t reg = (bytes[1] & 0070) >> 3;
    const uint8_t r_m = bytes[1] & 0007;

    instr.dest = decode_addr_expr(W, mod, r_m, byte_stream);
    instr.src = op_reg(W, reg);

    if(D)
      op_swap(&instr.dest, &instr.src);
    return instr;
}

struct Instr decode_instr_im2rm(enum Instr_Op op, bool wide_im, uint8_t* bytes, struct Byte_Stream* byte_stream)
{
  struct Instr instr = {.op=op};

  const bool W = bytes[0] & 1;

  const uint8_t mod = (bytes[1] & 0300) >> 6;
  const uint8_t r_m = bytes[1] & 0007;

  instr.dest = decode_addr_expr(W, mod, r_m, byte_stream);
  instr.src = op_im(W, read_payload(wide_im, byte_stream));
  return instr;
}

struct Instr decode_instr_im2r(enum Instr_Op op, uint8_t* bytes, struct Byte_Stream* byte_stream)
{
  struct Instr instr = {.op=op};

  const bool W = bytes[0] & 0010;
  const uint8_t reg = bytes[0] & 0007;

  const union Payload data = read_payload(W, byte_stream);

  instr.dest = op_reg(W, reg);
  instr.src = op_im(W, data);
  return instr;
}

struct Instr decode_instr_mem_acc(enum Instr_Op op, uint8_t* bytes, struct Byte_Stream* byte_stream)
{
  struct Instr instr = {.op=op};

  const bool D = (bytes[0] & 2);
  instr.op = MOV;

  const bool W = bytes[0] & 1;
  instr.dest = op_reg(W, AL);
  instr.src = op_addr_direct(read_u16(byte_stream));

  if(D)
    op_swap(&instr.dest, &instr.src);
  return instr;
}