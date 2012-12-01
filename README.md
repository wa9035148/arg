arg
===

Example-1
---------

~~~~~~
#include <stdio.h>
#include "arg.h"

int main(int argc, char **argv) {
  
  arg_begin();
  argi(testi); // integer option
  argd(testd); // double option
  argb(testb); // bool option
  args(tests); // string option
  arg_end;
  
  printf("testi = %d\n", testi);
  printf("testd = %lf\n", testd);
  printf("testb = %s\n", testb?"true":"false");
  printf("tests = %s\n", tests);
  
  for (int i=0; i<argc; i++)
    printf("argv[%d] = %s\n", i, argv[i]);
  
  return 0;
}
~~~~~~

~~~~~~
$ ./example a --testi 100 --testd -3.14 --testb b --tests str c
testi = 100
testd = -3.140000
testb = true
tests = str
argv[0] = /PATH_TO_EXAMPLE/example
argv[1] = a
argv[2] = b
argv[3] = c
~~~~~~

Example-2
---------

~~~~~~
#include <stdio.h>
#include "arg.h"

int main(int argc, char **argv) {
  
  arg_begin("example");
  argi(testi, /*short option*/ 'i', "integer message", /*default value*/ 123);
  argd(testd, "double message", 1.23);
  argb(testb, 'b', false);
  args(tests, "string message", "default value");
  arg_end;
  
  printf("testi = %d\n", testi);
  printf("testd = %lf\n", testd);
  printf("testb = %s\n", testb?"true":"false");
  printf("tests = %s\n", tests);
  
  for (int i=0; i<argc; i++)
    printf("argv[%d] = %s\n", i, argv[i]);
  
  return 0;
}
~~~~~~

~~~~~~
$ ./example --help
Usage: example

  -i, --testi                 integer message
      --testd                 double message
  -b, --testb
      --tests                 string message
  -h, --help                  Show this message
~~~~~~

~~~~~~
$ ./example
testi = 123
testd = 1.230000
testb = false
tests = default value
argv[0] = /PATH_TO_EXAMPLE/example
~~~~~~
