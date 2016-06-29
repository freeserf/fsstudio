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

#include <QMediaPlayer>
#include <QTemporaryFile>
#include <QDir>
#include <QSlider>
#include <QHBoxLayout>
#include <QPushButton>

FSSAudioView::FSSAudioView(QWidget *pParent) : QWidget(pParent) {
  file = NULL;
  player = new QMediaPlayer(this);

  QHBoxLayout *main_layout = new QHBoxLayout(this);
  setLayout(main_layout);

  buttonPlay = new QPushButton(this);
  buttonPlay->setText("Play");
  buttonPlay->setEnabled(false);
  main_layout->addWidget(buttonPlay);
  connect(buttonPlay, &QPushButton::clicked,
          this, &FSSAudioView::on_play);

  slider = new QSlider(this);
  slider->setOrientation(Qt::Horizontal);
  slider->setEnabled(false);
  main_layout->addWidget(slider);

  connect(player, &QMediaPlayer::mediaStatusChanged,
          this, &FSSAudioView::on_media_status_changed);
  connect(player, &QMediaPlayer::durationChanged,
          this, &FSSAudioView::on_duration_changed);
  connect(player, &QMediaPlayer::positionChanged,
          this, &FSSAudioView::on_position_changed);
  connect(player, &QMediaPlayer::stateChanged,
          this, &FSSAudioView::on_state_changed);
}

FSSAudioView::~FSSAudioView() {
  player->disconnect();
  player->stop();
  player->setMedia(QMediaContent());

  if (file != NULL) {
    delete file;
    file = NULL;
  }
}

void
FSSAudioView::setAudioData(void *data, size_t size, const QString &format) {
  player->stop();
  player->setMedia(QMediaContent());
  if (file != NULL) {
    delete file;
    file = NULL;
  }
  buttonPlay->setEnabled(false);
  slider->setEnabled(false);

  if (data == NULL) {
    return;
  }

  file = new QTemporaryFile(QDir::tempPath() + "/XXXXXX." + format);
  if (!file->open()) {
    qDebug() << "Failed to open temporary file";
    return;
  }
  if (file->write((char*)data, size) != (qint64)size) {
    qDebug() << "Failed to write to temporary file";
    return;
  }
  file->close();

  QUrl url = QUrl::fromLocalFile(file->fileName());
  qDebug() << "Sound writed to temporary file '" << url.toDisplayString() << "'";

  player->setMedia(url);
  player->setVolume(50);
  player->setNotifyInterval(200);
  qDebug() << player->mediaStatus();
}

void
FSSAudioView::on_media_status_changed(QMediaPlayer::MediaStatus status) {
  qDebug() << player->mediaStatus();

  switch (status) {
    case QMediaPlayer::UnknownMediaStatus:
      break;
    case QMediaPlayer::NoMedia:
      break;
    case QMediaPlayer::LoadingMedia:
      break;
    case QMediaPlayer::LoadedMedia:
      buttonPlay->setText("Play");
      buttonPlay->setEnabled(true);
      slider->setEnabled(true);
      break;
    case QMediaPlayer::StalledMedia:
      break;
    case QMediaPlayer::BufferingMedia:
      break;
    case QMediaPlayer::BufferedMedia:
      break;
    case QMediaPlayer::EndOfMedia:
      player->setPosition(0);
      break;
    case QMediaPlayer::InvalidMedia:
      qDebug() << player->error();
      break;
  }
}

void
FSSAudioView::on_duration_changed(qint64 duration) {
  slider->setMinimum(0);
  slider->setMaximum(duration);
}

void
FSSAudioView::on_position_changed(qint64 position) {
  slider->setValue(position);
}

void
FSSAudioView::on_state_changed(QMediaPlayer::State state) {
  switch(state) {
    case QMediaPlayer::StoppedState:
    case QMediaPlayer::PausedState:
      buttonPlay->setText("Play");
      buttonPlay->setEnabled(true);
      slider->setEnabled(true);
      break;
    case QMediaPlayer::PlayingState:
      buttonPlay->setText("Stop");
      buttonPlay->setEnabled(true);
      slider->setEnabled(true);
      break;
  }
}

void
FSSAudioView::on_play() {
  switch(player->state()) {
    case QMediaPlayer::StoppedState:
    case QMediaPlayer::PausedState:
      player->play();
      break;
    case QMediaPlayer::PlayingState:
      player->stop();
      break;
  }
}
