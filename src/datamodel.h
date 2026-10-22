/*
 * datamodel.h - FSSDataModel declaration
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

#ifndef SRC_DATAMODEL_H_
#define SRC_DATAMODEL_H_

#include <string>
#include <vector>

#include "src/data.h"

// Data sources of all known types, created by set_source_path(). A source is
// kept even when its data could not be loaded, so its path can be fixed.
class FSSDataModel {
 public:
  enum DataSourceType {
    DataSourceTypeDos,
    DataSourceTypeAmiga,
    DataSourceTypeCustom,
    DataSourceTypeCount
  };

 protected:
  std::vector<Data::PSource> data_sources;
  // load() result: Data::Source::is_loaded() is not set by every source type
  std::vector<bool> loaded;

 public:
  FSSDataModel();

  size_t source_count() const { return data_sources.size(); }
  Data::PSource get_source(size_t index) const;
  bool is_loaded(size_t index) const {
    return index < loaded.size() && loaded[index];
  }
  static const char *get_type_name(DataSourceType type);
  static bool is_file_source(DataSourceType type) {
    return type == DataSourceTypeDos;
  }

  // Recreates the source with a new path; returns true if it was loaded.
  bool set_source_path(DataSourceType type, const std::string &path);

  static std::string default_path(DataSourceType type);
};

#endif  // SRC_DATAMODEL_H_
