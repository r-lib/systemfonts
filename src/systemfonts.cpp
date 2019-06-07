#include <R.h>
#include <Rinternals.h>

#include "systemfonts.h"
#include "FontDescriptor.h"

// these functions are implemented by the platform
ResultSet *getAvailableFonts();
ResultSet *findFonts(FontDescriptor *);
FontDescriptor *findFont(FontDescriptor *);
FontDescriptor *substituteFont(char *, char *);

void locate_font(const char *family, int italic, int bold, char *path, int max_path_length) {
  FontDescriptor font_desc(family, italic, bold);
  FontDescriptor* font_loc = findFont(&font_desc);

  strncpy(path, font_loc->path, max_path_length);
  delete font_loc;
}

SEXP match_font(SEXP family, SEXP italic, SEXP bold) {
  char *path = new char[PATH_MAX+1];
  locate_font(Rf_translateCharUTF8(STRING_ELT(family, 0)), LOGICAL(italic)[0],
              LOGICAL(bold)[0], path, PATH_MAX);

  SEXP path_r = PROTECT(Rf_mkString(path));
  delete[] path;
  UNPROTECT(1);
  return path_r;
}

SEXP system_fonts() {
  SEXP res = PROTECT(Rf_allocVector(VECSXP, 8));
  ResultSet* all_fonts = getAvailableFonts();
  int n = all_fonts->n_fonts();
  SEXP path = PROTECT(Rf_allocVector(STRSXP, n));
  SEXP name = PROTECT(Rf_allocVector(STRSXP, n));
  SEXP family = PROTECT(Rf_allocVector(STRSXP, n));
  SEXP style = PROTECT(Rf_allocVector(STRSXP, n));
  SEXP weight = PROTECT(Rf_allocVector(INTSXP, n));
  SEXP width = PROTECT(Rf_allocVector(INTSXP, n));
  SEXP italic = PROTECT(Rf_allocVector(LGLSXP, n));
  SEXP monospace = PROTECT(Rf_allocVector(LGLSXP, n));
  SET_VECTOR_ELT(res, 0, path);
  SET_VECTOR_ELT(res, 1, name);
  SET_VECTOR_ELT(res, 2, family);
  SET_VECTOR_ELT(res, 3, style);
  SET_VECTOR_ELT(res, 4, weight);
  SET_VECTOR_ELT(res, 5, width);
  SET_VECTOR_ELT(res, 6, italic);
  SET_VECTOR_ELT(res, 7, monospace);

  int i = 0;
  for (ResultSet::iterator it = all_fonts->begin(); it != all_fonts->end(); it++) {

    i++;
  }

  UNPROTECT(9);
  return res;
}
