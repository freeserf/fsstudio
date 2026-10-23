/*
 * filedialog.cpp - native file dialogs implementation
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

#include "src/filedialog.h"

#include <SDL3/SDL.h>

#include <mutex>
#include <utility>
#include <vector>

namespace {

SDL_Window *dialog_window = nullptr;

std::mutex pending_mutex;
std::vector<std::pair<FSSFileDialog::Handler, std::string>> pending;

struct Request {
  FSSFileDialog::Handler handler;
  // the filter must stay valid until the dialog calls back
  std::string filter_name;
  std::string filter_pattern;
  SDL_DialogFileFilter filter;
};

void SDLCALL
on_dialog_result(void *userdata, const char * const *filelist, int /*filter*/) {
  Request *request = static_cast<Request*>(userdata);
  // filelist is nullptr on error and empty when the dialog was cancelled
  if (filelist != nullptr && filelist[0] != nullptr) {
    std::lock_guard<std::mutex> lock(pending_mutex);
    pending.emplace_back(std::move(request->handler), filelist[0]);
  }
  delete request;
}

Request *
make_request(FSSFileDialog::Handler handler) {
  Request *request = new Request();
  request->handler = std::move(handler);
  return request;
}

const char *
location(const std::string &path) {
  return path.empty() ? nullptr : path.c_str();
}

}  // namespace

void
FSSFileDialog::set_window(SDL_Window *window) {
  dialog_window = window;
}

void
FSSFileDialog::open_file(const std::string &default_path, Handler handler) {
  SDL_ShowOpenFileDialog(on_dialog_result, make_request(std::move(handler)),
                         dialog_window, nullptr, 0, location(default_path),
                         false);
}

void
FSSFileDialog::open_folder(const std::string &default_path, Handler handler) {
  SDL_ShowOpenFolderDialog(on_dialog_result, make_request(std::move(handler)),
                           dialog_window, location(default_path), false);
}

void
FSSFileDialog::save_file(const std::string &default_path,
                         const std::string &filter_name,
                         const std::string &pattern, Handler handler) {
  Request *request = make_request(std::move(handler));
  request->filter_name = filter_name;
  request->filter_pattern = pattern;
  request->filter = { request->filter_name.c_str(),
                      request->filter_pattern.c_str() };
  SDL_ShowSaveFileDialog(on_dialog_result, request, dialog_window,
                         &request->filter, 1, location(default_path));
}

void
FSSFileDialog::process() {
  std::vector<std::pair<Handler, std::string>> results;
  {
    std::lock_guard<std::mutex> lock(pending_mutex);
    results.swap(pending);
  }
  for (auto &result : results) {
    result.first(result.second);
  }
}
