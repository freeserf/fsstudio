/*
 * datamodel.cpp - FSSDataModel implementation
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

#include "src/datamodel.h"

#include <SDL3/SDL.h>

#include <memory>

#include "src/data-source-dos.h"
#include "src/data-source-amiga.h"
#include "src/data-source-custom.h"

FSSDataModel::FSSDataModel()
  : data_sources(DataSourceTypeCount)
  , loaded(DataSourceTypeCount, false) {
}

Data::PSource
FSSDataModel::get_source(size_t index) const {
  if (index >= data_sources.size()) {
    return nullptr;
  }
  return data_sources[index];
}

const char *
FSSDataModel::get_type_name(DataSourceType type) {
  switch (type) {
    case DataSourceTypeDos: return "DOS";
    case DataSourceTypeAmiga: return "Amiga";
    case DataSourceTypeCustom: return "Custom";
    default: return "";
  }
}

bool
FSSDataModel::set_source_path(DataSourceType type, const std::string &path) {
  Data::PSource source;
  switch (type) {
    case DataSourceTypeDos:
      source = std::make_shared<DataSourceDOS>(path);
      break;
    case DataSourceTypeAmiga:
      source = std::make_shared<DataSourceAmiga>(path);
      break;
    case DataSourceTypeCustom:
      source = std::make_shared<DataSourceCustom>(path);
      break;
    default:
      return false;
  }

  data_sources[type] = source;
  loaded[type] = source->load();
  return loaded[type];
}

std::string
FSSDataModel::default_path(DataSourceType type) {
  // the home folder path ends with a separator
  const char *home = SDL_GetUserFolder(SDL_FOLDER_HOME);
  std::string base_path = home != nullptr ? home : "";
  base_path += ".local/share/freeserf";

  switch (type) {
    case DataSourceTypeDos: return base_path + "/spae.pa";
    case DataSourceTypeCustom: return base_path + "/custom";
    default: return base_path;
  }
}
