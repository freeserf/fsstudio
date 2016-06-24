/*
 * resourceview.h - FVResourceView declaration
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
#include "src/paletteview.h"

class data_source_t;
class sprite_t;
class QLabel;
class QTextBrowser;
class QScrollArea;

class FSSResourceView : public QWidget {
  Q_OBJECT

 protected:
  data_source_t *source;

  QScrollArea *scrollArea;
  QTextBrowser *textBrowser;

 public:
  explicit FSSResourceView(data_source_t *source, QWidget *parent = 0);
  virtual ~FSSResourceView();

 protected:
  QPixmap pixmapFromSprite(sprite_t *sprite);

 public slots:
  void onResourceSelected(data_res_class_t resource_class,
                          unsigned int index);
};

#endif  // SRC_RESOURCEVIEW_H_
