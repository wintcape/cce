# cce
A bitboard chess engine.

## About.
This is an experiment in improving my skills at programming basic applications in C.

The application consists of three separate libraries: 

### `engine` 
A generic application launcher and subsystems manager, including lots of utility headers for robust C programming (string, memory, platform abstraction, etc.). It is heavily based on early boilerplate tutorials from Travis Vroman's *Kohi Game Engine* YouTube series.

### `src`
The chess application, including the chess engine subsystem. Chess engine is heavily based on tutorials from Code_Monkey_King's *Bitboard Chess Engine in C* YouTube series.

Right now, it cannot calculate the "best move" from a list of valid chess moves; it merely validates if a given move is valid and performs a random move each ply; added functionality to have it actually evaluate before playing is coming very soon.

### `test`
A generic unit testing subsystem. It is heavily based on tutorials from Travis Vroman's *Kohi Game Engine* YouTube series.

## Running the executable.
Executables are located in `bin` directory. From a command console, launch `cce` if platform is GNU/Linux, or `cce.exe` if platform is Windows.

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
> bin/chess

       or

> make windows
> bin/chess.exe
```