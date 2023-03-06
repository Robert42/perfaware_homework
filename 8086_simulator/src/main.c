#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool LOG = false;

#define ASSERT(COND, ...) do { \
    if(!(COND)) \
    { \
      fprintf(stderr, "==== ASSERT ====\n"); \
      fprintf(stderr, "%s:%i\n", __FILE__, __LINE__); \
      fprintf(stderr, "" __VA_ARGS__); \
      fprintf(stderr, "\n"); \
      exit(1); \
    } \
  } while(false)
#define UNIMPLEMENTED(...) ASSERT(false, "UNIMPLEMENTED!\n" __VA_ARGS__)

#define ARRAY_LEN(X) (sizeof(X) / sizeof(X[0]))

#include "operand.h"
#include "byte_stream.h"
#include "instr.h"

#include "instr.c"
#include "operand.c"
#include "decoder.c"

#define BUF_SIZE 4096
uint8_t bytes[BUF_SIZE] = {}; // allocating MAX_INSTR_LEN more bytes so I can always copy bytes without having to worry about buffer boundaries

int main(int argc, char** argv)
{
  // get filename from args
  const char* const program = argv[0];
  ASSERT(argc == 2, "usage: %s [FILE]\n", program);
  const char* const filepath = argv[1];

  LOG = strstr(filepath, "0041");
  if(LOG)
    fprintf(stderr, "==== %s ====", filepath);

  // read file
  struct Byte_Stream byte_stream = {
    .begin = bytes,
  };
  {
    FILE* f = fopen(filepath, "rb");
    ASSERT(f != NULL, "Could not open: <%s> for reading\n", filepath);
    byte_stream.end = byte_stream.begin + fread(bytes, 1, BUF_SIZE, f);
    fclose(f);
    ASSERT(byte_stream.begin <= byte_stream.end, "File <%s> is too large to fit to the buffer (%i)\n", filepath, BUF_SIZE);
  }

  // output
  printf("; %s\n", filepath);
  printf("bits 16\n\n");

  while(byte_stream.begin < byte_stream.end)
  {
    if(LOG)
      fprintf(stderr, "\n%4lu |", byte_stream.begin - bytes);
    instr_print(instr_decode(&byte_stream), stdout);
  }
  if(LOG)
    fprintf(stderr, "\n");
}