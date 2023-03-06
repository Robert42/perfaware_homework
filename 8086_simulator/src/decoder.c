// Using octals because of this article https://gist.github.com/seanjensengrey/f971c20d05d4d0efc0781f2f3c0353da suggested by x13pixels [in the comments of the course](https://www.computerenhance.com/p/instruction-decoding-on-the-8086/comment/13235714)

struct Byte_Stream
{
  const uint8_t* begin;
  const uint8_t* end;
};

void read_bytes(void* dest, size_t n_bytes, struct Byte_Stream* s)
{
  ASSERT(s->begin+n_bytes <= s->end, "unexpected end!");

  memcpy(dest, s->begin, n_bytes);

  if(LOG)
  {
    for(int i=0; i<n_bytes; ++i)
      fprintf(stderr, " %02X", s->begin[i]);
  }
  
  s->begin += n_bytes;
}

uint8_t read_u8(struct Byte_Stream* s)
{
  uint8_t x;
  read_bytes(&x, sizeof(x), s);
  return x;
}

uint8_t read_u16(struct Byte_Stream* s)
{
  uint16_t x;
  read_bytes(&x, sizeof(x), s);
  return x;
}

struct Instr instr_decode(struct Byte_Stream* byte_stream)
{
  uint8_t bytes[6] = {};
  
  bytes[0] = read_u8(byte_stream);
  
  struct Instr instr = {};

  switch(bytes[0] & 0xfc)
  {
  case 0210:
  {
    instr.op = MOV;

    const bool D = bytes[0] & 2;
    const bool W = bytes[0] & 1;

    bytes[1] = read_u8(byte_stream);
    const uint8_t mod = (bytes[1] & 0300) >> 6;
    const uint8_t reg = (bytes[1] & 0070) >> 3;
    const uint8_t r_m = bytes[1] & 0007;

    ASSERT(mod == 3, "%i", mod);
    instr.dest = op_reg(W, r_m);
    instr.src = op_reg(W, reg);
    if(D)
      op_swap(&instr.dest, &instr.src);
    return instr;
  }
  }

  UNIMPLEMENTED();
}
