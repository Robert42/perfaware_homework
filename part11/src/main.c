#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
  // read filename
  const char* program = argv[0];
  if(argc != 2)
  {
    fprintf(stderr, "usage: %s [FILE]\n", program);
    exit(1);
  }
}