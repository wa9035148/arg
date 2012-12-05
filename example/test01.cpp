#include <stdio.h>
#include "arg.h"

int main(int argc, char **argv) {
  
  arg_begin("example");
  argi(testi, /*short option*/ 'i', "integer message", /*default value*/ 123);
  argd(testd, "double message", 1.23);
  argb(testb, 'b', false);
  args(test_s, "string message", "default value");
  arg_end;
  
  printf("testi = %d\n", testi);
  printf("testd = %lf\n", testd);
  printf("testb = %s\n", testb?"true":"false");
  printf("test_s = %s\n", test_s);
  
  for (int i=0; i<argc; i++)
    printf("argv[%d] = %s\n", i, argv[i]);
  
  return 0;
}
