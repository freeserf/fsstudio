/*
 * audioview.h - FSSAudioView declaration
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

#ifndef SRC_AUDIOVIEW_H_
#define SRC_AUDIOVIEW_H_

#include <QWidget>
#include <QMediaPlayer>

#include "src/data-source.h"

class QTemporaryFile;
class QSlider;
class QPushButton;

class FSSAudioView : public QWidget {
  Q_OBJECT

 protected:
  QTemporaryFile *file;
  QMediaPlayer *player;
  QSlider *slider;
  QPushButton *buttonPlay;

 public:
  explicit FSSAudioView(QWidget *pParent = NULL);
  virtual ~FSSAudioView();

  void setAudioData(void *data, size_t size, const QString &format);

 public slots:
  void on_media_status_changed(QMediaPlayer::MediaStatus status);
  void on_duration_changed(qint64 duration);
  void on_position_changed(qint64 position);
  void on_state_changed(QMediaPlayer::State state);
  void on_play();
};

#endif  // SRC_AUDIOVIEW_H_
