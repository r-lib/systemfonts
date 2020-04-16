#include <string>

#include <R.h>
#include <Rinternals.h>
#include <R_ext/GraphicsEngine.h>

#include "systemfonts.h"
#include "utils.h"
#include "FontDescriptor.h"

// these functions are implemented by the platform
ResultSet *getAvailableFonts();
ResultSet *findFonts(FontDescriptor *);
FontDescriptor *findFont(FontDescriptor *);
FontDescriptor *substituteFont(char *, char *);
void resetFontCache();

// Default fonts based on browser behaviour
#if defined _WIN32
#define SANS "Arial"
#define SERIF "Times New Roman"
#define MONO "Courier New"
#define EMOJI "Segoe UI Emoji"
#elif defined __APPLE__
#define SANS "Helvetica"
#define SERIF "Times"
#define MONO "Courier"
#define EMOJI "Apple Color Emoji"
#else
#define SANS "sans"
#define SERIF "serif"
#define MONO "mono"
#define EMOJI "emoji"
#endif

// Access font location cache
FontMap& get_font_map();

bool locate_in_registry(const char *family, int italic, int bold, FontLoc& res) {
  FontReg& registry = get_font_registry();
  if (registry.empty()) return false;
  auto search = registry.find(std::string(family));
  if (search == registry.end()) {
    return false;
  }
  int index = bold ? (italic ? 3 : 1) : (italic ? 2 : 0);
  res.first = search->second[index].first;
  res.second = search->second[index].second;
  return true;
}

int locate_font(const char *family, int italic, int bold, char *path, int max_path_length) {
  FontLoc registry_match;
  if (locate_in_registry(family, italic, bold, registry_match)) {
    strncpy(path, registry_match.first.c_str(), max_path_length);
    return registry_match.second;
  }
  
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
  FontKey key = std::make_tuple(std::string((char *) resolved_family), bold, italic);
  FontMap::iterator font_it = font_map.find(key);
  if (font_it != font_map.end()) {
    strncpy(path, font_it->second.first.c_str(), max_path_length);
    return font_it->second.second;
  }
  
  FontDescriptor font_desc(resolved_family, italic, bold);
  FontDescriptor* font_loc = findFont(&font_desc);
  
  int index;
  
  if (font_loc == NULL) {
    SEXP fallback_call = PROTECT(Rf_lang1(Rf_install("get_fallback")));
    SEXP fallback = PROTECT(Rf_eval(fallback_call, sf_ns_env));
    SEXP fallback_path = VECTOR_ELT(fallback, 0);
    strncpy(path, CHAR(STRING_ELT(fallback_path, 0)), max_path_length);
    index = INTEGER(VECTOR_ELT(fallback, 1))[0];
    UNPROTECT(2);
  } else {
    strncpy(path, font_loc->path, max_path_length);
    index = font_loc->index;
  }
  
  font_map[key] = {std::string(path), index};
  
  delete font_loc;
  return index;
}

SEXP match_font(SEXP family, SEXP italic, SEXP bold) {
  char *path = new char[PATH_MAX+1];
  path[PATH_MAX] = '\0';
  int index = locate_font(Rf_translateCharUTF8(STRING_ELT(family, 0)),
                          LOGICAL(italic)[0], LOGICAL(bold)[0], path, PATH_MAX);

  SEXP font = PROTECT(Rf_allocVector(VECSXP, 2));
  SET_VECTOR_ELT(font, 0, Rf_mkString(path));
  SET_VECTOR_ELT(font, 1, Rf_ScalarInteger(index));
  SEXP names = PROTECT(Rf_allocVector(STRSXP, 2));
  SET_STRING_ELT(names, 0, Rf_mkChar("path"));
  SET_STRING_ELT(names, 1, Rf_mkChar("index"));
  Rf_setAttrib(font, Rf_install("names"), names);
  delete[] path;
  UNPROTECT(2);
  return font;
}

SEXP system_fonts() {
  SEXP res = PROTECT(Rf_allocVector(VECSXP, 9));

  SEXP cl = PROTECT(Rf_allocVector(STRSXP, 3));
  SET_STRING_ELT(cl, 0, Rf_mkChar("tbl_df"));
  SET_STRING_ELT(cl, 1, Rf_mkChar("tbl"));
  SET_STRING_ELT(cl, 2, Rf_mkChar("data.frame"));
  Rf_classgets(res, cl);

  SEXP names = PROTECT(Rf_allocVector(STRSXP, 9));
  SET_STRING_ELT(names, 0, Rf_mkChar("path"));
  SET_STRING_ELT(names, 1, Rf_mkChar("index"));
  SET_STRING_ELT(names, 2, Rf_mkChar("name"));
  SET_STRING_ELT(names, 3, Rf_mkChar("family"));
  SET_STRING_ELT(names, 4, Rf_mkChar("style"));
  SET_STRING_ELT(names, 5, Rf_mkChar("weight"));
  SET_STRING_ELT(names, 6, Rf_mkChar("width"));
  SET_STRING_ELT(names, 7, Rf_mkChar("italic"));
  SET_STRING_ELT(names, 8, Rf_mkChar("monospace"));
  setAttrib(res, Rf_install("names"), names);

  ResultSet* all_fonts = getAvailableFonts();
  int n = all_fonts->n_fonts();
  SEXP path = PROTECT(Rf_allocVector(STRSXP, n));
  SEXP index = PROTECT(Rf_allocVector(INTSXP, n));
  SEXP name = PROTECT(Rf_allocVector(STRSXP, n));
  SEXP family = PROTECT(Rf_allocVector(STRSXP, n));
  SEXP style = PROTECT(Rf_allocVector(STRSXP, n));

  SEXP fct_cl = PROTECT(Rf_allocVector(STRSXP, 2));
  SET_STRING_ELT(fct_cl, 0, Rf_mkChar("ordered"));
  SET_STRING_ELT(fct_cl, 1, Rf_mkChar("factor"));

  SEXP weight = PROTECT(Rf_allocVector(INTSXP, n));
  SEXP weight_lvl = PROTECT(Rf_allocVector(STRSXP, 9));
  SET_STRING_ELT(weight_lvl, 0, Rf_mkChar("thin"));
  SET_STRING_ELT(weight_lvl, 1, Rf_mkChar("ultralight"));
  SET_STRING_ELT(weight_lvl, 2, Rf_mkChar("light"));
  SET_STRING_ELT(weight_lvl, 3, Rf_mkChar("normal"));
  SET_STRING_ELT(weight_lvl, 4, Rf_mkChar("medium"));
  SET_STRING_ELT(weight_lvl, 5, Rf_mkChar("semibold"));
  SET_STRING_ELT(weight_lvl, 6, Rf_mkChar("bold"));
  SET_STRING_ELT(weight_lvl, 7, Rf_mkChar("ultrabold"));
  SET_STRING_ELT(weight_lvl, 8, Rf_mkChar("heavy"));
  Rf_classgets(weight, fct_cl);
  Rf_setAttrib(weight, Rf_install("levels"), weight_lvl);

  SEXP width = PROTECT(Rf_allocVector(INTSXP, n));
  SEXP width_lvl = PROTECT(Rf_allocVector(STRSXP, 9));
  SET_STRING_ELT(width_lvl, 0, Rf_mkChar("ultracondensed"));
  SET_STRING_ELT(width_lvl, 1, Rf_mkChar("extracondensed"));
  SET_STRING_ELT(width_lvl, 2, Rf_mkChar("condensed"));
  SET_STRING_ELT(width_lvl, 3, Rf_mkChar("semicondensed"));
  SET_STRING_ELT(width_lvl, 4, Rf_mkChar("normal"));
  SET_STRING_ELT(width_lvl, 5, Rf_mkChar("semiexpanded"));
  SET_STRING_ELT(width_lvl, 6, Rf_mkChar("expanded"));
  SET_STRING_ELT(width_lvl, 7, Rf_mkChar("extraexpanded"));
  SET_STRING_ELT(width_lvl, 8, Rf_mkChar("ultraexpanded"));
  Rf_classgets(width, fct_cl);
  Rf_setAttrib(width, Rf_install("levels"), width_lvl);

  SEXP italic = PROTECT(Rf_allocVector(LGLSXP, n));
  SEXP monospace = PROTECT(Rf_allocVector(LGLSXP, n));
  SET_VECTOR_ELT(res, 0, path);
  SET_VECTOR_ELT(res, 1, index);
  SET_VECTOR_ELT(res, 2, name);
  SET_VECTOR_ELT(res, 3, family);
  SET_VECTOR_ELT(res, 4, style);
  SET_VECTOR_ELT(res, 5, weight);
  SET_VECTOR_ELT(res, 6, width);
  SET_VECTOR_ELT(res, 7, italic);
  SET_VECTOR_ELT(res, 8, monospace);

  int i = 0;
  for (ResultSet::iterator it = all_fonts->begin(); it != all_fonts->end(); it++) {
    SET_STRING_ELT(path, i, Rf_mkChar((*it)->get_path()));
    INTEGER(index)[i] = (*it)->index;
    SET_STRING_ELT(name, i, Rf_mkChar((*it)->get_psname()));
    SET_STRING_ELT(family, i, Rf_mkChar((*it)->get_family()));
    SET_STRING_ELT(style, i, Rf_mkChar((*it)->get_style()));
    INTEGER(weight)[i] = (*it)->get_weight();
    if (INTEGER(weight)[i] == 0) {
      INTEGER(weight)[i] = NA_INTEGER;
    }
    INTEGER(width)[i] = (*it)->get_width();
    if (INTEGER(width)[i] == 0) {
      INTEGER(width)[i] = NA_INTEGER;
    }
    LOGICAL(italic)[i] = (int) (*it)->italic;
    LOGICAL(monospace)[i] = (int) (*it)->monospace;
    ++i;
  }
  
  delete all_fonts;

  SEXP row_names = PROTECT(Rf_allocVector(REALSXP, 2));
  REAL(row_names)[0] = NA_REAL;
  REAL(row_names)[1] = -n;
  setAttrib(res, Rf_install("row.names"), row_names);

  UNPROTECT(16);
  return res;
}

SEXP reset_font_cache() {
  resetFontCache();
  FontMap& font_map = get_font_map();
  font_map.clear();
  return R_NilValue;
}

SEXP dev_string_widths(SEXP strings, SEXP family, SEXP face, SEXP size, SEXP cex, SEXP unit) {
  GEUnit u = GE_INCHES;
  switch (INTEGER(unit)[0]) {
  case 0:
    u = GE_CM;
    break;
  case 1:
    u = GE_INCHES;
    break;
  case 2:
    u = GE_DEVICE;
    break;
  case 3:
    u = GE_NDC;
    break;
  }
  pGEDevDesc dev = GEcurrentDevice();
  R_GE_gcontext gc;
  double width;
  int n_total = LENGTH(strings);
  int scalar_family = LENGTH(family) == 1;
  int scalar_rest = LENGTH(face) == 1;
  strcpy(gc.fontfamily, Rf_translateCharUTF8(STRING_ELT(family, 0)));
  gc.fontface = INTEGER(face)[0];
  gc.ps = REAL(size)[0];
  gc.cex = REAL(cex)[0];
  SEXP res = PROTECT(allocVector(REALSXP, n_total));
  
  for (int i = 0; i < n_total; ++i) {
    if (i > 0 && !scalar_family) {
      strcpy(gc.fontfamily, Rf_translateCharUTF8(STRING_ELT(family, i)));
    }
    if (i > 0 && !scalar_rest) {
      gc.fontface = INTEGER(face)[i];
      gc.ps = REAL(size)[i];
      gc.cex = REAL(cex)[i];
    }
    width = GEStrWidth(
      CHAR(STRING_ELT(strings, i)), 
      getCharCE(STRING_ELT(strings, i)), 
      &gc, 
      dev
    );
    REAL(res)[i] = GEfromDeviceWidth(width, u, dev);
  }
  
  UNPROTECT(1);
  return res;
}

SEXP dev_string_metrics(SEXP strings, SEXP family, SEXP face, SEXP size, SEXP cex, SEXP unit) {
  GEUnit u = GE_INCHES;
  switch (INTEGER(unit)[0]) {
  case 0:
    u = GE_CM;
    break;
  case 1:
    u = GE_INCHES;
    break;
  case 2:
    u = GE_DEVICE;
    break;
  case 3:
    u = GE_NDC;
    break;
  }
  pGEDevDesc dev = GEcurrentDevice();
  R_GE_gcontext gc;
  double width, ascent, descent;
  int n_total = LENGTH(strings);
  int scalar_family = LENGTH(family) == 1;
  int scalar_rest = LENGTH(face) == 1;
  strcpy(gc.fontfamily, Rf_translateCharUTF8(STRING_ELT(family, 0)));
  gc.fontface = INTEGER(face)[0];
  gc.ps = REAL(size)[0];
  gc.cex = REAL(cex)[0];
  SEXP w = PROTECT(allocVector(REALSXP, n_total));
  SEXP a = PROTECT(allocVector(REALSXP, n_total));
  SEXP d = PROTECT(allocVector(REALSXP, n_total));
  
  for (int i = 0; i < n_total; ++i) {
    if (i > 0 && !scalar_family) {
      strcpy(gc.fontfamily, Rf_translateCharUTF8(STRING_ELT(family, i)));
    }
    if (i > 0 && !scalar_rest) {
      gc.fontface = INTEGER(face)[i];
      gc.ps = REAL(size)[i];
      gc.cex = REAL(cex)[i];
    }
    GEStrMetric(
      CHAR(STRING_ELT(strings, i)), 
      getCharCE(STRING_ELT(strings, i)), 
      &gc,
      &ascent, &descent, &width,
      dev
    );
    REAL(w)[i] = GEfromDeviceWidth(width, u, dev);
    REAL(a)[i] = GEfromDeviceWidth(ascent, u, dev);
    REAL(d)[i] = GEfromDeviceWidth(descent, u, dev);
  }
  
  SEXP res = PROTECT(allocVector(VECSXP, 3));
  SET_VECTOR_ELT(res, 0, w);
  SET_VECTOR_ELT(res, 1, a);
  SET_VECTOR_ELT(res, 2, d);
  
  SEXP row_names = PROTECT(Rf_allocVector(REALSXP, 2));
  REAL(row_names)[0] = NA_REAL;
  REAL(row_names)[1] = -n_total;
  setAttrib(res, Rf_install("row.names"), row_names);
  
  SEXP names = PROTECT(Rf_allocVector(STRSXP, 3));
  SET_STRING_ELT(names, 0, Rf_mkChar("width"));
  SET_STRING_ELT(names, 1, Rf_mkChar("ascent"));
  SET_STRING_ELT(names, 2, Rf_mkChar("descent"));
  setAttrib(res, Rf_install("names"), names);
  
  SEXP cl = PROTECT(Rf_allocVector(STRSXP, 3));
  SET_STRING_ELT(cl, 0, Rf_mkChar("tbl_df"));
  SET_STRING_ELT(cl, 1, Rf_mkChar("tbl"));
  SET_STRING_ELT(cl, 2, Rf_mkChar("data.frame"));
  Rf_classgets(res, cl);
  
  UNPROTECT(7);
  return res;
}

SEXP register_font(SEXP family, SEXP paths, SEXP indices) {
  FontReg& registry = get_font_registry();
  std::string name = Rf_translateCharUTF8(STRING_ELT(family, 0));
  FontCollection col;
  for (int i = 0; i < LENGTH(paths); ++i) {
    std::string font_path = Rf_translateCharUTF8(STRING_ELT(paths, i));
    FontLoc font(font_path, INTEGER(indices)[i]);
    col.push_back(font);
  }
  registry[name] = col;
  
  FontMap& font_map = get_font_map();
  font_map.clear();
  
  return R_NilValue;
}

SEXP clear_registry() {
  FontReg& registry = get_font_registry();
  registry.clear();
  FontMap& font_map = get_font_map();
  font_map.clear();
  
  return R_NilValue;
}

SEXP registry_fonts() {
  FontReg& registry = get_font_registry();
  int n_reg = registry.size();
  int n = n_reg * 4;
  
  SEXP res = PROTECT(Rf_allocVector(VECSXP, 6));
  
  SEXP cl = PROTECT(Rf_allocVector(STRSXP, 3));
  SET_STRING_ELT(cl, 0, Rf_mkChar("tbl_df"));
  SET_STRING_ELT(cl, 1, Rf_mkChar("tbl"));
  SET_STRING_ELT(cl, 2, Rf_mkChar("data.frame"));
  Rf_classgets(res, cl);
  
  SEXP names = PROTECT(Rf_allocVector(STRSXP, 6));
  SET_STRING_ELT(names, 0, Rf_mkChar("path"));
  SET_STRING_ELT(names, 1, Rf_mkChar("index"));
  SET_STRING_ELT(names, 2, Rf_mkChar("family"));
  SET_STRING_ELT(names, 3, Rf_mkChar("style"));
  SET_STRING_ELT(names, 4, Rf_mkChar("weight"));
  SET_STRING_ELT(names, 5, Rf_mkChar("italic"));
  setAttrib(res, Rf_install("names"), names);
  
  SEXP path = PROTECT(Rf_allocVector(STRSXP, n));
  SEXP index = PROTECT(Rf_allocVector(INTSXP, n));
  SEXP family = PROTECT(Rf_allocVector(STRSXP, n));
  SEXP style = PROTECT(Rf_allocVector(STRSXP, n));
  
  SEXP fct_cl = PROTECT(Rf_allocVector(STRSXP, 2));
  SET_STRING_ELT(fct_cl, 0, Rf_mkChar("ordered"));
  SET_STRING_ELT(fct_cl, 1, Rf_mkChar("factor"));
  
  SEXP weight = PROTECT(Rf_allocVector(INTSXP, n));
  SEXP weight_lvl = PROTECT(Rf_allocVector(STRSXP, 2));
  SET_STRING_ELT(weight_lvl, 0, Rf_mkChar("normal"));
  SET_STRING_ELT(weight_lvl, 1, Rf_mkChar("bold"));
  Rf_classgets(weight, fct_cl);
  Rf_setAttrib(weight, Rf_install("levels"), weight_lvl);
  
  SEXP italic = PROTECT(Rf_allocVector(LGLSXP, n));
  
  SET_VECTOR_ELT(res, 0, path);
  SET_VECTOR_ELT(res, 1, index);
  SET_VECTOR_ELT(res, 2, family);
  SET_VECTOR_ELT(res, 3, style);
  SET_VECTOR_ELT(res, 4, weight);
  SET_VECTOR_ELT(res, 5, italic);
  
  int i = 0;
  for (auto it = registry.begin(); it != registry.end(); ++it) {
    for (int j = 0; j < 4; j++) {
      SET_STRING_ELT(path, i, Rf_mkChar(it->second[j].first.c_str()));
      INTEGER(index)[i] = it->second[j].second;
      SET_STRING_ELT(family, i, Rf_mkChar(it->first.c_str()));
      switch (j) {
      case 0: 
        SET_STRING_ELT(style, i, Rf_mkChar("Regular"));
        break;
      case 1:
        SET_STRING_ELT(style, i, Rf_mkChar("Bold"));
        break;
      case 2:
        SET_STRING_ELT(style, i, Rf_mkChar("Italic"));
        break;
      case 3:
        SET_STRING_ELT(style, i, Rf_mkChar("Bold Italic"));
        break;
      }
      INTEGER(weight)[i] = 1 + (int) (j == 1 || j == 3);
      INTEGER(italic)[i] = (int) (j > 1);
      ++i;
    }
  }
  
  SEXP row_names = PROTECT(Rf_allocVector(REALSXP, 2));
  REAL(row_names)[0] = NA_REAL;
  REAL(row_names)[1] = -n;
  setAttrib(res, Rf_install("row.names"), row_names);
  
  UNPROTECT(12);
  return res;
}

SEXP sf_ns_env = NULL;
SEXP sf_init(SEXP ns) {
  sf_ns_env = ns;
  return R_NilValue;
}
