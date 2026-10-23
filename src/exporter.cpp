/*
 * exporter.cpp - FSSExporter implementation
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

#include "src/exporter.h"

#include <filesystem>
#include <iomanip>
#include <sstream>
#include <tuple>

#include "src/buffer.h"
#include "src/configfile.h"
#include "src/data-source.h"
#include "src/spriteimage.h"

namespace {

std::string
index_name(size_t index) {
  std::stringstream stream;
  stream << std::setfill('0') << std::setw(3) << index;
  return stream.str();
}

}  // namespace

FSSExporter::FSSExporter(Data::PSource _source, const std::string &_path,
                         unsigned int _scale)
  : source(_source)
  , path(_path)
  , scale(_scale)
  , name("Unnamed") {
}

bool
FSSExporter::do_export() {
  std::error_code error;
  if (!source || !std::filesystem::is_directory(path, error)) {
    return false;
  }

  ConfigFile meta;
  meta.set_value("general", "scale", scale);
  meta.set_value("general", "name", name);

  for (size_t r = Data::AssetExtroBg; r <= Data::AssetStar; r++) {
    Data::Resource res = static_cast<Data::Resource>(r);
    switch (Data::get_resource_type(res)) {
      case Data::TypeSprite:
        export_resource_sprite(res);
        break;
      case Data::TypeAnimation:
        export_resource_animation(res);
        break;
      case Data::TypeSound:
      case Data::TypeMusic:
        export_resource_data(res);
        break;
      default:
        break;
    }
    std::string res_name = Data::get_resource_name(res);
    meta.set_value("resources", res_name, res_name);
  }

  return meta.save(path + "/meta.ini");
}

std::string
FSSExporter::resource_dir(Data::Resource res) const {
  std::string dir = path + "/" + Data::get_resource_name(res);
  std::error_code error;
  std::filesystem::create_directories(dir, error);
  return dir;
}

void
FSSExporter::export_resource_data(Data::Resource res) {
  std::string dir = resource_dir(res);
  ConfigFile meta;

  std::string ext = "wav";
  if (res == Data::AssetMusic) {
    ext = (source->get_music_format() == Data::MusicFormatMod) ? "mod" : "mid";
  }

  for (size_t i = 0; i < Data::get_resource_count(res); i++) {
    PBuffer data = (res == Data::AssetSound) ? source->get_sound(i)
                                             : source->get_music(i);
    if (!data) {
      continue;
    }
    std::string file_name = index_name(i) + "." + ext;
    if (data->write(dir + "/" + file_name)) {
      meta.set_value(index_name(i), "path", file_name);
    }
  }

  meta.save(dir + "/meta.ini");
}

void
FSSExporter::export_resource_sprite(Data::Resource res) {
  std::string dir = resource_dir(res);
  ConfigFile meta;

  for (size_t i = 0; i < Data::get_resource_count(res); i++) {
    std::string section = index_name(i);
    Data::PSprite mask, image;
    std::tie(mask, image) = source->get_sprite_parts(res, i);

    if (mask && FSSSpriteImage::save_png(mask, dir + "/" + section + "m.png",
                                         scale)) {
      meta.set_value(section, "mask_path", section + "m.png");
    }
    if (image && FSSSpriteImage::save_png(image, dir + "/" + section + ".png",
                                          scale)) {
      meta.set_value(section, "image_path", section + ".png");
    }

    // geometry is taken from the mask when the sprite has one
    Data::PSprite sprite = mask ? mask : image;
    if (sprite) {
      int s = static_cast<int>(scale);
      meta.set_value(section, "delta_x", sprite->get_delta_x() * s);
      meta.set_value(section, "delta_y", sprite->get_delta_y() * s);
      meta.set_value(section, "offset_x", sprite->get_offset_x() * s);
      meta.set_value(section, "offset_y", sprite->get_offset_y() * s);
    }
  }

  meta.save(dir + "/meta.ini");
}

void
FSSExporter::export_resource_animation(Data::Resource res) {
  std::string dir = resource_dir(res);
  ConfigFile meta;

  for (size_t i = 0; i < Data::get_resource_count(res); i++) {
    std::string file_name = index_name(i) + ".ini";
    meta.set_value(index_name(i), "path", file_name);

    ConfigFile data;
    size_t count = source->get_animation_phase_count(i);
    data.set_value("general", "count", count);
    for (size_t j = 0; j < count; j++) {
      // phases are addressed in 1/8 steps
      Data::Animation animation = source->get_animation(i, j << 3);
      std::string phase = index_name(j);
      data.set_value(phase, "sprite", static_cast<int>(animation.sprite));
      data.set_value(phase, "x", animation.x * static_cast<int>(scale));
      data.set_value(phase, "y", animation.y * static_cast<int>(scale));
    }
    data.save(dir + "/" + file_name);
  }

  meta.save(dir + "/meta.ini");
}
