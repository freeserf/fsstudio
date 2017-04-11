/*
 * sourcesview.h - FSSSourcesView declaration
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

#ifndef SRC_SOURCESVIEW_H_
#define SRC_SOURCESVIEW_H_

#include <QSplitter>
#include <QList>

#include "src/data.h"

class FSSResourceView;

class FSSSourcesView : public QSplitter {
  Q_OBJECT

 protected:
  QList<FSSResourceView*> sourceViews;

 public:
  explicit FSSSourcesView(QWidget *parent = 0);

 signals:
  void resourceSelected(Data::Resource resource_class, unsigned int index);

 public slots:
  void addSource(DataSource *source);
  void onResourceSelected(Data::Resource resource_class, unsigned int index);
  void showSource(DataSource *source, bool show);
};

#endif  // SRC_SOURCESVIEW_H_
