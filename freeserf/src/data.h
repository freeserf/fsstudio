/*
 * data.h - Definitions for data file access.
 *
 * Copyright (C) 2012-2016  Jon Lund Steffensen <jonlst@gmail.com>
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

#ifndef SRC_DATA_H_
#define SRC_DATA_H_

#include <string>
#include <list>

typedef enum {
  data_type_unknown,
  data_type_sprite,
  data_type_animation,
  data_type_sound,
  data_type_music,
  data_type_palette,
} data_type_t;

typedef enum {
  data_res_none,
  data_res_art_landscape,
  data_res_animation,
  data_res_serf_shadow,
  data_res_dotted_lines,
  data_res_art_flag,
  data_res_art_box,
  data_res_credits_bg,
  data_res_logo,
  data_res_symbol,
  data_res_map_mask_up,
  data_res_map_mask_down,
  data_res_path_mask,
  data_res_map_ground,
  data_res_path_ground,
  data_res_game_object,
  data_res_frame_top,
  data_res_map_border,
  data_res_map_waves,
  data_res_frame_popup,
  data_res_indicator,
  data_res_font,
  data_res_font_shadow,
  data_res_icon,
  data_res_map_object,
  data_res_map_shadow,
  data_res_panel_button,
  data_res_frame_bottom,
  data_res_serf_torso,
  data_res_serf_head,
  data_res_frame_split,
  data_res_sound,
  data_res_music,
  data_res_cursor,
  data_res_palette,
} data_res_class_t;

class data_source_t;

class data_t {
 protected:
  static data_t *instance;
  data_source_t *data_source;
  std::list<std::string> search_paths;

  data_t();

 public:
  virtual ~data_t();

  static data_t *get_instance();

  bool load(const std::string &path);

  data_source_t *get_data_source() const { return data_source; }

  static data_type_t get_resource_type(data_res_class_t resource);
  static unsigned int get_resource_count(data_res_class_t resource);
  static const char *get_resource_name(data_res_class_t resource);

 protected:
  void add_to_search_paths(const char *path, const char *suffix);
};

#endif  // SRC_DATA_H_
