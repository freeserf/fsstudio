/*
 * resourceview.cpp - FSSResourceView implementation
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
#include "src/spriteview.h"
#include "src/paletteview.h"
#include "src/audioview.h"

FSSResourceView::FSSResourceView(data_source_t *source,
                                 QWidget *parent)
  : QWidget(parent) {
  this->source = source;

  QVBoxLayout *layout = new QVBoxLayout(this);
  setLayout(layout);

  resourcesStack = new QStackedLayout(this);
  layout->addLayout(resourcesStack);

  viewEmpty = new QWidget(this);
  viewEmpty->setMinimumSize(200, 200);
  resourcesStack->addWidget(viewEmpty);

  viewSprite = new FSSSpriteView(this);
  viewSprite->setMinimumSize(200, 200);
  resourcesStack->addWidget(viewSprite);

  viewPalette = new FSSPaletteView(this);
  viewPalette->setMinimumSize(200, 200);
  resourcesStack->addWidget(viewPalette);

  viewAudio = new FSSAudioView(this);
  viewAudio->setMinimumSize(200, 200);
  resourcesStack->addWidget(viewAudio);

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
        viewSprite->setSprite(sprite);
        resView = viewSprite;
        break;
      }
      case data_type_animation: {
        resView = viewEmpty;
        break;
      }
      case data_type_sound: {
        size_t size = 0;
        void *data = source->get_sound(index, &size);
        viewAudio->setAudioData(data, size, "wav");
        resView = viewAudio;
        break;
      }
      case data_type_music: {
        size_t size = 0;
        void *data = source->get_music(index, &size);
        viewAudio->setAudioData(data, size, "mid");
        resView = viewAudio;
        break;
      }
      case data_type_palette: {
        palette_t *palette = source->get_palette(index);
        viewPalette->setPalette(palette);
        QString info = QString::asprintf("Palette:\n%zu colors",
                                         palette->get_size());
        textBrowser->setText(info);
        resView = viewPalette;
        break;
      }
      case data_type_unknown: {
        resView = viewEmpty;
        break;
      }
    }
  }

  resourcesStack->setCurrentWidget(resView);
}
