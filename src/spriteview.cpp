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

#include <QTimer>
#include <QApplication>
#include <QMouseEvent>
#include <QClipboard>
#include <QFileDialog>
#include <QPainter>

// FSSClickableLabel

FSSClickableLabel::FSSClickableLabel(QWidget *pParent) : QLabel(pParent) {
  click = false;
  click_timer.setInterval(QApplication::doubleClickInterval());
  connect(&click_timer, &QTimer::timeout,
          this, &FSSClickableLabel::on_drop_click);
}

FSSClickableLabel::~FSSClickableLabel() {
}

void
FSSClickableLabel::mouseReleaseEvent(QMouseEvent *event) {
  if (event->button() != Qt::LeftButton) {
    return;
  }

  if (click) {
    click_timer.stop();
    emit doubleClicked();
    click = false;
  } else {
    emit clicked();
    click_timer.start();
    click = true;
  }
}

void
FSSClickableLabel::on_drop_click() {
  click_timer.stop();
  click = false;
}

// FSSSpriteView

FSSSpriteView::FSSSpriteView(QWidget *pParent) : QScrollArea(pParent) {
  sprite = NULL;
  labelImage = new FSSClickableLabel(this);
  setWidget(labelImage);
  connect(labelImage, &FSSClickableLabel::doubleClicked,
          this, &FSSSpriteView::copy);
}

FSSSpriteView::~FSSSpriteView() {
}

void
FSSSpriteView::setSprite(PSprite _sprite) {
  sprite = _sprite;

  labelImage->setBackgroundRole(QPalette::LinkVisited);
  labelImage->setSizePolicy(QSizePolicy::Ignored,
                            QSizePolicy::Ignored);
  labelImage->setScaledContents(false);
  labelImage->setSizePolicy(QSizePolicy::MinimumExpanding,
                            QSizePolicy::MinimumExpanding);
  QPixmap pixmap = getPixmap();
  if (pixmap.isNull()) {
    labelImage->resize(QSize());
  } else {
    QSize size = pixmap.size();
    size *= 2;
    pixmap = pixmap.scaled(size);
    labelImage->resize(size);
  }
  labelImage->setPixmap(pixmap);

  update();
}

QImage
FSSSpriteView::getImage() {
  if (sprite == NULL) {
    return QImage();
  }

  QImage image(reinterpret_cast<uchar*>(sprite->get_data()),
               sprite->get_width(), sprite->get_height(),
               QImage::Format_ARGB32);

  return image;
}

QPixmap
FSSSpriteView::getPixmap() {
  if (!sprite) {
    return QPixmap();
  }

  QImage image = getImage();
  QPixmap pixmap = QPixmap::fromImage(image);
/*
  QPainter p(&pixmap);
  p.setPen(Qt::black);
  p.drawLine(-sprite->get_offset_x() - 1, -sprite->get_offset_y(),
             -sprite->get_offset_x() + 1, -sprite->get_offset_y());
  p.drawLine(-sprite->get_offset_x(), -sprite->get_offset_y() - 1,
             -sprite->get_offset_x(), -sprite->get_offset_y() + 1);
  p.end();
*/
  return pixmap;
}

void
FSSSpriteView::copy() {
  if (sprite == NULL) {
    return;
  }

  QImage image = getImage();

  QClipboard *clipboard = QApplication::clipboard();
  clipboard->setImage(image);
}

void
FSSSpriteView::save() {
  if (sprite == NULL) {
    return;
  }

  QString path = QFileDialog::getSaveFileName(this, "Save sprite", QString(), "Image File (*.png)");
  if (path.isEmpty()) {
    return;
  }

  QImage image = getImage();
  image.save(path, "png");
}
