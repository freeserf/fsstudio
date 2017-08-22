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
#include <QToolBar>

#include "src/data-source.h"
#include "src/spriteview.h"
#include "src/audioview.h"
#include "src/animationview.h"
#include "src/colorlabel.h"

FSSResourceView::FSSResourceView(PDataSource source,
                                 QWidget *parent)
  : QWidget(parent) {
  theSource = source;

  QVBoxLayout *layout = new QVBoxLayout(this);
  setLayout(layout);

  labelName = new QLabel(this);
  labelName->setText(source->get_name().c_str());
  layout->addWidget(labelName);

  resourcesStack = new QStackedLayout();
  layout->addLayout(resourcesStack);

  viewEmpty = new QWidget(this);
  viewEmpty->setMinimumSize(200, 200);
  resourcesStack->addWidget(viewEmpty);

  viewSprite = new FSSSpriteView(this);
  viewSprite->setMinimumSize(200, 200);
  resourcesStack->addWidget(viewSprite);

  viewAudio = new FSSAudioView(this);
  viewAudio->setMinimumSize(200, 200);
  resourcesStack->addWidget(viewAudio);

  viewAnimation = new FSSAnimationView(this);
  viewAnimation->setMinimumSize(200, 200);
  resourcesStack->addWidget(viewAnimation);

  QToolBar *toolBar = new QToolBar(this);
  toolBar->addAction(QIcon(":/icons/Actions-document-save-as-icon.png"), "Save", this, &FSSResourceView::on_save);
  toolBar->addAction(QIcon(":/icons/Actions-edit-paste-icon.png"), "Copy", this, &FSSResourceView::on_copy);
  field_color = new FSSColorLabel(QColor(0x00, 0x00, 0xFF), this);
  field_color->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  toolBar->addWidget(field_color);
  layout->addWidget(toolBar);

  textBrowser = new QTextBrowser(this);
  textBrowser->setMinimumSize(200, 200);
  layout->addWidget(textBrowser);
}

FSSResourceView::~FSSResourceView() {
}

QString
spriteInfo(PSprite sprite) {
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
FSSResourceView::selectResource(Data::Resource resource_class,
                                unsigned int index) {
  QWidget *resView = viewEmpty;
  QString info;

  if (resource_class != Data::AssetNone) {
    switch (Data::get_resource_type(resource_class)) {
      case Data::TypeSprite: {
        QColor c = field_color->color();
        Sprite::Color color;
        color.red = c.red();
        color.green = c.green();
        color.blue = c.blue();
        color.alpha = 0xFF;
        PSprite sprite = theSource->get_sprite(resource_class, index, color);
        viewSprite->setSprite(sprite);
        if (sprite) {
          info = spriteInfo(sprite);
        }
        resView = viewSprite;
        break;
      }
      case Data::TypeAnimation: {
//        Animation animation = source->get_animation(index);
//        viewAnimation->setAnimation(&animation);
//        info.sprintf("size = %lu", animation.get_size());
//        resView = viewAnimation;
        break;
      }
      case Data::TypeSound: {
        PBuffer data = theSource->get_sound(index);
        viewAudio->setAudioData(data->get_data(), data->get_size(), "wav");
        resView = viewAudio;
        break;
      }
      case Data::TypeMusic: {
        PBuffer data = theSource->get_music(index);
        viewAudio->setAudioData(data->get_data(), data->get_size(), "mid");
        resView = viewAudio;
        break;
      }
      case Data::TypeUnknown: {
        break;
      }
    }
  }

  resourcesStack->setCurrentWidget(resView);
  textBrowser->setText(info);
}

void
FSSResourceView::on_save() {
  QWidget *current_view = resourcesStack->currentWidget();
  QMetaObject::invokeMethod(current_view, "save");
}

void
FSSResourceView::on_copy() {
  QWidget *current_view = resourcesStack->currentWidget();
  QMetaObject::invokeMethod(current_view, "copy");
}
