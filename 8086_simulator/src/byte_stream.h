
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