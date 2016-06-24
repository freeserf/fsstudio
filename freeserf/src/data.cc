/*
 * data.cc - Game resources file functions
 *
 * Copyright (C) 2014-2016  Wicked_Digger <wicked_digger@mail.ru>
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

#include "src/data.h"

#include <cstdlib>

#include "src/log.h"
#include "src/data-source-dos.h"
#include "src/data-source-amiga.h"

typedef struct {
  data_res_class_t resource;
  data_type_t type;
  unsigned int count;
  const char *name;
} data_resource_t;

data_resource_t data_resources[] = {
  { data_res_none,          data_type_unknown,   0,   "error"         },
  { data_res_art_landscape, data_type_sprite,    1,   "art_landscape" },
  { data_res_animation,     data_type_animation, 1,   "animation"     },
  { data_res_serf_shadow,   data_type_sprite,    1,   "serf_shadow"   },
  { data_res_dotted_lines,  data_type_sprite,    7,   "dotted_lines"  },
  { data_res_art_flag,      data_type_sprite,    7,   "art_flag"      },
  { data_res_art_box,       data_type_sprite,    14,  "art_box"       },
  { data_res_credits_bg,    data_type_sprite,    1,   "credits_bg"    },
  { data_res_logo,          data_type_sprite,    1,   "logo"          },
  { data_res_symbol,        data_type_sprite,    16,  "symbol"        },
  { data_res_map_mask_up,   data_type_sprite,    81,  "map_mask_up"   },
  { data_res_map_mask_down, data_type_sprite,    81,  "map_mask_down" },
  { data_res_path_mask,     data_type_sprite,    26,  "path_mask"     },
  { data_res_map_ground,    data_type_sprite,    33,  "map_ground"    },
  { data_res_path_ground,   data_type_sprite,    10,  "path_ground"   },
  { data_res_game_object,   data_type_sprite,    280, "game_object"   },
  { data_res_frame_top,     data_type_sprite,    4,   "frame_top"     },
  { data_res_map_border,    data_type_sprite,    10,  "map_border"    },
  { data_res_map_waves,     data_type_sprite,    16,  "map_waves"     },
  { data_res_frame_popup,   data_type_sprite,    4,   "frame_popup"   },
  { data_res_indicator,     data_type_sprite,    8,   "indicator"     },
  { data_res_font,          data_type_sprite,    44,  "font"          },
  { data_res_font_shadow,   data_type_sprite,    44,  "font_shadow"   },
  { data_res_icon,          data_type_sprite,    318, "icon"          },
  { data_res_map_object,    data_type_sprite,    194, "map_object"    },
  { data_res_map_shadow,    data_type_sprite,    194, "map_shadow"    },
  { data_res_panel_button,  data_type_sprite,    25,  "panel_button"  },
  { data_res_frame_bottom,  data_type_sprite,    26,  "frame_bottom"  },
  { data_res_serf_torso,    data_type_sprite,    541, "serf_torso"    },
  { data_res_serf_head,     data_type_sprite,    630, "serf_head"     },
  { data_res_frame_split,   data_type_sprite,    3,   "frame_split"   },
  { data_res_sound,         data_type_sound,     90,  "sound"         },
  { data_res_music,         data_type_music,     7,   "music"         },
  { data_res_cursor,        data_type_sprite,    1,   "cursor"        },
  { data_res_palette,       data_type_palette,   3,   "palette"       },
};

data_t *data_t::instance = NULL;

data_t::data_t() {
  data_source = NULL;
  instance = this;
}

data_t::~data_t() {
  if (data_source != NULL) {
    delete data_source;
    data_source = NULL;
  }

  instance = NULL;
}

data_t *
data_t::get_instance() {
  if (instance == NULL) {
    instance = new data_t();
  }

  return instance;
}

void
data_t::add_to_search_paths(const char *path,
                            const char *suffix) {
  if (path == NULL) {
    return;
  }

  std::string res_path = path;
  if (suffix != NULL) {
    res_path += '/';
    res_path += suffix;
  }

  search_paths.push_back(res_path);
}

bool
data_t::load(const std::string &path) {
  /* If it possible, prefer DOS game data. */
  data_source_t *data_sources[] = { new data_source_dos_t(),
                                    new data_source_amiga_t(),
                                    NULL, };

  /* Use specified path. If something was specified
     but not found, this function should fail without
     looking anywhere else. */
  add_to_search_paths(path.c_str(), NULL);

  /* If a path is not specified (path is NULL) then
     the configuration file is searched for in the directories
     specified by the XDG Base Directory Specification
     <http://standards.freedesktop.org/basedir-spec/basedir-spec-latest.html>.

     On windows platforms the %localappdata% is used in place of XDG_CONFIG_HOME.
  */

  /* Look in home */
  add_to_search_paths(std::getenv("XDG_DATA_HOME"), "freeserf");
  add_to_search_paths(std::getenv("HOME"), ".local/share/freeserf");
#ifdef _WIN32
  add_to_search_paths(std::getenv("userprofile"), ".local/share/freeserf");
  add_to_search_paths(std::getenv("LOCALAPPDATA"), "/freeserf");
#endif

  char *env = std::getenv("XDG_DATA_DIRS");
  std::string dirs = (env == NULL) ? std::string() : env;
  while (!dirs.empty()) {
    size_t pos = dirs.find(':');
    std::string dir = dirs.substr(0, pos);
    dirs.replace(0, (pos == std::string::npos) ? pos : pos + 1, std::string());
    add_to_search_paths(dir.c_str(), "freeserf");
  }

  add_to_search_paths("/usr/local/share", "freeserf");
  add_to_search_paths("/usr/share", "freeserf");

  for (int i = 0; data_sources[i] != NULL; i++) {
    std::list<std::string>::iterator it = search_paths.begin();
    for (; it != search_paths.end(); ++it) {
      std::string res_path;
      if (data_sources[i]->check(*it, &res_path)) {
        LOGI("data", "Game data found in '%s'...", res_path.c_str());
        if (data_sources[i]->load(res_path)) {
          data_source = data_sources[i];
          break;
        }
      }
    }
    if (data_source != NULL) {
      break;
    }
  }

  for (int i = 0; data_sources[i] != NULL; i++) {
    if (data_sources[i] != data_source) {
      delete data_sources[i];
    }
  }

  return (data_source != NULL);
}

data_type_t
data_t::get_resource_type(data_res_class_t resource) {
  return data_resources[resource].type;
}

unsigned int
data_t::get_resource_count(data_res_class_t resource) {
  return data_resources[resource].count;
}

const char *
data_t::get_resource_name(data_res_class_t resource) {
  return data_resources[resource].name;
}
