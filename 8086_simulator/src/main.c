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
      fprintf(stderr, "\n==== ASSERT ====\n"); \
      fprintf(stderr, "%s:%i\n", __FILE__, __LINE__); \
      fprintf(stderr, "" __VA_ARGS__); \
      fprintf(stderr, "\n"); \
      exit(1); \
    } \
  } while(false)
#define UNIMPLEMENTED(...) ASSERT(false, "UNIMPLEMENTED!\n" __VA_ARGS__)
#define UNREACHABLE(...) abort()

#define ARRAY_LEN(X) (sizeof(X) / sizeof(X[0]))

#include "operand.h"
#include "byte_stream.h"
#include "instr.h"

#include "instr.c"
#include "operand.c"
#include "decoder.c"

uint8_t bytes[4096] = {}; // allocating MAX_INSTR_LEN more bytes so I can always copy bytes without having to worry about buffer boundaries

int main(int argc, char** argv)
{
  // get filename from args
  const char* const program = argv[0];
  ASSERT(argc == 2, "usage: %s [FILE]\n", program);
  const char* const filepath = argv[1];

  LOG = strstr(filepath, "0042");
  if(LOG)
    fprintf(stderr, "==== %s ====\n", filepath);

  // read file
  struct Byte_Stream byte_stream = {
    .begin = bytes,
  };
  {
    FILE* f = fopen(filepath, "rb");
    ASSERT(f != NULL, "Could not open: <%s> for reading\n", filepath);
    byte_stream.end = byte_stream.begin + fread(bytes, 1, ARRAY_LEN(bytes), f);
    fclose(f);
    ASSERT(byte_stream.begin <= byte_stream.end, "File <%s> is too large to fit to the buffer (%lu)\n", filepath, ARRAY_LEN(bytes));
  }

  // == first pass finding the labels (and debug printing) ==
  uint16_t num_labels = 0;
  uint16_t LABELS[ARRAY_LEN(bytes)] = {};

  while(byte_stream.begin < byte_stream.end)
  {
    if(LOG)
      fprintf(stderr, "%4lu |", byte_stream.begin - bytes);
    const struct Instr instr = instr_decode(&byte_stream);

    const size_t curr_pos = byte_stream.begin-bytes;
    ASSERT(curr_pos < UINT16_MAX);
    if(has_label(instr.op))
    {
      if(LABELS[curr_pos+instr.ip_incr] == 0)
      {
        ASSERT(num_labels < UINT16_MAX);
        LABELS[curr_pos+instr.ip_incr] = ++num_labels;
      }
    }

    if(LOG)
    {
      for(; BYTES_READ < 6; ++BYTES_READ)
        fprintf(stderr, "    " + (LOG_BYTES==LB_HEX));
      BYTES_READ = 0;
      fprintf(stderr, "\t\t");
      instr_print(instr, stderr, NULL, SIZE_MAX);
    }
  }
  if(LOG)
    fprintf(stderr, "\n");
  LOG = false;
  
  // == second pass with the actual output ==

  printf("; %s\n", filepath);
  printf("bits 16\n\n");

  byte_stream.begin = bytes;
  while(byte_stream.begin < byte_stream.end)
  {
    {
      const size_t label_pos = byte_stream.begin-bytes;
      if(LABELS[label_pos] != 0)
        printf("label%" PRIu16 ":\n", LABELS[label_pos]-1);
    }

    const struct Instr instr = instr_decode(&byte_stream);
    instr_print(instr, stdout, LABELS, byte_stream.begin-bytes);
  }
}