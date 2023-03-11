// Using octals because of this article https://gist.github.com/seanjensengrey/f971c20d05d4d0efc0781f2f3c0353da suggested by x13pixels [in the comments of the course](https://www.computerenhance.com/p/instruction-decoding-on-the-8086/comment/13235714)

struct Operand decode_addr_expr(bool W, uint8_t mode, uint8_t r_m, struct Byte_Stream* byte_stream);
struct Instr decode_rm(enum Instr_Op op, uint8_t* bytes, struct Byte_Stream* byte_stream);
struct Instr decode_instr_rm2rm_dw(enum Instr_Op op, bool d, bool w, uint8_t* bytes, struct Byte_Stream* byte_stream);
struct Instr decode_instr_rm2rm(enum Instr_Op op, uint8_t* bytes, struct Byte_Stream* byte_stream);
struct Instr decode_instr_im2rm(enum Instr_Op op, bool wide_im, uint8_t* bytes, struct Byte_Stream* byte_stream);
struct Instr decode_instr_im2r(enum Instr_Op op, uint8_t* bytes, struct Byte_Stream* byte_stream);
struct Instr decode_instr_mem_acc(enum Instr_Op op, uint8_t* bytes, struct Byte_Stream* byte_stream);
struct Instr decode_instr_unary_rm(enum Instr_Op op, uint8_t* bytes, struct Byte_Stream* byte_stream);

enum Instr_Op arith_op(uint8_t encoded, uint8_t rshift){return (7 & (encoded >> rshift)) | ARITH_OP;}
enum Instr_Op jmp_op(uint8_t encoded){return (encoded & 0x0f) | JMP_OP;}
enum Instr_Op loop_op(uint8_t encoded){return (encoded & 0x03) | LOOP_OP;}

struct Instr instr_decode(struct Byte_Stream* byte_stream)
{
  uint8_t bytes[6] = {};
  
  bytes[0] = read_u8(byte_stream);

  if((bytes[0] & 0b11000100) == 0) // Arithmetic --   Reg/memory and register to either
    return decode_instr_rm2rm(arith_op(bytes[0], 3), bytes, byte_stream);
  
  // ==== 1111 1111 ============================================================
  switch(bytes[0])
  {
  case 0xff:
    bytes[1] = peek_u8(byte_stream);
    if((bytes[1]&0070) == 0060) // PUSH -- register/memory
    {
      bytes[1] = read_u8(byte_stream);
      return decode_rm(PUSH, bytes, byte_stream);
    }
    break;
  case 0x8f:
    bytes[1] = peek_u8(byte_stream);
    if((bytes[1]&0070) == 0) // POP -- register/memory
    {
      bytes[1] = read_u8(byte_stream);
      return decode_rm(POP, bytes, byte_stream);
    }
    break;
  case 0327: return (struct Instr){.op = XLAT};
  case 0237: return (struct Instr){.op = LAHF};
  case 0236: return (struct Instr){.op = SAHF};
  case 0234: return (struct Instr){.op = PUSHF};
  case 0235: return (struct Instr){.op = POPF};
  case 0067: return (struct Instr){.op = AAA};
  case 0047: return (struct Instr){.op = DAA};
  case 0077: return (struct Instr){.op = AAS};
  case 0057: return (struct Instr){.op = DAS};
  case 0230: return (struct Instr){.op = CBW};
  case 0231: return (struct Instr){.op = CWD};
  case 0316: return (struct Instr){.op = INTO};
  case 0317: return (struct Instr){.op = IRET};
  case 0324:
  {
    bytes[1] = peek_u8(byte_stream);
    switch(bytes[1])
    {
    case 012:
    {
      bytes[1] = read_u8(byte_stream);
      return (struct Instr){.op = AAM, };
    }
    }
    break;
  }
  case 0325:
  {
    bytes[1] = peek_u8(byte_stream);
    switch(bytes[1])
    {
    case 012:
    {
      bytes[1] = read_u8(byte_stream);
      return (struct Instr){.op = AAD, };
    }
    }
    break;
  }
  case 0215:
  case 0305:
  case 0304:
  {
    enum Instr_Op op;
    switch(bytes[0])
    {
    case 0215: op = LEA; break;
    case 0305: op = LDS; break;
    case 0304: op = LES; break;
    default: UNREACHABLE();
    }
    return decode_instr_rm2rm_dw(op, true, true, bytes, byte_stream);
  }
  case 0302: // RET -- Within seg adding to SP
      return (struct Instr){.op = RET, .src = op_data16(read_u16(byte_stream))};
  case 0303: // RET -- Within seg adding to SP
      return (struct Instr){.op = RET};
  case 0315: // int -- TYPE SPECIFIED
  {
      return (struct Instr){.op = INT, .src=op_data8(read_u8(byte_stream))};
  }
  }
  
  // ==== 1111 1110 ============================================================
  switch(bytes[0] & 0xfe)
  {
  case 0306: // MOV -- Immediate to register/memory
  {
    const bool W = bytes[0] & 1;
    return decode_instr_im2rm(MOV, W, bytes, byte_stream);
  }
  case 0206: // XCHG -- Register/memory to/from register
  {
    const bool W = bytes[0] & 1;
    return decode_instr_rm2rm_dw(XCHG, true, W, bytes, byte_stream);
  }
  case 0250: // TEST -- Immediate data and accumulator
  {
    const bool W = bytes[0] & 1;
    return (struct Instr){.op=TEST, .dest=op_reg(W, AL), .src=op_data(W, read_payload(W, byte_stream))};
  }
  case 0362: // REP
  {
    const bool Z = bytes[0] & 1;
    (void)Z;

    bytes[1] = read_u8(byte_stream);
    const bool W = bytes[1] & 1;

    enum Operand_Str_Manip str_manip;
    switch(bytes[1] & 0xfe)
    {
    case 0244: str_manip = OSM_MOVS; break;
    case 0246: str_manip = OSM_CMPS; break;
    case 0256: str_manip = OSM_SCAS; break;
    case 0254: str_manip = OSM_LODS; break;
    case 0252: str_manip = OSM_STOS; break;
    default:
      UNIMPLEMENTED("%03o %03o", bytes[0], bytes[1]);
    }
    return (struct Instr){.op=REP, .src=op_str_manip_op(W, str_manip), };
  }
  case 0376:
  {
    bytes[1] = peek_u8(byte_stream); // warning, peek, not read
    enum Instr_Op op = 0;
    switch(bytes[1]&0070)
    {
    case 0010: // DEC -- Register/memory
      op = DEC;
      break;
    case 0000: // INC -- Register/memory
      op = INC;
      break;
    case 0020: // CALL -- indirect with segment
      bytes[1] = read_u8(byte_stream);
      return decode_rm(CALL, bytes, byte_stream);
    case 0040: // JMP -- indirect with segment
      bytes[1] = read_u8(byte_stream);
      return decode_rm(JMP, bytes, byte_stream);
    default:
      UNIMPLEMENTED("%03o %03o", bytes[0], bytes[1]);
    }
    return decode_instr_unary_rm(op, bytes, byte_stream);
  }
  case 0366:
  {
    bytes[1] = peek_u8(byte_stream); // warning, peek, not read
    enum Instr_Op op = 0;
    switch(bytes[1]&0070)
    {
    case 0030: // NEG -- Register/memory
      op = NEG;
      break;
    case 0040: // MUL -- multiply unsigned
      op = MUL;
      break;
    case 0050: // IMUL -- integer multiply signed
      op = IMUL;
      break;
    case 0060: // DIV -- multiply unsigned
      op = DIV;
      break;
    case 0070: // IDIV -- integer multiply signed
      op = IDIV;
      break;
    case 0020: // NOT -- invert
      op = NOT;
      break;
    case 0: // TEST -- Immediate to register/memory
    {
      const bool W = bytes[0] & 1;
      return decode_instr_im2rm(TEST, W, bytes, byte_stream);
    }
    default:
      UNIMPLEMENTED("%03o %03o", bytes[0], bytes[1]);
    }
    return decode_instr_unary_rm(op, bytes, byte_stream);
  }
  }

  // ==== 1111 1100 ============================================================
  switch(bytes[0] & 0xfc)
  {
  // Arithmetic -- Immediate to register/memory
  case 0200:
    const bool wide_im = (bytes[0] & 3) == 1;
    bytes[1] = peek_u8(byte_stream);
    return decode_instr_im2rm(arith_op(bytes[1], 3), wide_im, bytes, byte_stream);

  case 0210: // MOV -- Register/memory to/from register
    return decode_instr_rm2rm(MOV, bytes, byte_stream);
  case 0240: // MOV -- Memory to/from accumulator
    return decode_instr_mem_acc(MOV, bytes, byte_stream);

  case 0204: // TEST -- Register/memory and register
    return decode_instr_rm2rm(TEST, bytes, byte_stream);
  
  case 0340: // Loop
    bytes[1] = read_u8(byte_stream);
    return (struct Instr){.op=loop_op(bytes[0]), .ip_incr=bytes[1]};

  case 0344: // IN/OUT -- fixed port
  {
    const bool W = bytes[0] & 1;
    const bool O = bytes[0] & 2;
    struct Instr instr = {.op=O?OUT:IN , .dest=op_reg(W, AL), .src=op_data8(read_u8(byte_stream))};
    if(O)
      op_swap(&instr.dest, &instr.src);
    return instr;
  }
  case 0354: // IN/OUT -- variable port
  {
    const bool W = bytes[0] & 1;
    const bool O = bytes[0] & 2;
    struct Instr instr = {.op=O?OUT:IN, .dest=op_reg(W, AL), .src=op_reg(true, DL)};
    if(O)
      op_swap(&instr.dest, &instr.src);
    return instr;
  }
  case 0320:
  {
    bytes[1] = read_u8(byte_stream);;

    struct Instr instr = {};
    switch(bytes[1]&0070)
    {
    case 0040:
      instr.op = SHL;
      break;
    case 0050:
      instr.op = SHR;
      break;
    case 0070:
      instr.op = SAR;
      break;
    case 0000:
      instr.op = ROL;
      break;
    case 0010:
      instr.op = ROR;
      break;
    case 0020:
      instr.op = RCL;
      break;
    case 0030:
      instr.op = RCR;
      break;
    default:
      UNIMPLEMENTED("%03o %03o", bytes[0], bytes[1]);
    }

    const bool V = bytes[0] & 2;
    const bool W = bytes[0] & 1;

    const uint8_t mod = (bytes[1] & 0300) >> 6;
    const uint8_t r_m = bytes[1] & 0007;
    instr.dest = decode_addr_expr(W, mod, r_m, byte_stream);
    if(op_is_addr(instr.dest.variant))
      instr.dest.expl_size = W ? OP_EXPL_SIZE_U16 : OP_EXPL_SIZE_U8;
    
    if(V == 0)
      instr.src = op_data8(1);
    else
      instr.src = op_reg(false, CL);

    return instr;
  }
  case 0020: // TEST -- Register/memory to either
    UNIMPLEMENTED("TEST");
    return decode_instr_rm2rm(TEST, bytes, byte_stream);
  }

  // ==== 1111 1000 ============================================================
  switch(bytes[0] & 0xf8)
  {
  case 0220: // XCHG -- Register with accumulator
    return (struct Instr){.op=XCHG, .dest=op_reg(true, AL), .src=op_reg(true, bytes[0]&7)};
  case 0100: // INC -- Register
    return (struct Instr){.op=INC, .src=op_reg(true, bytes[0]&7)};
  case 0110: // DEC -- Register
    return (struct Instr){.op=DEC, .src=op_reg(true, bytes[0]&7)};
  }

  // ==== 1111 0000 ============================================================
  switch(bytes[0] & 0xf0)
  {
  case 0260: // MOV -- Immediate to register
    return decode_instr_im2r(MOV, bytes, byte_stream);
  case 0x70:
    bytes[1] = read_u8(byte_stream);
    return (struct Instr){.op=jmp_op(bytes[0]), .ip_incr=bytes[1]};
  case 0120: // PUSH/POP - register
    const bool pop = bytes[0] & 8;
    return (struct Instr){.op=pop ? POP : PUSH, .src=op_reg(true, bytes[0]&7)};
  }

  if((bytes[0] & 0b11100110) == 0b110) // PUSH -- segment register
  {
    const bool pop = bytes[0] & 1;
    return (struct Instr){.op=pop ? POP : PUSH, .src=op_seg_reg(3 & (bytes[0]>>3))};
  }

  // Arithmetic -- Immediate to accumulator
  if((bytes[0] & 0b11000100) == 0b100)
  {
    struct Instr instr = {.op=arith_op(bytes[0], 3)};
    const bool W = bytes[0] & 1;
    const union Payload data = read_payload(W, byte_stream);
    instr.dest = op_reg(W, AL);
    instr.src = op_im(W, data);
    return instr;
  }
  
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

  UNREACHABLE();
}

struct Instr decode_rm(enum Instr_Op op, uint8_t* bytes, struct Byte_Stream* byte_stream)
{
  struct Instr instr = {.op=op};

  const bool W = true;

  const uint8_t mod = (bytes[1] & 0300) >> 6;
  const uint8_t r_m = bytes[1] & 0007;
  instr.src = decode_addr_expr(W, mod, r_m, byte_stream);

  return instr;
}

struct Instr decode_instr_rm2rm(enum Instr_Op op, uint8_t* bytes, struct Byte_Stream* byte_stream)
{
    const bool D = bytes[0] & 2;
    const bool W = bytes[0] & 1;

    return decode_instr_rm2rm_dw(op, D, W, bytes, byte_stream);
}

struct Instr decode_instr_rm2rm_dw(enum Instr_Op op, bool D, bool W, uint8_t* bytes, struct Byte_Stream* byte_stream)
{
    struct Instr instr = {.op=op};

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

  bytes[1] = read_u8(byte_stream);
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

struct Instr decode_instr_unary_rm(enum Instr_Op op, uint8_t* bytes, struct Byte_Stream* byte_stream)
{
  struct Instr instr = {.op=op};
  const bool W = bytes[0] & 1;

  bytes[1] = read_u8(byte_stream); // we've only peeked above, now we actually move forward
  const uint8_t mod = (bytes[1] & 0300) >> 6;
  const uint8_t r_m = bytes[1] & 0007;

  instr.src = decode_addr_expr(W, mod, r_m, byte_stream);
  instr.src.expl_size = W ? OP_EXPL_SIZE_U16 : OP_EXPL_SIZE_U8;

  return instr;
}