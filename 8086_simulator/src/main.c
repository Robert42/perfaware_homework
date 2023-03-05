#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

#include "decoder.c"

#define BUF_SIZE 4096
uint8_t bytes[BUF_SIZE + sizeof(struct Instr)] = {}; // allocating sizeof(Instr) more bytes so I can always copy the same number of bytes withou having to worry about buffer boundaries
int num_bytes = 0;

int main(int argc, char** argv)
{
  // get filename from args
  const char* const program = argv[0];
  ASSERT(argc == 2, "usage: %s [FILE]\n", program);
  const char* const filepath = argv[1];

  // read file
  {
    FILE* f = fopen(filepath, "rb");
    ASSERT(f != NULL, "Could not open: <%s> for reading\n", filepath);
    num_bytes = fread(bytes, 1, BUF_SIZE, f);
    fclose(f);
    ASSERT(num_bytes < BUF_SIZE, "File <%s> is too large to fit to the buffer (%i)\n", filepath, BUF_SIZE);
  }

  // output
  printf("; %s\n", filepath);
  printf("bits 16\n\n");

  for(int i=0; i < num_bytes;)
    instr_print(instr_decode(bytes, &i, num_bytes));
}