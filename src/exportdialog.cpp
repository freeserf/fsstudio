/*
 * exportdialog.cpp - FSSExportDialog implementation
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

#include "src/exportdialog.h"

#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QStandardPaths>
#include <QPushButton>
#include <QComboBox>
#include <QSpinBox>
#include <QLineEdit>

#include "src/qpathedit.h"
#include "src/exporter.h"
#include "src/datamodel.h"

FSSExportDialog::FSSExportDialog(FSSDataModel *_dataModel, QWidget *parent)
  : QDialog(parent)
  , dataModel(_dataModel) {
  source_num = -1;

  QVBoxLayout *vLayout = new QVBoxLayout(this);
  this->setLayout(vLayout);

  QFormLayout *layout = new QFormLayout();
  layout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
  vLayout->addLayout(layout);

  field_source = new QComboBox(this);
  field_source->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  layout->addRow("Asset source:", field_source);

  field_name = new QLineEdit(this);
  field_name->setText("Unnamed");
  field_name->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  layout->addRow("Name:", field_name);

  field_folder = new QPathEdit(this);
  field_folder->setAllowEmptyPath(false);
  field_folder->setPathMode(QPathEdit::ExistingFolder);
  field_folder->setPath(QStandardPaths::writableLocation(
                                            QStandardPaths::DocumentsLocation));
  field_folder->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  layout->addRow("Target folder:", field_folder);

  field_scale = new QSpinBox(this);
  field_scale->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  field_scale->setValue(1);
  field_scale->setMaximum(999);
  field_scale->setMinimum(1);
  layout->addRow("Scale:", field_scale);

  QHBoxLayout *hLayout = new QHBoxLayout();
  vLayout->addLayout(hLayout);

  button_export = new QPushButton("Export", this);
  button_export->setEnabled(false);
  connect(button_export, SIGNAL(pressed()), SLOT(do_export()));
  button_cancel = new QPushButton("Cancel", this);
  connect(button_cancel, SIGNAL(pressed()), SLOT(close()));

  hLayout->addStretch();
  hLayout->addWidget(button_cancel);
  hLayout->addWidget(button_export);

  for (size_t i = 0; i < dataModel->dataSourceCount(); i++) {
    Data::PSource data_source = dataModel->getDataSource(i);
    add_source(data_source);
  }

  setMinimumWidth(500);
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());
}

FSSExportDialog::~FSSExportDialog() {
}

void
FSSExportDialog::add_source(Data::PSource data_source) {
  if (data_source == nullptr) {
    return;
  }

  field_source->addItem(data_source->get_name().c_str());

  if (field_source->count() == 1) {
    source_num = 0;
    button_export->setEnabled(true);
    field_source->setCurrentIndex(source_num);
  }
}

void
FSSExportDialog::do_export() {
  button_cancel->setEnabled(false);
  button_export->setEnabled(false);
  source_num = field_source->currentIndex();
  Data::PSource source = dataModel->getDataSource(source_num);
  FSSExporter exporter(source, field_folder->path(), field_scale->value());
  exporter.set_name(field_name->text().toLocal8Bit().data());
  exporter.do_export();
  close();
}
