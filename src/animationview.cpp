/*
 * paletteview.cpp - FSSPaletteModel implementation
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

#include "src/animationview.h"

FSSAnimationView::FSSAnimationView(QWidget *pParent) : QLabel(pParent) {
  index = 0;
}

FSSAnimationView::~FSSAnimationView() {
}

void
FSSAnimationView::setAnimation(PDataSource data_source, size_t index) {
  this->data_source = data_source;
  this->index = index;

  if (!data_source) {
    return;
  }

  QString text;

  for (size_t i = 0; i < data_source->get_animation_phase_count(index); i++) {
    Animation stage = data_source->get_animation(index, i << 3);
    QString line = QString("%1\tsprite = %2\tx = %3\ty = %4\n").arg(i).arg(stage.sprite).arg(stage.x).arg(stage.y);
    text += line;
  }

  setText(text);
}
