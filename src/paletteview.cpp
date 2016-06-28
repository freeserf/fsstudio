/*
 * paletteview.cpp - FVPaletteView implementation
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

#include "src/paletteview.h"

#include <QFontDatabase>
#include <QClipboard>
#include <QApplication>

FSSPaletteModel::FSSPaletteModel(QObject *parent) : QAbstractItemModel(parent) {
  palette = NULL;
}

QVariant
FSSPaletteModel::headerData(int section, Qt::Orientation orientation, int role) const {
  QVariant result = QVariant();

  switch (role) {
    case Qt::DisplayRole: {
      if (orientation == Qt::Horizontal) {
        if (section < 4) {
          QString header[4] = {"R", "G", "B", "   COLOR   "};
          result = QVariant(header[section]);
        }
      } else {
        QString str = QString::number(section);
        result = QVariant(str);
      }
      break;
    }
    default: break;
  }

  return result;
}

// Basic functionality:
QModelIndex
FSSPaletteModel::index(int row, int column, const QModelIndex &parent) const {
  if (!parent.isValid()) {
    return createIndex(row, column);
  }
  return QModelIndex();
}

QModelIndex
FSSPaletteModel::parent(const QModelIndex &) const {
  return QModelIndex();
}

int
FSSPaletteModel::rowCount(const QModelIndex &parent) const {
  if (!parent.isValid() && palette != NULL) {
    return palette->get_size();
  }
  return 0;
}

int
FSSPaletteModel::columnCount(const QModelIndex &parent) const {
  if (!parent.isValid()) {
    return 4;
  }
  return 0;
}

QVariant
FSSPaletteModel::data(const QModelIndex &index, int role) const {
  QVariant data;
  switch (role) {
    case Qt::DisplayRole:
      if (index.column() < 3) {
        color_t color = palette->get_color(index.row());
        QString text;
        switch (index.column()) {
          case 0:
            text.sprintf("0x%02x", color.red);
            break;
          case 1:
            text.sprintf("0x%02x", color.green);
            break;
          case 2:
            text.sprintf("0x%02x", color.blue);
            break;
        }
        data = text;
      }
      break;
    case Qt::BackgroundColorRole:
      if (index.column() == 3) {
        color_t color = palette->get_color(index.row());
        data = QVariant(QColor(color.red, color.green, color.blue));
      }
      break;
  }

  return data;
}

void
FSSPaletteModel::setPalette(palette_t *_palette) {
  beginResetModel();
  palette = _palette;
  endResetModel();
}

FSSPaletteView::FSSPaletteView(QWidget *pParent) : QTableView(pParent) {
  palette = NULL;

  const QFont font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
  setFont(font);

  model = new FSSPaletteModel(this);
  setModel(model);

  setSelectionBehavior(QAbstractItemView::SelectRows);

  connect(this, &QTableView::doubleClicked,
          this, &FSSPaletteView::on_copy_color);
}

void
FSSPaletteView::setPalette(palette_t *_palette) {
  if (palette != NULL) {
    delete palette;
    palette = NULL;
  }

  palette = _palette;

  model->setPalette(palette);
  resizeColumnsToContents();
  update();
}

void
FSSPaletteView::on_copy_color(const QModelIndex &index) {
  if (palette != NULL) {
    color_t color = palette->get_color(index.row());
    QString text;
    text.sprintf("0x%02x, 0x%02x, 0x%02x", color.red, color.green, color.blue);
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(text);
  }
}

/*
void
FSSPaletteView::paintEvent(QPaintEvent *event) {
  if (palette == NULL) {
    return;
  }

  QPainter painter(this);

  painter.setBrush(QBrush(QColor(0, 0, 0, 0)));
  const QFont font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
  QFontMetrics metrics(font);
  painter.setFont(font);

  QRect tr = metrics.boundingRect(QString("000 0x00 0x00 0x00"));
  int cx = tr.width() + 10;

  for (size_t i = 0; i < palette->get_size(); i++) {
    QRect row_rect(0, i * row_height, DEF_WIDTH, row_height);
    if (!event->region().intersected(row_rect).isEmpty()) {
      color_t c = palette->get_color(i);
      QString text = QString::asprintf("%03zu 0x%02x 0x%02x 0x%02x", i,
                                       c.red, c.green, c.blue);
      painter.drawText(row_rect, text);
      QColor color(c.red, c.green, c.blue);
      row_rect.setLeft(row_rect.left() + cx);
      row_rect.adjust(1, 1, -1, -1);
      painter.fillRect(row_rect, QBrush(color));
    }
  }
}
*/
