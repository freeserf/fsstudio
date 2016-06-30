/*
 * paletteview.h - FSSPaletteView declaration
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

#ifndef SRC_PALETTEVIEW_H_
#define SRC_PALETTEVIEW_H_

#include <QTableView>

#include "src/data-source.h"

class palette_t;

class FSSPaletteModel : public QAbstractItemModel {
  Q_OBJECT

 protected:
  palette_t *palette;

 public:
  explicit FSSPaletteModel(QObject *parent = 0);

  // Header:
  QVariant headerData(int section, Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const override;

  // Basic functionality:
  QModelIndex index(int row, int column,
                    const QModelIndex &parent = QModelIndex()) const override;
  QModelIndex parent(const QModelIndex &) const override;

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;

  QVariant data(const QModelIndex &index,
                int role = Qt::DisplayRole) const override;

  void setPalette(palette_t *_palette);
};

class FSSPaletteView : public QTableView {
  Q_OBJECT

 protected:
  FSSPaletteModel *model;
  palette_t *palette;

 public:
  explicit FSSPaletteView(QWidget *pParent = NULL);
  virtual ~FSSPaletteView();

  void setPalette(palette_t *palette);

 public slots:
  void on_copy_color(const QModelIndex &index);
  void copy();
};

#endif  // SRC_PALETTEVIEW_H_
