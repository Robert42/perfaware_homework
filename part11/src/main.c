#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "decoder.c"

#define BUF_SIZE 4096
uint8_t bytes[BUF_SIZE] = {};
int num_bytes = 0;

int main(int argc, char** argv)
{
  // get filename from args
  const char* const program = argv[0];
  if(argc != 2)
  {
    fprintf(stderr, "usage: %s [FILE]\n", program);
    exit(1);
  }
  const char* const filepath = argv[1];

  // read file
  {
    FILE* f = fopen(filepath, "rb");
    if(f == NULL)
    {
      fprintf(stderr, "Could not open: <%s> for reading\n", filepath);
      exit(1);
    }
    num_bytes = fread(bytes, 1, BUF_SIZE, f);
    fclose(f);
    if(num_bytes >= BUF_SIZE)
    {
      fprintf(stderr, "File <%s> is too large to fit to the buffer (%i)\n", filepath, BUF_SIZE);
      exit(1);
    }
  }

  // output
  printf("; %s\n", filepath);

  for(int i=0; i < num_bytes; ++i)
    decode_and_print(bytes, &i, num_bytes);
}