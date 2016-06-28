/*
 * spriteview.cpp - FSSSpriteView implementation
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

#include "src/spriteview.h"

#include <QLabel>

FSSSpriteView::FSSSpriteView(QWidget *pParent) : QScrollArea(pParent) {
  sprite = NULL;
  labelImage = new QLabel(this);
  setWidget(labelImage);
}

void
FSSSpriteView::setSprite(sprite_t *_sprite) {
  if (sprite != NULL) {
    delete sprite;
    sprite = NULL;
  }

  sprite = _sprite;

  labelImage->setBackgroundRole(QPalette::LinkVisited);
  labelImage->setSizePolicy(QSizePolicy::Ignored,
                            QSizePolicy::Ignored);
  labelImage->setScaledContents(false);
  labelImage->setSizePolicy(QSizePolicy::MinimumExpanding,
                            QSizePolicy::MinimumExpanding);
  QPixmap pixmap = getPixmap();
  QSize size = pixmap.size();
  size *= 2;
  pixmap = pixmap.scaled(size);
  labelImage->resize(size);
  labelImage->setPixmap(pixmap);

  update();
}

QPixmap
FSSSpriteView::getPixmap() {
  if (sprite == NULL) {
    return QPixmap();
  }

  QImage *image = new QImage(reinterpret_cast<uchar*>(sprite->get_data()),
                             sprite->get_width(),
                             sprite->get_height(),
                             QImage::Format_ARGB32);

  QPixmap pixmap = QPixmap::fromImage(*image);

  delete image;

  return pixmap;
}
