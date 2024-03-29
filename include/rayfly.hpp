/// rayfly: main include
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
#include "../include/rayfly_cfg.hpp"
// =============================================================================
// EXTERNAL INCLUDE HEADERS
// =============================================================================
#include <GLFW/glfw3.h>
#include <bits/stdc++.h>
#include <glm/glm.hpp>
#include <raylib.h>
// =============================================================================
// TYPEDEFS
// =============================================================================
using U8 = std::uint8_t;
using U16 = std::uint16_t;
using U32 = std::uint32_t;
using U64 = std::uint64_t;

using S8 = std::int8_t;
using S16 = std::int16_t;
using S32 = std::int32_t;
using S64 = std::int64_t;

using F32 = float;
using F64 = double;
// =============================================================================
// CONSTANTS
// =============================================================================
constexpr static auto RAD2DEGS = 360 / (2 * M_PIf32);
constexpr static auto HEIGHTMAP_SIZE = 16;
constexpr static auto HEIGHTMAP_SCALE = 16;