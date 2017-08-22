/*
 * datamodel.h - FSSDataModel declaration
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

#ifndef SRC_DATAMODEL_H_
#define SRC_DATAMODEL_H_

#include <QObject>

#include <memory>

class DataSource;
typedef std::shared_ptr<DataSource> PDataSource;

class FSSDataModel : public QObject {
  Q_OBJECT

 public:
  enum DataSourceType {
    DataSourceTypeDos,
    DataSourceTypeAmiga,
    DataSourceTypeCustom
  };

 protected:
  QList<PDataSource> data_sources;

 public:
  explicit FSSDataModel(QObject *parent = 0);
  virtual ~FSSDataModel();

  size_t dataSourceCount() const { return data_sources.count(); }
  size_t addDataSource(PDataSource source);
  size_t addDataSource(DataSourceType type, const QString &path);
  PDataSource getDataSource(size_t index) const;
  void setDataSource(PDataSource source, size_t index);
  static DataSourceType getType(PDataSource source);

 protected:
  void saveSetting(PDataSource source);

 signals:
  void sourceChanged(size_t index);
};

#endif  // SRC_DATAMODEL_H_
