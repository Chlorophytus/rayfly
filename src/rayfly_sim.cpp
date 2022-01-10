/// rayfly: simulation
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
#include "../include/rayfly_sim.hpp"
#include "../include/rayfly_util.hpp"
using namespace rayfly;

static auto camera = Camera3D{.position = Vector3{.x = 5, .y = 5, .z = 0},
                              .target = Vector3{.x = 0, .y = 0, .z = 0},
                              .up = Vector3{.x = 0, .y = 1, .z = 0},
                              .fovy = 110.0f,
                              .projection = CAMERA_PERSPECTIVE};

static auto controls_angular = glm::mat3{0.0f};
static auto controls_linear = glm::vec3{0.0f};
static auto controls_thrust = 0.0f;

static auto velocity_angular = glm::mat3{0.0f};
static auto velocity_linear = glm::vec3{0.0f};

static auto position = glm::vec3{0.0f};

static auto init_ok = false;
static auto init_height = 0;
static auto init_width = 0;

Vector2 raylib_cast2(glm::vec2 v) { return Vector2{.x = v.x, .y = v.y}; }

Vector3 raylib_cast3(glm::vec3 v) {
  return Vector3{.x = v.x, .y = v.y, .z = v.z};
}

std::vector<F32> sim::get_joystick_axes(const U8 joystick_idx) {
  if (glfwJoystickPresent(joystick_idx)) {
    auto axis_count = S32{0};
    auto axis_carray = glfwGetJoystickAxes(joystick_idx, &axis_count);
    auto i = 0;
    auto axis_vector = std::vector<F32>{};
    axis_vector.resize(axis_count);
    for (auto &&axis : axis_vector) {
      axis = axis_carray[i];
      i++;
    }
    return axis_vector;
  } else {
    throw std::runtime_error{"sim::get_joystick_axes - joystick not present"};
  }
}

void sim::init(const U16 width, const U16 height, const U16 fps_target = 60,
               const bool fullscreen = false) {
  if (!init_ok) {
    init_width = width;
    init_height = height;
    InitWindow(width, height,
               (std::string{"rayfly "} + rayfly_VSTRING_FULL).c_str());
    SetTargetFPS(fps_target);
    // This is an XOR, it is useful here.
    if (fullscreen ^ IsWindowFullscreen()) {
      ToggleFullscreen();
    }

    init_ok = true;
  } else {
    throw std::runtime_error{
        "sim::init - already initialized, no need to do it again"};
  }
}

void sim::tick(U8 &joystick) {
  auto axes = sim::get_joystick_axes(joystick);

  auto controls_raw = glm::vec3{axes.at(5), -axes.at(1), -axes.at(0)};
  controls_linear *= glm::vec3{1.0f, 0.999f, 0.999f};
  controls_linear -=
      (controls_raw * M_PI_2f32) * glm::vec3{0.01f, 0.02f, 0.005f};

  util::wrap(controls_linear, glm::vec3{-M_PIf32}, glm::vec3{M_PIf32});

  auto controls_angular_yaw = glm::mat3{std::cos(controls_linear.x),
                                        0.0f,
                                        std::sin(controls_linear.x),
                                        0.0f,
                                        1.0f,
                                        0.0f,
                                        -std::sin(controls_linear.x),
                                        0.0f,
                                        std::cos(controls_linear.x)};

  auto controls_angular_pitch = glm::mat3{std::cos(controls_linear.y),
                                          -std::sin(controls_linear.y),
                                          0.0f,
                                          std::sin(controls_linear.y),
                                          std::cos(controls_linear.y),
                                          0.0f,
                                          0.0f,
                                          0.0f,
                                          1.0f};

  auto controls_angular_roll = glm::mat3{1.0f,
                                         0.0f,
                                         0.0f,
                                         0.0f,
                                         std::cos(controls_linear.z),
                                         -std::sin(controls_linear.z),
                                         0.0f,
                                         std::sin(controls_linear.z),
                                         std::cos(controls_linear.z)};

  controls_angular =
      controls_angular_roll * controls_angular_pitch * controls_angular_yaw;

  controls_thrust = std::min(
      controls_thrust * 0.9975f + ((1.0f - axes.at(2)) * 0.00001f), 0.01f);
  velocity_linear +=
      glm::vec3{controls_thrust, 0.0f, 0.0f} * controls_angular * 1.005f;

  auto gravity = glm::smoothstep(glm::vec3{0.0f, 0.0f, 0.0f},
                                 glm::vec3{0.01f, 1.0f, 0.01f},
                                 glm::vec3{0.0f, position.y, 0.0f});

  gravity *= 0.001f;
  velocity_linear -= gravity;
  velocity_linear *= 0.75f;
  position += velocity_linear;
  auto up = glm::vec3{0.0f, 1.0f, 0.0f} * controls_angular;
  auto cam = glm::vec3{1.0f, 0.0f, 0.0f} * controls_angular;

  camera.up = raylib_cast3(up);
  camera.target = raylib_cast3(position + up);
  camera.position = raylib_cast3(position - cam + up);
  BeginDrawing();
  ClearBackground(RAYWHITE);
  BeginMode3D(camera);
  DrawGrid(10, 1.0f);
  DrawRay(
      Ray{.position = Vector3{0}, .direction = Vector3{.x = 1, .y = 0, .z = 0}},
      RED);
  DrawRay(
      Ray{.position = Vector3{0}, .direction = Vector3{.x = 0, .y = 1, .z = 0}},
      GREEN);
  DrawRay(
      Ray{.position = Vector3{0}, .direction = Vector3{.x = 0, .y = 0, .z = 1}},
      BLUE);
  DrawTriangle3D(
      raylib_cast3(position + (glm::vec3{0.0f, 0.0f, 0.5f} * controls_angular)),
      raylib_cast3(position + (glm::vec3{1.0f, 0.0f, 0.0f} * controls_angular)),
      raylib_cast3(position +
                   (glm::vec3{0.0f, 0.0f, -0.5f} * controls_angular)),
      GRAY);

  EndMode3D();

  // Attitude indicator
  auto attitude =
      glm::mat2{std::cos(controls_linear.z), -std::sin(controls_linear.z),
                std::sin(controls_linear.z), std::cos(controls_linear.z)};
  auto attitude_pos = glm::vec2(96.0f, init_height - 128.0f);
  auto pitch = controls_linear.y;
  util::wrap(pitch, -M_PI_4f32, M_PI_4f32);
  DrawCircle(attitude_pos.x, attitude_pos.y, 64.0f,
             ((std::abs(controls_linear.z) > (M_PI_4f32 * 1.5f)) ||
                      ((std::abs(controls_linear.y) > (M_PI_4f32 * 1.5f)))
                  ? RED
                  : BLACK));
  DrawLineV(raylib_cast2(attitude_pos + (glm::vec2(-48.0f, 0.0f) * attitude)),
            raylib_cast2(attitude_pos + (glm::vec2(-32.0f, 0.0f) * attitude)),
            GOLD);
  DrawLineV(raylib_cast2(attitude_pos + (glm::vec2(32.0f, 0.0f) * attitude)),
            raylib_cast2(attitude_pos + (glm::vec2(48.0f, 0.0f) * attitude)),
            GOLD);
  DrawLineV(
      raylib_cast2(attitude_pos +
                   (glm::vec2(-24.0f, pitch * 24.0f / M_PI_4f32) * attitude)),
      raylib_cast2(attitude_pos +
                   (glm::vec2(24.0f, pitch * 24.0f / M_PI_4f32) * attitude)),
      GOLD);
  DrawTextPro(GetFontDefault(),
              std::to_string(
                  static_cast<S16>(std::round((controls_linear.z) * RAD2DEGS)))
                  .c_str(),
              raylib_cast2(attitude_pos),
              raylib_cast2(glm::vec2{-72.0f, 10.0f}),
              (controls_linear.z) * RAD2DEGS, 20, 1.0f, GRAY);
  DrawTextPro(GetFontDefault(),
              std::to_string(
                  static_cast<S16>(std::round((controls_linear.y) * RAD2DEGS)))
                  .c_str(),
              raylib_cast2(attitude_pos), raylib_cast2(glm::vec2{5.0f, 10.0f}),
              (controls_linear.z) * RAD2DEGS, 20, 1.0f, RAYWHITE);
  // Altimeter
  auto altimeter = glm::mat2{std::cos(position.y), -std::sin(position.y),
                             std::sin(position.y), std::cos(position.y)};
  auto heading =
      glm::mat2{std::cos(controls_linear.x), -std::sin(controls_linear.x),
                std::sin(controls_linear.x), std::cos(controls_linear.x)};
  auto altimeter_pos = glm::vec2(320.0f, init_height - 128.0f);
  DrawCircle(altimeter_pos.x, altimeter_pos.y, 64.0f, BLACK);
  DrawLineV(raylib_cast2(altimeter_pos),
            raylib_cast2(altimeter_pos + (glm::vec2(0.0f, -48.0f) * altimeter)),
            GOLD);
  DrawTextPro(
      GetFontDefault(),
      std::to_string(static_cast<S16>(std::round((position.y) * 100.0f)))
          .c_str(),
      raylib_cast2(altimeter_pos), raylib_cast2(glm::vec2{10.0f, 10.0f}), 0, 20,
      1.0f, RAYWHITE);
  DrawTextPro(GetFontDefault(),
              std::to_string(
                  static_cast<S16>(std::round((controls_linear.x * RAD2DEGS))))
                  .c_str(),
              raylib_cast2(altimeter_pos),
              raylib_cast2(glm::vec2{-72.0f, 10.0f}),
              ((controls_linear.x - M_PI_2f32) * RAD2DEGS), 20, 1.0f, GRAY);
  DrawTextEx(GetFontDefault(), "Attitude",
             raylib_cast2(attitude_pos - glm::vec2(36, 128)), 20, 1.0f, GRAY);
  DrawTextEx(GetFontDefault(), "Alt./Heading",
             raylib_cast2(altimeter_pos - glm::vec2(56, 128)), 20, 1.0f, GRAY);
  EndDrawing();
}

void sim::deinit() {
  CloseWindow();
  init_ok = false;
}