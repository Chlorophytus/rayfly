/// rayfly: terragen
// Copyright (c) 2021-2022 Roland Metivier <metivier.roland@chlorophyt.us>
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
#include "../include/rayfly_terrain.hpp"
#include "../include/rayfly_util.hpp"
using namespace rayfly;

static auto random_seed = U32{0};

// https://en.wikipedia.org/wiki/Xorshift
U32 xorshift32(const U32 seed) {
  auto point = seed;
  point ^= point << 13;
  point ^= point >> 17;
  point ^= point << 5;
  return point;
}

void terrain::init() {
  auto random_dev = std::random_device{};
  random_seed = random_dev();
  std::fprintf(stderr, "with terragen seed %u\n", random_seed);
}

Model terrain::chunk(const S32 x, const S32 z) {
  auto image = GenImageColor(HEIGHTMAP_SIZE, HEIGHTMAP_SIZE, WHITE);
  ImageFormat(&image, PIXELFORMAT_UNCOMPRESSED_GRAYSCALE);
  for (auto i = 0; i < HEIGHTMAP_SIZE; i++) {
    for (auto j = 0; j < HEIGHTMAP_SIZE; j++) {
      auto xx = static_cast<U32>(((x * HEIGHTMAP_SIZE) - i));
      auto zz = static_cast<U32>(((z * HEIGHTMAP_SIZE) - j));
      reinterpret_cast<U8 *>(image.data)[(i * HEIGHTMAP_SIZE) + j] =
          static_cast<U8>(xorshift32(random_seed * (xx ^ zz)));
    }
  }
  ImageResize(&image, HEIGHTMAP_SCALE * HEIGHTMAP_SIZE,
              HEIGHTMAP_SCALE * HEIGHTMAP_SIZE);
  auto mesh =
      GenMeshHeightmap(image, Vector3{HEIGHTMAP_SCALE, 0.5f, HEIGHTMAP_SCALE});
  auto tex = LoadTextureFromImage(image);
  auto model = LoadModelFromMesh(mesh);
  model.materials[0].maps[MATERIAL_MAP_ALBEDO].texture = tex;
  UnloadImage(image);
  return model;
}