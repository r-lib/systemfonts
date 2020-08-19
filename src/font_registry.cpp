#include "font_registry.h"
#include "caches.h"

#include <cpp11/logicals.hpp>
#include <cpp11/named_arg.hpp>

using namespace cpp11;
namespace writable = cpp11::writable;

void register_font_c(strings family, strings paths, integers indices, strings features, integers settings) {
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

writable::data_frame registry_fonts_c() {
  FontReg& registry = get_font_registry();
  int n_reg = registry.size();
  
  int n = n_reg * 4;
  
  writable::strings path(n);
  writable::integers index(n);
  writable::strings family(n);
  writable::strings style(n);
  writable::integers weight(n);
  weight.attr("class") = {"ordered", "factor"};
  weight.attr("levels") = {"normal", "bold"};
  writable::logicals italic(n);
  
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
      ++i;
    }
  }
  
  writable::data_frame res({
    "path"_nm = path,
    "index"_nm = index,
    "family"_nm = family,
    "style"_nm = style,
    "weight"_nm = weight,
    "italic"_nm = italic
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
