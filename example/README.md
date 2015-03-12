```bash
$ g++ test1.cpp -o test1 -I../arg

$ ./test1 --help
USAGE
    test1 INT1 INT2

OPTIONS
    --[no-]help    (default: false)
      Show this help and exit.

    --int-option <integer>   (default: 0)
      An option which takes an integer value.

    --[no-]bool-option    (default: false)
      An option which takes an bool value.

$ ./test1
error: the number of arguments is less than expected! (expected at least: 2, given: 0)

USAGE
    test1 INT1 INT2

$ ./test1 --int-option
error: option '--int-option' requires value!

$ ./test1 --int-option 123 456 789
args[0] = 456
args[1] = 789
int_option = 123
bool_option = 0

$ ./test1 --int-option 123 --bool-option 456 789
args[0] = 456
args[1] = 789
int_option = 123
bool_option = 1
```
