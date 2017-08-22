/*
 * datamodel.cpp - FSSDataModel implementation
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

#include "src/datamodel.h"

#include <QSettings>
#include <QStandardPaths>

#include "src/data-source-dos.h"
#include "src/data-source-amiga.h"
#include "src/data-source-custom.h"

FSSDataModel::FSSDataModel(QObject *parent)
  : QObject(parent) {
  QString base_path = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
  base_path += "/.local/share/freeserf";

  QSettings settings;
  settings.sync();

  QString path = settings.value("sources/dos", base_path + "/spae.pa").toString();
  addDataSource(DataSourceTypeDos, path);

  path = settings.value("sources/amiga", base_path).toString();
  addDataSource(DataSourceTypeAmiga, path);

  path = settings.value("sources/custom", base_path + "/custom").toString();
  addDataSource(DataSourceTypeCustom, path);
}

FSSDataModel::~FSSDataModel() {
  for (PDataSource dataSource : data_sources) {
    saveSetting(dataSource);
  }
}

size_t
FSSDataModel::addDataSource(PDataSource source) {
  size_t index = data_sources.count();
  data_sources.push_back(source);
  saveSetting(source);
  emit sourceChanged(index);
  return index;
}

size_t
FSSDataModel::addDataSource(DataSourceType type, const QString &path) {
  PDataSource source;

  switch (type) {
    case DataSourceTypeDos:
      source = std::make_shared<DataSourceDOS>(path.toLocal8Bit().data());
      break;
    case DataSourceTypeAmiga:
      source = std::make_shared<DataSourceAmiga>(path.toLocal8Bit().data());
      break;
    case DataSourceTypeCustom:
      source = std::make_shared<DataSourceCustom>(path.toLocal8Bit().data());
      break;
  }

  if (source && source->load()) {
    saveSetting(source);
    return addDataSource(source);
  }

  return std::numeric_limits<size_t>::max();
}

PDataSource
FSSDataModel::getDataSource(size_t index) const {
  if (index >= (size_t)data_sources.count()) {
    return nullptr;
  }
  return data_sources[index];
}

void
FSSDataModel::setDataSource(PDataSource source, size_t index) {
  if (index >= (size_t)data_sources.count()) {
    return;
  }
  data_sources[index] = source;
  saveSetting(source);
  emit sourceChanged(index);
}

FSSDataModel::DataSourceType
FSSDataModel::getType(PDataSource source) {
  std::string name = source->get_name();

  if (name == "DOS") {
    return DataSourceTypeDos;
  } else if (name == "Amiga") {
    return DataSourceTypeAmiga;
  }

  return DataSourceTypeCustom;
}

void
FSSDataModel::saveSetting(PDataSource source) {
  QSettings settings;
  switch (getType(source)) {
    case DataSourceTypeDos:
      settings.setValue("sources/dos", source->get_path().c_str());
      break;
    case DataSourceTypeAmiga:
      settings.setValue("sources/amiga", source->get_path().c_str());
      break;
    case DataSourceTypeCustom:
      settings.setValue("sources/custom", source->get_path().c_str());
      break;
  }
  settings.sync();
}
