/*
 * exporter.h - FSSExporter declaration
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

#ifndef SRC_EXPORTER_H_
#define SRC_EXPORTER_H_

#include <QObject>

class DataSource;

class FSSExporter : public QObject {
  Q_OBJECT

 protected:
  DataSource *source;
  QString path;

 public:
  FSSExporter(DataSource *source, QString path);
  virtual ~FSSExporter();

  bool do_export();
};

#endif  // SRC_EXPORTER_H_
