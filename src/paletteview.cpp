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

#include <QPainter>
#include <QPaintEvent>
#include <QRegion>
#include <QFontDatabase>

#define DEF_WIDTH (200)

FSSPaletteView::FSSPaletteView(QWidget *pParent) : QWidget(pParent) {
  palette = NULL;
  row_height = 15;
}

void
FSSPaletteView::setPalette(palette_t *palette) {
  if (this->palette != NULL) {
    delete this->palette;
    this->palette = NULL;
  }

  this->palette = palette;

  update();
}

QSize
FSSPaletteView::sizeHint() const {
  if (palette == NULL) {
    return QSize();
  }

  return QSize(DEF_WIDTH, palette->get_size() * row_height);
}

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
