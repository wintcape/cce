# cce
A bitboard chess engine.

## About

This is an experiment in improving my skills at programming basic applications in C.

The application consists of three separate libraries: 
### `engine` 
A generic application launcher and subsystems manager, including lots of utility headers for robust C programming (string, memory, platform abstraction, etc.). It is heavily based on early boilerplate tutorials from Travis Vroman's *Kohi Game Engine* YouTube series.
### `src`
The chess application, including the chess engine subsystem. Chess engine is heavily based on tutorials from Code_Monkey_King's *Bitboard Chess Engine in C* YouTube series.
### `test`
A generic unit testing subsystem. It is heavily based on tutorials from Travis Vroman's *Kohi Game Engine* YouTube series.


## Build

Requires GNU make.

Currently works on Unix only. MS/Windows build coming soon...I just have to finish testing the Windows platform layer.

To build and launch: 
```
> make all
```
To run unit tests, then build and launch:
```
> make test
```
To compile and launch the application separately:
```
> make app
> bin/chess
```
