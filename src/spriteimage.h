/*
 * spriteimage.h - sprite image helpers declaration
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

#ifndef SRC_SPRITEIMAGE_H_
#define SRC_SPRITEIMAGE_H_

#include <string>

#include "src/data.h"

struct SDL_Surface;

namespace FSSSpriteImage {

// Sprite pixels are ARGB8888, the returned surface owns a copy of them.
// The caller destroys it with SDL_DestroySurface().
SDL_Surface *create_surface(Data::PSprite sprite, unsigned int scale = 1);

bool save_png(Data::PSprite sprite, const std::string &path,
              unsigned int scale = 1);
bool copy_to_clipboard(Data::PSprite sprite);

}  // namespace FSSSpriteImage

#endif  // SRC_SPRITEIMAGE_H_
