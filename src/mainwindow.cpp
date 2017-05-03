/*
 * mainwindow.cpp - FSSMainWindow implementation
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

#include <QMenuBar>
#include <QDockWidget>

#include "src/resourcemodel.h"
#include "src/sourcesview.h"

#include "src/data.h"
#include "src/data-source.h"
#include "src/exportdialog.h"
#include "src/sourcesdialog.h"
#include "src/datamodel.h"

FSSMainWindow::FSSMainWindow(FSSDataModel *_dataModel, QWidget *parent)
  : QMainWindow(parent)
  , resourceModel(nullptr)
  , dataModel(_dataModel) {
  QSplitter *mainSplitter = new QSplitter(Qt::Horizontal, this);
  setCentralWidget(mainSplitter);

  resourceModel = new FSSResourceModel();

  QDockWidget *dockResources = new QDockWidget("Resources", this);
  treeResources = new QTreeView(dockResources);
  treeResources->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
  treeResources->setModel(resourceModel);
  dockResources->setWidget(treeResources);
  addDockWidget(Qt::LeftDockWidgetArea, dockResources);

  QMenuBar *menuBar = this->menuBar();

  QMenu *menuFile = new QMenu("File", this);
  menuFile->addAction("Export", this, SLOT(exportSource()));
  menuBar->addMenu(menuFile);
  QAction *action_src = menuFile->addAction("Sources", this, SLOT(openSources()));
  action_src->setMenuRole(QAction::PreferencesRole);

  QMenu *menuView = new QMenu("View", this);
  menuBar->addMenu(menuView);
  QAction *action = menuView->addAction("Resources", dockResources, SLOT(setVisible(bool)));
  action->setCheckable(true);
  connect(dockResources, SIGNAL(visibilityChanged(bool)), action, SLOT(setChecked(bool)));

  menuView->addSeparator();

  viewSources = new FSSSourcesView(mainSplitter);
  viewSources->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  for (size_t i = 0; i < dataModel->dataSourceCount(); i++) {
    PDataSource data_source = dataModel->getDataSource(i);
    viewSources->addSource(data_source);
    QAction *action = menuView->addAction(data_source->get_name().c_str(), this, SLOT(switchSource(bool)));
    action->setCheckable(true);
    action->setChecked(true);
    actions[action] = data_source;
  }

  connect(treeResources->selectionModel(),
          SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)),
          SLOT(onCurrentChanged(const QModelIndex&, const QModelIndex&)));

  connect(this, SIGNAL(resourceSelected(Data::Resource, unsigned int)),
          viewSources, SLOT(onResourceSelected(Data::Resource,
                                               unsigned int)));
}

FSSMainWindow::~FSSMainWindow() {
}

void
FSSMainWindow::onCurrentChanged(const QModelIndex &current,
                                const QModelIndex & /*previous*/) {
  Data::Resource resource_class = Data::AssetNone;
  unsigned int index = 0;

  if (current.parent().isValid()) {
    resource_class = (Data::Resource)(current.parent().row() + 1);
    index = current.row();
  }

  emit resourceSelected(resource_class, index);
}

void
FSSMainWindow::exportSource() {
  FSSExportDialog dialog(dataModel, this);
  dialog.setWindowModality(Qt::WindowModal);
  dialog.exec();
}

void
FSSMainWindow::openSources() {
  FSSSourcesDialog dialog(dataModel, this);
  dialog.setWindowModality(Qt::ApplicationModal);
  dialog.exec();
}

void
FSSMainWindow::switchSource(bool checked) {
  QAction *action = (QAction*)sender();
  PDataSource source = actions[action];
  viewSources->showSource(source, checked);
}
