/*
 * paletteview.h - FSSPaletteView declaration
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

#ifndef SRC_ANIMATIONVIEW_H_
#define SRC_ANIMATIONVIEW_H_

#include <QLabel>

#include "src/data-source.h"

class FSSAnimationView : public QLabel {
  Q_OBJECT

 protected:
  const Animation *animation;

 public:
  explicit FSSAnimationView(QWidget *pParent = NULL);
  virtual ~FSSAnimationView();

  void setAnimation(const Animation *animation);
};

#endif  // SRC_ANIMATIONVIEW_H_