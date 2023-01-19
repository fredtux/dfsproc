There are 2 important files **f1_c.c** and **f2_haskell.hs**. There is also a symlink to **f2** that is the executable produced by cabal build. Everything else is used to build the haskell executable.

## f1_c.c
A test file written in C in order to test the syscall

## f2_haskell.hs
A userland application written in Haskell that uses C code in order to call the system call

___
For more instructions check out the main README.md file: [https://github.com/fredtux/dfsproc#florin](https://github.com/fredtux/dfsproc#florin)
