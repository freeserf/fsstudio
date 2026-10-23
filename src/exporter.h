/*
 * exporter.h - FSSExporter declaration
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

#ifndef SRC_EXPORTER_H_
#define SRC_EXPORTER_H_

#include <string>

#include "src/data.h"

// Exports a data source to the Freeserf custom data format.
class FSSExporter {
 protected:
  Data::PSource source;
  std::string path;
  unsigned int scale;
  std::string name;

 public:
  FSSExporter(Data::PSource source, const std::string &path,
              unsigned int scale = 1);

  void set_name(const std::string &_name) { name = _name; }
  bool do_export();

 protected:
  void export_resource_data(Data::Resource res);
  void export_resource_sprite(Data::Resource res);
  void export_resource_animation(Data::Resource res);
  std::string resource_dir(Data::Resource res) const;
};

#endif  // SRC_EXPORTER_H_
