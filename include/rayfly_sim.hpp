/// rayfly: simulation include
// Copyright (c) 2021-2023 Roland Metivier <metivier.roland@chlorophyt.us>
//
// Permission to use, copy, modify, and distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
// WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
// ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
// ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
// OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
#pragma once
#include "rayfly.hpp"
namespace rayfly {
namespace sim {

struct terrain_data {
  S32 x;
  Model y;
  S32 z;
};

std::vector<F32> get_joystick_axes(const U8);
void init(const U16, const U16, const U16, const bool);
void tick(U8 &);
void deinit();
} // namespace sim
} // namespace rayfly