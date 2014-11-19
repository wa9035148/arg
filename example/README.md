```bash
$ g++ test1.cpp -o test1 -I../include

$ ./test1 --help
USAGE
    test1 IN1 IN2

OPTIONS
    --help
        Show this help and exit.

    --int-argument <integer>
        an argument which takes an integer

$ ./test1
error: option '--int-argument' is required!

$ ./test1 --int-argument
error: option '--int-argument' requires value!

$ ./test1 --int-argument 123
error: the number of arguments is less than expected! (expected at least: 2, passed: 0)

$ ./test1 --int-argument 123 arg1 arg2
args[0] = arg1
args[1] = arg2
int_argument = 123

```
