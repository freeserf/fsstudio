/*
 * colorlabel.h - FSSColorLabel declaration
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

#ifndef SRC_COLORLABEL_H_
#define SRC_COLORLABEL_H_

#include <QLabel>

class FSSColorLabel : public QLabel {
  Q_OBJECT

 protected:
  QColor cur_color;

 public:
  explicit FSSColorLabel(const QColor& color, QWidget* parent=0);
  virtual ~FSSColorLabel();

  void setColor(const QColor color);
  QColor color() const;

 signals:
  void clicked();

 protected:
  void mousePressEvent(QMouseEvent* event);
};

#endif  // SRC_COLORLABEL_H_
