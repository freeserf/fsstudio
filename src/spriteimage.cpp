/*
 * spriteimage.cpp - sprite image helpers implementation
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

#include "src/spriteimage.h"

#include <SDL3/SDL.h>

#include <cstring>
#include <vector>

SDL_Surface *
FSSSpriteImage::create_surface(Data::PSprite sprite, unsigned int scale) {
  if (!sprite || sprite->get_width() == 0 || sprite->get_height() == 0) {
    return nullptr;
  }

  int width = static_cast<int>(sprite->get_width());
  int height = static_cast<int>(sprite->get_height());
  SDL_Surface *surface = SDL_CreateSurface(width, height,
                                           SDL_PIXELFORMAT_ARGB8888);
  if (surface == nullptr) {
    return nullptr;
  }
  const uint8_t *src = sprite->get_data();
  for (int y = 0; y < height; y++) {
    memcpy(static_cast<uint8_t*>(surface->pixels) + y * surface->pitch,
           src + y * width * 4, width * 4);
  }

  if (scale > 1) {
    SDL_Surface *scaled = SDL_ScaleSurface(surface, width * scale,
                                           height * scale,
                                           SDL_SCALEMODE_NEAREST);
    SDL_DestroySurface(surface);
    surface = scaled;
  }

  return surface;
}

bool
FSSSpriteImage::save_png(Data::PSprite sprite, const std::string &path,
                         unsigned int scale) {
  SDL_Surface *surface = create_surface(sprite, scale);
  if (surface == nullptr) {
    return false;
  }
  bool result = SDL_SavePNG(surface, path.c_str());
  if (!result) {
    SDL_Log("Failed to save '%s': %s", path.c_str(), SDL_GetError());
  }
  SDL_DestroySurface(surface);
  return result;
}

namespace {

const char *png_mime_types[] = { "image/png" };

const void * SDLCALL
clipboard_data(void *userdata, const char *mime_type, size_t *size) {
  std::vector<uint8_t> *png = static_cast<std::vector<uint8_t>*>(userdata);
  if (mime_type == nullptr || strcmp(mime_type, png_mime_types[0]) != 0) {
    *size = 0;
    return nullptr;
  }
  *size = png->size();
  return png->data();
}

void SDLCALL
clipboard_cleanup(void *userdata) {
  delete static_cast<std::vector<uint8_t>*>(userdata);
}

}  // namespace

bool
FSSSpriteImage::copy_to_clipboard(Data::PSprite sprite) {
  SDL_Surface *surface = create_surface(sprite);
  if (surface == nullptr) {
    return false;
  }

  SDL_IOStream *stream = SDL_IOFromDynamicMem();
  bool result = (stream != nullptr) && SDL_SavePNG_IO(surface, stream, false);
  SDL_DestroySurface(surface);
  if (!result) {
    if (stream != nullptr) {
      SDL_CloseIO(stream);
    }
    return false;
  }

  const uint8_t *data = static_cast<const uint8_t*>(
                  SDL_GetPointerProperty(SDL_GetIOProperties(stream),
                                         SDL_PROP_IOSTREAM_DYNAMIC_MEMORY_POINTER,
                                         nullptr));
  std::vector<uint8_t> *png = new std::vector<uint8_t>(
                                           data, data + SDL_GetIOSize(stream));
  SDL_CloseIO(stream);

  return SDL_SetClipboardData(clipboard_data, clipboard_cleanup, png,
                              png_mime_types, 1);
}
