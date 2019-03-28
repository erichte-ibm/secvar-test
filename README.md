## Building tests

Run `make` to build all the tests.
The Makefile assumes that the target skiboot tree is located at `../skiboot`, override `SKIBOOT_PATH` to change.
`make run` will execute all the tests for feature checks.
`make valgrind` will run all the tests through valgrind, and complain if there are any errors.
Individual tests can be run with `make run_test_<name>`, e.g. `make run_test_void` will run the canary void test.
Replacing `s/run/valgrind` will run an individual valgrind test.

The above tests will abort running if any test fails.
To run all the tests regardless of pass or fail, run with `make -k`.

Each test will generate a `<test_name>.log` file containing all `prlog()` outputs in the `log/` directory.
If running in `valgrind` mode, each test will also generate a `valgrind_<test_name>.log` file, for analyzing the complaints.

If `gcovr` is installed, you may also run the `coverage` and `coverage_html` make targets.
The former option will display the coverage information to stdout.
Running `coverage_html` will output file-by-file coverage information in the `html/` directory.
Compilations are done with `gcov` support enabled by default, so even without the useful `gcovr` utility, coverage reports can still be generated manually.

`make clean` will remove all the garbage that **will** clutter the directory.

## Writing tests

Tests require four things: a file name starting with `test_` and ending in `.c`, `#include "test.c"` somewhere at the top, a `char *test_name`, and a `int run_test(void) {..}`.

The `test_name` must be set to something useful to identify the test, probably the same thing as the file name.

The main entry point to the test case is the `run_test()` function.
Put all testing logic in there, and if the test case should pass, return a 0.
Otherwise, the function should return literally any other value.
Given that `test.c` is directly included, test cases have access to *every* static function or variable in `keystore.c` and `secboot_part.c`.

Wrappers to the OPAL runtime services have been provided, so that casting each parameter is not necessary.
Each wrapper is same name as the original function, just without the `opal_` prefix (e.g. `opal_secvar_read` is `secvar_read`).

Also included are some potentially useful functions and macros that are not originally part of `libstb`:
 - `ASSERT(condition)` is a helpful shortcut to print an error and line number, and return nonzero.
 - `ASSERT_POST(condition, post-expr)` is a more flexible version of `ASSERT(..)` that executes `post-expr` after printing the error.
   - Example: `ASSERT_POST(0 == 1, goto clean);` would jump to label "clean" instead of just returning 1.
 - `list_length` counts how many items are in a keystore in-memory list.

