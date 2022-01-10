/// rayfly: main source
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
#include "../include/rayfly.hpp"
#include "../include/rayfly_cfg.hpp"

constexpr static auto WIDTH = 1280;
constexpr static auto HEIGHT = 720;
constexpr static auto RAD2DEGS = 360 / (2 * M_PIf32);

// https://stackoverflow.com/a/29871193
void wrap(F32 &num, F32 min, F32 max) {
  const auto maxmin = max - min;
  num = min + std::fmod(maxmin + std::fmod(num - min, maxmin), maxmin);
}

Vector3 raylib_cast(glm::vec3 v) { return Vector3{.x=v.x, .y=v.y, .z=v.z}; }

int main(int argc, char **argv) {
  auto status = EXIT_FAILURE;

  try {
    InitWindow(
        WIDTH, HEIGHT,
        (std::string{"rayfly "} + rayfly_VSTRING_FULL).c_str());
    SetTargetFPS(60);
    if (!IsWindowFullscreen()) {
      ToggleFullscreen();
    }
    auto camera = Camera3D{
      .position = Vector3{.x = 5, .y = 5, .z = 0},
      .target = Vector3{.x = 0, .y = 0, .z = 0},
      .up = Vector3{.x = 0, .y = 1, .z = 0},
      .fovy = 110.0f,
      .projection = CAMERA_PERSPECTIVE
    };


    auto position = glm::vec3{0.0f, 1.0f, 0.0f};
    auto velocity = glm::vec3{0.0f, 0.0f, 0.0f};
    auto controlZ = glm::mat3{1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
    auto controlY = glm::mat3{1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
    auto controlX = glm::mat3{1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};

    auto control_yaw = 0.0f; // z
    auto control_pitch = 0.0f; // y'
    auto control_roll = 0.0f; // x''
    auto control_mag = 0.0f;
    while (!WindowShouldClose()) {
      BeginDrawing();
      ClearBackground(RAYWHITE);
      const auto js = 0;
      // HACK: VERY UGLY but low-level GLFW calls are supported in Raylib!
      if(glfwJoystickPresent(js)) {
        auto axis_count = S32{0};
        auto axis = glfwGetJoystickAxes(js, &axis_count); 		
        std::fprintf(stderr, "detected %u axes\n", axis_count);
        for(auto i = 0; i < axis_count; i++) {
          std::fprintf(stderr, "%u: %f\n", i, axis[i]);
        }
        // delta_roll = std::clamp(std::fmod(delta_roll + (axis[5] * (2.0f * M_PIf32) * 0.0005f), M_PIf32 * 2.0f), -0.1f, 0.1f);
        // delta_yaw = std::fmod(delta_yaw + (axis[0] * (2.0f * M_PIf32) * 0.005f), M_PIf32 * 2.0f);
        // delta_pitch = std::fmod(delta_pitch + (axis[1] * (2.0f * M_PIf32) * 0.005f), M_PIf32 * 2.0f);
        
        control_mag = std::min(control_mag * 0.995f + ((1.0f - axis[2]) * 0.00001f), 0.005f);
        control_roll *= 0.99f;
        control_pitch *= 0.99f;
        control_yaw *= 0.99f;
        control_roll -= 0.001f * (axis[0] * M_PI_2f32); // z
        control_pitch -= 0.001f * (axis[1] * M_PI_2f32); // y'
        control_yaw += 0.002f * (axis[5] * M_PI_2f32); // x''
        wrap(control_yaw, -M_PIf32, M_PIf32);
        wrap(control_pitch, -M_PIf32, M_PIf32);
        wrap(control_roll, -M_PIf32, M_PIf32);

        controlX = glm::mat3{
          1.0f, 0.0f, 0.0f,
          0.0f, std::cos(control_yaw), -std::sin(control_yaw),
          0.0f, std::sin(control_yaw), std::cos(control_yaw)
        };

        controlY = glm::mat3{
          std::cos(control_roll), 0.0f, std::sin(control_roll),
          0.0f, 1.0f, 0.0f, 
          -std::sin(control_roll), 0.0f, std::cos(control_roll)
        };

        controlZ = glm::mat3{
          std::cos(control_pitch), -std::sin(control_pitch), 0.0f,
          std::sin(control_pitch), std::cos(control_pitch), 0.0f,
          0.0f, 0.0f, 1.0f
        };
        velocity -= glm::vec3{control_mag, 0.0f, 0.0f} * (controlZ * controlY * controlX);
        if(position.x > 0.0f) { velocity -= glm::vec3{0.0f, 0.00001f, 0.0f}; }
        velocity *= 0.75f;
        position += velocity;
      }
      camera.up = raylib_cast(position - (glm::normalize(velocity) * (controlZ * controlY * controlX)));
      camera.position = raylib_cast(position - glm::normalize(velocity));
      camera.target = raylib_cast(position);
      
      DrawText((std::string{"Cmag   "} + std::to_string(control_mag)).c_str(), 50, 50, 20, GRAY);
      DrawText((std::string{"Cpitch "} + std::to_string(control_pitch * RAD2DEGS) + "degs").c_str(), 50, 75, 20, GRAY);
      DrawText((std::string{"Cyaw   "} + std::to_string(control_yaw * RAD2DEGS) + "degs").c_str(), 50, 100, 20, GRAY);
      DrawText((std::string{"Croll  "} + std::to_string(control_roll * RAD2DEGS) + "degs").c_str(), 50, 125, 20, GRAY);
      DrawText((std::string{"Vmag   "} + std::to_string(glm::length(velocity))).c_str(), 50, 150, 20, GRAY);
      DrawLineV(Vector2{.x = 96, .y = HEIGHT - 96},
                Vector2{
                  .x = 96 + (std::sin(control_roll - M_PI_2f32) * 64.0f),
                  .y = HEIGHT - 96 + (std::cos(control_roll - M_PI_2f32) * 64.0f)},
                GRAY); 
      DrawLineV(Vector2{.x = 96, .y = HEIGHT - 96},
                Vector2{
                  .x = 96 + (std::sin(control_roll + M_PI_2f32) * 64.0f),
                  .y = HEIGHT - 96 + (std::cos(control_roll + M_PI_2f32) * 64.0f)},
                GRAY); 
      DrawLineV(Vector2{.x = 96, .y = HEIGHT - 96},
                Vector2{
                  .x = 96 + (std::sin(control_roll - M_PIf32) * 32.0f),
                  .y = HEIGHT - 96 + (std::cos(control_roll - M_PIf32) * 32.0f)},
                BLACK); 
      BeginMode3D(camera);
      DrawGrid(10, 1.0f);
      const auto o = Vector3{0};
      DrawRay(Ray{.position = o, .direction = Vector3{.x = 1, .y = 0, .z = 0}}, RED);
      DrawRay(Ray{.position = o, .direction = Vector3{.x = 0, .y = 1, .z = 0}}, GREEN);
      DrawRay(Ray{.position = o, .direction = Vector3{.x = 0, .y = 0, .z = 1}}, BLUE);
      DrawLine3D(raylib_cast(position), raylib_cast(position + velocity), BLACK);
      EndMode3D();
      EndDrawing();
    }
    CloseWindow();
    std::fprintf(stderr, "bye\n");
    status = EXIT_SUCCESS;
  } catch (const std::exception &e) {
    std::fprintf(stderr, "FATAL: %s\n", e.what());
  }

  return status;
}