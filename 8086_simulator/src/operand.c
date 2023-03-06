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

static const char* addr_expr_to_str(enum Addr_Expr addr_expr)
{
  static const char* const addr_TABLE[] = {
    "BX + SI", "BX + DI", "BP + SI", "BP + DI",
    "SI", "DI", "BP", "BX",
  };

  ASSERT(0 <= addr_expr && addr_expr < 8);

  return addr_TABLE[addr_expr];
}