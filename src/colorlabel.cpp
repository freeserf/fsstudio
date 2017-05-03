/*
 * colorlabel.h - FSSColorLabel implementation
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

#include <QColorDialog>

#include "src/colorlabel.h"

FSSColorLabel::FSSColorLabel(const QColor& _color, QWidget* parent)
  : QLabel("       ", parent) {
  setColor(_color);
}

FSSColorLabel::~FSSColorLabel() {
}

void
FSSColorLabel::setColor(const QColor _color) {
  cur_color = _color;
  QPalette pal = palette();
  pal.setColor(backgroundRole(), cur_color);
  setPalette(pal);
  setAutoFillBackground(true);
  update();
}

QColor
FSSColorLabel::color() const {
  return cur_color;
}

void
FSSColorLabel::mousePressEvent(QMouseEvent*) {
  QColorDialog dialog(this);
  dialog.setCurrentColor(cur_color);
  if (dialog.exec() == QDialog::Accepted) {
    setColor(dialog.currentColor());
  }
}
