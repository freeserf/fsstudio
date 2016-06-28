/*
 * data-source-amiga.cc - Amiga data loading
 *
 * Copyright (C) 2016  Wicked_Digger <wicked_digger@mail.ru>
 *
 * This file is part of freeserf.
 *
 * freeserf is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * freeserf is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with freeserf.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "src/data-source-amiga.h"

#include <cstdlib>
#include <cassert>
#include <vector>
#include <sstream>
#include <string>

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#ifdef HAVE_CSTDINT
# include <cstdint>
#endif

#include "src/data.h"
#include "src/freeserf_endian.h"
#include "src/sfx2wav.h"
#include "src/log.h"

uint8_t palette[] = {
  0x00, 0x00, 0x00,   // 0
  0xFF, 0xAA, 0x00,   // 1
  0x00, 0x00, 0x00,   // 2
  0x00, 0xEE, 0xEE,   // 3
  0x00, 0x00, 0xBB,   // 4
  0x44, 0x44, 0xDD,   // 5
  0x88, 0x88, 0xFF,   // 6
  0x00, 0xCC, 0xBB,   // 7
  0x22, 0x44, 0x00,   // 8
  0x33, 0x55, 0x00,   // 9
  0x33, 0x66, 0x33,   // 10
  0xEE, 0x88, 0xFF,   // 11
  0x44, 0x88, 0x00,   // 12
  0x66, 0x99, 0x00,   // 13
  0x77, 0xBB, 0x44,   // 14
  0xCC, 0x77, 0xDD,   // 15
  0x44, 0x44, 0x44,   // 16
  0x99, 0x99, 0x99,   // 17
  0xFF, 0xFF, 0xFF,   // 18
  0xEE, 0x66, 0x66,   // 19
  0x22, 0x22, 0x22,   // 20
  0x66, 0x66, 0x66,   // 21
  0xCC, 0xCC, 0xCC,   // 22
  0xDD, 0x33, 0x33,   // 23
  0x55, 0x22, 0x00,   // 24
  0x77, 0x33, 0x00,   // 25
  0x99, 0x55, 0x22,   // 26
  0xFF, 0xFF, 0x99,   // 27
  0xBB, 0x88, 0x55,   // 28
  0xDD, 0xAA, 0x77,   // 29
  0xFF, 0xDD, 0xBB,   // 30
  0xDD, 0xDD, 0x00,   // 31
};

uint8_t palette1[] = {
  0x00, 0x00, 0x00,   // 0
  0x55, 0x22, 0x00,   // 1
  0x77, 0x33, 0x00,   // 2
  0x99, 0x55, 0x22,   // 3
  0xBB, 0x88, 0x55,   // 4
  0xDD, 0xAA, 0x77,   // 5
  0xFF, 0xDD, 0xBB,   // 6
  0x00, 0xCC, 0xBB,   // 7
  0x22, 0x44, 0x00,   // 8
  0x33, 0x55, 0x00,   // 9
  0x33, 0x66, 0x33,   // 10
  0xEE, 0x88, 0xFF,   // 11
  0x44, 0x88, 0x00,   // 12
  0x66, 0x99, 0x00,   // 13
  0x77, 0xBB, 0x44,   // 14
  0xCC, 0x77, 0xDD,   // 15
  0x44, 0x44, 0x44,   // 16
  0x99, 0x99, 0x99,   // 17
  0xFF, 0xFF, 0xFF,   // 18
  0xEE, 0x66, 0x66,   // 19
  0x22, 0x22, 0x22,   // 20
  0x66, 0x66, 0x66,   // 21
  0xCC, 0xCC, 0xCC,   // 22
  0xDD, 0x33, 0x33,   // 23
  0x55, 0x22, 0x00,   // 24
  0x77, 0x33, 0x00,   // 25
  0x99, 0x55, 0x22,   // 26
  0xFF, 0xFF, 0x99,   // 27
  0xBB, 0x88, 0x55,   // 28
  0xDD, 0xAA, 0x77,   // 29
  0xFF, 0xDD, 0xBB,   // 30
  0xDD, 0xDD, 0x00,   // 31
};

uint8_t palette_patch[] = {
  0x00, 0x00, 0x00,  // 0
  0xFF, 0xAA, 0x00,  // 1
  0x00, 0x00, 0x00,  // 2
  0x00, 0xEE, 0xEE,  // 3
  0x00, 0x00, 0xBB,  // 4
  0x44, 0x44, 0xDD,  // 5
  0x88, 0x88, 0xFF,  // 6
};

uint8_t palette2[] = {
  0x00, 0x00, 0x00,  // 0
  0x44, 0x44, 0xEE,  // 1
  0x55, 0xCC, 0x00,  // 2
  0x88, 0xFF, 0x00,  // 3
  0xCC, 0x33, 0x33,  // 4
  0x44, 0x44, 0x44,  // 5
  0x99, 0x99, 0x99,  // 6
  0xFF, 0xFF, 0xFF,  // 7
  0x33, 0x33, 0x33,  // 8
  0xCC, 0xCC, 0xCC,  // 9
  0x55, 0x22, 0x00,  // 10
  0x77, 0x33, 0x11,  // 11
  0x99, 0x55, 0x22,  // 12
  0xBB, 0x88, 0x55,  // 13
  0xDD, 0xAA, 0x77,  // 14
  0xFF, 0xDD, 0xBB,  // 15
  0x00, 0x00, 0x00,  // 16 ???
  0x00, 0x00, 0x00,  // 17 ???
  0x00, 0x00, 0x00,  // 18 ???
  0x00, 0x00, 0x00,  // 19 ???
  0x00, 0x00, 0x00,  // 20 ???
  0xFF, 0xFF, 0x00,  // 21
  0xCC, 0xAA, 0x11,  // 22
  0xAA, 0x77, 0x11,  // 23
  0x00, 0x00, 0x00,  // 24 ???
  0xFF, 0xFF, 0x00,  // 25
  0xCC, 0xAA, 0x11,  // 26
  0xAA, 0x77, 0x11,  // 27
  0x00, 0x00, 0x00,  // 28 ???
  0x00, 0x00, 0x00,  // 29 ???
  0x00, 0x00, 0x00,  // 30 ???
  0x00, 0x00, 0x00,  // 31 ???
};

uint8_t palette3[] = {
  0x00, 0x00, 0x00,  // 0
  0xFF, 0xFF, 0xFF,  // 1
  0x88, 0x44, 0x00,  // 2
  0xEE, 0x66, 0x00,  // 3
  0x77, 0x55, 0x44,  // 4
  0xAA, 0x88, 0x66,  // 5
  0xEE, 0xAA, 0x88,  // 6
  0xFF, 0xCC, 0x99,  // 7
  0x44, 0x33, 0x11,  // 8
  0x77, 0x55, 0x22,  // 9
  0x99, 0x88, 0x33,  // 10
  0xBB, 0xAA, 0x44,  // 11
  0x33, 0x55, 0x11,  // 12
  0x55, 0x77, 0x22,  // 13
  0x77, 0x99, 0x22,  // 14
  0x99, 0xCC, 0x33,  // 15
  0xAA, 0xDD, 0x44,  // 16
  0xAA, 0xEE, 0x44,  // 17
  0x11, 0x33, 0x22,  // 18
  0x11, 0x44, 0x55,  // 19
  0x22, 0x55, 0x55,  // 20
  0x33, 0x77, 0x77,  // 21
  0x33, 0x88, 0x88,  // 22
  0x44, 0x99, 0xAA,  // 23
  0x00, 0x00, 0xCC,  // 24
  0x11, 0x44, 0xCC,  // 25
  0x33, 0x77, 0xDD,  // 26
  0x55, 0x99, 0xDD,  // 27
  0x77, 0xBB, 0xEE,  // 28
  0x99, 0xDD, 0xEE,  // 29
  0xBB, 0xFF, 0xFF,  // 30
  0xDD, 0xFF, 0xFF,  // 31
};

data_source_amiga_t::data_source_amiga_t()
  : gfxfast(NULL)
  , gfxchip(NULL)
  , sound(NULL)
  , music(NULL)
  , music_size(0)
  , icon_catalog(NULL) {
}

data_source_amiga_t::~data_source_amiga_t() {
}

bool
data_source_amiga_t::check(const std::string &path, std::string *res_path) {
  const char *data_files[] = {
    "gfxheader",  // catalog file
    "gfxfast",    // fast graphics file
    "gfxchip",    // chip graphics file
    NULL
  };

  for (const char **df = data_files; *df != NULL; df++) {
    std::string cp = path + '/' + *df;
    LOGI("data", "Looking for game data in '%s'...", cp.c_str());
    if (!check_file(cp)) {
      return false;
    }
  }

  if (res_path != NULL) {
    *res_path = path;
  }

  return true;
}

bool
data_source_amiga_t::load(const std::string &path) {
  size_t data_size = 0;
  void *data = file_read(path + "/gfxfast", &data_size);
  if (NULL == data) {
    return false;
  }
  decode(data, data_size);
  size_t gfxfast_size = 0;
  gfxfast = unpack(data, data_size, &gfxfast_size);
  free(data);
  data = NULL;
  LOGD("data", "Data file 'gfxfast' loaded (size = %d)", gfxfast_size);

  data_size = 0;
  data = file_read(path + "/gfxchip", &data_size);
  if (NULL == data) {
    return false;
  }
  decode(data, data_size);
  size_t gfxchip_size = 0;
  gfxchip = unpack(data, data_size, &gfxchip_size);
  free(data);
  data = NULL;
  LOGD("data", "Data file 'gfxchip' loaded (size = %d)", gfxchip_size);

  size_t gfxheader_size = 0;
  uint32_t *gfxheader =
    reinterpret_cast<uint32_t*>(file_read(path + "/gfxheader",
                                          &gfxheader_size));
  if (gfxheader == NULL) {
    return false;
  }

  // Prepare icons catalog
  size_t icon_catalog_offset = be16toh(*reinterpret_cast<uint16_t*>(gfxheader));
  size_t icon_catalog_size =
                           be16toh(*(reinterpret_cast<uint16_t*>(gfxheader)+1));
  uint32_t *icon_catalog_tmp = reinterpret_cast<uint32_t*>(gfxfast) +
                                                            icon_catalog_offset;
  icon_catalog = new size_t[icon_catalog_size];
  for (unsigned int i = 0; i < icon_catalog_size; i++) {
    icon_catalog[i] = (size_t)be32toh(icon_catalog_tmp[i]);
  }

  // Prepare data pointer bases
  data_pointers[1] = gfxfast;   // Animations
  data_pointers[2] = gfxfast;   // Ground masks catalog (4 * 81)
  data_pointers[3] = gfxfast;   // ? (4 * 27)?
  data_pointers[4] = gfxfast;   // Ground sprites catalog (4 * 32)
  data_pointers[5] = gfxchip;
  data_pointers[6] = gfxfast;   // Map objects catalog (4 * 194)
  data_pointers[7] = gfxfast;   // Hud multiplayer
  data_pointers[8] = gfxchip;   // Borders
  data_pointers[9] = gfxchip;   // Waves
  data_pointers[10] = gfxchip;  // Popup frame horizontal
  data_pointers[11] = gfxfast;  // Popup frame vertical
  data_pointers[12] = gfxfast;  // Cursor
  data_pointers[13] = gfxfast;  // Icons catalog
  data_pointers[14] = gfxfast;  // Font data (8 * 44)
  data_pointers[15] = gfxfast;  // Game objects catalog
  data_pointers[16] = gfxfast;  // Panel buttons (17 images)
  data_pointers[17] = gfxfast;  // Rotated star catalog
  data_pointers[18] = gfxfast;  // Hud
  data_pointers[19] = gfxfast;  // Serf torse+arms catalog
  data_pointers[20] = gfxfast;  // Serf heads
  data_pointers[21] = gfxchip;  // Screen top
  data_pointers[22] = gfxchip;  // Screen sides (2 * 1864)
  data_pointers[23] = gfxfast;  // Title (1 * 43200)

  for (unsigned int i = 1; i < gfxheader_size/4; i++) {
//    printf("Block %d: %d\n", i, be32toh(gfxheader[i]));
    data_pointers[i] = reinterpret_cast<uint8_t*>(data_pointers[i]) +
                       be32toh(gfxheader[i]);
  }
/*
  uint32_t *catalog = (uint32_t*)data_pointers[4];
  for (int i = 0; i < 20; i++) {
      uint32_t offset = catalog[i];
      offset = be32toh(offset);
      printf("%d\n", offset);
  }
*/
  free(gfxheader);

  size_t sound_size = 0;
  sound = file_read(path + "/sounds", &sound_size);
  if (sound != NULL) {
    decode(sound, sound_size);
  } else {
    LOGW("data", "Unable to load sound data.");
  }

  data = file_read(path + "/music", &data_size);
  if (data != NULL) {
    decode(data, data_size);
    music = unpack(data, data_size, &music_size);
    free(data);
  } else {
    LOGW("data", "Unable to load music data.");
  }

  return true;
}

sprite_t *
data_source_amiga_t::get_sprite(data_res_class_t res, unsigned int index,
                                int color_off) {
  sprite_t *sprite = NULL;
  switch (res) {
    case data_res_art_landscape:
      break;
    case data_res_serf_shadow:
      break;
    case data_res_dotted_lines:
      break;
    case data_res_art_flag:
      break;
    case data_res_art_box:
      break;
    case data_res_credits_bg:
      sprite = decode_interlased_sprite(data_pointers[23], 40, 200,
                                        0, 0, palette);
      break;
    case data_res_logo: {
      uint8_t *data = reinterpret_cast<uint8_t*>(gfxfast);
      data += 188356;
      sprite = decode_interlased_sprite(data, 64/8, 96, 0, 0, palette);
      break;
    }
    case data_res_symbol: {
      uint8_t *data = reinterpret_cast<uint8_t*>(gfxfast);
      data += 178116 + (640*index);
      sprite = decode_interlased_sprite(data, 32/8, 32, 0, 0, palette);
      break;
    }
    case data_res_map_mask_up:
      sprite = get_ground_mask_sprite(index);
      break;
    case data_res_map_mask_down: {
      sprite_t *mask = get_ground_mask_sprite(index);
      if (mask != NULL) {
        sprite_amiga_t *s = get_mirrored_horizontaly_sprite(mask);
        s->set_offset(0, -(static_cast<int>(s->get_height()) - 1));
        delete mask;
        sprite = s;
      }
      break;
    }
    case data_res_path_mask:
      sprite = get_path_mask_sprite(index);
      break;
    case data_res_map_ground: {
      sprite_amiga_t *s = NULL;
      if (index == 32) {
        s = new sprite_amiga_t(32, 21);
        color_t c = {0xBB, 0x00, 0x00, 0xFF};
        s->fill(c);
      } else {
        s = get_ground_sprite(index);
      }
      sprite = s;
      break;
    }
    case data_res_path_ground:
      sprite = get_ground_sprite(index);
      break;
    case data_res_game_object:
      sprite = get_game_object_sprite(15, index+1);
      break;
    case data_res_frame_top:
      if (index < 2) {
        sprite = decode_amiga_sprite(
                                 reinterpret_cast<uint8_t*>(data_pointers[22]) +
                                     (1864*index),
                                     2, 233, palette);
      } else if (index == 2) {
        sprite = decode_planned_sprite(data_pointers[21], 39, 8, 24, 24,
                                       palette);
      } else if (index == 3) {
        sprite_amiga_t *left = decode_interlased_sprite(data_pointers[7], 2,
                                                        216, 0, 0, palette);
        sprite_amiga_t *right = decode_interlased_sprite(
                            reinterpret_cast<uint8_t*>(data_pointers[7]) + 2160,
                                                         2, 216, 0, 0, palette);
        sprite = left->merge_horizontaly(right);
        delete left;
        delete right;
      }
      break;
    case data_res_map_border: {
      uint8_t *data = reinterpret_cast<uint8_t*>(data_pointers[8]) +
                      index * 120;
      sprite_amiga_t *s = decode_interlased_sprite(data, 2, 6, 0, 0, palette);
      sprite_amiga_t *m = decode_interlased_sprite(data + 60, 2, 6, 0, 0,
                                                   palette);
      m->make_transparent();
      sprite = s->get_masked(m);
      break;
    }
    case data_res_map_waves: {
      uint8_t *data = reinterpret_cast<uint8_t*>(data_pointers[9]) +
                      index * 228;
      sprite_amiga_t *s = decode_interlased_sprite(data, 6, 19, 28, 5, palette);
      s->make_transparent(0x0000BB);
      s->set_delta(1, 0);
      sprite = s;
      break;
    }
    case data_res_frame_popup:
      if (index == 0) {
        sprite = decode_interlased_sprite(data_pointers[10], 18, 9, 0, 0,
                                          palette, 1);
      } else if (index == 1) {
        sprite = decode_interlased_sprite(
                            reinterpret_cast<uint8_t*>(data_pointers[10]) + 972,
                                          18, 7, 0, 0, palette, 1);
      } else if (index > 1) {
        sprite_amiga_t *s = decode_interlased_sprite(data_pointers[11], 2, 144,
                                                     0, 0, palette);
        sprite = s->split_horizontaly(index == 3);
        delete s;
      }
      break;
    case data_res_indicator:
//      sprite = decode_planned_sprite(reinterpret_cast<uint8_t*>(gfxfast) +
//                                     43076, 1, 7, 0, 0);
//      if (index > 3) {
//        index -= 4;
//      }
//    sprite = decode_amiga_sprite(reinterpret_cast<uint8_t*>(gfxfast) + 43076,
//                                 5, 7, palette);  // 5 indicators WTF?
      sprite = decode_amiga_sprite(reinterpret_cast<uint8_t*>(gfxfast) + 44676,
                                   10, 7, palette2);  // 10 indicators WTF?
      break;
    case data_res_font: {
      uint8_t *data = reinterpret_cast<uint8_t*>(data_pointers[14]);
      data += index * 8;
      sprite_amiga_t *s = decode_mask_sprite(data, 8, 8);
      color_t color;
      color.red = palette[color_off*3];
      color.green = palette[color_off*3+1];
      color.blue = palette[color_off*3+2];
      color.alpha = 0xFF;
      s->fill_masked(color);
      sprite = s;
      break;
    }
    case data_res_font_shadow:
      break;
    case data_res_icon:
      sprite = get_icon_sprite(index);
      break;
    case data_res_map_object:
      sprite = get_map_object_sprite(index);
      break;
    case data_res_map_shadow:
      sprite = get_map_object_shadow(index);
      break;
    case data_res_panel_button:
      if (index < 17) {
        sprite = get_menu_sprite(index, data_pointers[16], 32, 32, 16, 0);
      } else {
        index -= 17;
        unsigned int backs[] = {3, 4, 10, 12, 14, 16, 2, 8};
        sprite_amiga_t *s = get_menu_sprite(backs[index], data_pointers[16],
                                            32, 32, 16, 0);
        uint8_t *l_data = get_data_from_catalog(17, 0, gfxchip);
        uint8_t *r_data = get_data_from_catalog(17, 16, gfxchip);
        sprite_amiga_t *left = decode_interlased_sprite(l_data, 2, 29, 28, 20,
                                                        palette2);
        left->make_transparent();
        sprite_amiga_t *right = decode_interlased_sprite(r_data, 2, 29, 28, 20,
                                                         palette2);
        right->make_transparent();
        sprite_amiga_t *star = left->merge_horizontaly(right);
        delete left;
        delete right;
        s->stick(star, 0, 1);
        delete star;
        sprite = s;
      }
      break;
    case data_res_frame_bottom:
      if (index == 0) {
        sprite_amiga_t *s = get_hud_sprite(6);
        sprite = s->split_horizontaly(true);
        delete s;
      } else if (index == 1) {
        sprite_amiga_t *s = get_hud_sprite(6);
        sprite = s->split_horizontaly(false);
        delete s;
      } else if (index == 2) {
        sprite = get_hud_sprite(18);
      } else if (index == 4) {
        sprite = get_hud_sprite(19);
      } else if (index == 6) {
        sprite = get_hud_sprite(17);
      } else if ((index > 6) && (index < 17)) {
        sprite = get_hud_sprite(7 + (index - 7));
      } else if (index > 19) {
        sprite = get_hud_sprite(index - 20);
      }
      break;
    case data_res_serf_torso:
      sprite = get_torso_sprite(index);
      break;
    case data_res_serf_head:
      sprite = get_game_object_sprite(20, index);
      break;
    case data_res_frame_split:
      break;
    case data_res_cursor: {
      uint8_t *data = reinterpret_cast<uint8_t*>(data_pointers[12]);
      sprite_amiga_t *s = decode_interlased_sprite(data, 2, 16, 28, 16,
                                                   palette);
      s->make_transparent(0x00444444);
      sprite = s;
      break;
    }
    default:
      break;
  }

  return sprite;
}

color_t
data_source_amiga_t::get_color(unsigned int index) {
  if (index > 31) {
    color_t color = {0, 0, 0, 0};
    return color;
  }

  color_t color = {palette[index*3+2],
                   palette[index*3+1],
                   palette[index*3],
                   0xFF};

  return color;
}

animation_t *
data_source_amiga_t::get_animation(unsigned int animation,
                                   unsigned int phase) {
  uint32_t *animation_catalog = reinterpret_cast<uint32_t*>(data_pointers[1]);
  uint8_t *data = reinterpret_cast<uint8_t*>(animation_catalog);
  unsigned int offset = be32toh(animation_catalog[animation]);

  animation_t *anim = reinterpret_cast<animation_t*>(data + offset);

  return anim + (phase >> 3);
}

typedef struct {
  ptrdiff_t offset;
  size_t size;
  size_t unknown_2;
  size_t unknown_3;
  size_t unknown_4;
  size_t unknown_5;
  size_t unknown_6;
} sound_struct_t;

sound_struct_t sound_info[] = {
  {0x000000, 0x0000, 0, 0x000, 0x0000, 0x0000, 0x000},
  {0x000000, 0x0548, 0, 0x1AB, 0x0000, 0x4000, 0x060},
  {0x000A90, 0x0237, 0, 0x1AB, 0x0000, 0x3000, 0x028},
  {0x000EFE, 0x0000, 0, 0x000, 0x0000, 0x0000, 0x000},
  {0x000EFE, 0x04BA, 0, 0x1AB, 0x0000, 0x4000, 0x056},
  {0x001872, 0x0000, 0, 0x000, 0x0000, 0x0000, 0x000},
  {0x001872, 0x01FB, 0, 0x1AB, 0x0000, 0x1400, 0x024},
  {0x001C68, 0x0000, 0, 0x000, 0x0000, 0x0000, 0x000},
  {0x001C68, 0x0027, 0, 0x0DC, 0x0000, 0x1900, 0x001},
  {0x001CB6, 0x0000, 0, 0x000, 0x0000, 0x0000, 0x000},
  {0x001CB6, 0x0421, 0, 0x19C, 0x1F00, 0x190F, 0x04B},
  {0x0024F8, 0x0000, 0, 0x000, 0x0000, 0x0000, 0x000},
  {0x0024F8, 0x0000, 0, 0x000, 0x0000, 0x0000, 0x000},
  {0x0024F8, 0x0000, 0, 0x000, 0x0000, 0x0000, 0x000},
  {0x0024F8, 0x052A, 0, 0x19C, 0x1F00, 0x190F, 0x05E},
  {0x002F4C, 0x0000, 0, 0x000, 0x0000, 0x0000, 0x000},
  {0x002F4C, 0x0000, 0, 0x000, 0x0000, 0x0000, 0x000},
  {0x002F4C, 0x0000, 0, 0x000, 0x0000, 0x0000, 0x000},
  {0x002F4C, 0x0469, 0, 0x19C, 0x1F00, 0x190F, 0x050},
  {0x00381E, 0x0000, 0, 0x000, 0x0000, 0x0000, 0x000},
  {0x00381E, 0x0000, 0, 0x000, 0x0000, 0x0000, 0x000},
  {0x00381E, 0x0000, 0, 0x000, 0x0000, 0x0000, 0x000},
  {0x00381E, 0x02C9, 0, 0x19C, 0x1F00, 0x190F, 0x033},
  {0x003DB0, 0x0000, 0, 0x000, 0x0000, 0x0000, 0x000},
  {0x003DB0, 0x0000, 0, 0x000, 0x0000, 0x0000, 0x000},
  {0x003DB0, 0x0000, 0, 0x000, 0x0000, 0x0000, 0x000},
  {0x003DB0, 0x0806, 0, 0x19C, 0x1F00, 0x1D07, 0x093},
  {0x004DBC, 0x0000, 0, 0x000, 0x0000, 0x0000, 0x000},
  {0x004DBC, 0x01BF, 0, 0x18C, 0x3F00, 0x170F, 0x020},
  {0x00513A, 0x0000, 0, 0x000, 0x0000, 0x0000, 0x000},
  {0x00513A, 0x02FA, 0, 0x1A4, 0x0F00, 0x0907, 0x036},
  {0x00572E, 0x0000, 0, 0x000, 0x0000, 0x0000, 0x000},
  {0x00572E, 0x0220, 0, 0x18C, 0x3F00, 0x190F, 0x027},
  {0x005B6E, 0x0000, 0, 0x000, 0x0000, 0x0000, 0x000},
  {0x005B6E, 0x0377, 0, 0x6EE, 0x1F00, 0x190F, 0x10A},
  {0x00625C, 0x0000, 0, 0x000, 0x0000, 0x0000, 0x000},
  {0x00625C, 0x0223, 0, 0x19C, 0x1F00, 0x0907, 0x027},
  {0x0066A2, 0x0000, 0, 0x000, 0x0000, 0x0000, 0x000},
  {0x0066A2, 0x0186, 0, 0x19C, 0x1F00, 0x1107, 0x01B},
  {0x0069AE, 0x0000, 0, 0x000, 0x0000, 0x0000, 0x000},
  {0x0069AE, 0x00F2, 0, 0x18C, 0x3F00, 0x190F, 0x011},
  {0x006B92, 0x0000, 0, 0x000, 0x0000, 0x0000, 0x000},
  {0x006B92, 0x03CC, 0, 0x36F, 0x1F00, 0x0903, 0x091},
  {0x006B92, 0x03CC, 0, 0x2D9, 0x1F00, 0x0903, 0x091},
  {0x00732A, 0x01CA, 0, 0x19C, 0x1F00, 0x0703, 0x020},
  {0x0076BE, 0x0000, 0, 0x000, 0x0000, 0x0000, 0x000},
  {0x0076BE, 0x00EB, 0, 0x0CD, 0x1F00, 0x190F, 0x008},
  {0x007894, 0x0000, 0, 0x000, 0x0000, 0x0000, 0x000},
  {0x007894, 0x0130, 0, 0x19C, 0x1F00, 0x190F, 0x015},
  {0x007AF4, 0x0000, 0, 0x000, 0x0000, 0x0000, 0x000},
  {0x007AF4, 0x03B3, 0, 0x3B1, 0x1F00, 0x190F, 0x98},
  {0x00825A, 0x0000, 0, 0x000, 0x0000, 0x0000, 0x000},
  {0x00825A, 0x024B, 0, 0x1EC, 0x1F00, 0x1107, 0x031},
  {0x0086F0, 0x0000, 0, 0x000, 0x0000, 0x0000, 0x000},
  {0x0086F0, 0x0096, 0, 0x0F8, 0x0000, 0x2000, 0x006},
  {0x00881C, 0x0000, 0, 0x000, 0x0000, 0x0000, 0x000},
  {0x00881C, 0x0000, 0, 0x000, 0x0000, 0x0000, 0x000},
  {0x00881C, 0x0000, 0, 0x000, 0x0000, 0x0000, 0x000},
  {0x00881C, 0x0155, 0, 0x19C, 0x1F00, 0x190F, 0x018},
  {0x008AC6, 0x0000, 0, 0x000, 0x0000, 0x0000, 0x000},
  {0x008AC6, 0x0249, 0, 0x16A, 0xFF00, 0x0C07, 0x030},
  {0x008F58, 0x0000, 0, 0x000, 0x0000, 0x0000, 0x000},
  {0x008F58, 0x047B, 0, 0x484, 0x3F00, 0x0703, 0x0E4},
  {0x00984E, 0x0000, 0, 0x000, 0x0000, 0x0000, 0x000},
  {0x00984E, 0x069E, 0, 0x339, 0x3F00, 0x0907, 0x0F3},
  {0x00A58A, 0x0000, 0, 0x000, 0x0000, 0x0000, 0x000},
  {0x00A58A, 0x0537, 0, 0x377, 0x0F00, 0x0703, 0x0C8},
  {0x00AFF8, 0x0000, 0, 0x000, 0x0000, 0x0000, 0x000},
  {0x00AFF8, 0x0000, 0, 0x000, 0x0000, 0x0000, 0x000},
  {0x00AFF8, 0x0686, 0, 0x1D1, 0x3F00, 0x0D07, 0x08B},
  {0x00BD04, 0x01DD, 0, 0x0CD, 0x1F00, 0x050F, 0x011},
  {0x00C0BE, 0x0000, 0, 0x000, 0x0000, 0x0000, 0x000},
  {0x00C0BE, 0x0000, 0, 0x000, 0x0000, 0x0000, 0x000},
  {0x00C0BE, 0x0000, 0, 0x000, 0x0000, 0x0000, 0x000},
  {0x00C0BE, 0x06EA, 0, 0x0CD, 0x1F00, 0x050F, 0x041},
  {0x00CE92, 0x0000, 0, 0x000, 0x0000, 0x0000, 0x000},
  {0x00CE92, 0x0B76, 0, 0x18C, 0x3F00, 0x1107, 0x0D2},
  {0x00E57E, 0x0000, 0, 0x000, 0x0000, 0x0000, 0x000},
  {0x00E57E, 0x08AA, 0, 0x0FB, 0x1F00, 0x050F, 0x063},
  {0x00F6D2, 0x0000, 0, 0x000, 0x0000, 0x0000, 0x000},
  {0x00F6D2, 0x0000, 0, 0x000, 0x0000, 0x0000, 0x000},
  {0x00F6D2, 0x0000, 0, 0x000, 0x0000, 0x0000, 0x000},
  {0x00F6D2, 0x0E53, 0, 0x0CD, 0x1F00, 0x050F, 0x087},
  {0x011378, 0x0000, 0, 0x000, 0x0000, 0x0000, 0x000},
  {0x011378, 0x1614, 0, 0x19C, 0x1F00, 0x0907, 0x195},
  {0x013FA0, 0x0000, 0, 0x000, 0x0000, 0x0000, 0x000},
  {0x013FA0, 0x1D0D, 0, 0x33F, 0x7F00, 0x0000, 0x45A},
  {0x0179BA, 0x0000, 0, 0x000, 0x0000, 0x0000, 0x000},
  {0x0179BA, 0x1DE8, 0, 0x215, 0x7F00, 0x0000, 0x2FC},
  {0x01B58A, 0x0000, 0, 0x000, 0x0000, 0x0000, 0x000},
  {0x01B58A, 0x0000, 0, 0x000, 0x0000, 0x0000, 0x000},
  {0x01B58A, 0x0000, 0, 0x000, 0x0000, 0x0000, 0x000},
  {0x01B58A, 0x0000, 0, 0x000, 0x0000, 0x0000, 0x000},
  {0x01B58A, 0x0000, 0, 0x000, 0x0000, 0x0000, 0x000},
  {0x01B58A, 0x0000, 0, 0x000, 0x0000, 0x0000, 0x000}
};

void *
data_source_amiga_t::get_sound_data(unsigned int index, size_t *size) {
  if (sound != NULL) {
    size_t count = sizeof(sound_info) / sizeof(sound_struct_t);
    if (index < count) {
      *size = sound_info[index].size * 2;
      if (*size != 0) {
        uint8_t *res = reinterpret_cast<uint8_t*>(sound);
        res += sound_info[index].offset;
        return res;
      }
    }
  }

  *size = 0;
  return NULL;
}

void *
data_source_amiga_t::get_sound(unsigned int index, size_t *size) {
  *size = 0;
  size_t sfx_size = 0;
  void *data = get_sound_data(index, &sfx_size);
  if (data == NULL) {
    LOGE("data", "Sound sample with index %d not present.", index);
    return NULL;
  }

  void *wav = sfx2wav(data, sfx_size, size, 0, true);
  if (wav == NULL) {
    LOGE("data", "Could not convert PCM clip to WAV: %d.", index);
    return NULL;
  }

  return wav;
}

void *
data_source_amiga_t::get_music(unsigned int /*index*/, size_t *size) {
/*
  if (music != NULL) {
    uint8_t *mod = reinterpret_cast<uint8_t*>(music) + 4370;
    size_t mod_size = music_size - 4370;
    void *mid = mod2mid(mod, mod_size, size);
    return mid;
  }
*/
  *size = 0;
  return NULL;
}

void
data_source_amiga_t::decode(void *data, size_t size) {
  uint8_t *byte = reinterpret_cast<uint8_t*>(data);
  for (size_t i = 0; i < size; i++, byte++) {
    *byte = *byte ^ static_cast<uint8_t>(i);
  }
}

void *
data_source_amiga_t::unpack(void *data, size_t size, size_t *unpacked_size) {
  std::stringstream input(std::ios_base::in | std::ios_base::out |
                          std::ios_base::binary);
  input.write(reinterpret_cast<char*>(data), size);
  input.seekp(0);

  std::stringstream output(std::ios_base::in | std::ios_base::out |
                           std::ios_base::binary);

  *unpacked_size = 0;
  uint8_t flag = input.get();

  while (!input.eof()) {
    uint8_t val = input.get();
    size_t count = 0;

    if (val == flag) {
      count = input.get();
      val = input.get();
    }

    *unpacked_size += count+1;
    for (unsigned int i = 0; i < count + 1; i++) {
      output << val;
    }
  }

  if (*unpacked_size == 0) {
    return NULL;
  }

  void *result = malloc(*unpacked_size);
  output.seekp(0);
  output.read(reinterpret_cast<char*>(result), *unpacked_size);

  return result;
}

unsigned int data_source_amiga_t::bitplane_count_from_compression(
                                    unsigned char compression) {
  unsigned int bpp = 5;

  for (int i = 0 ; i < 5 ; i++) {
    if (compression & 0x01) {
      bpp--;
    }
    compression = compression >> 1;
  }

  return bpp;
}

sprite_amiga_t *
data_source_amiga_t::get_menu_sprite(unsigned int index, void *block,
                                     unsigned int width, unsigned int height,
                                     unsigned char compression,
                                     unsigned char filling) {
  unsigned int bpp = bitplane_count_from_compression(compression);
  size_t sprite_size = (width * height) / 8 * bpp;
  uint8_t *data = reinterpret_cast<uint8_t*>(block) + (sprite_size * index);
  return decode_interlased_sprite(data, width/8, height, compression, filling,
                                  palette2);
}

typedef struct {
  uint16_t width;
  uint16_t height;
  uint8_t filling;
  uint8_t compression;
} icon_header_t;

sprite_t *
data_source_amiga_t::get_icon_sprite(unsigned int index) {
  size_t offset = icon_catalog[index];
  uint8_t *data = reinterpret_cast<uint8_t*>(gfxfast)+offset;
  icon_header_t header = *reinterpret_cast<icon_header_t*>(data);
  data += sizeof(icon_header_t);
  header.width = be16toh(header.width);
  header.height = be16toh(header.height);

  return decode_planned_sprite(data, header.width, header.height,
                               header.compression, header.filling, palette);
}
/*
static unsigned int
calc_plaines_count(uint8_t filled) {
  unsigned int count = 0;

  for ( int i = 0 ; i < 5 ; i++ ) {
    if ((filled & 1) != 1) {
      count++;
    }
    filled = filled >> 1;
  }

  return count;
}
*/
static void
calc_compression_filling(uint16_t compression, uint8_t *compressed,
                         uint8_t *filled) {
  *compressed = 0;
  *filled = 0;
  for ( int i = 0 ; i < 5 ; i++ ) {
    *compressed  = *compressed >> 1;
    *filled = *filled >> 1;

    if ( compression & 0x8000 ) {
      *compressed = *compressed | 0x10;
      if ( compression & 0x4000 ) {
        *filled = *filled | 0x10;
      }
      compression = compression << 1;
    }
    compression = compression << 1;
  }
}

static uint8_t
invert5bit(uint8_t src) {
  uint8_t res = 0;

  for (int i = 0; i < 5; i++) {
    res <<= 1;
    res |= (src & 0x01);
    src >>= 1;
  }

  return res;
}

unsigned char *
data_source_amiga_t::get_data_from_catalog(size_t catalog_index, size_t index,
                                           void *base) {
  uint32_t *catalog = reinterpret_cast<uint32_t*>(data_pointers[catalog_index]);
  uint32_t offset = catalog[index];
  if (offset == 0) {
    return NULL;
  }
  offset = be32toh(offset);

  return (unsigned char*)base + offset;
}

sprite_amiga_t *
data_source_amiga_t::get_ground_sprite(unsigned int index) {
  uint8_t *data = get_data_from_catalog(4, index, gfxchip);
  if (data == NULL) {
    return NULL;
  }

  uint16_t cf = *reinterpret_cast<uint16_t*>(data);
  data += 2;

  uint8_t compressed = 0;
  uint8_t filled = 0;
  calc_compression_filling(cf, &compressed, &filled);

  sprite_amiga_t *sprite = decode_planned_sprite(data, 4, 21, compressed,
                                                 filled, palette);

  if (sprite != NULL) {
    sprite->set_delta(1, 0);
    sprite->set_offset(compressed, filled);
  }

  return sprite;
}

sprite_t *
data_source_amiga_t::get_ground_mask_sprite(unsigned int index) {
  uint8_t *data = NULL;
  if (index == 0) {
    data = reinterpret_cast<uint8_t*>(gfxchip);
  } else {
    data = get_data_from_catalog(2, index, gfxchip);
  }

  if (data == NULL) {
    return NULL;
  }
  uint16_t height = *reinterpret_cast<uint16_t*>(data);
  data += 2;
  height = be16toh(height);

  sprite_amiga_t *sprite = new sprite_amiga_t(32, height);
  sprite->set_delta(2, 0);
  sprite->set_offset(0, 0);

  uint32_t *pixel = reinterpret_cast<uint32_t*>(sprite->get_data());
  for (int i = 0 ; i < 32*height/8 ; i++) {
    uint8_t byte = *data;
    for (unsigned int j = 0 ; j < 8; j++) {
      if (((byte >> (7-j)) & 0x01) == 0x01) {
        *pixel = 0xFFFFFFFF;
      } else {
        *pixel = 0x00000000;
      }
      pixel++;
    }
    data++;
  }

  return sprite;
}

sprite_amiga_t *
data_source_amiga_t::get_mirrored_horizontaly_sprite(sprite_t *sprite) {
  if (sprite == NULL) {
    return NULL;
  }
  sprite_amiga_t *result = new sprite_amiga_t(sprite->get_width(),
                                              sprite->get_height());
  result->set_delta(sprite->get_delta_x(), sprite->get_delta_y());
  result->set_offset(sprite->get_offset_x(), sprite->get_offset_y());

  uint8_t *src_pixels = reinterpret_cast<uint8_t*>(sprite->get_data());
  uint8_t *dst_pixels = reinterpret_cast<uint8_t*>(result->get_data()) +
                        result->get_width() * (result->get_height() - 1) * 4;
  for (unsigned int i = 0 ; i < sprite->get_height() ; i++) {
      memcpy(dst_pixels, src_pixels, sprite->get_width() * 4);
      src_pixels += sprite->get_width() * 4;
      dst_pixels -= sprite->get_width() * 4;
  }

  return result;
}

sprite_t *
data_source_amiga_t::get_path_mask_sprite(unsigned int index) {
  uint8_t *data = get_data_from_catalog(3, index, gfxchip);
  if (data == NULL) {
    return NULL;
  }

  uint8_t n = *reinterpret_cast<uint8_t*>(data++);
  uint8_t k = *reinterpret_cast<uint8_t*>(data++);

  int width = (k == 66) ? 32 : 16;

  int heghts[] = { 0, 5, 9, 13, 17, 21, 25, 29, 33, 37, 41 };

  sprite_amiga_t *sprite = decode_mask_sprite(data, width, heghts[n]);
  sprite->set_delta(2, 0);
  sprite->set_offset(0, 0);

  return sprite;
}

sprite_amiga_t *
data_source_amiga_t::decode_mask_sprite(void *data,
                                        unsigned int width,
                                        unsigned int height) {
  sprite_amiga_t *sprite = new sprite_amiga_t(width, height);

  unsigned int size = width/8 * height;
  uint8_t *source = reinterpret_cast<uint8_t*>(data);

  uint32_t *pixel = reinterpret_cast<uint32_t*>(sprite->get_data());
  for (unsigned int i = 0 ; i < size ; i++) {
    uint8_t byte = *source;
    for (unsigned int j = 0 ; j < 8; j++) {
      if (((byte >> (7-j)) & 0x01) == 0x01) {
        *pixel = 0xFFFFFFFF;
      } else {
        *pixel = 0x00000000;
      }
      pixel++;
    }
    source++;
  }

  return sprite;
}

sprite_t *
data_source_amiga_t::get_game_object_sprite(unsigned int catalog,
                                            unsigned int index) {
  uint8_t *data = get_data_from_catalog(catalog, index, gfxchip);
  if (data == NULL) {
    return NULL;
  }

  uint8_t h = *data++;
  uint8_t offset_y = *data++;
  uint8_t w = *data++;
  uint8_t offset_x = *data++;
  uint8_t filling = *data++;
  uint8_t compression = *data++;

  if (w < 16) w = 16;

  int width = (w+7)/8;

  sprite_amiga_t *mask = decode_mask_sprite(data, width*8, h);
  data += w*h/8;
  sprite_amiga_t *pixels = decode_planned_sprite(data, width, h,
                                                 compression, filling, palette);

  sprite_amiga_t *sprite = pixels->get_amiga_masked(mask);
  if (sprite == NULL) {
    return NULL;
  }
  sprite->set_delta(1, 0);
  sprite->set_offset(-offset_x, -offset_y);

  delete mask;
  delete pixels;

  return sprite;
}

sprite_t *
data_source_amiga_t::get_torso_sprite(unsigned int index) {
  uint8_t *data = get_data_from_catalog(19, index, gfxchip);
  if (data == NULL) {
    return NULL;
  }

  uint8_t offset_x = *data++;
  uint8_t w = *data++;
  uint8_t offset_y = *data++;
  uint8_t h = *data++;
  int8_t delta_y = *reinterpret_cast<int8_t*>(data++);
  int8_t delta_x = *reinterpret_cast<int8_t*>(data++);

  if (w < 16) {
    w = 16;
  }
  w = (w+7)/8;

  sprite_amiga_t *pixels = decode_planned_sprite(data, w, h, 0, 0, palette);

  pixels->set_delta(delta_y, delta_x);
  pixels->set_offset(-offset_x, -offset_y);

  return pixels;
}

typedef struct {
  uint16_t shadow_offset;
  uint16_t bitplane_size;
  uint8_t width;
  uint8_t height;
  uint8_t offset_y;
  uint8_t compression;
} transparent_sprite_heder_t;

sprite_t *
data_source_amiga_t::get_map_object_sprite(unsigned int index) {
  uint8_t *data = get_data_from_catalog(6, index, gfxchip);
  if (data == NULL) {
    return NULL;
  }
  transparent_sprite_heder_t header =
                           *reinterpret_cast<transparent_sprite_heder_t*>(data);
  if (header.height == 0) {
    return NULL;
  }
  header.shadow_offset = be16toh(header.shadow_offset);
  header.bitplane_size = be16toh(header.bitplane_size);
  if (header.width == 0) {
    header.width = header.bitplane_size / header.height;
  }
  data += sizeof(transparent_sprite_heder_t);

  uint8_t compressed = 0;
  uint8_t filled = 0;
  uint16_t compression = header.compression;
  for ( int i = 0 ; i < 2 ; i++ ) {
    compressed  = compressed >> 1;
    filled = filled >> 1;

    if ( compression & 0x80 ) {
      compressed = compressed | 0x10;
      if ( compression & 0x40 ) {
        filled = filled | 0x10;
      }
      compression = compression << 1;
    }
    compression = compression << 1;
  }

  sprite_amiga_t *mask = decode_mask_sprite(data, header.width * 8,
                                            header.height);

  sprite_amiga_t *pixels = decode_planned_sprite(data + header.bitplane_size,
                                                 header.width, header.height,
                                                 compressed, filled, palette);

  sprite_amiga_t *sprite = pixels->get_amiga_masked(mask);
  sprite->set_delta(1, 0);
  sprite->set_offset(-header.width * 4, -header.offset_y);

  delete mask;
  delete pixels;

  return sprite;
}

sprite_t *
data_source_amiga_t::get_map_object_shadow(unsigned int index) {
  uint8_t *data = get_data_from_catalog(6, index, gfxchip);
  if (data == NULL) {
    return NULL;
  }
  transparent_sprite_heder_t header =
                           *reinterpret_cast<transparent_sprite_heder_t*>(data);
  if (header.height == 0) {
    return NULL;
  }
  header.shadow_offset = be16toh(header.shadow_offset);
  header.bitplane_size = be16toh(header.bitplane_size);
  if (header.width == 0) {
    header.width = header.bitplane_size / header.height;
  }
  uint8_t *shadow = data + header.shadow_offset;

  unsigned int shadow_scanline_size = *(shadow+3);
  unsigned int shadow_height = *(shadow+1);
  unsigned int shadow_size = shadow_scanline_size * shadow_height;

  sprite_amiga_t *sprite = new sprite_amiga_t(shadow_scanline_size * 8,
                                              shadow_height);
  sprite->clear();
  sprite->set_delta(*(shadow+2), *(shadow+3));
  sprite->set_offset(0 /* *(shadow) */,
                     -header.offset_y + header.height - *(shadow+1));

  shadow += 4;

  uint32_t *pixel = reinterpret_cast<uint32_t*>(sprite->get_data());
  uint32_t *end = pixel + sprite->get_width()*sprite->get_height();
  for (unsigned int i = 0 ; i < shadow_size ; i++) {
    for (unsigned int j = 0 ; j < 8; j++) {
      if (((*shadow >> (7-j)) & 0x01) == 0x01) {
        *pixel = 0x80000000;
      }
      pixel++;
      ptrdiff_t s = end - pixel;
      if (s < 0) {
        exit(0);
      }
    }
    shadow++;
  }

  return sprite;
}

unsigned int hud_offsets[] = {
  0,    320, 2, 40,
  320,  320, 2, 40,
  640,  320, 2, 40,
  960,  320, 2, 40,
  1280, 320, 2, 40,
  1600, 320, 2, 40,
  1920, 320, 2, 40,
  2240,  64, 4,  4,
  2304,  64, 4,  4,
  2368,  64, 4,  4,
  2432,  64, 4,  4,
  2496,  64, 4,  4,
  2560,  64, 4,  4,
  2624,  64, 4,  4,
  2688,  64, 4,  4,
  2752,  64, 4,  4,
  2816,  64, 4,  4,
  2880, 800, 5, 40,
  3680,  48, 1, 12,
  3728,  40, 1, 10,
};

sprite_amiga_t *
data_source_amiga_t::get_hud_sprite(unsigned int index) {
  uint8_t *data = reinterpret_cast<uint8_t*>(data_pointers[18]);
  data += hud_offsets[index*4];

  return decode_interlased_sprite(data, hud_offsets[index*4 + 2],
                                  hud_offsets[index*4 + 3], 16, 0, palette2);
}

sprite_amiga_t *
data_source_amiga_t::decode_planned_sprite(void *data,
                                           unsigned int width,
                                           unsigned int height,
                                           uint8_t compression,
                                           uint8_t filling,
                                           uint8_t *palette, bool invert) {
  sprite_amiga_t *sprite = new sprite_amiga_t(width*8, height);

  uint8_t *src = reinterpret_cast<uint8_t*>(data);
  color_t *res = sprite->get_writable_data();

  size_t bps = width * height;  // bitplane size in bytes

  for (size_t i = 0; i < bps; i++) {
    for (int k = 7; k >= 0; k--) {
      uint8_t color = 0;
      int n = 0;
      for (size_t b = 0; b < 5; b++) {
        color = color << 1;
        if ((compression >> b) & 0x01) {
          if ((filling >> b) & 0x01) {
            color |= 0x01;
          }
        } else {
          color |= ((*(src+(n*bps)) >> k) & 0x01);
          n++;
        }
      }
      if (invert) {
        color = invert5bit(color);
      }
      res->red = palette[color*3+0];    // R
      res->green = palette[color*3+1];  // G
      res->blue = palette[color*3+2];   // B
      res->alpha = 0xFF;                // A
      res++;
    }
    src++;
  }

  return sprite;
}

sprite_amiga_t *
data_source_amiga_t::decode_interlased_sprite(void *data,
                                              unsigned int width,
                                              unsigned int height,
                                              uint8_t compression,
                                              uint8_t filling,
                                              uint8_t *palette,
                                              size_t skip_lines) {
  sprite_amiga_t *sprite = new sprite_amiga_t(width*8, height);

  uint8_t *src = reinterpret_cast<uint8_t*>(data);
  color_t *res = sprite->get_writable_data();

  size_t bpp = bitplane_count_from_compression(compression);

  for (size_t y = 0; y < height; y++) {
    for (size_t i = 0; i < width; i++) {
      for (int k = 7; k >= 0; k--) {
        uint8_t color = 0;
        int n = 0;
        for (size_t b = 0; b < 5; b++) {
          color = color << 1;
          if ((compression >> b) & 0x01) {
            if ((filling >> b) & 0x01) {
              color |= 0x01;
            }
          } else {
            color |= ((*(src+(n*width)+((skip_lines*width*y))) >> k) & 0x01);
            n++;
          }
        }
        color = invert5bit(color);
        res->red = palette[color*3+0];    // R
        res->green = palette[color*3+1];  // G
        res->blue = palette[color*3+2];   // B
        res->alpha = 0xFF;                // A
        res++;
      }
      src++;
    }
    src += (bpp-1) * width;
  }

  return sprite;
}

sprite_amiga_t *
data_source_amiga_t::decode_amiga_sprite(void *data, unsigned int width,
                                         unsigned int height,
                                         uint8_t *palette) {
  sprite_amiga_t *sprite = new sprite_amiga_t(width*8, height);

  uint8_t *src_1 = reinterpret_cast<uint8_t*>(data);
  size_t bp2s = width * 2 * height;
  uint8_t *src_2 = src_1 + bp2s;
  color_t *res = sprite->get_writable_data();

  for (size_t y = 0; y < height; y++) {
    for (size_t i = 0; i < width; i++) {
      for (int k = 7; k >= 0; k--) {
        uint8_t color = 0;
        color |= (((*src_1) >> k) & 0x01) << 0;
        color |= (((*(src_1 + width)) >> k) & 0x01) << 1;
        color |= (((*src_2) >> k) & 0x01) << 2;
        color |= (((*(src_2 + width)) >> k) & 0x01) << 3;
        color |= 0x10;
        res->red = palette[color*3+0];    // R
        res->green = palette[color*3+1];  // G
        res->blue = palette[color*3+2];   // B
        res->alpha = 0xFF;                // A
        res++;
      }
      src_1++;
      src_2++;
    }
    src_1 += width;
    src_2 += width;
  }

  return sprite;
}

sprite_amiga_t::sprite_amiga_t(unsigned int width, unsigned int height) {
  this->width = width;
  this->height = height;
  delta_x = 0;
  delta_y = 0;
  offset_x = 0;
  offset_y = 0;
  size_t size = width * height * 4;
  data = new uint8_t[size];
  memset(data, 0, size);
}

sprite_amiga_t::~sprite_amiga_t() {
  delete[] data;
}

sprite_amiga_t *
sprite_amiga_t::get_amiga_masked(sprite_t *mask) {
  if (mask->get_width() > width) {
    assert(0);
  }

  sprite_amiga_t *masked = new sprite_amiga_t(mask->get_width(),
                                              mask->get_height());

  uint32_t *pos = reinterpret_cast<uint32_t*>(masked->get_data());

  uint32_t *s_beg = reinterpret_cast<uint32_t*>(data);
  uint32_t *s_pos = s_beg;
  uint32_t *s_end = s_beg + (width * height);
  size_t s_delta = width - masked->get_width();

  uint32_t *m_pos = reinterpret_cast<uint32_t*>(mask->get_data());

  for (size_t y = 0; y < masked->get_height(); y++) {
    for (size_t x = 0; x < masked->get_width(); x++) {
      if (s_pos >= s_end) {
        s_pos = s_beg;
      }
      *pos++ = *s_pos++ & *m_pos++;
    }
    s_pos += s_delta;
  }

  return masked;
}

sprite_t *
sprite_amiga_t::get_masked(sprite_t *mask) {
  if (mask->get_width() > width) {
    assert(0);
  }

  sprite_amiga_t *masked = new sprite_amiga_t(mask->get_width(),
                                              mask->get_height());
  masked->set_offset(mask->get_offset_x(), mask->get_offset_y());

  uint32_t *pos = reinterpret_cast<uint32_t*>(masked->get_data());

  uint32_t *s_beg = reinterpret_cast<uint32_t*>(data);
  uint32_t *s_pos = s_beg;
  uint32_t *s_end = s_beg + (width * height);
  size_t s_delta = width - masked->get_width();

  uint32_t *m_pos = reinterpret_cast<uint32_t*>(mask->get_data());

  for (size_t y = 0; y < masked->get_height(); y++) {
    for (size_t x = 0; x < masked->get_width(); x++) {
      if (s_pos >= s_end) {
        s_pos = s_beg;
      }
      *pos++ = *s_pos++ & *m_pos++;
    }
    s_pos += s_delta;
  }

  return masked;
}

void
sprite_amiga_t::make_transparent(uint32_t rc) {
  uint32_t *p = reinterpret_cast<uint32_t*>(data);
  for (unsigned int y = 0; y < height; y++) {
    for (unsigned int x = 0; x < width; x++) {
      if ((*p & 0x00FFFFFF) == rc) {
        *p = 0x00000000;
      }
      p++;
    }
  }
}

sprite_amiga_t *
sprite_amiga_t::merge_horizontaly(sprite_amiga_t *right) {
  if (right->height != height) {
    return NULL;
  }

  sprite_amiga_t *result = new sprite_amiga_t(width + right->width, height);

  uint32_t *src_l = reinterpret_cast<uint32_t*>(data);
  uint32_t *src_r = reinterpret_cast<uint32_t*>(right->data);
  uint32_t *res = reinterpret_cast<uint32_t*>(result->data);
  for (size_t y = 0; y < height; y++) {
    for (size_t x = 0; x < width; x++) {
      *res++ = *src_l++;
    }
    for (size_t x = 0; x < right->width; x++) {
      *res++ = *src_r++;
    }
  }

  return result;
}

sprite_amiga_t *
sprite_amiga_t::split_horizontaly(bool return_right) {
  unsigned int res_width = width/2;
  sprite_amiga_t *s = new sprite_amiga_t(res_width, height);
  uint32_t *src = reinterpret_cast<uint32_t*>(data);
  uint32_t *res = reinterpret_cast<uint32_t*>(s->data);
  if (return_right) {
    src += res_width;
  }
  for (size_t y = 0; y < height; y++) {
    for (size_t x = 0; x < res_width; x++) {
      *res++ = *src++;
    }
    src += res_width;
  }

  return s;
}

void
sprite_amiga_t::stick(sprite_amiga_t *sticker, unsigned int x, unsigned int y) {
  uint32_t *base = reinterpret_cast<uint32_t*>(data);
  uint32_t *stkr = reinterpret_cast<uint32_t*>(sticker->data);

  base += y * width;
  for (size_t y = 0; y < sticker->height; y++) {
    base += x;
    for (size_t x = 0; x < sticker->width; x++) {
      uint32_t pixel = *stkr++;
      if ((pixel & 0xFF000000) != 0) {
        *base = pixel;
      }
      base++;
    }
  }
}

void
sprite_amiga_t::fill(color_t color) {
  color_t *c = get_writable_data();
  for (unsigned int i = 0; i < (width * height); i++) {
    *c++ = color;
  }
}

void
sprite_amiga_t::fill_masked(color_t color) {
  color_t *c = get_writable_data();
  for (unsigned int i = 0; i < (width * height); i++) {
    color_t *cc = c++;
    if (cc->alpha != 0x00) {
      *cc = color;
    }
  }
}

palette_amiga_t::palette_amiga_t(uint8_t *palette_amiga) {
  this->palette_amiga = palette_amiga;
}

color_t
palette_amiga_t::get_color(size_t index) const {
  color_t color = {0, 0, 0, 0};

  if (index < 32) {
    color.red   = palette_amiga[index*3+0];  // R
    color.green = palette_amiga[index*3+1];  // G
    color.blue  = palette_amiga[index*3+2];  // B
    color.alpha = 0xFF;                      // A
  }

  return color;
}

palette_t *
data_source_amiga_t::get_palette(unsigned int index) {
  palette_t *res = NULL;

  if (index < 3) {
    uint8_t *pals[3] = {palette, palette3, palette2};
    res = new palette_amiga_t(pals[index]);
  }

  return res;
}
