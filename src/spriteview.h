/*
 * spriteview.h - FSSSpriteView declaration
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

#ifndef SRC_SPRITEVIEW_H_
#define SRC_SPRITEVIEW_H_

#include <QScrollArea>
#include <QLabel>
#include <QTimer>

#include "src/data-source.h"

class FSSClickableLabel : public QLabel {
  Q_OBJECT

 protected:
  bool click;
  QTimer click_timer;

 public:
  explicit FSSClickableLabel(QWidget *pParent = NULL);
  virtual ~FSSClickableLabel();

 protected:
  virtual void mouseReleaseEvent(QMouseEvent *event);

 signals:
  void clicked();
  void doubleClicked();

 public slots:
  void on_drop_click();
};

class FSSSpriteView : public QScrollArea {
  Q_OBJECT

 protected:
  PSprite sprite;
  FSSClickableLabel *labelImage;

 public:
  explicit FSSSpriteView(QWidget *pParent = NULL);
  virtual ~FSSSpriteView();

  void setSprite(PSprite sprite);

 protected:
  QImage getImage();
  QPixmap getPixmap();

 public slots:
  void copy();
  void save();
};

#endif  // SRC_SPRITEVIEW_H_
