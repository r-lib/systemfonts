#include <R.h>
#include <Rinternals.h>

#include "string_metrics.h"
#include "string_shape.h"

SEXP get_string_shape(SEXP string, SEXP path, SEXP index, SEXP size, SEXP res) {
  int n_strings = LENGTH(string);
  bool one_path = LENGTH(path) == 1;
  const char* first_path = Rf_translateCharUTF8(STRING_ELT(path, 0));
  int first_index = INTEGER(index)[0];
  bool one_size = LENGTH(size) == 1;
  double first_size = REAL(size)[0];
  bool one_res = LENGTH(res) == 1;
  double first_res = REAL(res)[0];
  FreetypeShaper shaper;
  
  // Return list
  SEXP ret = PROTECT(Rf_allocVector(VECSXP, 2));
  SEXP names = PROTECT(Rf_allocVector(STRSXP, 2));
  SET_STRING_ELT(names, 0, Rf_mkChar("shape"));
  SET_STRING_ELT(names, 1, Rf_mkChar("metrics"));
  setAttrib(ret, Rf_install("names"), names);
  
  // First element - shape df
  SEXP info_df = SET_VECTOR_ELT(ret, 0, Rf_allocVector(VECSXP, 5));
  SEXP names_gl = PROTECT(Rf_allocVector(STRSXP, 5));
  SET_STRING_ELT(names_gl, 0, Rf_mkChar("glyph"));
  SET_STRING_ELT(names_gl, 1, Rf_mkChar("index"));
  SET_STRING_ELT(names_gl, 2, Rf_mkChar("string_id"));
  SET_STRING_ELT(names_gl, 3, Rf_mkChar("x_offset"));
  SET_STRING_ELT(names_gl, 4, Rf_mkChar("y_offset"));
  setAttrib(info_df, Rf_install("names"), names_gl);
  
  SEXP glyph = SET_VECTOR_ELT(info_df, 0, Rf_allocVector(INTSXP, 0));
  SEXP glyph_id = SET_VECTOR_ELT(info_df, 1, Rf_allocVector(INTSXP, 0));
  SEXP string_id = SET_VECTOR_ELT(info_df, 2, Rf_allocVector(INTSXP, 0));
  SEXP x_offset = SET_VECTOR_ELT(info_df, 3, Rf_allocVector(REALSXP, 0));
  SEXP y_offset = SET_VECTOR_ELT(info_df, 4, Rf_allocVector(REALSXP, 0));
  
  // Second element - metric df
  SEXP string_df = SET_VECTOR_ELT(ret, 1, Rf_allocVector(VECSXP, 5));
  SEXP names_str = PROTECT(Rf_allocVector(STRSXP, 5));
  SET_STRING_ELT(names_str, 0, Rf_mkChar("string"));
  SET_STRING_ELT(names_str, 1, Rf_mkChar("width"));
  SET_STRING_ELT(names_str, 2, Rf_mkChar("height"));
  SET_STRING_ELT(names_str, 3, Rf_mkChar("left_bearing"));
  SET_STRING_ELT(names_str, 4, Rf_mkChar("right_bearing"));
  setAttrib(string_df, Rf_install("names"), names_str);
  
  SET_VECTOR_ELT(string_df, 0, string);
  SEXP widths = SET_VECTOR_ELT(string_df, 1, Rf_allocVector(REALSXP, n_strings));
  SEXP heights = SET_VECTOR_ELT(string_df, 2, Rf_allocVector(REALSXP, n_strings));
  SEXP left_bearings = SET_VECTOR_ELT(string_df, 3, Rf_allocVector(REALSXP, n_strings));
  SEXP right_bearings = SET_VECTOR_ELT(string_df, 4, Rf_allocVector(REALSXP, n_strings));
  
  // Make them data.frames
  SEXP cl = PROTECT(Rf_allocVector(STRSXP, 3));
  SET_STRING_ELT(cl, 0, Rf_mkChar("tbl_df"));
  SET_STRING_ELT(cl, 1, Rf_mkChar("tbl"));
  SET_STRING_ELT(cl, 2, Rf_mkChar("data.frame"));
  Rf_classgets(info_df, cl);
  Rf_classgets(string_df, cl);
  
  // Shape the text
  int it = 0;
  for (int i = 0; i < n_strings; i++) {
    SEXP this_string = STRING_ELT(string, i);
    if (Rf_StringBlank(this_string))  {
      REAL(widths)[i] = 0.0;
      REAL(heights)[i] = 0.0;
      REAL(left_bearings)[i] = 0.0;
      REAL(right_bearings)[i] = 0.0;
      continue;
    }
    shaper.shape_string(Rf_translateCharUTF8(this_string), 
                        one_path ? first_path : Rf_translateCharUTF8(STRING_ELT(path, i)), 
                        one_path ? first_index : INTEGER(index)[i], 
                        one_size ? first_size : REAL(size)[i], 
                        one_res ? first_res : REAL(res)[i]);
    int n_glyphs = shaper.glyph_id.size();
    int req_size = it + n_glyphs;
    int cur_size = Rf_xlength(glyph);
    if (cur_size >= req_size) {
      int new_size = cur_size * 2;
      new_size = new_size < req_size ? req_size : new_size;
      glyph = Rf_lengthgets(glyph, new_size);
      glyph_id = Rf_lengthgets(glyph_id, new_size);
      string_id = Rf_lengthgets(string_id, new_size);
      x_offset = Rf_lengthgets(x_offset, new_size);
      y_offset = Rf_lengthgets(y_offset, new_size);
    }
    for (int j = 0; j < n_glyphs; j++) {
      INTEGER(glyph)[it] = shaper.glyph_uc[j];
      INTEGER(glyph_id)[it] = shaper.glyph_id[j];
      INTEGER(string_id)[it] = i + 1;
      REAL(x_offset)[it] = shaper.x_pos[j] / 26.6;
      REAL(y_offset)[it] = shaper.y_pos[j] / 26.6;
      it++;
    }
    REAL(widths)[i] = shaper.width / 26.6;
    REAL(heights)[i] = shaper.height / 26.6;
    REAL(left_bearings)[i] = shaper.left_bearing / 26.6;
    REAL(right_bearings)[i] = shaper.right_bearing / 26.6;
  }
  
  // Make sure the length of the glyps columns match the number of glyphs
  if (Rf_xlength(glyph) != it) {
    glyph = Rf_xlengthgets(glyph, it);
    glyph_id = Rf_xlengthgets(glyph_id, it);
    string_id = Rf_xlengthgets(string_id, it);
    x_offset = Rf_xlengthgets(x_offset, it);
    y_offset = Rf_xlengthgets(y_offset, it);
  }
  
  // Add row.names (we didn't know final size before)
  SEXP row_names_gl = PROTECT(Rf_allocVector(REALSXP, 2));
  REAL(row_names_gl)[0] = NA_REAL;
  REAL(row_names_gl)[1] = -it;
  setAttrib(info_df, Rf_install("row.names"), row_names_gl);
  
  SEXP row_names_str = PROTECT(Rf_allocVector(REALSXP, 2));
  REAL(row_names_str)[0] = NA_REAL;
  REAL(row_names_str)[1] = -n_strings;
  setAttrib(string_df, Rf_install("row.names"), row_names_str);
  
  UNPROTECT(7);
  return ret;
}
