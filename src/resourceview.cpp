/*
 * resourceview.cpp - FVResourceView implementation
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

#include "src/resourceview.h"

#include <QVBoxLayout>
#include <QStackedLayout>
#include <QTextBrowser>
#include <QScrollArea>
#include <QLabel>

#include "src/data-source.h"

FSSResourceView::FSSResourceView(data_source_t *source,
                                 QWidget *parent)
  : QWidget(parent) {
  this->source = source;

  QVBoxLayout *layout = new QVBoxLayout(this);
  setLayout(layout);

  scrollArea = new QScrollArea(this);
  scrollArea->setMinimumSize(200, 200);
  layout->addWidget(scrollArea);

  textBrowser = new QTextBrowser(this);
  textBrowser->setMinimumSize(200, 200);
  layout->addWidget(textBrowser);
}

FSSResourceView::~FSSResourceView() {
}

QString
spriteInfo(sprite_t *sprite) {
  QString str("Sprite:\n");
  if (NULL != sprite) {
    str += "width = ";
    str += QString::number(sprite->get_width()) + "\n";
    str += "height = ";
    str += QString::number(sprite->get_height()) + "\n";
    str += "delta_x = ";
    str += QString::number(sprite->get_delta_x()) + "\n";
    str += "delta_y = ";
    str += QString::number(sprite->get_delta_y()) + "\n";
    str += "offset_x = ";
    str += QString::number(sprite->get_offset_x()) + "\n";
    str += "offset_y = ";
    str += QString::number(sprite->get_offset_y());
  }
  return str;
}

void
FSSResourceView::onResourceSelected(data_res_class_t resource_class,
                                    unsigned int index) {
  QWidget *resView = NULL;

  if (resource_class != data_res_none) {
    switch (data_t::get_resource_type(resource_class)) {
      case data_type_sprite: {
        sprite_t *sprite = source->get_sprite(resource_class, index, 0);
        if (sprite != NULL) {
          QLabel *spriteView = new QLabel(scrollArea);
          spriteView->setBackgroundRole(QPalette::LinkVisited);
          spriteView->setSizePolicy(QSizePolicy::Ignored,
                                    QSizePolicy::Ignored);
          spriteView->setScaledContents(false);
          spriteView->setSizePolicy(QSizePolicy::MinimumExpanding,
                                    QSizePolicy::MinimumExpanding);
          QPixmap pixmap = pixmapFromSprite(sprite);
          QSize size = pixmap.size();
          size *= 2;
          pixmap = pixmap.scaled(size);
          spriteView->resize(size);
          spriteView->setPixmap(pixmap);
          textBrowser->setText(spriteInfo(sprite));
          resView = spriteView;
        }
        break;
      }
      case data_type_animation: {
        break;
      }
      case data_type_sound: {
        break;
      }
      case data_type_music: {
        break;
      }
      case data_type_palette: {
        palette_t *palette = source->get_palette(index);
        if (palette != NULL) {
          FSSPaletteView *paletteView = new FSSPaletteView(scrollArea);
          paletteView->setPalette(palette);
          QString info = QString::asprintf("Palette:\n%zu colors",
                                           palette->get_size());
          textBrowser->setText(info);
          resView = paletteView;
        }
        break;
      }
      case data_type_unknown: {
        scrollArea->setWidget(NULL);
        break;
      }
    }
  }

  if (resView == NULL) {
    resView = new QWidget(this);
    textBrowser->setText(QString());
  }
  scrollArea->setWidget(resView);
}

QPixmap
FSSResourceView::pixmapFromSprite(sprite_t *sprite) {
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
