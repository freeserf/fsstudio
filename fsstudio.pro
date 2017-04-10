#
# fs_studio.pro - FSStudio Qt project file
#
# Copyright (C) 2016  Wicked_Digger <wicked_digger@mail.ru>
#
# This file is part of FSViewer.
#
# FSStudio is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# FSStudio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with FSStudio.  If not, see <http://www.gnu.org/licenses/>.
#

QT += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets xml

TARGET = FSStudio
TEMPLATE = app

SOURCES += src/main.cpp\
    src/mainwindow.cpp \
    freeserf/src/data.cc \
    freeserf/src/log.cc \
    freeserf/src/sfx2wav.cc \
    freeserf/src/xmi2mid.cc \
    freeserf/src/data-source-amiga.cc \
    freeserf/src/data-source-dos.cc \
    freeserf/src/data-source.cc \
    freeserf/src/tpwm.cc \
    src/resourcemodel.cpp \
    src/resourceview.cpp \
    src/sourcesview.cpp \
    src/spriteview.cpp \
    src/audioview.cpp \
    src/animationview.cpp \
    src/exporter.cpp \
    src/exportdialog.cpp \
    src/qpathedit.cpp

HEADERS  += src/mainwindow.h \
    freeserf/src/data-source-amiga.h \
    freeserf/src/data-source-dos.h \
    freeserf/src/data-source.h \
    freeserf/src/data.h \
    freeserf/src/log.h \
    freeserf/src/misc.h \
    freeserf/src/sfx2wav.h \
    freeserf/src/xmi2mid.h \
    freeserf/src/tpwm.h \
    src/resourcemodel.h \
    src/resourceview.h \
    src/sourcesview.h \
    src/spriteview.h \
    src/audioview.h \
    src/animationview.h \
    src/exporter.h \
    src/exportdialog.h \
    src/qpathedit.h

INCLUDEPATH += .
INCLUDEPATH += freeserf

RESOURCES += \
    res/resources.qrc
