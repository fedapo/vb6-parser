//: color_console.hpp

// vb6_parser
// Copyright (c) 2022 Federico Aponte
// This code is licensed under GNU Software License (see LICENSE.txt for details)

#pragma once

#include <cstdint>
#include <iostream>

struct color_manip
{
  uint8_t color;

  explicit color_manip(uint8_t c) : color(c) {}
};

color_manip setcolor(uint8_t col);
std::ostream& operator<<(std::ostream&, color_manip c);

std::ostream& tag_ok(std::ostream&);
std::ostream& tag_fail(std::ostream&);

void wait_key_pressed();