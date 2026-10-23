/*
 * main.cpp - FSStudio entry point
 *
 * Copyright (C) 2016-2026  Wicked_Digger <wicked_digger@mail.ru>
 *
 * This file is part of FSStudio.
 *
 * FSStudio is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * FSStudio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with FSStudio.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <memory>
#include <string>

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

#include "src/application.h"
#include "src/filedialog.h"

namespace {

// Without input for this long the app waits for events instead of redrawing
// continuously.
const Uint64 idle_after_ms = 1000;
const Sint32 idle_wait_ms = 100;

// With --screenshot the window is saved after this many frames, when the
// layout has settled.
const int screenshot_frame = 10;

bool
save_screenshot(SDL_Renderer *renderer, const std::string &path) {
  SDL_Surface *surface = SDL_RenderReadPixels(renderer, nullptr);
  if (surface == nullptr) {
    SDL_Log("Failed to read the window: %s", SDL_GetError());
    return false;
  }
  bool result = SDL_SavePNG(surface, path.c_str());
  if (!result) {
    SDL_Log("Failed to save '%s': %s", path.c_str(), SDL_GetError());
  }
  SDL_DestroySurface(surface);
  return result;
}

}  // namespace

int
main(int argc, char *argv[]) {
  // --select <resource>:<index>  open with the resource selected
  // --screenshot <file.png>      save the window to a file and exit
  std::string select_resource;
  std::string screenshot_path;
  for (int i = 1; i + 1 < argc; i++) {
    std::string arg = argv[i];
    if (arg == "--select") {
      select_resource = argv[++i];
    } else if (arg == "--screenshot") {
      screenshot_path = argv[++i];
    }
  }

  SDL_SetAppMetadata("FreeSerf Studio", nullptr, "org.freeserf.fsstudio");
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
    SDL_Log("SDL_Init failed: %s", SDL_GetError());
    return 1;
  }

  float scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
  if (scale <= 0.f) {
    scale = 1.f;
  }
  SDL_WindowFlags flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN |
                          SDL_WINDOW_HIGH_PIXEL_DENSITY;
  SDL_Window *window = SDL_CreateWindow("FreeSerf Studio",
                                        static_cast<int>(1280 * scale),
                                        static_cast<int>(800 * scale), flags);
  if (window == nullptr) {
    SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
    return 1;
  }
  SDL_Renderer *renderer = SDL_CreateRenderer(window, nullptr);
  if (renderer == nullptr) {
    SDL_Log("SDL_CreateRenderer failed: %s", SDL_GetError());
    return 1;
  }
  SDL_SetRenderVSync(renderer, 1);
  SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED,
                        SDL_WINDOWPOS_CENTERED);
  SDL_ShowWindow(window);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  // settings are kept in the user preferences folder
  std::string ini_path;
  char *pref_path = SDL_GetPrefPath("freeserf", "fsstudio");
  if (pref_path != nullptr) {
    ini_path = std::string(pref_path) + "fsstudio.ini";
    SDL_free(pref_path);
    io.IniFilename = ini_path.c_str();
  } else {
    io.IniFilename = nullptr;
  }

  ImGui::StyleColorsDark();
  ImGuiStyle &style = ImGui::GetStyle();
  style.FrameRounding = 3.f;
  style.GrabRounding = 3.f;
  style.ScaleAllSizes(scale);
  style.FontScaleDpi = scale;

  ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
  ImGui_ImplSDLRenderer3_Init(renderer);
  FSSFileDialog::set_window(window);

  std::unique_ptr<FSSApplication> app =
                                     std::make_unique<FSSApplication>(renderer);
  app->init();
  if (!select_resource.empty() && !app->select(select_resource)) {
    SDL_Log("Unknown resource '%s'", select_resource.c_str());
  }
  if (!screenshot_path.empty()) {
    io.IniFilename = nullptr;  // don't overwrite the user settings
  }

  Uint64 last_input = SDL_GetTicks();
  int frame = 0;
  bool done = false;
  while (!done) {
    SDL_Event event;
    bool idle = (SDL_GetTicks() - last_input > idle_after_ms) &&
                !app->is_playing_audio();
    bool have_event = idle ? SDL_WaitEventTimeout(&event, idle_wait_ms)
                           : SDL_PollEvent(&event);
    while (have_event) {
      last_input = SDL_GetTicks();
      ImGui_ImplSDL3_ProcessEvent(&event);
      if (event.type == SDL_EVENT_QUIT ||
          (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED &&
           event.window.windowID == SDL_GetWindowID(window))) {
        done = true;
      }
      have_event = SDL_PollEvent(&event);
    }
    if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED) {
      SDL_Delay(10);
      continue;
    }

    FSSFileDialog::process();

    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
    app->draw();
    if (app->is_quit_requested()) {
      done = true;
    }
    ImGui::Render();

    SDL_SetRenderScale(renderer, io.DisplayFramebufferScale.x,
                       io.DisplayFramebufferScale.y);
    SDL_SetRenderDrawColorFloat(renderer, .1f, .1f, .1f, 1.f);
    SDL_RenderClear(renderer);
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
    if (!screenshot_path.empty() && ++frame == screenshot_frame) {
      save_screenshot(renderer, screenshot_path);
      done = true;
    }
    SDL_RenderPresent(renderer);
  }

  // settings are saved by ImGui on shutdown, the app must still exist then
  ImGui_ImplSDLRenderer3_Shutdown();
  ImGui_ImplSDL3_Shutdown();
  ImGui::DestroyContext();
  // textures of the views must be destroyed before the renderer
  app.reset();

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
