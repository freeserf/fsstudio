/*
 * resourcemodel.cpp - FSSResourceModel implementation
 *
 * Copyright (C) 2016  Wicked_Digger <wicked_digger@mail.ru>
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

#include "src/resourcemodel.h"

#include "src/data.h"

FSSResourceModel::FSSResourceModel(QObject *parent)
  : QAbstractItemModel(parent) {
}

QVariant
FSSResourceModel::headerData(int section,
                            Qt::Orientation /*orientation*/,
                            int role) const {
  QVariant result = QVariant();

  switch (role) {
    case Qt::DisplayRole: {
      switch (section) {
        case 0: {
          result = "Resource";
          break;
        }
        case 1: {
          result = "Type";
          break;
        }
      }
      break;
    }
    default:
      return QVariant();
  }

  return result;
}

QModelIndex
FSSResourceModel::index(int row, int column,
                        const QModelIndex &parent) const {
  if (parent.isValid()) {
    return createIndex(row, column,
                       ((row+1) << 16) |
                       (parent.internalId() & 0xFF));
  } else {
    return createIndex(row, column, row+1);
  }
}

QModelIndex
FSSResourceModel::parent(const QModelIndex &index) const {
  int sub_row = (int)(index.internalId() >> 16);
  if (sub_row == 0) {
    return QModelIndex();
  }

  int row = (index.internalId() & 0xFF);

  return createIndex(row-1, 0, row);
}

int
FSSResourceModel::rowCount(const QModelIndex &parent) const {
  if (!parent.isValid()) {
    return static_cast<int>(Data::AssetCursor);
  }

  QModelIndex p = parent.parent();
  if (!p.isValid()) {
    return Data::get_resource_count((Data::Resource)(parent.row() + 1));
  }

  return 0;
}

int
FSSResourceModel::columnCount(const QModelIndex & /*parent*/) const {
  return 2;
}

QString
FSSResourceModel::getResName(int res) const {
  Data::Resource rclass = (Data::Resource)res;
  std::string name = Data::get_resource_name(rclass);
  return QString::fromLatin1(name.c_str());
}

QString
FSSResourceModel::getResTypeName(int res) const {
  const char *types[] = { "unknown", "sprite", "animation",
                          "sound", "music", "palette" };
  Data::Resource rclass = (Data::Resource)res;
  const char *name = types[Data::get_resource_type(rclass)];
  return QString::fromLatin1(name);
}

QVariant
FSSResourceModel::data(const QModelIndex &index, int role) const {
  QVariant result = QVariant();

  QModelIndex parent = index.parent();
  if (!parent.isValid()) {
    switch (role) {
      case Qt::DisplayRole: {
        switch (index.column()) {
          case 0: {
            result = getResName(index.row() + 1);
            break;
          }
          case 1: {
            result = getResTypeName(index.row() + 1);
            break;
          }
        }
        break;
      }
      default:
        return QVariant();
    }

    return result;
  }

  switch (role) {
    case Qt::DisplayRole: {
      switch (index.column()) {
        case 0: {
          result = QVariant(QString::number(index.row()));
          break;
        }
        case 1: {
          result = getResTypeName(index.parent().row() + 1);
          break;
        }
      }
      break;
    }
    default:
      return QVariant();
  }

  return result;
}
