/*
 * serfbody.cpp - torso and head sprites of the serf animations
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

#include "src/serfbody.h"

namespace {

// The offsets of the usual states of each serf type, from
// Viewport::serf_get_body(); some states use other sprites (carrying,
// fighting...), they are not covered.
const FSSSerfBody::Type types[] = {
  { "Transporter",  0x0000, 0x0000 },
  { "Sailor",       0x0100, 0x0200 },
  { "Digger",       0x0300, 0x0380 },
  { "Builder",      0x0500, 0x0580 },
  { "Lumberjack",   0x0b00, 0x0e80 },
  { "Sawmiller",    0x0c00, 0x1580 },
  { "Stonecutter",  0x0d00, 0x1280 },
  { "Forester",     0x0e00, 0x1080 },
  { "Miner",        0x1800, 0x2a80 },
  { "Smelter",      0x1900, 0x2980 },
  { "Fisher",       0x2c00, 0x2c80 },
  { "Pig farmer",   0x3200, 0x3280 },
  { "Butcher",      0x3700, 0x3780 },
  { "Geologist",    0x3900, 0x4c80 },
  { "Farmer",       0x3d00, 0x3d80 },
  { "Miller",       0x4300, 0x4380 },
  { "Baker",        0x4800, 0x4880 },
  { "Boat builder", 0x4e00, 0x4e80 },
  { "Weaponsmith",  0x5200, 0x5280 },
  { "Toolmaker",    0x5800, 0x5880 },
  { "Knight 0",     0x7800, 0x7cd0 },
  { "Knight 1",     0x7900, 0x7ed0 },
  { "Knight 2",     0x7a00, 0x80d0 },
  { "Knight 3",     0x7b00, 0x82d0 },
  { "Knight 4",     0x7c00, 0x84d0 },
  { "Dead",         0x8700, 0x8700 },
};

// Viewport::draw_row_serf(): base torso sprite and head sprite of the high
// byte of the body, offsets of both for the low byte.
const int index1[] = {
  0, 0, 48, 6, 96, -1, 48, 24,
  240, -1, 48, 30, 248, -1, 48, 12,
  48, 18, 96, 306, 96, 300, 48, 54,
  48, 72, 48, 36, 0, 48, 272, -1,
  48, 60, 264, -1, 48, 42, 280, -1,
  48, 66, 96, 312, 500, 600, 48, 318,
  48, 78, 0, 84, 48, 90, 48, 96,
  48, 102, 48, 108, 48, 114, 96, 324,
  96, 330, 96, 336, 96, 342, 96, 348,
  48, 354, 48, 360, 48, 366, 48, 372,
  48, 378, 48, 384, 504, 604, 509, -1,
  48, 120, 288, -1, 288, 420, 48, 126,
  48, 132, 96, 426, 0, 138, 304, -1,
  48, 390, 48, 144, 96, 432, 48, 198,
  510, 608, 48, 204, 48, 402, 48, 150,
  96, 438, 48, 156, 312, -1, 320, -1,
  48, 162, 48, 168, 96, 444, 0, 174,
  513, -1, 48, 408, 48, 180, 96, 450,
  0, 186, 520, -1, 48, 414, 48, 192,
  96, 456, 328, -1, 48, 210, 344, -1,
  48, 6, 48, 6, 48, 216, 528, -1,
  48, 534, 48, 528, 48, 288, 48, 282,
  48, 222, 533, -1, 48, 540, 48, 546,
  48, 552, 48, 558, 48, 564, 96, 468,
  96, 462, 48, 570, 48, 576, 48, 582,
  48, 396, 48, 228, 48, 234, 48, 240,
  48, 246, 48, 252, 48, 258, 48, 264,
  48, 270, 48, 276, 96, 474, 96, 480,
  96, 486, 96, 492, 96, 498, 96, 504,
  96, 510, 96, 516, 96, 522, 96, 612,
  144, 294, 144, 588, 144, 594, 144, 618,
  144, 624, 401, 294, 352, 297, 401, 588,
  352, 591, 401, 594, 352, 597, 401, 618,
  352, 621, 401, 624, 352, 627, 450, -1,
  192, -1
};

const int index2[] = {
  0, 0, 1, 0, 2, 0, 3, 0,
  4, 0, 5, 0, 6, 0, 7, 0,
  8, 1, 9, 1, 10, 1, 11, 1,
  12, 1, 13, 1, 14, 1, 15, 1,
  16, 2, 17, 2, 18, 2, 19, 2,
  20, 2, 21, 2, 22, 2, 23, 2,
  24, 3, 25, 3, 26, 3, 27, 3,
  28, 3, 29, 3, 30, 3, 31, 3,
  32, 4, 33, 4, 34, 4, 35, 4,
  36, 4, 37, 4, 38, 4, 39, 4,
  40, 5, 41, 5, 42, 5, 43, 5,
  44, 5, 45, 5, 46, 5, 47, 5,
  0, 0, 1, 0, 2, 0, 3, 0,
  4, 0, 5, 0, 6, 0, 2, 0,
  0, 1, 1, 1, 2, 1, 3, 1,
  4, 1, 5, 1, 6, 1, 2, 1,
  0, 2, 1, 2, 2, 2, 3, 2,
  4, 2, 5, 2, 6, 2, 2, 2,
  0, 3, 1, 3, 2, 3, 3, 3,
  4, 3, 5, 3, 6, 3, 2, 3,
  0, 0, 1, 0, 2, 0, 3, 0,
  4, 0, 5, 0, 6, 0, 7, 0,
  8, 0, 9, 0, 10, 0, 11, 0,
  12, 0, 13, 0, 14, 0, 15, 0,
  16, 0, 17, 0, 18, 0, 19, 0,
  20, 0, 21, 0, 22, 0, 23, 0,
  24, 0, 25, 0, 26, 0, 27, 0,
  28, 0, 29, 0, 30, 0, 31, 0,
  32, 0, 33, 0, 34, 0, 35, 0,
  36, 0, 37, 0, 38, 0, 39, 0,
  40, 0, 41, 0, 42, 0, 43, 0,
  44, 0, 45, 0, 46, 0, 47, 0,
  48, 0, 49, 0, 50, 0, 51, 0,
  52, 0, 53, 0, 54, 0, 55, 0,
  56, 0, 57, 0, 58, 0, 59, 0,
  60, 0, 61, 0, 62, 0, 63, 0,
  64, 0
};

}  // namespace

namespace FSSSerfBody {

size_t
type_count() {
  return sizeof(types) / sizeof(types[0]);
}

const Type &
type(size_t index) {
  return types[index < type_count() ? index : 0];
}

bool
sprites(int phase_sprite, size_t type_index, int *torso, int *head) {
  const Type &t = type(type_index);
  int body = phase_sprite + ((phase_sprite < 0x80) ? t.walk : t.work);
  size_t hi = static_cast<size_t>((body >> 8) & 0xff) * 2;
  size_t lo = static_cast<size_t>(body & 0xff) * 2;
  if (hi + 1 >= sizeof(index1) / sizeof(index1[0]) ||
      lo + 1 >= sizeof(index2) / sizeof(index2[0])) {
    return false;
  }

  *torso = index1[hi] + index2[lo];
  *head = (index1[hi + 1] < 0) ? -1 : index1[hi + 1] + index2[lo + 1];
  return true;
}

}  // namespace FSSSerfBody
