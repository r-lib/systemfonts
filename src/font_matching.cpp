#include "font_matching.h"
#include "types.h"
#include "caches.h"
#include "utils.h"
#include "FontDescriptor.h"
#include "font_registry.h"

#include <string>
#include <memory>

#include <cpp11/function.hpp>
#include <cpp11/r_string.hpp>

namespace writable = cpp11::writable;
using namespace cpp11;

// these functions are implemented by the platform
ResultSet *getAvailableFonts();
ResultSet *findFonts(FontDescriptor *);
FontDescriptor *findFont(FontDescriptor *);
FontDescriptor *substituteFont(char *, char *);
void resetFontCache();

int locate_systemfont(const char *family, int italic, int bold, char *path, int max_path_length) {
  const char* resolved_family = family;
  if (strcmp_no_case(family, "") || strcmp_no_case(family, "sans")) {
    resolved_family = SANS;
  } else if (strcmp_no_case(family, "serif")) {
    resolved_family = SERIF;
  } else if (strcmp_no_case(family, "mono")) {
    resolved_family = MONO;
  } else if (strcmp_no_case(family, "emoji")) {
    resolved_family = EMOJI;
  }
  
  FontMap& font_map = get_font_map();
  static FontKey key;
  key.family.assign(resolved_family);
  key.bold = bold;
  key.italic = italic;
  FontMap::iterator font_it = font_map.find(key);
  if (font_it != font_map.end()) {
    strncpy(path, font_it->second.file.c_str(), max_path_length);
    return font_it->second.index;
  }
  
  FontDescriptor font_desc(resolved_family, italic, bold);
  std::unique_ptr<FontDescriptor> font_loc(findFont(&font_desc));
  
  int index = 0;
  
  if (!font_loc) {
    list fallback = as_cpp<list>(cpp11::package("systemfonts")["get_fallback"]());
    strncpy(path, CHAR(STRING_ELT(fallback[0], 0)), max_path_length);
    index = INTEGER(fallback[1])[0];
  } else {
    strncpy(path, font_loc->path, max_path_length);
    index = font_loc->index;
  }
  
  font_map[key] = {std::string(path), (unsigned int) index};
  
  return index;
}

int locate_font(const char *family, int italic, int bold, char *path, int max_path_length) {
  BEGIN_CPP
  
  FontSettings registry_match;
  if (locate_in_registry(family, italic, bold, registry_match)) {
    strncpy(path, registry_match.file, max_path_length);
    return registry_match.index;
  }
  
  return locate_systemfont(family, italic, bold, path, max_path_length);
  
  END_CPP
    
  return 0;
}

FontSettings locate_font_with_features(const char *family, int italic, int bold) {
  FontSettings registry_match;
  registry_match.n_features = 0;
  
  BEGIN_CPP
  
  if (locate_in_registry(family, italic, bold, registry_match)) {
    return registry_match;
  }
  registry_match.index = locate_systemfont(family, italic, bold, registry_match.file, PATH_MAX);
  
  END_CPP
    
  registry_match.file[PATH_MAX] = '\0';
  return registry_match;
}

list match_font_c(strings family, logicals italic, logicals bold) {
  char path[PATH_MAX+1];
  path[PATH_MAX] = '\0';
  int index = locate_font(
    Rf_translateCharUTF8(family[0]), italic[0], bold[0], path, PATH_MAX
  );
  
  return writable::list({
    "path"_nm = r_string(path),
    "index"_nm = index
  });
}

writable::data_frame system_fonts_c() {
  int n = 0;
  
  std::unique_ptr<ResultSet> all_fonts(getAvailableFonts());
  n = all_fonts->n_fonts();
  
  writable::strings path(n);
  writable::integers index(n);
  writable::strings name(n);
  writable::strings family(n);
  writable::strings style(n);
  writable::integers weight(n);
  weight.attr("class") = {"ordered", "factor"};
  weight.attr("levels") = {
    "thin",
    "ultralight",
    "light",
    "normal",
    "medium",
    "semibold",
    "bold",
    "ultrabold",
    "heavy"
  };
  writable::integers width(n);
  width.attr("class") = {"ordered", "factor"};
  width.attr("levels") = {
    "ultracondensed",
    "extracondensed",
    "condensed",
    "semicondensed",
    "normal",
    "semiexpanded",
    "expanded",
    "extraexpanded",
    "ultraexpanded"
  };
  writable::logicals italic(n);
  writable::logicals monospace(n);

  int i = 0;
  
  for (ResultSet::iterator it = all_fonts->begin(); it != all_fonts->end(); it++) {
    path[i] = (*it)->get_path();
    index[i] = (*it)->index;
    name[i] = (*it)->get_psname();
    family[i] = (*it)->get_family();
    style[i] = (*it)->get_style();
    weight[i] = (*it)->get_weight();
    if (weight[i] == 0) {
      weight[i] = NA_INTEGER;
    }
    width[i] = (*it)->get_width();
    if (width[i] == 0) {
      width[i] = NA_INTEGER;
    }
    italic[i] = (Rboolean) (*it)->italic;
    monospace[i] = (Rboolean) (*it)->monospace;
    ++i;
  }
  writable::data_frame res({
    "path"_nm = path,
    "index"_nm = index,
    "name"_nm = name,
    "family"_nm = family,
    "style"_nm = style,
    "weight"_nm = weight,
    "width"_nm = width,
    "italic"_nm = italic,
    "monospace"_nm = monospace
  });
  res.attr("class") = {"tbl_df", "tbl", "data.frame"};
  return res;
}

void reset_font_cache_c() {
  resetFontCache();
  FontMap& font_map = get_font_map();
  font_map.clear();
}

void export_font_matching(DllInfo* dll) {
  R_RegisterCCallable("systemfonts", "locate_font", (DL_FUNC)locate_font);
  R_RegisterCCallable("systemfonts", "locate_font_with_features", (DL_FUNC)locate_font_with_features);
}
