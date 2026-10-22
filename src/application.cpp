/*
 * application.cpp - FSSApplication implementation
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

#include "src/application.h"

#include <SDL3/SDL.h>

#include <algorithm>
#include <cstdio>
#include <cstring>

#include "imgui.h"
#include "imgui_internal.h"
#include "misc/cpp/imgui_stdlib.h"

#include "src/exporter.h"
#include "src/filedialog.h"

namespace {

const char *settings_type = "FSStudio";
const char *path_keys[FSSDataModel::DataSourceTypeCount] = {
  "path_dos", "path_amiga", "path_custom"
};
const char *show_keys[FSSDataModel::DataSourceTypeCount] = {
  "show_dos", "show_amiga", "show_custom"
};

#ifdef __APPLE__
const char *quit_shortcut = "Cmd+Q";
#else
const char *quit_shortcut = "Ctrl+Q";
#endif

const char *
resource_type_name(Data::Resource resource) {
  static const char *types[] = { "unknown", "sprite", "animation", "sound",
                                 "music" };
  int type = Data::get_resource_type(resource);
  return (type >= 0 && type < 5) ? types[type] : types[0];
}

std::string
documents_folder() {
  const char *folder = SDL_GetUserFolder(SDL_FOLDER_DOCUMENTS);
  return folder != nullptr ? folder : "";
}

FSSDataModel::DataSourceType
source_type(int type) {
  return static_cast<FSSDataModel::DataSourceType>(type);
}

}  // namespace

FSSApplication::FSSApplication(SDL_Renderer *renderer)
  : show_resources(true)
  , selected_resource(Data::AssetNone)
  , selected_index(0)
  , reveal_selection(false)
  , export_name("Unnamed")
  , export_scale(1)
  , open_sources_dialog(false)
  , open_export_dialog(false)
  , quit_requested(false)
  , export_source(0)
  , export_pending(false) {
  for (int type = 0; type < FSSDataModel::DataSourceTypeCount; type++) {
    views[type] = std::make_unique<FSSResourceView>(renderer);
    show_view[type] = true;
  }
  options.sprite_color = { 0xFF, 0x00, 0x00, 0xFF };  // blue (BGRA)
  options.zoom = 2;
  export_folder = documents_folder();
}

void
FSSApplication::init() {
  ImGuiSettingsHandler handler;
  handler.TypeName = settings_type;
  handler.TypeHash = ImHashStr(settings_type);
  handler.ReadOpenFn = settings_read_open;
  handler.ReadLineFn = settings_read_line;
  handler.WriteAllFn = settings_write_all;
  handler.UserData = this;
  ImGui::AddSettingsHandler(&handler);

  const char *ini_file = ImGui::GetIO().IniFilename;
  if (ini_file != nullptr) {
    ImGui::LoadIniSettingsFromDisk(ini_file);
  }

  for (int type = 0; type < FSSDataModel::DataSourceTypeCount; type++) {
    std::string path = source_paths[type];
    if (path.empty()) {
      path = FSSDataModel::default_path(source_type(type));
    }
    set_source_path(source_type(type), path);
  }
}

void
FSSApplication::set_source_path(FSSDataModel::DataSourceType type,
                                const std::string &path) {
  source_paths[type] = path;
  source_edit[type] = path;
  data_model.set_source_path(type, path);
  views[type]->set_source(data_model.get_source(type),
                          data_model.is_loaded(type));
  ImGui::MarkIniSettingsDirty();
}

bool
FSSApplication::select(const std::string &resource) {
  size_t colon = resource.rfind(':');
  std::string name = resource.substr(0, colon);
  size_t index = (colon == std::string::npos)
                 ? 0 : strtoul(resource.c_str() + colon + 1, nullptr, 10);
  for (int r = Data::AssetExtroBg; r <= Data::AssetStar; r++) {
    Data::Resource res = static_cast<Data::Resource>(r);
    if (Data::get_resource_name(res) == name &&
        index < Data::get_resource_count(res)) {
      selected_resource = res;
      selected_index = index;
      reveal_selection = true;
      return true;
    }
  }
  return false;
}

bool
FSSApplication::is_playing_audio() const {
  for (const auto &view : views) {
    if (view->is_playing()) {
      return true;
    }
  }
  return false;
}

void
FSSApplication::draw() {
  draw_menu();

  ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");
  if (ImGui::DockBuilderGetNode(dockspace_id) == nullptr) {
    setup_layout(dockspace_id);
  }
  ImGui::DockSpaceOverViewport(dockspace_id, ImGui::GetMainViewport());

  if (show_resources) {
    draw_resources();
  }

  FSSResourceView::Options old_options = options;
  for (int type = 0; type < FSSDataModel::DataSourceTypeCount; type++) {
    views[type]->select(selected_resource, selected_index,
                        options.sprite_color);
    if (show_view[type]) {
      if (ImGui::Begin(FSSDataModel::get_type_name(source_type(type)),
                       &show_view[type])) {
        views[type]->draw(&options);
      }
      ImGui::End();
    }
  }
  if (memcmp(&old_options, &options, sizeof(options)) != 0) {
    ImGui::MarkIniSettingsDirty();
  }

  draw_sources_dialog();
  draw_export_dialog();

  if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_Q, ImGuiInputFlags_RouteGlobal)) {
    quit_requested = true;
  }
}

void
FSSApplication::setup_layout(ImGuiID dockspace_id) {
  ImGui::DockBuilderRemoveNode(dockspace_id);
  ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
  ImGui::DockBuilderSetNodeSize(dockspace_id,
                                ImGui::GetMainViewport()->WorkSize);

  ImGuiID left, right, view_ids[FSSDataModel::DataSourceTypeCount];
  ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, .22f, &left,
                              &right);
  // split the rest into equal columns, one per source
  for (int type = 0; type < FSSDataModel::DataSourceTypeCount - 1; type++) {
    float ratio = 1.f / (FSSDataModel::DataSourceTypeCount - type);
    ImGui::DockBuilderSplitNode(right, ImGuiDir_Left, ratio, &view_ids[type],
                                &right);
  }
  view_ids[FSSDataModel::DataSourceTypeCount - 1] = right;

  ImGui::DockBuilderDockWindow("Resources", left);
  for (int type = 0; type < FSSDataModel::DataSourceTypeCount; type++) {
    ImGui::DockBuilderDockWindow(FSSDataModel::get_type_name(source_type(type)),
                                 view_ids[type]);
  }
  ImGui::DockBuilderFinish(dockspace_id);
}

void
FSSApplication::draw_menu() {
  if (!ImGui::BeginMainMenuBar()) {
    return;
  }

  if (ImGui::BeginMenu("File")) {
    if (ImGui::MenuItem("Sources...")) {
      open_sources_dialog = true;
    }
    if (ImGui::MenuItem("Export...")) {
      open_export_dialog = true;
    }
    ImGui::Separator();
    if (ImGui::MenuItem("Quit", quit_shortcut)) {
      quit_requested = true;
    }
    ImGui::EndMenu();
  }

  if (ImGui::BeginMenu("View")) {
    ImGui::MenuItem("Resources", nullptr, &show_resources);
    ImGui::Separator();
    for (int type = 0; type < FSSDataModel::DataSourceTypeCount; type++) {
      ImGui::MenuItem(FSSDataModel::get_type_name(source_type(type)), nullptr,
                      &show_view[type]);
    }
    ImGui::EndMenu();
  }

  ImGui::EndMainMenuBar();
}

void
FSSApplication::draw_resources() {
  if (!ImGui::Begin("Resources", &show_resources)) {
    ImGui::End();
    return;
  }

  ImGuiTableFlags flags = ImGuiTableFlags_BordersV | ImGuiTableFlags_RowBg |
                          ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY |
                          ImGuiTableFlags_NoBordersInBody;
  if (ImGui::BeginTable("resources", 2, flags)) {
    ImGui::TableSetupScrollFreeze(0, 1);
    ImGui::TableSetupColumn("Resource", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed,
                            ImGui::GetFontSize() * 5.f);
    ImGui::TableHeadersRow();

    for (int r = Data::AssetExtroBg; r <= Data::AssetStar; r++) {
      Data::Resource resource = static_cast<Data::Resource>(r);
      const char *type_name = resource_type_name(resource);

      ImGui::TableNextRow();
      ImGui::TableNextColumn();
      if (reveal_selection && resource == selected_resource) {
        ImGui::SetNextItemOpen(true);
      }
      bool open = ImGui::TreeNodeEx(Data::get_resource_name(resource).c_str(),
                                    ImGuiTreeNodeFlags_SpanAllColumns);
      ImGui::TableNextColumn();
      ImGui::TextDisabled("%s", type_name);
      if (!open) {
        continue;
      }

      ImGuiListClipper clipper;
      clipper.Begin(static_cast<int>(Data::get_resource_count(resource)));
      if (reveal_selection && resource == selected_resource) {
        clipper.IncludeItemByIndex(static_cast<int>(selected_index));
      }
      while (clipper.Step()) {
        for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++) {
          bool selected = (resource == selected_resource &&
                           static_cast<size_t>(i) == selected_index);
          ImGui::TableNextRow();
          ImGui::TableNextColumn();
          char label[16];
          snprintf(label, sizeof(label), "%d", i);
          ImGui::TreeNodeEx(label, ImGuiTreeNodeFlags_Leaf |
                                   ImGuiTreeNodeFlags_NoTreePushOnOpen |
                                   ImGuiTreeNodeFlags_SpanAllColumns |
                                   (selected ? ImGuiTreeNodeFlags_Selected : 0));
          if (selected && reveal_selection) {
            ImGui::SetScrollHereY();
            reveal_selection = false;
          }
          // selection follows clicks and keyboard navigation
          if (!selected && (ImGui::IsItemClicked() || ImGui::IsItemFocused())) {
            selected_resource = resource;
            selected_index = i;
          }
          ImGui::TableNextColumn();
          ImGui::TextDisabled("%s", type_name);
        }
      }
      ImGui::TreePop();
    }
    ImGui::EndTable();
  }
  ImGui::End();
}

void
FSSApplication::draw_sources_dialog() {
  if (open_sources_dialog) {
    ImGui::OpenPopup("Sources");
    for (int type = 0; type < FSSDataModel::DataSourceTypeCount; type++) {
      source_edit[type] = source_paths[type];
    }
    open_sources_dialog = false;
  }

  float font = ImGui::GetFontSize();
  ImGui::SetNextWindowSize(ImVec2(font * 45.f, 0), ImGuiCond_Appearing);
  if (!ImGui::BeginPopupModal("Sources")) {
    return;
  }

  if (ImGui::BeginTable("sources", 3, ImGuiTableFlags_SizingFixedFit)) {
    ImGui::TableSetupColumn("name");
    ImGui::TableSetupColumn("status");
    ImGui::TableSetupColumn("path", ImGuiTableColumnFlags_WidthStretch);
    for (int type = 0; type < FSSDataModel::DataSourceTypeCount; type++) {
      FSSDataModel::DataSourceType t = source_type(type);
      ImGui::PushID(type);
      ImGui::TableNextRow();
      ImGui::TableNextColumn();
      ImGui::AlignTextToFramePadding();
      ImGui::TextUnformatted(FSSDataModel::get_type_name(t));
      ImGui::TableNextColumn();
      if (data_model.is_loaded(type)) {
        ImGui::TextColored(ImVec4(.4f, .9f, .4f, 1.f), "loaded");
      } else {
        ImGui::TextColored(ImVec4(1.f, .5f, .4f, 1.f), "not found");
      }
      ImGui::TableNextColumn();
      ImGui::SetNextItemWidth(-font * 2.5f);
      ImGui::InputText("##path", &source_edit[type]);
      if (ImGui::IsItemDeactivatedAfterEdit()) {
        set_source_path(t, source_edit[type]);
      }
      ImGui::SetItemTooltip(FSSDataModel::is_file_source(t)
                            ? "Data file (SPAx.PA)"
                            : "Data folder");
      ImGui::SameLine();
      if (ImGui::Button("...")) {
        auto handler = [this, t](const std::string &path) {
          set_source_path(t, path);
        };
        if (FSSDataModel::is_file_source(t)) {
          FSSFileDialog::open_file(source_paths[type], handler);
        } else {
          FSSFileDialog::open_folder(source_paths[type], handler);
        }
      }
      ImGui::PopID();
    }
    ImGui::EndTable();
  }

  ImGui::Spacing();
  if (ImGui::Button("Reset to defaults")) {
    for (int type = 0; type < FSSDataModel::DataSourceTypeCount; type++) {
      set_source_path(source_type(type),
                      FSSDataModel::default_path(source_type(type)));
    }
  }
  ImGui::SameLine(ImGui::GetContentRegionAvail().x - font * 4.f +
                  ImGui::GetCursorPosX());
  if (ImGui::Button("Close", ImVec2(font * 4.f, 0)) ||
      ImGui::Shortcut(ImGuiKey_Escape)) {
    ImGui::CloseCurrentPopup();
  }
  ImGui::EndPopup();
}

void
FSSApplication::draw_export_dialog() {
  if (open_export_dialog) {
    ImGui::OpenPopup("Export");
    export_result.clear();
    open_export_dialog = false;
  }

  float font = ImGui::GetFontSize();
  ImGui::SetNextWindowSize(ImVec2(font * 35.f, 0), ImGuiCond_Appearing);
  if (!ImGui::BeginPopupModal("Export")) {
    return;
  }

  // only loaded sources can be exported
  std::vector<int> loaded;
  for (int type = 0; type < FSSDataModel::DataSourceTypeCount; type++) {
    if (data_model.is_loaded(type)) {
      loaded.push_back(type);
    }
  }
  if (std::find(loaded.begin(), loaded.end(), export_source) == loaded.end()) {
    export_source = loaded.empty() ? -1 : loaded.front();
  }

  ImGui::BeginDisabled(export_pending);
  const char *preview = (export_source >= 0)
                  ? FSSDataModel::get_type_name(source_type(export_source))
                  : "no data loaded";
  if (ImGui::BeginCombo("Source", preview)) {
    for (int type : loaded) {
      if (ImGui::Selectable(FSSDataModel::get_type_name(source_type(type)),
                            type == export_source)) {
        export_source = type;
      }
    }
    ImGui::EndCombo();
  }
  if (ImGui::InputText("Name", &export_name)) {
    ImGui::MarkIniSettingsDirty();
  }
  ImGui::SetNextItemWidth(ImGui::CalcItemWidth() - font * 2.5f);
  if (ImGui::InputText("##folder", &export_folder)) {
    ImGui::MarkIniSettingsDirty();
  }
  ImGui::SameLine();
  if (ImGui::Button("...")) {
    FSSFileDialog::open_folder(export_folder, [this](const std::string &path) {
      export_folder = path;
      ImGui::MarkIniSettingsDirty();
    });
  }
  ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
  ImGui::TextUnformatted("Folder");
  if (ImGui::InputInt("Scale", &export_scale)) {
    export_scale = std::clamp(export_scale, 1, 999);
    ImGui::MarkIniSettingsDirty();
  }
  ImGui::EndDisabled();

  if (export_pending) {
    // the export runs one frame after the button press, so the message
    // below is shown while it works
    Data::PSource source = data_model.get_source(export_source);
    FSSExporter exporter(source, export_folder, export_scale);
    exporter.set_name(export_name);
    export_result = exporter.do_export() ? "Exported to " + export_folder
                                         : "Export failed";
    export_pending = false;
  }

  ImGui::Spacing();
  if (!export_result.empty()) {
    ImGui::TextWrapped("%s", export_result.c_str());
  }

  ImGui::BeginDisabled(export_pending || export_source < 0 ||
                       export_folder.empty());
  if (ImGui::Button("Export", ImVec2(font * 5.f, 0))) {
    export_pending = true;
    export_result = "Exporting...";
  }
  ImGui::EndDisabled();
  ImGui::SameLine();
  ImGui::BeginDisabled(export_pending);
  if (ImGui::Button("Close", ImVec2(font * 5.f, 0)) ||
      ImGui::Shortcut(ImGuiKey_Escape)) {
    ImGui::CloseCurrentPopup();
  }
  ImGui::EndDisabled();
  ImGui::EndPopup();
}

void *
FSSApplication::settings_read_open(ImGuiContext * /*ctx*/,
                                   ImGuiSettingsHandler *handler,
                                   const char *name) {
  return (strcmp(name, "Settings") == 0) ? handler->UserData : nullptr;
}

void
FSSApplication::settings_read_line(ImGuiContext * /*ctx*/,
                                   ImGuiSettingsHandler * /*handler*/,
                                   void *entry, const char *line) {
  FSSApplication *app = static_cast<FSSApplication*>(entry);
  const char *eq = strchr(line, '=');
  if (eq == nullptr) {
    return;
  }
  std::string key(line, eq);
  std::string value(eq + 1);

  for (int type = 0; type < FSSDataModel::DataSourceTypeCount; type++) {
    if (key == path_keys[type]) {
      app->source_paths[type] = value;
    } else if (key == show_keys[type]) {
      app->show_view[type] = (value == "1");
    }
  }

  if (key == "show_resources") {
    app->show_resources = (value == "1");
  } else if (key == "sprite_color") {
    int r, g, b;
    if (sscanf(value.c_str(), "%d,%d,%d", &r, &g, &b) == 3) {
      app->options.sprite_color.red = static_cast<unsigned char>(r);
      app->options.sprite_color.green = static_cast<unsigned char>(g);
      app->options.sprite_color.blue = static_cast<unsigned char>(b);
    }
  } else if (key == "zoom") {
    app->options.zoom = std::clamp(atoi(value.c_str()), 1, 8);
  } else if (key == "export_folder") {
    app->export_folder = value;
  } else if (key == "export_name") {
    app->export_name = value;
  } else if (key == "export_scale") {
    app->export_scale = std::clamp(atoi(value.c_str()), 1, 999);
  }
}

void
FSSApplication::settings_write_all(ImGuiContext * /*ctx*/,
                                   ImGuiSettingsHandler *handler,
                                   ImGuiTextBuffer *buf) {
  FSSApplication *app = static_cast<FSSApplication*>(handler->UserData);
  buf->appendf("[%s][Settings]\n", handler->TypeName);
  for (int type = 0; type < FSSDataModel::DataSourceTypeCount; type++) {
    buf->appendf("%s=%s\n", path_keys[type], app->source_paths[type].c_str());
    buf->appendf("%s=%d\n", show_keys[type], app->show_view[type] ? 1 : 0);
  }
  buf->appendf("show_resources=%d\n", app->show_resources ? 1 : 0);
  buf->appendf("sprite_color=%d,%d,%d\n", app->options.sprite_color.red,
               app->options.sprite_color.green,
               app->options.sprite_color.blue);
  buf->appendf("zoom=%d\n", app->options.zoom);
  buf->appendf("export_folder=%s\n", app->export_folder.c_str());
  buf->appendf("export_name=%s\n", app->export_name.c_str());
  buf->appendf("export_scale=%d\n", app->export_scale);
  buf->append("\n");
}
