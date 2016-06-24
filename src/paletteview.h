/*
 * paletteview.h - FVPaletteView declaration
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

#ifndef SRC_PALETTEVIEW_H_
#define SRC_PALETTEVIEW_H_

#include <QWidget>

#include "src/data-source.h"

class FSSPaletteView : public QWidget {
  Q_OBJECT

 protected:
  palette_t *palette;
  unsigned int row_height;

 public:
  explicit FSSPaletteView(QWidget *pParent = NULL);

  void setPalette(palette_t *palette);

  virtual QSize sizeHint() const;

 protected:
  virtual void paintEvent(QPaintEvent *event);
};

#endif  // SRC_PALETTEVIEW_H_
