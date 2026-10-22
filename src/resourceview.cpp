/*
 * resourceview.cpp - FSSResourceView implementation
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

#include "src/resourceview.h"

#include <SDL3/SDL.h>

#include <algorithm>
#include <cmath>
#include <cstring>

#include "imgui.h"

#include "src/buffer.h"
#include "src/data-source.h"
#include "src/filedialog.h"
#include "src/serfbody.h"
#include "src/spriteimage.h"

namespace {

bool
same_color(const Data::Sprite::Color &a, const Data::Sprite::Color &b) {
  return a.red == b.red && a.green == b.green && a.blue == b.blue &&
         a.alpha == b.alpha;
}

std::string
default_file_name(Data::Resource resource, size_t index,
                  const std::string &ext) {
  // the documents folder path ends with a separator
  const char *folder = SDL_GetUserFolder(SDL_FOLDER_DOCUMENTS);
  return std::string(folder != nullptr ? folder : "") +
         Data::get_resource_name(resource) + "_" + std::to_string(index) +
         "." + ext;
}

void
info_row(const char *name, int value) {
  ImGui::TableNextRow();
  ImGui::TableNextColumn();
  ImGui::TextUnformatted(name);
  ImGui::TableNextColumn();
  ImGui::Text("%d", value);
}

}  // namespace

FSSResourceView::FSSResourceView(SDL_Renderer *_renderer)
  : renderer(_renderer)
  , source_loaded(false)
  , resource(Data::AssetNone)
  , index(0)
  , texture(nullptr)
  , texture_color()
  , anim_color()
  , anim_serf_type(0)
  , anim_playing(false)
  , anim_position(0.)
  , anim_start_position(0.)
  , anim_start_ticks(0) {
}

FSSResourceView::~FSSResourceView() {
  clear();
}

void
FSSResourceView::set_source(Data::PSource _source, bool loaded) {
  source = _source;
  source_loaded = loaded;
  clear();
  resource = Data::AssetNone;
}

// SoundFont for the MIDI music, looked for like freeserf does: the
// SDL_SOUNDFONTS variable, then TimGM6mb.sf2 next to the program or the data.
std::string
FSSResourceView::find_soundfont() const {
  const char *env = SDL_getenv("SDL_SOUNDFONTS");
  if (env != nullptr) {
    return env;
  }

  std::vector<std::string> dirs;
  const char *base = SDL_GetBasePath();
  if (base != nullptr) {
    dirs.push_back(base);
  }
  if (source) {
    // the path is the data folder or the data file in it
    std::string path = source->get_path();
    dirs.push_back(path + "/");
    size_t sep = path.find_last_of("/\\");
    if (sep != std::string::npos) {
      dirs.push_back(path.substr(0, sep + 1));
    }
  }
  for (const std::string &dir : dirs) {
    std::string path = dir + "TimGM6mb.sf2";
    if (SDL_GetPathInfo(path.c_str(), nullptr)) {
      return path;
    }
  }

  return std::string();
}

void
FSSResourceView::clear() {
  if (texture != nullptr) {
    SDL_DestroyTexture(texture);
    texture = nullptr;
  }
  sprite = nullptr;
  data = nullptr;
  player.clear();
  animation.clear();
  clear_anim_textures();
  anim_playing = false;
  anim_position = 0.;
  status.clear();
}

void
FSSResourceView::clear_anim_textures() {
  for (auto &it : anim_textures) {
    if (it.second.texture != nullptr) {
      SDL_DestroyTexture(it.second.texture);
    }
  }
  anim_textures.clear();
}

const FSSResourceView::SpriteTexture *
FSSResourceView::anim_sprite(Data::Resource res, int sprite_index,
                             bool colored) {
  if (sprite_index < 0) {
    return nullptr;
  }
  uint64_t key = (static_cast<uint64_t>(res) << 32) |
                 static_cast<uint32_t>(sprite_index);
  auto it = anim_textures.find(key);
  if (it == anim_textures.end()) {
    SpriteTexture st = { nullptr, nullptr };
    st.sprite = source->get_sprite(res, static_cast<size_t>(sprite_index),
                                   colored ? anim_color
                                           : Data::Sprite::Color{0, 0, 0, 0});
    SDL_Surface *surface = FSSSpriteImage::create_surface(st.sprite);
    if (surface != nullptr) {
      st.texture = SDL_CreateTextureFromSurface(renderer, surface);
      SDL_DestroySurface(surface);
      if (st.texture != nullptr) {
        SDL_SetTextureScaleMode(st.texture, SDL_SCALEMODE_NEAREST);
      }
    }
    it = anim_textures.emplace(key, st).first;
  }
  return (it->second.texture != nullptr) ? &it->second : nullptr;
}

void
FSSResourceView::select(Data::Resource _resource, size_t _index,
                        const Data::Sprite::Color &color) {
  if (_resource == resource && _index == index) {
    if (Data::get_resource_type(resource) == Data::TypeSprite &&
        !same_color(color, texture_color)) {
      load_sprite(color);
    }
    return;
  }

  clear();
  resource = _resource;
  index = _index;
  if (!source || !source_loaded || resource == Data::AssetNone) {
    return;
  }

  switch (Data::get_resource_type(resource)) {
    case Data::TypeSprite:
      load_sprite(color);
      break;
    case Data::TypeAnimation:
      for (size_t i = 0; i < source->get_animation_phase_count(index); i++) {
        // phases are addressed in 1/8 steps
        animation.push_back(source->get_animation(index, i << 3));
      }
      break;
    case Data::TypeSound:
      data = source->get_sound(index);
      data_ext = "wav";
      if (data && !player.load_wav(data)) {
        status = "Failed to decode the sound";
      }
      break;
    case Data::TypeMusic:
      data = source->get_music(index);
      data_ext = (source->get_music_format() == Data::MusicFormatMod) ? "mod"
                                                                      : "mid";
      if (data && FSSAudioPlayer::can_play_music() &&
          !player.load_music(data, find_soundfont())) {
        status = (data_ext == "mid")
                 ? "Failed to decode the music (MIDI needs the TimGM6mb.sf2 "
                   "SoundFont next to the program or the data)"
                 : "Failed to decode the music";
      }
      break;
    default:
      break;
  }
}

void
FSSResourceView::load_sprite(const Data::Sprite::Color &color) {
  if (texture != nullptr) {
    SDL_DestroyTexture(texture);
    texture = nullptr;
  }
  texture_color = color;
  sprite = source->get_sprite(resource, index, color);

  SDL_Surface *surface = FSSSpriteImage::create_surface(sprite);
  if (surface != nullptr) {
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface);
    if (texture != nullptr) {
      SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
    }
  }
}

void
FSSResourceView::draw(Options *options) {
  if (!source) {
    return;
  }
  if (!source_loaded) {
    ImGui::TextDisabled("Data not found:");
    ImGui::TextWrapped("%s", source->get_path().c_str());
    ImGui::TextDisabled("Set the path in File > Sources.");
    return;
  }

  if (resource == Data::AssetNone) {
    ImGui::TextDisabled("Select a resource");
    return;
  }

  ImGui::TextDisabled("%s #%zu", Data::get_resource_name(resource).c_str(),
                      index);

  switch (Data::get_resource_type(resource)) {
    case Data::TypeSprite:
      draw_sprite(options);
      break;
    case Data::TypeAnimation:
      draw_animation(options);
      break;
    case Data::TypeSound:
      draw_sound();
      break;
    case Data::TypeMusic:
      draw_music();
      break;
    default:
      break;
  }

  if (!status.empty()) {
    ImGui::TextColored(ImVec4(1.f, .5f, .4f, 1.f), "%s", status.c_str());
  }
}

void
FSSResourceView::draw_sprite(Options *options) {
  if (!sprite) {
    ImGui::TextDisabled("Not present in the data");
    return;
  }

  if (ImGui::Button("Save...")) {
    save_sprite();
  }
  ImGui::SameLine();
  if (ImGui::Button("Copy")) {
    FSSSpriteImage::copy_to_clipboard(sprite);
  }
  ImGui::SetItemTooltip("Copy the image to the clipboard (or double-click it)");
  ImGui::SameLine();
  draw_zoom_and_color(options);

  if (ImGui::BeginTable("info", 2, ImGuiTableFlags_SizingFixedFit)) {
    info_row("width", static_cast<int>(sprite->get_width()));
    info_row("height", static_cast<int>(sprite->get_height()));
    info_row("delta_x", sprite->get_delta_x());
    info_row("delta_y", sprite->get_delta_y());
    info_row("offset_x", sprite->get_offset_x());
    info_row("offset_y", sprite->get_offset_y());
    ImGui::EndTable();
  }

  if (texture == nullptr) {
    return;
  }
  ImGui::BeginChild("image", ImVec2(0, 0), ImGuiChildFlags_Borders,
                    ImGuiWindowFlags_HorizontalScrollbar);
  ImVec2 size(static_cast<float>(sprite->get_width() * options->zoom),
              static_cast<float>(sprite->get_height() * options->zoom));
  ImGui::Image(ImTextureRef(reinterpret_cast<ImTextureID>(texture)), size);
  if (ImGui::IsItemHovered() &&
      ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
    FSSSpriteImage::copy_to_clipboard(sprite);
  }
  ImGui::EndChild();
}

void
FSSResourceView::draw_sound() {
  if (!data) {
    ImGui::TextDisabled("Not present in the data");
    return;
  }

  draw_player();
  if (ImGui::Button("Save...")) {
    save_data();
  }
  ImGui::Text("%zu bytes", data->get_size());
}

void
FSSResourceView::draw_player() {
  player.update();
  if (!player.is_loaded()) {
    return;
  }

  if (ImGui::Button(player.is_playing() ? "Stop" : "Play",
                    ImVec2(ImGui::GetFontSize() * 4.f, 0))) {
    if (player.is_playing()) {
      player.stop();
    } else {
      player.play();
    }
  }
  ImGui::SameLine();
  float position = static_cast<float>(player.get_position());
  ImGui::SetNextItemWidth(-ImGui::GetFontSize() * 6.f);
  // the duration grows while music is being decoded
  const char *format = player.is_decoding() ? "%.1f s (decoding...)"
                                            : "%.1f s";
  if (ImGui::SliderFloat("##position", &position, 0.f,
                         static_cast<float>(player.get_duration()),
                         format)) {
    player.seek(position);
  }
  ImGui::SameLine();
}

void
FSSResourceView::draw_music() {
  if (!data) {
    ImGui::TextDisabled("Not present in the data");
    return;
  }

  draw_player();
  if (ImGui::Button("Save...")) {
    save_data();
  }
  ImGui::Text("%s, %zu bytes", data_ext == "mod" ? "MOD" : "MIDI",
              data->get_size());
  if (!FSSAudioPlayer::can_play_music()) {
    ImGui::TextDisabled("Built without SDL3_mixer, save the music to listen.");
  }
}

void
FSSResourceView::draw_zoom_and_color(Options *options) {
  ImGui::SetNextItemWidth(ImGui::GetFontSize() * 6.f);
  ImGui::SliderInt("Zoom", &options->zoom, 1, 8, "x%d");

  float color[3] = { options->sprite_color.red / 255.f,
                     options->sprite_color.green / 255.f,
                     options->sprite_color.blue / 255.f };
  if (ImGui::ColorEdit3("Player color", color,
                        ImGuiColorEditFlags_NoInputs)) {
    options->sprite_color.red = static_cast<unsigned char>(color[0] * 255.f);
    options->sprite_color.green = static_cast<unsigned char>(color[1] * 255.f);
    options->sprite_color.blue = static_cast<unsigned char>(color[2] * 255.f);
  }
}

namespace {

// A phase lasts 8 game ticks, 80 ms at the default game speed. The counter of
// a serf goes down, so the phases are shown from the last to the first.
const double anim_phase_duration = 0.08;

}  // namespace

void
FSSResourceView::draw_animation(Options *options) {
  if (animation.empty()) {
    ImGui::TextDisabled("Not present in the data");
    return;
  }

  // playback
  size_t count = animation.size();
  double duration = static_cast<double>(count) * anim_phase_duration;
  if (anim_playing) {
    double elapsed = static_cast<double>(SDL_GetTicksNS() - anim_start_ticks)
                     / 1e9;
    anim_position = std::fmod(anim_start_position + elapsed, duration);
  }
  if (ImGui::Button(anim_playing ? "Stop" : "Play",
                    ImVec2(ImGui::GetFontSize() * 4.f, 0))) {
    anim_playing = !anim_playing;
    if (anim_playing) {
      anim_start_position = anim_position;
      anim_start_ticks = SDL_GetTicksNS();
    }
  }
  ImGui::SameLine();
  size_t step = std::min(count - 1, static_cast<size_t>(anim_position /
                                                        anim_phase_duration));
  size_t phase = count - 1 - step;
  float position = static_cast<float>(anim_position);
  char label[32];
  snprintf(label, sizeof(label), "phase %zu, %%.2f s", phase);
  ImGui::SetNextItemWidth(-ImGui::GetFontSize() * 6.f);
  if (ImGui::SliderFloat("##position", &position, 0.f,
                         static_cast<float>(duration - 1e-3), label)) {
    anim_position = position;
    anim_start_position = anim_position;
    anim_start_ticks = SDL_GetTicksNS();
  }

  // serf type, zoom and player color
  ImGui::SetNextItemWidth(ImGui::GetFontSize() * 9.f);
  if (ImGui::BeginCombo("Serf", FSSSerfBody::type(anim_serf_type).name)) {
    for (size_t i = 0; i < FSSSerfBody::type_count(); i++) {
      if (ImGui::Selectable(FSSSerfBody::type(i).name, i == anim_serf_type)) {
        anim_serf_type = i;
      }
    }
    ImGui::EndCombo();
  }
  ImGui::SetItemTooltip("The animations are the same for all serfs, the serf "
                        "type selects the sprites");
  ImGui::SameLine();
  draw_zoom_and_color(options);
  if (!same_color(options->sprite_color, anim_color)) {
    clear_anim_textures();
    anim_color = options->sprite_color;
  }

  // the layers of a phase as the game draws a serf: shadow, torso, head
  struct Layer {
    const SpriteTexture *st;
    int x;
    int y;
  };
  auto layers = [this](size_t ph, std::vector<Layer> *out) {
    out->clear();
    const Data::Animation &a = animation[ph];
    const SpriteTexture *shadow = anim_sprite(Data::AssetSerfShadow, 0, false);
    if (shadow != nullptr) {
      out->push_back({shadow, a.x + shadow->sprite->get_offset_x(),
                      a.y + shadow->sprite->get_offset_y()});
    }
    int torso_index = 0;
    int head_index = -1;
    if (!FSSSerfBody::sprites(a.sprite, anim_serf_type, &torso_index,
                              &head_index)) {
      return;
    }
    const SpriteTexture *torso = anim_sprite(Data::AssetSerfTorso,
                                             torso_index, true);
    if (torso == nullptr) {
      return;
    }
    out->push_back({torso, a.x + torso->sprite->get_offset_x(),
                    a.y + torso->sprite->get_offset_y()});
    const SpriteTexture *head = anim_sprite(Data::AssetSerfHead, head_index,
                                            false);
    if (head != nullptr) {
      out->push_back({head, a.x + torso->sprite->get_delta_x() +
                            head->sprite->get_offset_x(),
                      a.y + torso->sprite->get_delta_y() +
                            head->sprite->get_offset_y()});
    }
  };

  // the bounds of all phases, so the serf does not jump around
  std::vector<Layer> list;
  int min_x = 0, min_y = 0, max_x = 1, max_y = 1;
  bool first = true;
  for (size_t i = 0; i < count; i++) {
    layers(i, &list);
    for (const Layer &l : list) {
      int w = static_cast<int>(l.st->sprite->get_width());
      int h = static_cast<int>(l.st->sprite->get_height());
      if (first) {
        min_x = l.x; min_y = l.y; max_x = l.x + w; max_y = l.y + h;
        first = false;
      }
      min_x = std::min(min_x, l.x);
      min_y = std::min(min_y, l.y);
      max_x = std::max(max_x, l.x + w);
      max_y = std::max(max_y, l.y + h);
    }
  }

  float zoom = static_cast<float>(options->zoom);
  float pad = ImGui::GetFontSize();
  ImVec2 canvas((max_x - min_x) * zoom + pad * 2.f,
                (max_y - min_y) * zoom + pad * 2.f);
  ImGui::BeginChild("animation", ImVec2(0, std::min(canvas.y,
                                        ImGui::GetContentRegionAvail().y * .7f)),
                    ImGuiChildFlags_Borders,
                    ImGuiWindowFlags_HorizontalScrollbar);
  ImVec2 origin = ImGui::GetCursorScreenPos();
  layers(phase, &list);
  ImDrawList *draw_list = ImGui::GetWindowDrawList();
  for (const Layer &l : list) {
    ImVec2 p0(origin.x + pad + (l.x - min_x) * zoom,
              origin.y + pad + (l.y - min_y) * zoom);
    ImVec2 p1(p0.x + l.st->sprite->get_width() * zoom,
              p0.y + l.st->sprite->get_height() * zoom);
    draw_list->AddImage(ImTextureRef(reinterpret_cast<ImTextureID>(
                          l.st->texture)), p0, p1);
  }
  if (list.empty()) {
    ImGui::TextDisabled("No sprites for this serf type");
  }
  ImGui::Dummy(canvas);
  ImGui::EndChild();

  ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders |
                          ImGuiTableFlags_ScrollY |
                          ImGuiTableFlags_SizingStretchSame;
  if (ImGui::BeginTable("phases", 4, flags)) {
    ImGui::TableSetupScrollFreeze(0, 1);
    ImGui::TableSetupColumn("Phase");
    ImGui::TableSetupColumn("Sprite");
    ImGui::TableSetupColumn("X");
    ImGui::TableSetupColumn("Y");
    ImGui::TableHeadersRow();
    for (size_t i = 0; i < count; i++) {
      ImGui::TableNextRow();
      if (i == phase) {
        ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1,
                               ImGui::GetColorU32(ImGuiCol_Header));
      }
      ImGui::TableNextColumn();
      // a click on a phase shows it
      char row[16];
      snprintf(row, sizeof(row), "%zu", i);
      if (ImGui::Selectable(row, false, ImGuiSelectableFlags_SpanAllColumns)) {
        anim_playing = false;
        anim_position = (static_cast<double>(count - 1 - i) + .5) *
                        anim_phase_duration;
      }
      ImGui::TableNextColumn();
      ImGui::Text("%d", animation[i].sprite);
      ImGui::TableNextColumn();
      ImGui::Text("%d", animation[i].x);
      ImGui::TableNextColumn();
      ImGui::Text("%d", animation[i].y);
    }
    ImGui::EndTable();
  }
}

void
FSSResourceView::save_sprite() {
  Data::PSprite image = sprite;
  FSSFileDialog::save_file(default_file_name(resource, index, "png"),
                           "PNG image", "png",
                           [this, image](const std::string &path) {
    status = FSSSpriteImage::save_png(image, path) ? std::string()
                                          : "Failed to save " + path;
  });
}

void
FSSResourceView::save_data() {
  PBuffer buffer = data;
  FSSFileDialog::save_file(default_file_name(resource, index, data_ext),
                           data_ext == "wav" ? "Sound" : "Music", data_ext,
                           [this, buffer](const std::string &path) {
    status = buffer->write(path) ? std::string() : "Failed to save " + path;
  });
}
