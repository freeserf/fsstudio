/*
 * mainwindow.cpp - FVMainWindow implementation
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

#include "src/mainwindow.h"

#include <QBitmap>
#include <QLabel>
#include <QHBoxLayout>
#include <QSplitter>
#include <QStandardPaths>

#include "src/resourcemodel.h"
#include "src/sourcesview.h"

#include "src/data.h"
#include "src/data-source.h"
#include "src/data-source-dos.h"
#include "src/data-source-amiga.h"

data_source_t *data_source_dos;
data_source_t *data_source_amiga;

FSSMainWindow::FSSMainWindow(QWidget *parent)
  : QMainWindow(parent)
  , resourceModel(NULL) {
  QSplitter *mainSplitter = new QSplitter(Qt::Horizontal, this);
  setCentralWidget(mainSplitter);

  treeResources = new QTreeView(mainSplitter);
  treeResources->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  resourceModel = new FSSResourceModel();
  treeResources->setModel(resourceModel);

  FSSSourcesView *viewSources = new FSSSourcesView(mainSplitter);
  viewSources->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  QString path = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
  path += "/.local/share/freeserf";
  QString path_dos = path + "/spae.pa";

  data_source_dos = new data_source_dos_t();
  if (!data_source_dos->load(path_dos.toLocal8Bit().data())) {
    delete data_source_dos;
    data_source_dos = NULL;
  } else {
    viewSources->addSource(data_source_dos);
  }

  data_source_amiga = new data_source_amiga_t();
  if (!data_source_amiga->load(path.toLocal8Bit().data())) {
    delete data_source_amiga;
    data_source_amiga = NULL;
  } else {
    viewSources->addSource(data_source_amiga);
  }

  connect(treeResources->selectionModel(),
          SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)),
          SLOT(onCurrentChanged(const QModelIndex&, const QModelIndex&)));

  connect(this, SIGNAL(resourceSelected(data_res_class_t, unsigned int)),
          viewSources, SLOT(onResourceSelected(data_res_class_t,
                                               unsigned int)));
}

FSSMainWindow::~FSSMainWindow() {
}

void
FSSMainWindow::onCurrentChanged(const QModelIndex &current,
                               const QModelIndex & /*previous*/) {
  data_res_class_t resource_class = data_res_none;
  unsigned int index = 0;

  if (current.parent().isValid()) {
      resource_class = (data_res_class_t)(current.parent().row() + 1);
      index = current.row();
  }

  emit resourceSelected(resource_class, index);
}
