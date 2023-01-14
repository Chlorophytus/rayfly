/// rayfly: utility include
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
namespace util {
// https://stackoverflow.com/a/29871193
template <typename T> void wrap(T &num, T min, T max) {
  const auto maxmin = max - min;
  num = min + glm::mod(maxmin + glm::mod(num - min, maxmin), maxmin);
}
} // namespace util
} // namespace rayfly