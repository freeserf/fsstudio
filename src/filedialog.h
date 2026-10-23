/*
 * filedialog.h - native file dialogs declaration
 *
 * Copyright (C) 2026  Wicked_Digger <wicked_digger@mail.ru>
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

#ifndef SRC_FILEDIALOG_H_
#define SRC_FILEDIALOG_H_

#include <functional>
#include <string>

struct SDL_Window;

// Wrapper over SDL native dialogs. SDL may call back from another thread,
// so results are queued and handlers are invoked from process(), which must
// be called on the main thread every frame.
class FSSFileDialog {
 public:
  typedef std::function<void(const std::string &path)> Handler;

  static void set_window(SDL_Window *window);

  static void open_file(const std::string &default_path, Handler handler);
  static void open_folder(const std::string &default_path, Handler handler);
  // pattern is a list of extensions without dots, separated by ';'
  static void save_file(const std::string &default_path,
                        const std::string &filter_name,
                        const std::string &pattern, Handler handler);

  static void process();
};

#endif  // SRC_FILEDIALOG_H_
