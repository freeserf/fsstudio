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
#include <QColorDialog>

#include "src/qpathedit.h"
#include "src/exporter.h"
#include "src/data-source.h"

FSSExportDialog::FSSExportDialog(QWidget *parent) : QDialog(parent) {
  source_num = -1;

  QVBoxLayout *vLayout = new QVBoxLayout(this);
  this->setLayout(vLayout);

  QFormLayout *layout = new QFormLayout(this);
  layout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
  vLayout->addLayout(layout);

  field_source = new QComboBox(this);
  field_source->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  layout->addRow("Asset source:", field_source);

  field_folder = new QPathEdit(this);
  field_folder->setAllowEmptyPath(false);
  field_folder->setPathMode(QPathEdit::ExistingFolder);
  field_folder->setPath(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
  field_folder->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  layout->addRow("Target folder:", field_folder);

  field_scale = new QSpinBox(this);
  field_scale->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  field_scale->setValue(1);
  field_scale->setMaximum(999);
  field_scale->setMinimum(1);
  layout->addRow("Scale:", field_scale);

  field_color = new FSSColorLabel(QColor(0xff, 0x00, 0x00), this);
  field_color->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  layout->addRow("Color:", field_color);
  QHBoxLayout *hLayout = new QHBoxLayout(this);
  vLayout->addLayout(hLayout);

  button_export = new QPushButton("Export", this);
  button_export->setEnabled(false);
  connect(button_export, SIGNAL(pressed()), SLOT(do_export()));
  button_cancel = new QPushButton("Cancel", this);
  connect(button_cancel, SIGNAL(pressed()), SLOT(close()));

  hLayout->addStretch();
  hLayout->addWidget(button_cancel);
  hLayout->addWidget(button_export);
}

FSSExportDialog::~FSSExportDialog() {
}

void
FSSExportDialog::add_source(DataSource *data_source) {
  if (data_source == nullptr) {
    return;
  }

  sources.push_back(data_source);
  field_source->addItem(data_source->get_name());

  if (sources.count() == 1) {
    source_num = 0;
    button_export->setEnabled(true);
    field_source->setCurrentIndex(source_num);
  }
}

void
FSSExportDialog::do_export() {
  button_cancel->setEnabled(false);
  button_export->setEnabled(false);
  DataSource *source = sources[source_num];
  FSSExporter exporter(source, field_folder->path(), field_scale->value());
  exporter.set_color(field_color->color());
  exporter.do_export();
  close();
}

// FSSColorLabel
FSSColorLabel::FSSColorLabel(const QColor& _color, QWidget* parent)
  : QLabel("       ", parent) {
  setColor(_color);
}

FSSColorLabel::~FSSColorLabel() {
}

void
FSSColorLabel::setColor(const QColor _color) {
  cur_color = _color;
  QPalette pal = palette();
  pal.setColor(backgroundRole(), cur_color);
  setPalette(pal);
  setAutoFillBackground(true);
  update();
}

QColor
FSSColorLabel::color() const {
  return cur_color;
}

void
FSSColorLabel::mousePressEvent(QMouseEvent*) {
  QColorDialog dialog(this);
  dialog.setCurrentColor(cur_color);
  if (dialog.exec() == QDialog::Accepted) {
    setColor(dialog.currentColor());
  }
}
