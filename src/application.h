/*
 * application.h - FSSApplication declaration
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

#ifndef SRC_APPLICATION_H_
#define SRC_APPLICATION_H_

#include <memory>
#include <string>

#include "src/datamodel.h"
#include "src/resourceview.h"

struct SDL_Renderer;
struct ImGuiSettingsHandler;
struct ImGuiTextBuffer;
struct ImGuiContext;

// Main window content: menus, resource tree, source views and dialogs.
class FSSApplication {
 protected:
  FSSDataModel data_model;
  std::unique_ptr<FSSResourceView>
                              views[FSSDataModel::DataSourceTypeCount];
  bool show_view[FSSDataModel::DataSourceTypeCount];
  bool show_resources;
  FSSResourceView::Options options;

  Data::Resource selected_resource;
  size_t selected_index;
  bool reveal_selection;  // open and scroll the tree to the selection

  // settings
  std::string source_paths[FSSDataModel::DataSourceTypeCount];
  std::string export_folder;
  std::string export_name;
  int export_scale;

  // dialogs
  bool open_sources_dialog;
  bool open_export_dialog;
  bool quit_requested;
  std::string source_edit[FSSDataModel::DataSourceTypeCount];
  int export_source;
  bool export_pending;
  std::string export_result;

 public:
  explicit FSSApplication(SDL_Renderer *renderer);

  // Registers the settings handler and loads settings and data sources.
  // Must be called after ImGui is initialized, before the first frame.
  void init();
  void draw();

  // Selects a resource given as "name:index", e.g. "extro_bg:3".
  bool select(const std::string &resource);

  bool is_quit_requested() const { return quit_requested; }
  bool is_playing_audio() const;

 protected:
  void draw_menu();
  void draw_resources();
  void draw_sources_dialog();
  void draw_export_dialog();
  void setup_layout(unsigned int dockspace_id);  // ImGuiID
  void set_source_path(FSSDataModel::DataSourceType type,
                       const std::string &path);

  static void *settings_read_open(ImGuiContext *ctx,
                                  ImGuiSettingsHandler *handler,
                                  const char *name);
  static void settings_read_line(ImGuiContext *ctx,
                                 ImGuiSettingsHandler *handler,
                                 void *entry, const char *line);
  static void settings_write_all(ImGuiContext *ctx,
                                 ImGuiSettingsHandler *handler,
                                 ImGuiTextBuffer *buf);
};

#endif  // SRC_APPLICATION_H_
