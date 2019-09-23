# Tiny BASIC

Tinybasic is an implementation of the Tiny BASIC language. It conforms to the specification by Dennis Allison, published in People's Computer Company Vol.4 No.2 and reprinted in Dr. Dobb's Journal, January 1976.

## Building for Linux

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

After building, you can install the binary and the manual page with the following command, run as root:

```
# make install
```

The Tiny BASIC binary will now reside in the `/usr/local/bin` directory, and the manual page will be in `/usr/local/share/man/man1`. You can run the binary by just typing its name `tinybasic` followed by its arguments. The manual is now accessible with the usual `man tinybasic` command.

## Cross-compiling for Windows

You can create a Windows executable on a Linux machine by issuing the following command from the local repo's directory:

```
$ make -f Makefile.mingw
```

The binary `tinybasic.exe` will be in the `bin` directory. You can turn the man page into a PDF that Windows users would be comfortable with, using the command:

```
$ man -Tpdf doc/tinybasic.man > doc/tinybasic.pdf
```

Currently it's up to you to package these files up for transfer to a Windows machine.

### Prerequisites

Building for Windows requires the MinGW cross-compiler to be installed. For building 64-bit executables on an Ubuntu system, I installed it like this:

```
# apt install mingw-w64
```

## Cross-compiling for DOS

You can create a DOS executable on a Linux machine by issuing the following commands from the local repo's directory:

```
$ export WATCOM=your Watcom base directory
$ export PATH=$WATCOM/binl:$PATH
$ make -f Makefile.watcom
```

The 8086-compatible binary `tinybas.exe` will be in the `bin` directory. You can turn the man page into a plain text file of the kind that DOS users are used to, with the command:

```
$ man doc/tinybasic.man | col -bx > doc/tinybas.txt
```

The `col` command removes the backspace sequences that `man` normally generates to simulate bold and underlining on printers, producing cleaner output for viewing in text editors.

As with the Windows build, it's currently up to you how to package these files up. I recommend copying the `exe`, `txt` and `bas` files to a single directory `tinybas` and zipping that up (with stored relative path) for easy extraction on a DOS system.

### Prerequisites

Cross-compilation for DOS requires the OpenWatcom compiler, which is available from <http://openwatcom.org/>. A later fork is available at <https://github.com/open-watcom/open-watcom-v2>.
