/*
 * mainwindow.h - FSSMainWindow declaration
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

#ifndef SRC_MAINWINDOW_H_
#define SRC_MAINWINDOW_H_

#include <QMainWindow>
#include <QTreeView>

#include "src/data.h"

class FSSResourceModel;

class FSSMainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit FSSMainWindow(QWidget *parent = 0);
  virtual ~FSSMainWindow();

 private:
  QTreeView *treeResources;
  FSSResourceModel *resourceModel;

 signals:
  void resourceSelected(Data::Resource resource_class,
                        unsigned int index);

 public slots:
  void onCurrentChanged(const QModelIndex &current,
                        const QModelIndex &previous);
};

#endif  // SRC_MAINWINDOW_H_
