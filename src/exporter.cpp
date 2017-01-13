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

#include <QDir>
#include <QDomDocument>

#include "src/exporter.h"

FSSExporter::FSSExporter(DataSource *_source, QString _path) {
  source = _source;
  path = _path;
}

FSSExporter::~FSSExporter() {
}

bool
FSSExporter::do_export() {
  QDir dir(path);
  if (!dir.exists("freeserf_data")) {
    if (!dir.mkdir("freeserf_data")) {
      return false;
    }
  }
  if (!dir.cd("freeserf_data")) {
    return false;
  }

  QDomDocument catalog;
  QDomElement data_source_element = catalog.createElement("data_source");
  data_source_element.setAttribute("name", "New Data Source");

  return false;
}
