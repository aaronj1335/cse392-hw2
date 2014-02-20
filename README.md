# homework 2

## scan function interface

i opted for passing in an `op` function into the scan function (similar to
something like the stdlib `qsort`) rather than prof. biros's approach of
relying on some function that's been globally defined. i don't understand why
you'd choose his approach over the former.

## program interface

i've structured it right now to build a binary the same name as the directory.
it reads numbers from `stdin`, one per line into an array, and runs the scan
function, and then outputs them one per line.

## building

i dunno about the `makefile`, but as long as you add/edit files in `src/` and
make sure each `.cc` file has a corresponding `.h` file, then `make` should
work.

## testing

i've made a stupid simple test harness that iterates through the files in
`test/fixtures/in/`, `cat`'s them into the program, and then `diff`'s the
output with the corresponding file in `test/fixtures/out/`. `make test` tests
all the inputs.
