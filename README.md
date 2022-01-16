# vb6-parser

A parsing engine for Microsoft's Visual Basic 6 programming language based on the Boost.Spirit library.

## Introduction

This is a parser for the [Visual Basic 6](https://en.wikipedia.org/wiki/Visual_Basic_(classic)) programming language, implemented using the [Boost Spirit x3](https://www.boost.org/doc/libs/develop/libs/spirit/doc/x3/html/index.html) library.

https://github.com/fedapo/vb6-parser.git

## Building with CMake

```shell
> mkdir build
> cmake . -B build/
> cmake --build build/
```

## Building with Microsoft Visual Studio

Open the file `vb_parser.sln` with Microsoft Visual Studio. Build all.

## Usage

The project produces a library, vb_parser_lib, used for the moment only by two executables.

- vb6_parser
- vb6_parser_test

These run a series of tests to ensure the parser runs correctly.

## History

2022-01-15 First commit in a public Github repository.

## Acknowledgements

- Boost Spirit X3
- GTest
- CMake
- GCC
- Clang
- MSYS2
- Vcpkg
- The C++ Committee
