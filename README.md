# codesim

## Requirements

- `libllvm6.0`
- `libclang-6.0-dev`
- `cmake # >= 3.14`

## Build & Run

- Build: `$ make`
- Run: `$ ./codesim [-h|--help] [-v|--verbose] code1.cpp code2.cpp`

## Reference

The code similarity computation algorithm originates from
>Lutz Prechelt, Guido Malpohl, and Michael Philippsen. Finding Plagiarisms among a Set of Programs with JPlag. Journal of Universal Computer Science, vol. 8, no. 11 (2002), 1016-1038.

`CMakeLists.txt` and `Config.cmake` are copies of [CMakeLists.txt](https://github.com/wierton/codesim/blob/master/CMakeLists.txt) and [Config.cmake](https://github.com/wierton/codesim/blob/master/Config.cmake). Thanks to Wierton!
