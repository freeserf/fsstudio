/*
 * sourcesview.cpp - FSSSourcesView implementation
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

#include "src/sourcesview.h"

#include <QMenuBar>

#include "src/resourceview.h"

FSSSourcesView::FSSSourcesView(QWidget *parent)
  : QSplitter(Qt::Horizontal, parent) {
}

void
FSSSourcesView::addSource(PDataSource source) {
  FSSResourceView *resView = new FSSResourceView(source, this);
  resView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  resView->setMinimumWidth(250);
  sourceViews.push_back(resView);

  connect(this, SIGNAL(resourceSelected(Data::Resource, unsigned int)),
          resView, SLOT(selectResource(Data::Resource, unsigned int)));
}

void
FSSSourcesView::onResourceSelected(Data::Resource resource_class,
                                   unsigned int index) {
  emit resourceSelected(resource_class, index);
}

void
FSSSourcesView::showSource(PDataSource source, bool show) {
  FSSResourceView *view;
  foreach (view, sourceViews) {
    if (view->source() == source) {
      if (show) {
          view->show();
      } else {
          view->hide();
      }
      return;
    }
  }
}
