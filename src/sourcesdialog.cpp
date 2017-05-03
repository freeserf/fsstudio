/*
 * sourcesdialog.cpp - FSSSourcesDialog implementation
 *
 * Copyright (C) 2017  Wicked_Digger <wicked_digger@mail.ru>
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

#include "src/sourcesdialog.h"

#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>

#include "src/data-source.h"
#include "src/qpathedit.h"
#include "src/datamodel.h"

FSSSourcesDialog::FSSSourcesDialog(FSSDataModel *_dataModel, QWidget *parent)
  : QDialog(parent)
  , dataModel(_dataModel) {
  setWindowFlags((windowFlags() | Qt::CustomizeWindowHint)
                 & ~Qt::WindowMaximizeButtonHint
                 & ~Qt::WindowMinimizeButtonHint);

  QVBoxLayout *vLayout = new QVBoxLayout(this);
  this->setLayout(vLayout);

  layout = new QFormLayout();
  layout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
  vLayout->addLayout(layout);

  QHBoxLayout *hLayout = new QHBoxLayout();
  vLayout->addLayout(hLayout);

//  QPushButton *button_accept = new QPushButton("Accept", this);
//  connect(button_accept, SIGNAL(pressed()), SLOT(accept()));
//  QPushButton *button_cancel = new QPushButton("Cancel", this);
//  connect(button_cancel, SIGNAL(pressed()), SLOT(reject()));
  QPushButton *button_add = new QPushButton("Add", this);

  hLayout->addWidget(button_add);
  hLayout->addStretch();
//  hLayout->addWidget(button_cancel);
//  hLayout->addWidget(button_accept);

  for (size_t i = 0; i < dataModel->dataSourceCount(); i++) {
    add_source(dataModel->getDataSource(i));
  }

  setMinimumWidth(500);
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());
}

FSSSourcesDialog::~FSSSourcesDialog() {
}

void
FSSSourcesDialog::add_source(PDataSource source) {
  QHBoxLayout *line_layout = new QHBoxLayout();
  layout->addRow(source->get_name().c_str(), line_layout);

  QLabel *label = new QLabel(this);
  label->setPixmap(QPixmap(source->is_loaded() ?
                           ":/icons/Actions-dialog-ok-apply-icon.png" :
                           ":/icons/Actions-edit-delete-icon.png"));
  line_layout->addWidget(label);

  QPathEdit *path_edit = new QPathEdit(this);
  FSSDataModel::DataSourceType type = FSSDataModel::getType(source);
  path_edit->setPathMode((type == FSSDataModel::DataSourceTypeDos) ?
                         QPathEdit::ExistingFile :
                         QPathEdit::ExistingFolder);
  path_edit->setPath(source->get_path().c_str());
  line_layout->addWidget(path_edit);
}
