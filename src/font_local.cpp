#include "font_local.h"
#include "FontDescriptor.h"
#include "Rinternals.h"
#include "caches.h"

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

  FT_Library  library;
  FT_Face     face;
  FT_Error    error;
  error = FT_Init_FreeType(&library);
  if (error) {
    return 1;
  }

  for (R_xlen_t i = 0; i < paths.size(); ++i) {
    std::string path(paths[i]);
    if (current_files.find(path) != current_files.end()) {
      continue;
    }
    error = FT_New_Face(library,
                        path.c_str(),
                        0,
                        &face);
    if (error) {
      continue;
    }
    font_list.push_back(new FontDescriptor(face, path.c_str(), 0));
    int n_fonts = face->num_faces;
    FT_Done_Face(face);
    for (int i = 1; i < n_fonts; ++i) {
      error = FT_New_Face(library,
                          path.c_str(),
                          i,
                          &face);
      if (error) {
        continue;
      }
      font_list.push_back(new FontDescriptor(face, path.c_str(), i));
      FT_Done_Face(face);
    }
  }

  FT_Done_FreeType(library);

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
