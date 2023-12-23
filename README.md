# cce
A bitboard chess engine.

## About.
This is an experiment in improving my skills at programming basic applications in C.

The application consists of three separate libraries: 

### `engine` 
A generic application launcher and subsystems manager, including lots of utility headers for robust C programming (string, memory, platform abstraction, etc.). It is heavily based on early boilerplate tutorials from Travis Vroman's *Kohi Game Engine* YouTube series.

### `src`
A command-line chess game application, including the chess engine subsystem. Chess engine is heavily based on tutorials from Code_Monkey_King's *Bitboard Chess Engine* YouTube series.

Right now, it is in the primitive stages of development; its algorithm for determining the best move is rudimentary at best, but improvements will be added frequently.

### `test`
A generic unit testing subsystem. It is heavily based on tutorials from Travis Vroman's *Kohi Game Engine* YouTube series.

## Build.
Building from source requires GCC and GNU make.

To build and launch: 
```
> make linux-run
> make windows-run
```
To run unit tests, then build and launch:
```
> make linux-test
> make windows-test
```
To compile and launch the application separately:
```
> make linux
> bin/cce

       or

> make windows
> bin\cce.exe
```