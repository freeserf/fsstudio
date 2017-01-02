/*
 * resourceview.h - FSSResourceView declaration
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

#ifndef SRC_RESOURCEVIEW_H_
#define SRC_RESOURCEVIEW_H_

#include <QWidget>

#include "src/data.h"

class DataSource;
class QTextBrowser;
class FSSSpriteView;
class FSSAudioView;
class FSSAnimationView;
class QStackedLayout;

class FSSResourceView : public QWidget {
  Q_OBJECT

 protected:
  DataSource *source;

  QStackedLayout *resourcesStack;
  QTextBrowser *textBrowser;

  FSSSpriteView *viewSprite;
  FSSAudioView *viewAudio;
  FSSAnimationView *viewAnimation;
  QWidget *viewEmpty;

 public:
  explicit FSSResourceView(DataSource *source, QWidget *parent = 0);
  virtual ~FSSResourceView();

 public slots:
  void onResourceSelected(Data::Resource resource_class,
                          unsigned int index);
  void on_save();
  void on_copy();
};

#endif  // SRC_RESOURCEVIEW_H_
