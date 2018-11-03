/*
 * exporter.cpp - FSSExporter implementation
 *
 * Copyright (C) 2017  Wicked_Digger <wicked_digger@mail.ru>
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

#include <QSettings>
#include <QImage>

#include "src/data-source.h"
#include "src/buffer.h"

FSSExporter::FSSExporter(PDataSource _source, QString _path, unsigned int _scale) {
  source = _source;
  path = _path;
  scale = _scale;
  name = "Unnamed";
}

FSSExporter::~FSSExporter() {
}

bool
FSSExporter::do_export() {
  dir = QDir(path);
  if (!dir.exists()) {
    return false;
  }

  QSettings meta_file(path + "/meta.ini", QSettings::IniFormat);

  meta_file.setValue("scale", QString::number(scale));
  meta_file.setValue("name", name.c_str());

  for (size_t r = Data::AssetArtLandscape; r <= Data::AssetCursor; r++) {
    Data::Resource res = static_cast<Data::Resource>(r);
    switch (Data::get_resource_type(res)) {
      case Data::TypeSprite: {
        exportResourceSprite(res);
        break;
      }
      case Data::TypeAnimation: {
        exportResourceAnimation(res);
        break;
      }
      case Data::TypeSound: {
        exportResourceData(res, "wav");
        break;
      }
      case Data::TypeMusic: {
        exportResourceData(res, "mid");
        break;
      }
      default: {
        break;
      }
    }
    meta_file.setValue(QString("resources/") + Data::get_resource_name(res).c_str(),
                       Data::get_resource_name(res).c_str());
  }

  return true;
}

void
FSSExporter::exportResourceData(Data::Resource res, QString ext) {
  QString res_name = Data::get_resource_name(res).c_str();
  dir.mkdir(res_name);
  QString path = dir.path() + "/" + res_name;
  QSettings meta_file(path + "/meta.ini", QSettings::IniFormat);
  for (size_t i = 0; i < Data::get_resource_count(res); i++) {
    PBuffer data;
    if (res == Data::AssetSound) {
      data = source->get_sound(i);
      ext = "wav";
    } else if (res == Data::AssetMusic) {
      data = source->get_music(i);
      ext = "mid";
    }
    if (data != nullptr) {
      QString file_name;
      file_name.sprintf("%03zu", i);
      QString file_path = res_name + "/" + file_name + "." + ext;
      QFile file(dir.path() + "/" + file_path);
      if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        file.write(static_cast<char*>(data->get_data()),
                   static_cast<qint64>(data->get_size()));
        file.close();
      }
      meta_file.setValue(file_name + "/path", file_name + "." + ext);
    }
  }
}

void
FSSExporter::exportResourceSprite(Data::Resource res) {
  QString res_name = Data::get_resource_name(res).c_str();
  dir.mkdir(res_name);
  QString path = dir.path() + "/" + res_name;
  QSettings meta_file(path + "/meta.ini", QSettings::IniFormat);

  for (size_t i = 0; i < Data::get_resource_count(res); i++) {
    QString file_name;
    file_name.sprintf("%03zu", i);
    DataSource::MaskImage pair = source->get_sprite_parts(res, i);
    bool meta_saved = false;
    if (std::get<0>(pair)) {
      PSprite mask = std::get<0>(pair);
      QImage image(reinterpret_cast<uchar*>(mask->get_data()),
                   mask->get_width(),
                   mask->get_height(),
                   QImage::Format_ARGB32);
      if (scale != 1) {
        image = image.scaled(image.width() * scale, image.height() * scale);
      }
      image.save(path + "/" + file_name + "m.png", "png");
      meta_file.setValue(file_name + "/mask_path", file_name + "m.png");
      meta_file.setValue(file_name + "/delta_x", (int)(mask->get_delta_x() * scale));
      meta_file.setValue(file_name + "/delta_y", (int)(mask->get_delta_y() * scale));
      meta_file.setValue(file_name + "/offset_x", (int)(mask->get_offset_x() * scale));
      meta_file.setValue(file_name + "/offset_y", (int)(mask->get_offset_y() * scale));
      meta_saved = true;
    }

    if (std::get<1>(pair)) {
      PSprite front = std::get<1>(pair);
      QImage image(reinterpret_cast<uchar*>(front->get_data()), front->get_width(), front->get_height(), QImage::Format_ARGB32);
      if (scale != 1.) {
        image = image.scaled(image.width() * scale, image.height() * scale);
      }
      image.save(path + "/" + file_name + ".png", "png");
      meta_file.setValue(file_name + "/image_path", file_name + ".png");
      if (!meta_saved) {
        meta_file.setValue(file_name + "/delta_x", (int)(front->get_delta_x() * scale));
        meta_file.setValue(file_name + "/delta_y", (int)(front->get_delta_y() * scale));
        meta_file.setValue(file_name + "/offset_x", (int)(front->get_offset_x() * scale));
        meta_file.setValue(file_name + "/offset_y", (int)(front->get_offset_y() * scale));
      }
    }
  }
}

void
FSSExporter::exportResourceAnimation(Data::Resource res) {
  QString res_name = Data::get_resource_name(res).c_str();
  dir.mkdir(res_name);
  QString path = dir.path() + "/" + res_name;
  QSettings meta_file(path + "/meta.ini", QSettings::IniFormat);

  for (size_t i = 0; i < Data::get_resource_count(res); i++) {
    QString file_name;
    file_name.sprintf("%03zu", i);
    meta_file.setValue(file_name + "/path", file_name + ".ini");
    QSettings data_file(path + "/" + file_name + ".ini", QSettings::IniFormat);

    size_t count = source->get_animation_phase_count(i);
    data_file.setValue("count", (unsigned int)count);
    for (size_t j = 0; j < count; j++) {
      Animation animation = source->get_animation(i, j);
      QString phase_name;
      phase_name.sprintf("%03zu", j);
      data_file.setValue(phase_name + "/sprite", animation.sprite);
      data_file.setValue(phase_name + "/x", animation.x * (int)scale);
      data_file.setValue(phase_name + "/y", animation.y * (int)scale);
    }
  }
}
