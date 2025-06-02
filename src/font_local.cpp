#include "font_local.h"
#include "FontDescriptor.h"
#include "Rinternals.h"
#include "caches.h"
#include "ft_cache.h"

#include <cpp11/strings.hpp>
#include <string>
#include <set>

FontDescriptor *find_first_match(FontDescriptor *desc, ResultSet& font_list) {
  for (ResultSet::iterator it = font_list.begin(); it != font_list.end(); it++) {
    if ((*desc) == (**it)) {
      FontDescriptor* font = new FontDescriptor(*it);
      return font;
    }
  }
  return NULL;
}

FontDescriptor *match_local_fonts(FontDescriptor *desc) {
  FontDescriptor *font = find_first_match(desc, get_local_font_list());

  // if we didn't find anything, try again with postscriptName as family
  if (!font) {

    const char* tmp_psn = desc->postscriptName;
    desc->postscriptName = desc->family;
    desc->family = NULL;

    font = find_first_match(desc, get_local_font_list());

    desc->family = desc->postscriptName;
    desc->postscriptName = tmp_psn;
  }

  // might be NULL but that is ok
  return font;
}

int add_local_fonts(cpp11::strings paths) {
  ResultSet& font_list = get_local_font_list();

  std::set<std::string> current_files;
  for (size_t i = 0; i < font_list.size(); ++i) {
    current_files.insert(std::string(font_list[i]->get_path()));
  }

  FreetypeCache& cache = get_font_cache();

  for (R_xlen_t i = 0; i < paths.size(); ++i) {
    std::string path(paths[i]);
    if (current_files.find(path) != current_files.end()) {
      continue;
    }
    bool success = cache.load_font(path.c_str(), 0);

    if (!success) {
      continue;
    }

    font_list.push_back(new FontDescriptor(cache.get_face(), path.c_str(), 0, cache.n_axes() != 0));
    int n_fonts = cache.get_face()->num_faces;
    for (int i = 1; i < n_fonts; ++i) {
      success = cache.load_font(path.c_str(), i);
      if (!success) {
        continue;
      }
      font_list.push_back(new FontDescriptor(cache.get_face(), path.c_str(), i, cache.n_axes() != 0));
    }
  }

  FontMap& font_map = get_font_map();
  font_map.clear();

  return 0;
}

void clear_local_fonts_c() {
  ResultSet& font_list = get_local_font_list();
  font_list.clear();
  FontMap& font_map = get_font_map();
  font_map.clear();
}
