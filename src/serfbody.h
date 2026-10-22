/*
 * serfbody.h - torso and head sprites of the serf animations
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

#ifndef SRC_SERFBODY_H_
#define SRC_SERFBODY_H_

#include <cstddef>

// The sprite of an animation phase is turned into torso and head sprites
// the way freeserf draws a serf (Viewport::serf_get_body() and
// Viewport::draw_row_serf()): an offset depending on the serf type is added
// to it, one for walking (sprite < 0x80) and one for working.
namespace FSSSerfBody {

struct Type {
  const char *name;
  int walk;
  int work;
};

size_t type_count();
const Type &type(size_t index);

// Torso and head sprites of the phase sprite for the serf type; head is -1
// when there is no head. Returns false when the sprite is out of the tables.
bool sprites(int phase_sprite, size_t type, int *torso, int *head);

}  // namespace FSSSerfBody

#endif  // SRC_SERFBODY_H_
