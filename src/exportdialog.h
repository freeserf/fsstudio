/*
 * exportdialog.h - FSSExportDialog declaration
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

#ifndef SRC_EXPORTDIALOG_H_
#define SRC_EXPORTDIALOG_H_

#include <QDialog>
#include <QLabel>

#include "src/data.h"

class QComboBox;
class QPathEdit;
class QSpinBox;
class QPushButton;
class QLineEdit;

class FSSColorLabel;
class FSSDataModel;

class FSSExportDialog : public QDialog {
  Q_OBJECT

 protected:
  QComboBox *field_source;
  QLineEdit *field_name;
  QPathEdit *field_folder;
  QSpinBox *field_scale;
  QPushButton *button_cancel;
  QPushButton *button_export;
  int source_num;
  FSSDataModel *dataModel;

 public:
  explicit FSSExportDialog(FSSDataModel *dataModel, QWidget *parent = 0);
  virtual ~FSSExportDialog();

 protected:
  void add_source(Data::PSource data_source);

 public slots:
  void do_export();
};

#endif  // SRC_EXPORTDIALOG_H_
