/*
 * resourceview.h - FSSResourceView declaration
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

#ifndef SRC_RESOURCEVIEW_H_
#define SRC_RESOURCEVIEW_H_

#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include "src/audioplayer.h"
#include "src/data.h"

struct SDL_Renderer;
struct SDL_Texture;

// Shows one resource of one data source.
class FSSResourceView {
 public:
  // Settings shared by all views.
  struct Options {
    Data::Sprite::Color sprite_color;
    int zoom;
  };

 protected:
  SDL_Renderer *renderer;
  Data::PSource source;
  bool source_loaded;
  Data::Resource resource;
  size_t index;

  Data::PSprite sprite;
  SDL_Texture *texture;
  Data::Sprite::Color texture_color;

  PBuffer data;
  std::string data_ext;
  FSSAudioPlayer player;

  std::vector<Data::Animation> animation;
  // animation playback: the phases are shown as the game draws a serf
  struct SpriteTexture {
    Data::PSprite sprite;
    SDL_Texture *texture;
  };
  std::map<uint64_t, SpriteTexture> anim_textures;
  Data::Sprite::Color anim_color;
  size_t anim_serf_type;
  bool anim_playing;
  double anim_position;      // seconds
  double anim_start_position;
  uint64_t anim_start_ticks;  // ns
  std::string status;

 public:
  explicit FSSResourceView(SDL_Renderer *renderer);
  virtual ~FSSResourceView();

  void set_source(Data::PSource source, bool loaded);
  void select(Data::Resource resource, size_t index,
              const Data::Sprite::Color &color);
  // Draws the content of the current ImGui window.
  void draw(Options *options);
  // playing or decoding: the view needs frames
  bool is_playing() const {
    return player.is_playing() || player.is_decoding() || anim_playing;
  }

 protected:
  void clear();
  void load_sprite(const Data::Sprite::Color &color);
  void draw_sprite(Options *options);
  void draw_sound();
  void draw_music();
  void draw_player();
  std::string find_soundfont() const;
  void draw_animation(Options *options);
  void draw_zoom_and_color(Options *options);
  const SpriteTexture *anim_sprite(Data::Resource res, int index,
                                   bool colored);
  void clear_anim_textures();
  void save_sprite();
  void save_data();
};

#endif  // SRC_RESOURCEVIEW_H_
