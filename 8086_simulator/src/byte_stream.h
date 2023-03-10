
struct Byte_Stream
{
  const uint8_t* begin;
  const uint8_t* end;
};

size_t BYTES_READ = 0;

void read_bytes(void* dest, size_t n_bytes, struct Byte_Stream* s)
{
  ASSERT(s->begin+n_bytes <= s->end, "unexpected end!");

  memcpy(dest, s->begin, n_bytes);

  if(LOG)
  {
    for(int i=0; i<n_bytes; ++i)
    {
      fprintf(stderr, " %02X", s->begin[i]);
      BYTES_READ++;
    }
  }
  
  s->begin += n_bytes;
}

uint8_t read_u8(struct Byte_Stream* s)
{
  uint8_t x;
  read_bytes(&x, sizeof(x), s);
  return x;
}

uint16_t read_u16(struct Byte_Stream* s)
{
  uint16_t x;
  read_bytes(&x, sizeof(x), s);
  return x;
}

union Payload read_payload(bool wide, struct Byte_Stream* s)
{
  union Payload data = {};
  if(wide)
    data.wide = read_u16(s);
  else
    data.lo = read_u8(s);
  return data;
}

uint16_t read_s16_with_sign_extension(bool read_two_bytes, struct Byte_Stream* s)
{
  if(read_two_bytes)
    return read_u16(s);
  
  uint16_t x = read_u8(s);
  if(x & 0x80)
    x |= 0xff00;

  return x;
}