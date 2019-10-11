# Tiny BASIC

Tinybasic is an implementation of the Tiny BASIC language. It conforms to the specification by Dennis Allison, published in People's Computer Company Vol.4 No.2 and reprinted in Dr. Dobb's Journal, January 1976.

## Getting Started

Currently this package is configured for building only on Linux and other GNU-based environments, regardless of the target machine. There are target Makefiles for Linux, Windows, MS-DOS and Android.

In case you're new to git or github, the first thing you need to do (after installing git and getting a github account) is to clone the repo:

```
$ git clone https://github.com/cyningstan/TinyBASIC TinyBASIC
$ cd TinyBASIC
```

## Building for Linux

Building for Linux and other GNU-based environments is simply a case of typing `make` from the repo directory. This assumes you have the GCC suite of tools installed (make and gcc).

```
$ make
```

The `bin` directory will contain the Tiny BASIC binary, `tinybasic`. The `bas` directory will contain some BASIC sample programs, all games. To install the program and its associated files in the traditional directories, do the following as root:

```
# make install
```

The `tinybasic` binary will be installed in `/usr/local/bin`, the manual in `/usr/local/man/man1` and the BASIC samples in `/usr/local/share/doc/tinybasic`.

To run TinyBASIC, you need to invoke it with the filename of a BASIC program to run or compile, with a command like the following:

```
$ tinybasic /usr/local/share/doc/tinybasic/samples/wumpus.bas
```

Presumably you'll put your own BASIC programs somewhere more easily accessible. The documentation is in man page form. To see it, type:

```
$ man tinybasic
```

## Building for Windows

Building for Windows requires a Linux environment, with GNU make and the cross-compiler MinGW. To create a Windows executable, type the following in the TinyBASIC repo:

```
$ make -f Makefile.mingw
```

Once finished, there should be a file `tinybasic.exe` in the `bin` directory. You can turn the man page into a PDF that Windows users would be comfortable with, using the command:

```
$ man -Tpdf doc/tinybasic.man > doc/tinybasic.pdf
```

Currently it's up to you to package these files up for transfer to a Windows machine.

## Building for DOS

Cross-compilation for DOS requires the OpenWatcom compiler, which is available from <http://openwatcom.org/>. A later fork is available at <https://github.com/open-watcom/open-watcom-v2>. Note that we still need the GNU make utility, as the `wmake` program supplied with OpenWatcom lacks some necessary features.

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

## Building for Android using Termux

Termux is a Linux-like environment for Android. An Android system with a keyboard and a large enough screen is a viable system for BASIC programming. TinyBASIC was developed on both a Linux system and an Android device using Termux, and unlike Windows and DOS is built on the target device itself.

So to build TinyBASIC on Android, you need Termux installed, and within Termux, you need the packages clang and make. To build the executable, type:

```
$ make -f Makefile.termux
```

The `bin` directory will contain the Tiny BASIC binary, `tinybasic`. The `bas` directory will contain some BASIC sample programs, all games. To install these and the manuals in their proper place, type:

```
$ make -f Makefile.termux install
```

The `tinybasic` binary will be installed in `/data/data/termux/files/usr/bin`, the manual in `/data/data/termux/files/usr/man/man1` and the BASIC samples in `/data/data/termux/files/usr/share/doc/tinybasic`.

```
$ tinybasic /data/data/com.termux/files/usr/share/doc/tinybasic/samples/wumpus.bas
```

Presumably you'll put your own BASIC programs somewhere more easily accessible. The documentation is in man page form. To see it, type:

```
$ man tinybasic
```

One final note about Android. If you want to run TinyBASIC from an environment other than Termux (e.g. Terminal Emulator for Android) then I recommend you clone the repo into shared storage, `/sdcard`. That way, your preferred environment has access to the executable and the sample files.

## Sample Programs

The six sample programs are all classic games from the history of computing, of varying complexity and entertainment value. Some of them are actually fun to play.

* `lander.bas`: the classic Lunar Lander game from 1969. Your lander module is descending to the moon's surface. Your task is to control that descent and ensure that the module lands safely. Do this by applying thrust judiciously and keeping your eye on both altitude and fuel levels. This is the smallest of the games but is quite entertaining while you learn the proper strategy.

* `hurkle.bas`: Hunt the Hurkle, a game from the 1970s. The hurkle is hidden on a 10x10 grid and you must find it. Enter coordinates, and if your guess is wrong, the game lets you know in what direction to look.

* `mugwump.bas`: Mugwump is a similar game to Hunt the Hurkle, but instead of telling you in what direction the mugwump lies, the game tells you its distance instead. You must figure out the direction in this game of "hot" and "cold".

* `hammurabi.bas`: the ancestor of all city-building games. Hammurabi puts you in charge of an ancient civilisation. By carefully managing its resources you must build up its territory and population, hopefully leaving the kingdom more prosperous than you found it.

* `tictactoe.bas`: the traditional three-in-a-row game, with computer player. The computer in this version isn't perfect, and it's possible to win against it. Can you figure out how?

* `wumpus.bas`: Hunt the Wumpus is a very entertaining adventure game. Hidden in a cave system is an awful creature called the Wumpus, and you must find it and kill it. There are giant bats and bottomless pits to thwart your efforts. If you have crafting skills, this would make a very good computer-aided solo board game.
