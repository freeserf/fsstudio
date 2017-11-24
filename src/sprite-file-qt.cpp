/*
 * sprite-file-qt.cpp - Sprite loaded from file implementation
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

#include "src/sprite-file.h"

#include <QImage>

SpriteFile::SpriteFile() {
}

SpriteFile::~SpriteFile() {
}

bool
SpriteFile::load(const std::string &path) {
  QImage image;
  if (!image.load(path.c_str())) {
    return false;
  }
  image = image.convertToFormat(QImage::Format_ARGB32);
  width = image.width();
  height = image.height();
  size_t size = width * height * 4;
  data = new uint8_t[size];
  memcpy(data, image.bits(), size);
  return true;
}
