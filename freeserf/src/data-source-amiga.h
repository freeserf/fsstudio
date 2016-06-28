/*
 * data-source-amiga.h - Amiga data loading
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

#ifndef SRC_DATA_SOURCE_AMIGA_H_
#define SRC_DATA_SOURCE_AMIGA_H_

#include <string>

#include "src/data-source.h"

class sprite_amiga_t : public sprite_t {
 protected:
  unsigned int width;
  unsigned int height;
  int delta_x;
  int delta_y;
  int offset_x;
  int offset_y;
  uint8_t *data;

 public:
  sprite_amiga_t(unsigned int width, unsigned int height);
  virtual ~sprite_amiga_t();

  virtual uint8_t *get_data() const { return data; }
  virtual unsigned int get_width() const { return width; }
  virtual unsigned int get_height() const { return height; }
  virtual int get_delta_x() const { return delta_x; }
  virtual int get_delta_y() const { return delta_y; }
  virtual int get_offset_x() const { return offset_x; }
  virtual int get_offset_y() const { return offset_y; }

  virtual sprite_t *get_masked(sprite_t *mask);

  void set_delta(int x, int y) { delta_x = x; delta_y = y; }
  void set_offset(int x, int y) { offset_x = x; offset_y = y; }
  sprite_amiga_t *get_amiga_masked(sprite_t *mask);
  void clear() {}
  color_t *get_writable_data() { return reinterpret_cast<color_t*>(data); }

  void make_transparent(uint32_t rc = 0);
  sprite_amiga_t *merge_horizontaly(sprite_amiga_t *right);
  sprite_amiga_t *split_horizontaly(bool return_right);
  void stick(sprite_amiga_t *sticker, unsigned int x, unsigned int y);
  void fill(color_t color);
  void fill_masked(color_t color);
};

class palette_amiga_t : public palette_t {
 protected:
  uint8_t *palette_amiga;

 public:
  explicit palette_amiga_t(uint8_t *palette_amiga);
  virtual ~palette_amiga_t() {}

  virtual size_t get_size() const { return 32; }
  virtual color_t get_color(size_t index) const;
};

class data_source_amiga_t : public data_source_t {
 public:
  data_source_amiga_t();
  virtual ~data_source_amiga_t();

  virtual bool check(const std::string &path, std::string *load_path);
  virtual bool load(const std::string &path);

  virtual sprite_t *get_sprite(data_res_class_t res, unsigned int index,
                               int color_off);

  virtual color_t get_color(unsigned int index);

  virtual animation_t *get_animation(unsigned int animation,
                                     unsigned int phase);

  virtual void *get_sound(unsigned int index, size_t *size);
  virtual void *get_music(unsigned int index, size_t *size);

  virtual palette_t *get_palette(unsigned int index);

 private:
  void *gfxfast;
  void *gfxchip;
  void *sound;
  void *music;
  size_t music_size;

  void *data_pointers[24];

  size_t *icon_catalog;

  void decode(void *data, size_t size);
  void *unpack(void *data, size_t size, size_t *unpacked_size);

  unsigned char *get_data_from_catalog(size_t catalog_index, size_t index,
                                       void *base);

  sprite_amiga_t *get_menu_sprite(unsigned int index, void *block,
                                  unsigned int width, unsigned int height,
                                  unsigned char compression,
                                  unsigned char filling);
  sprite_t *get_icon_sprite(unsigned int index);
  sprite_amiga_t *get_ground_sprite(unsigned int index);
  sprite_t *get_ground_mask_sprite(unsigned int index);
  sprite_amiga_t *get_mirrored_horizontaly_sprite(sprite_t *sprite);
  sprite_t *get_path_mask_sprite(unsigned int index);
  sprite_t *get_game_object_sprite(unsigned int catalog, unsigned int index);
  sprite_t *get_torso_sprite(unsigned int index);
  sprite_t *get_map_object_sprite(unsigned int index);
  sprite_t *get_map_object_shadow(unsigned int index);
  sprite_amiga_t *get_hud_sprite(unsigned int index);

  sprite_amiga_t *decode_planned_sprite(void *data,
                                        unsigned int width, unsigned int height,
                                        uint8_t compression,
                                        uint8_t filling, uint8_t *palette,
                                        bool invert = true);
  sprite_amiga_t *decode_interlased_sprite(void *data,
                                           unsigned int width,
                                           unsigned int height,
                                           uint8_t compression,
                                           uint8_t filling, uint8_t *palette,
                                           size_t skip_lines = 0);
  sprite_amiga_t *decode_amiga_sprite(void *data,
                                      unsigned int width,
                                      unsigned int height,
                                      uint8_t *palette);
  sprite_amiga_t *decode_mask_sprite(void *data,
                                     unsigned int width, unsigned int height);

  unsigned int bitplane_count_from_compression(unsigned char compression);

  void *get_sound_data(unsigned int index, size_t *size);
};

#endif  // SRC_DATA_SOURCE_AMIGA_H_
