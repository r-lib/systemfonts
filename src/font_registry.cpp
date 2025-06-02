#include "font_registry.h"
#include "caches.h"

#include <cstring>

#include <cpp11/logicals.hpp>
#include <cpp11/doubles.hpp>
#include <cpp11/list.hpp>
#include <cpp11/r_string.hpp>
#include <cpp11/named_arg.hpp>

using list_t = cpp11::list;
using list_w = cpp11::writable::list;
using data_frame_w = cpp11::writable::data_frame;
using strings_t = cpp11::strings;
using strings_w = cpp11::writable::strings;
using integers_t = cpp11::integers;
using integers_w = cpp11::writable::integers;
using logicals_t = cpp11::logicals;
using logicals_w = cpp11::writable::logicals;

using namespace cpp11::literals;

void register_font_c(strings_t family, strings_t paths, integers_t indices, strings_t features, integers_t settings) {
  FontReg& registry = get_font_registry();
  std::string name(family[0]);
  FontCollection col = {};
  for (int i = 0; i < features.size(); ++i) {
    const char* f = Rf_translateCharUTF8(features[i]);
    col.features.push_back({{f[0], f[1], f[2], f[3]}, settings[i]});
  }
  for (int i = 0; i < Rf_length(paths); ++i) {
    if (i > 3) continue;
    col.fonts[i] = {paths[i], (unsigned int) indices[i]};
  }
  registry[name] = col;

  FontMap& font_map = get_font_map();
  font_map.clear();
}

void clear_registry_c() {
  FontReg& registry = get_font_registry();
  registry.clear();
  FontMap& font_map = get_font_map();
  font_map.clear();
}

data_frame_w registry_fonts_c() {
  FontReg& registry = get_font_registry();
  int n_reg = registry.size();

  int n = n_reg * 4;

  strings_w path(n);
  integers_w index(n);
  strings_w family(n);
  strings_w style(n);
  integers_w weight(n);
  weight.attr("class") = {"ordered", "factor"};
  weight.attr("levels") = {"normal", "bold"};
  logicals_w italic(n);
  list_w features(n);

  int i = 0;
  for (auto it = registry.begin(); it != registry.end(); ++it) {
    for (int j = 0; j < 4; j++) {
      path[i] = it->second.fonts[j].file;
      index[i] = it->second.fonts[j].index;
      family[i] = it->first;
      switch (j) {
      case 0:
        style[i] = "Regular";
        break;
      case 1:
        style[i] = "Bold";
        break;
      case 2:
        style[i] = "Italic";
        break;
      case 3:
        style[i] = "Bold Italic";
        break;
      }
      weight[i] = 1 + (int) (j == 1 || j == 3);
      italic[i] = (Rboolean) (j > 1);
      if (it->second.features.empty()) {
        features[i] = integers_w();
      } else {
        int n_features = it->second.features.size();
        integers_w feat(n_features);
        strings_w tag(n_features);
        for (int k = 0; k < n_features; ++k) {
          feat[k] = it->second.features[k].setting;
          tag[k] = cpp11::r_string({
            it->second.features[k].feature[0],
            it->second.features[k].feature[1],
            it->second.features[k].feature[2],
            it->second.features[k].feature[3]
          });
        }
        feat.names() = tag;
        features[i] = feat;
      }
      ++i;
    }
  }

  data_frame_w res({
    "path"_nm = path,
    "index"_nm = index,
    "family"_nm = family,
    "style"_nm = style,
    "weight"_nm = weight,
    "italic"_nm = italic,
    "features"_nm = features
  });
  res.attr("class") = {"tbl_df", "tbl", "data.frame"};
  return res;
}


bool locate_in_registry(const char *family, int italic, int bold, FontSettings& res) {
  FontReg& registry = get_font_registry();
  if (registry.empty()) return false;
  auto search = registry.find(std::string(family));
  if (search == registry.end()) {
    return false;
  }
  int index = bold ? (italic ? 3 : 1) : (italic ? 2 : 0);
  strncpy(res.file, search->second.fonts[index].file.c_str(), PATH_MAX);
  res.file[PATH_MAX] = '\0';
  res.index = search->second.fonts[index].index;
  res.features = search->second.features.data();
  res.n_features = search->second.features.size();
  return true;
}
