#include "font_local.h"
#include "Rinternals.h"
#include "caches.h"

#include <cpp11/strings.hpp>
#include <string>
#include <set>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_TRUETYPE_TABLES_H

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

inline FontWeight get_font_weight(FT_Face face) {
  void* table = FT_Get_Sfnt_Table(face, FT_SFNT_OS2);
  if (table == NULL) {
    return FontWeightUndefined;
  }
  TT_OS2* os2_table = (TT_OS2*) table;
  return (FontWeight) os2_table->usWeightClass;
}

inline FontWidth get_font_width(FT_Face face) {
  void* table = FT_Get_Sfnt_Table(face, FT_SFNT_OS2);
  if (table == NULL) {
    return FontWidthUndefined;
  }
  TT_OS2* os2_table = (TT_OS2*) table;
  return (FontWidth) os2_table->usWidthClass;
}

inline FontDescriptor* descriptor_from_face(FT_Face &face, const char* path, int index) {
  FontDescriptor* res = NULL;

  res = new FontDescriptor(
    path,
    index,
    FT_Get_Postscript_Name(face) == NULL ? "" : FT_Get_Postscript_Name(face),
    face->family_name,
    face->style_name,
    get_font_weight(face),
    get_font_width(face),
    face->style_flags & FT_STYLE_FLAG_ITALIC,
    FT_IS_FIXED_WIDTH(face)
  );
  return res;
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
    font_list.push_back(descriptor_from_face(face, path.c_str(), 0));
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
      font_list.push_back(descriptor_from_face(face, path.c_str(), i));
      FT_Done_Face(face);
    }
  }

  FT_Done_FreeType(library);

  return 0;
}
