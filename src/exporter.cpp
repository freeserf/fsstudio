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

FSSExporter::FSSExporter(DataSource *_source, QString _path, unsigned int _scale) {
  source = _source;
  path = _path;
  scale = _scale;
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

  for (size_t r = Data::AssetArtLandscape; r <= Data::AssetCursor; r++) {
    Data::Resource res = (Data::Resource)r;
    switch (Data::get_resource_type(res)) {
      case Data::TypeSprite: {
        exportResourceSprite(res);
        meta_file.setValue(QString("resources/") + Data::get_resource_name(res), Data::get_resource_name(res));
        break;
      }
      case Data::TypeAnimation: {
        break;
      }
      case Data::TypeSound: {
        exportResourceData(res, "wav");
        meta_file.setValue(QString("resources/") + Data::get_resource_name(res), Data::get_resource_name(res));
        break;
      }
      case Data::TypeMusic: {
        exportResourceData(res, "mid");
        meta_file.setValue(QString("resources/") + Data::get_resource_name(res), Data::get_resource_name(res));
        break;
      }
      default: {
        break;
      }
    }
  }

  return true;
}

void
FSSExporter::exportResourceData(Data::Resource res, QString ext) {
  QString res_name = Data::get_resource_name(res);
  dir.mkdir(res_name);
  for (size_t i = 0; i < Data::get_resource_count(res); i++) {
    QString file_name;
    file_name.sprintf("%03zu.", i);
    file_name += ext;
    size_t size = 0;
    void *data = source->get_sound(i, &size);
    if (data != nullptr) {
      QString file_path = res_name + "/" + file_name;
      QFile file(dir.path() + "/" + file_path);
      if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        file.write((const char*)data, size);
        file.close();
      }
    }
  }
}

void
FSSExporter::exportResourceSprite(Data::Resource res) {
  QString res_name = Data::get_resource_name(res);
  dir.mkdir(res_name);
  QString path = dir.path() + "/" + res_name;
  QSettings meta_file(path + "/meta.ini", QSettings::IniFormat);

  Sprite::Color c;
  c.red = color.red();
  c.green = color.green();
  c.blue = color.blue();
  c.alpha = color.alpha();

  for (size_t i = 0; i < Data::get_resource_count(res); i++) {
    QString file_name;
    file_name.sprintf("%03zu", i);
    Sprite *sprite = source->get_sprite(res, i, c);
    if ((sprite != nullptr) && (sprite->get_data() != nullptr)) {
      QImage image(reinterpret_cast<uchar*>(sprite->get_data()), sprite->get_width(), sprite->get_height(), QImage::Format_ARGB32);
      if (scale != 1.) {
        image = image.scaled(image.width() * scale, image.height() * scale);
      }
      image.save(path + "/" + file_name + ".png", "png");
      meta_file.setValue(file_name + "/path", file_name + ".png");
      meta_file.setValue(file_name + "/delta_x", (int)(sprite->get_delta_x() * scale));
      meta_file.setValue(file_name + "/delta_y", (int)(sprite->get_delta_y() * scale));
      meta_file.setValue(file_name + "/offset_x", (int)(sprite->get_offset_x() * scale));
      meta_file.setValue(file_name + "/offset_y", (int)(sprite->get_offset_y() * scale));
    }
  }
}
