#include <R.h>
#include <Rinternals.h>

#include "systemfonts.h"
#include "utils.h"
#include "FontDescriptor.h"

// these functions are implemented by the platform
ResultSet *getAvailableFonts();
ResultSet *findFonts(FontDescriptor *);
FontDescriptor *findFont(FontDescriptor *);
FontDescriptor *substituteFont(char *, char *);

// Default fonts based on browser behaviour
#if defined _WIN32
#define SANS "Arial"
#define SERIF "Times New Roman"
#define MONO "Courier New"
#elif defined __APPLE__
#define SANS "Helvetica"
#define SERIF "Times"
#define MONO "Courier"
#else
#define SANS "sans"
#define SERIF "serif"
#define MONO "mono"
#endif

int locate_font(const char *family, int italic, int bold, char *path, int max_path_length) {
  const char* resolved_family = family;
  if (strcmp_no_case(family, "") || strcmp_no_case(family, "sans")) {
    resolved_family = SANS;
  } else if (strcmp_no_case(family, "serif")) {
    resolved_family = SERIF;
  } else if (strcmp_no_case(family, "mono")) {
    resolved_family = MONO;
  }
  FontDescriptor font_desc(resolved_family, italic, bold);
  FontDescriptor* font_loc = findFont(&font_desc);

  strncpy(path, font_loc->path, max_path_length);
  int index = font_loc->index;
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
  SET_STRING_ELT(fct_cl, 0, Rf_mkChar("factor"));

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
    SET_STRING_ELT(path, i, Rf_mkChar((*it)->path));
    INTEGER(index)[i] = (*it)->index;
    SET_STRING_ELT(name, i, Rf_mkChar((*it)->postscriptName));
    SET_STRING_ELT(family, i, Rf_mkChar((*it)->family));
    SET_STRING_ELT(style, i, Rf_mkChar((*it)->style));
    if ((*it)->weight == 0) {
      INTEGER(weight)[i] = NA_INTEGER;
    } else {
      INTEGER(weight)[i] = (*it)->weight / 100;
    }
    if ((*it)->width == 0) {
      INTEGER(width)[i] = NA_INTEGER;
    } else {
      INTEGER(width)[i] = (int) (*it)->width;
    }
    LOGICAL(italic)[i] = (int) (*it)->italic;
    LOGICAL(monospace)[i] = (int) (*it)->monospace;
    i++;
  }

  SEXP row_names = PROTECT(Rf_allocVector(REALSXP, 2));
  REAL(row_names)[0] = NA_REAL;
  REAL(row_names)[1] = -n;
  setAttrib(res, Rf_install("row.names"), row_names);

  UNPROTECT(16);
  return res;
}
