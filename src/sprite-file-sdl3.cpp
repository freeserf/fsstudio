/*
 * sprite-file-sdl3.cpp - sprite file loading with SDL3
 *
 * Copyright (C) 2026  Wicked_Digger <wicked_digger@mail.ru>
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

#include "src/sprite-file.h"

#include <SDL3/SDL.h>

#include <cstring>

SpriteFile::SpriteFile() {
}

bool
SpriteFile::load(const std::string &path) {
  // SDL3 loads BMP and PNG files
  SDL_Surface *image = SDL_LoadSurface(path.c_str());
  if (image == nullptr) {
    return false;
  }
  SDL_Surface *surface = SDL_ConvertSurface(image, SDL_PIXELFORMAT_ARGB8888);
  SDL_DestroySurface(image);
  if (surface == nullptr) {
    return false;
  }

  create(surface->w, surface->h);
  for (int y = 0; y < surface->h; y++) {
    memcpy(data + y * surface->w * 4,
           static_cast<uint8_t*>(surface->pixels) + y * surface->pitch,
           surface->w * 4);
  }
  SDL_DestroySurface(surface);

  return true;
}

bool
SpriteFile::load_resource(const std::string &name) {
  // resources folder next to the program (in the bundle resources on macOS)
  const char *base = SDL_GetBasePath();
  return base != nullptr && load(std::string(base) + "resources/" + name);
}
