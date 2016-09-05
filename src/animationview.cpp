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
  animation = NULL;
}

FSSAnimationView::~FSSAnimationView() {
}

void
FSSAnimationView::setAnimation(const Animation *_animation) {
  animation = _animation;

  if (animation == NULL) {
    return;
  }

  QString text;
/*
  for (size_t i = 0; i < animation->get_size(); i++) {
    const animation_stage_t *stage = animation->get_stage(i);
    QString t;
    t.sprintf("%lu\ttime = %d\tx = %d\ty = %d\n", i, stage->sprite, stage->x, stage->y);
    text += t;
  }
*/
  setText(text);
}
