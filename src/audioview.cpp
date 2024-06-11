/*
 * audioview.cpp - FSSAudioView implementation
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

#include "src/audioview.h"

//#include <QMediaPlayer>
#include <QTemporaryFile>
#include <QDir>
#include <QSlider>
#include <QHBoxLayout>
#include <QPushButton>

#include "src/buffer.h"

FSSAudioView::FSSAudioView(QWidget *pParent) : QWidget(pParent) {
  file = nullptr;

  QHBoxLayout *main_layout = new QHBoxLayout(this);
  setLayout(main_layout);

  buttonPlay = new QPushButton(this);
  buttonPlay->setText("Play");
  buttonPlay->setEnabled(false);
  main_layout->addWidget(buttonPlay);

  slider = new QSlider(this);
  slider->setOrientation(Qt::Horizontal);
  slider->setEnabled(false);
  main_layout->addWidget(slider);
}

FSSAudioView::~FSSAudioView() {
  if (file != nullptr) {
    delete file;
    file = nullptr;
  }
}

void
FSSAudioView::setAudioData(PBuffer data, const QString &format) {
  if (file != nullptr) {
    delete file;
    file = nullptr;
  }
  buttonPlay->setEnabled(false);
  slider->setEnabled(false);

  if (!data) {
    return;
  }

  file = new QTemporaryFile(QDir::tempPath() + "/XXXXXX." + format);
  if (!file->open()) {
    qDebug() << "Failed to open temporary file";
    return;
  }
  qint64 size = data->get_size();
  if (file->write((const char*)data->get_data(), size) != size) {
    qDebug() << "Failed to write to temporary file";
    return;
  }
  file->close();
}

void
FSSAudioView::on_duration_changed(qint64 duration) {
  slider->setMinimum(0);
  slider->setMaximum((int)duration);
}

void
FSSAudioView::on_position_changed(qint64 position) {
  slider->setValue((int)position);
}
