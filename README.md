# Triplebyte C++ debug challenge

Compiling this program requires a C++ compiler with C++11 support; this means GCC version 4.9 or newer, or Clang version 3.3 or newer.
This code is not well tested with compilers other than GCC or Clang.

Libcurl with SSL support and development headers must also be installed; on Ubuntu, `apt install libcurl4-openssl-dev` should take care of this.

# Building

On Unix-like systems or Cygwin, you can usually just run `make`:

    make

If all goes well, that will build an executable named `crawler` in this directory.

If that doesn't work, and you have CMake installed, try using CMake instead:

    cmake . && make

Otherwise, you have to compile it directly. This command might work:

    g++ -std=c++11 *.cpp -lcurl -lpthread -o crawler

# Running the tests

Once the `crawler` executable is built, this is how you run the already passing tests:

    ./crawler 0

To run the first failing test:

    ./crawler 1
