# Tiny BASIC

Tinybasic is an implementation of the Tiny BASIC language. It conforms to the specification by Dennis Allison, published in People's Computer Company Vol.4 No.2 and reprinted in Dr. Dobb's Journal, January 1976.

## Getting Started

In its unfinished state, Tiny BASIC can be built on a Unix-like operating system that has the tools *gcc* and *make*. Build instructions are as follows:

```
$ git clone https://github.com/cyningstan/TinyBASIC TinyBASIC
$ cd TinyBASIC
$ make
```

The `bin` directory will contain the Tiny BASIC binary, `tinybasic`. The `bas` directory will contain some BASIC sample programs, all games. To run one of them (e.g. wumpus), type a command like the following:

```
$ bin/tinybasic bas/wumpus.bas
```

The documentation is in man page form. To see it, type:

```
$ man doc/tinybasic.man
```

### Prerequisites

The Gnu C Compiler and the GNU Make utility, or tools compatible with them, are required to build Tiny BASIC.

### Installing

As the project is unfinished, there is not yet an 'install' target in the Makefile. Simply add the `bin` directory to your path, or move the binary somewhere that is already in your path, to run the program. Or specify the path each time you run it.
