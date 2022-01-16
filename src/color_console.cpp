//: color_console.cpp

// vb6_parser
// Copyright (c) 2022 Federico Aponte
// This code is licensed under GNU Software License (see LICENSE.txt for details)

#include "color_console.hpp"

#ifdef _MSC_VER
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <iomanip>
#include <iostream>

/*
  bit 0 - foreground blue
  bit 1 - foreground green
  bit 2 - foreground red
  bit 3 - foreground intensity

  bit 4 - background blue
  bit 5 - background green
  bit 6 - background red
  bit 7 - background intensity
*/

color_manip setcolor(uint8_t col)
{
  return color_manip(col);
}

std::ostream& operator<<(std::ostream& os, color_manip c)
{
#ifdef _MSC_VER
  HANDLE hstdout = GetStdHandle(STD_OUTPUT_HANDLE);

  //SetConsoleMode(STD_OUTPUT_HANDLE,  ENABLE_VIRTUAL_TERMINAL_INPUT);

  // restore console state
  //SetConsoleTextAttribute(hstdout, csbi.wAttributes);

  // save console state
  //CONSOLE_SCREEN_BUFFER_INFO csbi;
  //GetConsoleScreenBufferInfo(hstdout, &csbi);

  SetConsoleTextAttribute(hstdout, c.color);
#else
  // ANSI escape codes could also be used in some terminals
  switch(c.color)
  {
    case 0: os << std::setfill('c') << "\x1b[30;1m"; break; // bold;black
    case 1: os << "\x1b[34;1m"; break; // bold;blue
    case 2: os << "\x1b[32;1m"; break; // bold;green
    case 3: os << "\x1b[36;1m"; break; // bold;cyan
    case 4: os << "\x1b[31;1m"; break; // bold;red
    case 5: os << "\x1b[35;1m"; break; // bold;magenta
    case 14: os << "\x1b[33;1m"; break; // bold;yellow
    case 15: os << "\x1b[37;1m"; break; // bold;white
    case 7: os << "\x1b[0m"; break; // reset
  }
#endif

  return os;
}

std::ostream& tag_ok(std::ostream& os)
{
  os << "[" << setcolor(0x02) << "ok" << setcolor(0x07) << "]";
  return os;
}

std::ostream& tag_fail(std::ostream& os)
{
  os << "[" << setcolor(0x04) << "fail" << setcolor(0x07) << "]";
  return os;
}

void wait_key_pressed()
{
#ifdef _MSC_VER
  HANDLE hstdin = GetStdHandle(STD_INPUT_HANDLE);

  // wait for a key to be pressed
  WaitForSingleObject(hstdin, INFINITE);

  // what's this for? removes all characters input by the users?
  FlushConsoleInputBuffer(hstdin);
#endif
}