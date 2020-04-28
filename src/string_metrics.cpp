#define R_NO_REMAP

#include <Rinternals.h>

#include "string_metrics.h"
#include "string_shape.h"
#include "utils.h"

SEXP get_string_shape(SEXP string, SEXP id, SEXP path, SEXP index, SEXP size, 
                      SEXP res, SEXP lineheight, SEXP align, SEXP hjust, 
                      SEXP vjust, SEXP width, SEXP tracking, SEXP indent, 
                      SEXP hanging, SEXP space_before, SEXP space_after) {
  int n_strings = LENGTH(string);
  bool one_path = LENGTH(path) == 1;
  const char* first_path = Rf_translateCharUTF8(STRING_ELT(path, 0));
  int first_index = INTEGER(index)[0];
  bool one_size = LENGTH(size) == 1;
  double first_size = REAL(size)[0];
  bool one_res = LENGTH(res) == 1;
  double first_res = REAL(res)[0];
  bool one_lht = LENGTH(lineheight) == 1;
  double first_lht = REAL(lineheight)[0];
  bool one_align = LENGTH(align) == 1;
  int first_align = INTEGER(align)[0];
  bool one_hjust = LENGTH(hjust) == 1;
  double first_hjust = REAL(hjust)[0];
  bool one_vjust = LENGTH(vjust) == 1;
  double first_vjust = REAL(vjust)[0];
  bool one_width = LENGTH(width) == 1;
  double first_width = REAL(width)[0] * 64;
  bool one_tracking = LENGTH(tracking) == 1;
  double first_tracking = REAL(tracking)[0];
  bool one_indent = LENGTH(indent) == 1;
  double first_indent = REAL(indent)[0] * 64;
  bool one_hanging = LENGTH(hanging) == 1;
  double first_hanging = REAL(hanging)[0] * 64;
  bool one_before = LENGTH(space_before) == 1;
  double first_before = REAL(space_before)[0] * 64;
  bool one_after = LENGTH(space_after) == 1;
  double first_after = REAL(space_after)[0] * 64;
  
  // Return list
  SEXP ret = PROTECT(Rf_allocVector(VECSXP, 2));
  SEXP names = PROTECT(Rf_allocVector(STRSXP, 2));
  SET_STRING_ELT(names, 0, Rf_mkChar("shape"));
  SET_STRING_ELT(names, 1, Rf_mkChar("metrics"));
  Rf_setAttrib(ret, Rf_install("names"), names);
  
  // First element - shape df
  SEXP info_df = SET_VECTOR_ELT(ret, 0, Rf_allocVector(VECSXP, 7));
  SEXP names_gl = PROTECT(Rf_allocVector(STRSXP, 7));
  SET_STRING_ELT(names_gl, 0, Rf_mkChar("glyph"));
  SET_STRING_ELT(names_gl, 1, Rf_mkChar("index"));
  SET_STRING_ELT(names_gl, 2, Rf_mkChar("metric_id"));
  SET_STRING_ELT(names_gl, 3, Rf_mkChar("string_id"));
  SET_STRING_ELT(names_gl, 4, Rf_mkChar("x_offset"));
  SET_STRING_ELT(names_gl, 5, Rf_mkChar("y_offset"));
  SET_STRING_ELT(names_gl, 6, Rf_mkChar("x_midpoint"));
  Rf_setAttrib(info_df, Rf_install("names"), names_gl);
  
  int cur_size = 0;
  SEXP glyph, glyph_id, metric_id, string_id, x_offset, y_offset, x_midpoint;
  PROTECT_INDEX pr_gl, pr_gli, pr_met, pr_str, pr_x, pr_y, pr_xm;
  PROTECT_WITH_INDEX(glyph = Rf_allocVector(INTSXP, cur_size), &pr_gl);
  PROTECT_WITH_INDEX(glyph_id = Rf_allocVector(INTSXP, cur_size), &pr_gli);
  PROTECT_WITH_INDEX(metric_id = Rf_allocVector(INTSXP, cur_size), &pr_met);
  PROTECT_WITH_INDEX(string_id = Rf_allocVector(INTSXP, cur_size), &pr_str);
  PROTECT_WITH_INDEX(x_offset = Rf_allocVector(REALSXP, cur_size), &pr_x);
  PROTECT_WITH_INDEX(y_offset = Rf_allocVector(REALSXP, cur_size), &pr_y);
  PROTECT_WITH_INDEX(x_midpoint = Rf_allocVector(REALSXP, cur_size), &pr_xm);
  int* glyph_p = INTEGER(glyph);
  int* glyph_id_p = INTEGER(glyph_id);
  int* metric_id_p = INTEGER(metric_id);
  int* string_id_p = INTEGER(string_id);
  double* x_offset_p = REAL(x_offset);
  double* y_offset_p = REAL(y_offset);
  double* x_midpoint_p = REAL(x_midpoint);
  
  // Second element - metric df
  SEXP string_df = SET_VECTOR_ELT(ret, 1, Rf_allocVector(VECSXP, 11));
  SEXP names_str = PROTECT(Rf_allocVector(STRSXP, 11));
  SET_STRING_ELT(names_str, 0, Rf_mkChar("string"));
  SET_STRING_ELT(names_str, 1, Rf_mkChar("width"));
  SET_STRING_ELT(names_str, 2, Rf_mkChar("height"));
  SET_STRING_ELT(names_str, 3, Rf_mkChar("left_bearing"));
  SET_STRING_ELT(names_str, 4, Rf_mkChar("right_bearing"));
  SET_STRING_ELT(names_str, 5, Rf_mkChar("top_bearing"));
  SET_STRING_ELT(names_str, 6, Rf_mkChar("bottom_bearing"));
  SET_STRING_ELT(names_str, 7, Rf_mkChar("left_border"));
  SET_STRING_ELT(names_str, 8, Rf_mkChar("top_border"));
  SET_STRING_ELT(names_str, 9, Rf_mkChar("pen_x"));
  SET_STRING_ELT(names_str, 10, Rf_mkChar("pen_y"));
  Rf_setAttrib(string_df, Rf_install("names"), names_str);
  
  SEXP widths = SET_VECTOR_ELT(string_df, 1, Rf_allocVector(REALSXP, n_strings));
  SEXP heights = SET_VECTOR_ELT(string_df, 2, Rf_allocVector(REALSXP, n_strings));
  SEXP left_bearings = SET_VECTOR_ELT(string_df, 3, Rf_allocVector(REALSXP, n_strings));
  SEXP right_bearings = SET_VECTOR_ELT(string_df, 4, Rf_allocVector(REALSXP, n_strings));
  SEXP top_bearings = SET_VECTOR_ELT(string_df, 5, Rf_allocVector(REALSXP, n_strings));
  SEXP bottom_bearings = SET_VECTOR_ELT(string_df, 6, Rf_allocVector(REALSXP, n_strings));
  SEXP left_border = SET_VECTOR_ELT(string_df, 7, Rf_allocVector(REALSXP, n_strings));
  SEXP top_border = SET_VECTOR_ELT(string_df, 8, Rf_allocVector(REALSXP, n_strings));
  SEXP pen_x = SET_VECTOR_ELT(string_df, 9, Rf_allocVector(REALSXP, n_strings));
  SEXP pen_y = SET_VECTOR_ELT(string_df, 10, Rf_allocVector(REALSXP, n_strings));
  double* widths_p = REAL(widths);
  double* heights_p = REAL(heights);
  double* left_b_p = REAL(left_bearings);
  double* right_b_p = REAL(right_bearings);
  double* top_b_p = REAL(top_bearings);
  double* bottom_b_p = REAL(bottom_bearings);
  double* left_bor_p = REAL(left_border);
  double* top_bor_p = REAL(top_border);
  double* pen_x_p = REAL(pen_x);
  double* pen_y_p = REAL(pen_y);
  
  // Make them data.frames
  SEXP cl = PROTECT(Rf_allocVector(STRSXP, 3));
  SET_STRING_ELT(cl, 0, Rf_mkChar("tbl_df"));
  SET_STRING_ELT(cl, 1, Rf_mkChar("tbl"));
  SET_STRING_ELT(cl, 2, Rf_mkChar("data.frame"));
  Rf_classgets(info_df, cl);
  Rf_classgets(string_df, cl);
  
  // Shape the text
  int it = 0, it_m = 0;
  int cur_id = INTEGER(id)[0] - 1; // make sure it differs from first
  bool success = false;
  
  BEGIN_CPP
  FreetypeShaper shaper;
  for (int i = 0; i < n_strings; ++i) {
    SEXP this_string = STRING_ELT(string, i);
    int this_id = INTEGER(id)[i];
    if (cur_id == this_id) {
      success = shaper.add_string(Rf_translateCharUTF8(this_string),
                        one_path ? first_path : Rf_translateCharUTF8(STRING_ELT(path, i)),
                        one_path ? first_index : INTEGER(index)[i], 
                        one_size ? first_size : REAL(size)[i],
                        one_tracking ? first_tracking : REAL(tracking)[i]);
      if (!success) {
        Rf_error("Failed to shape string (%s) with font file (%s) with freetype error %i", Rf_translateCharUTF8(this_string), Rf_translateCharUTF8(STRING_ELT(path, i)), shaper.error_code);
      }
    } else {
      cur_id = this_id;
      success = shaper.shape_string(Rf_translateCharUTF8(this_string), 
                          one_path ? first_path : Rf_translateCharUTF8(STRING_ELT(path, i)), 
                          one_path ? first_index : INTEGER(index)[i], 
                          one_size ? first_size : REAL(size)[i], 
                          one_res ? first_res : REAL(res)[i],
                          one_lht ? first_lht : REAL(lineheight)[i],
                          one_align ? first_align : INTEGER(align)[i],
                          one_hjust ? first_hjust : REAL(hjust)[i],
                          one_vjust ? first_vjust : REAL(vjust)[i],
                          one_width ? first_width : REAL(width)[i] * 64,
                          one_tracking ? first_tracking : REAL(tracking)[i],
                          one_indent ? first_indent : REAL(indent)[i] * 64,
                          one_hanging ? first_hanging : REAL(hanging)[i] * 64,
                          one_before ? first_before : REAL(space_before)[i] * 64,
                          one_after ? first_after : REAL(space_after)[i] * 64);
      if (!success) {
        Rf_error("Failed to shape string (%s) with font file (%s) with freetype error %i", Rf_translateCharUTF8(this_string), Rf_translateCharUTF8(STRING_ELT(path, i)), shaper.error_code);
      }
    }
    bool store_string = i == n_strings - 1 || cur_id != INTEGER(id)[i + 1];
    if (store_string) {
      success = shaper.finish_string();
      if (!success) {
        Rf_error("Failed to finalise string shaping");
      }
      int n_glyphs = shaper.glyph_id.size();
      int req_size = it + n_glyphs;
      if (cur_size < req_size) {
        int new_size = cur_size * 2;
        new_size = new_size < req_size ? req_size : new_size;
        REPROTECT(glyph = Rf_lengthgets(glyph, new_size), pr_gl);
        glyph_p = INTEGER(glyph);
        REPROTECT(glyph_id = Rf_lengthgets(glyph_id, new_size), pr_gli);
        glyph_id_p = INTEGER(glyph_id);
        REPROTECT(metric_id = Rf_lengthgets(metric_id, new_size), pr_met);
        metric_id_p = INTEGER(metric_id);
        REPROTECT(string_id = Rf_lengthgets(string_id, new_size), pr_str);
        string_id_p = INTEGER(string_id);
        REPROTECT(x_offset = Rf_lengthgets(x_offset, new_size), pr_x);
        x_offset_p = REAL(x_offset);
        REPROTECT(y_offset = Rf_lengthgets(y_offset, new_size), pr_y);
        y_offset_p = REAL(y_offset);
        REPROTECT(x_midpoint = Rf_lengthgets(x_midpoint, new_size), pr_xm);
        x_midpoint_p = REAL(x_midpoint);
        cur_size = new_size;
      }
      for (int j = 0; j < n_glyphs; j++) {
        glyph_p[it] = (int) shaper.glyph_uc[j];
        glyph_id_p[it] = (int) shaper.glyph_id[j];
        metric_id_p[it] = it_m + 1;
        string_id_p[it] = shaper.string_id[j] + 1;
        x_offset_p[it] = shaper.x_pos[j] / 64.0;
        y_offset_p[it] = shaper.y_pos[j] / 64.0;
        x_midpoint_p[it] = shaper.x_mid[j] / 64.0;
        it++;
      }
      widths_p[it_m] = shaper.width / 64.0;
      heights_p[it_m] = shaper.height / 64.0;
      left_b_p[it_m] = shaper.left_bearing / 64.0;
      right_b_p[it_m] = shaper.right_bearing / 64.0;
      top_b_p[it_m] = shaper.top_bearing / 64.0;
      bottom_b_p[it_m] = shaper.bottom_bearing / 64.0;
      left_bor_p[it_m] = shaper.left_border / 64.0;
      top_bor_p[it_m] = shaper.top_border / 64.0;
      pen_x_p[it_m] = shaper.pen_x / 64.0;
      pen_y_p[it_m] = shaper.pen_y / 64.0;
      it_m++;
    }
  }
  END_CPP
  
  SEXP str = SET_VECTOR_ELT(string_df, 0, Rf_allocVector(STRSXP, it_m));
  for (int i = 0; i < it_m; ++i) {
    SET_STRING_ELT(str, i, R_NaString);
  }
  SET_VECTOR_ELT(string_df, 1, Rf_lengthgets(widths, it_m));
  SET_VECTOR_ELT(string_df, 2, Rf_lengthgets(heights, it_m));
  SET_VECTOR_ELT(string_df, 3, Rf_lengthgets(left_bearings, it_m));
  SET_VECTOR_ELT(string_df, 4, Rf_lengthgets(right_bearings, it_m));
  SET_VECTOR_ELT(string_df, 5, Rf_lengthgets(top_bearings, it_m));
  SET_VECTOR_ELT(string_df, 6, Rf_lengthgets(bottom_bearings, it_m));
  SET_VECTOR_ELT(string_df, 7, Rf_lengthgets(left_border, it_m));
  SET_VECTOR_ELT(string_df, 8, Rf_lengthgets(top_border, it_m));
  SET_VECTOR_ELT(string_df, 9, Rf_lengthgets(pen_x, it_m));
  SET_VECTOR_ELT(string_df, 10, Rf_lengthgets(pen_y, it_m));
  
  // Assign final columns with correct size
  SET_VECTOR_ELT(info_df, 0, Rf_lengthgets(glyph, it));
  SET_VECTOR_ELT(info_df, 1, Rf_lengthgets(glyph_id, it));
  SET_VECTOR_ELT(info_df, 2, Rf_lengthgets(metric_id, it));
  SET_VECTOR_ELT(info_df, 3, Rf_lengthgets(string_id, it));
  SET_VECTOR_ELT(info_df, 4, Rf_lengthgets(x_offset, it));
  SET_VECTOR_ELT(info_df, 5, Rf_lengthgets(y_offset, it));
  SET_VECTOR_ELT(info_df, 6, Rf_lengthgets(x_midpoint, it));
  
  // Add row.names (we didn't know final size before)
  SEXP row_names_gl = PROTECT(Rf_allocVector(REALSXP, 2));
  REAL(row_names_gl)[0] = NA_REAL;
  REAL(row_names_gl)[1] = -it;
  Rf_setAttrib(info_df, Rf_install("row.names"), row_names_gl);
  
  SEXP row_names_str = PROTECT(Rf_allocVector(REALSXP, 2));
  REAL(row_names_str)[0] = NA_REAL;
  REAL(row_names_str)[1] = -it_m;
  Rf_setAttrib(string_df, Rf_install("row.names"), row_names_str);
  
  UNPROTECT(14);
  return ret;
}

SEXP get_line_width(SEXP string, SEXP path, SEXP index, SEXP size, SEXP res, 
                    SEXP include_bearing) {
  int n_strings = LENGTH(string);
  bool one_path = LENGTH(path) == 1;
  const char* first_path = Rf_translateCharUTF8(STRING_ELT(path, 0));
  int first_index = INTEGER(index)[0];
  bool one_size = LENGTH(size) == 1;
  double first_size = REAL(size)[0];
  bool one_res = LENGTH(res) == 1;
  double first_res = REAL(res)[0];
  bool one_bear = LENGTH(include_bearing) == 1;
  int first_bear = LOGICAL(include_bearing)[0];
  
  SEXP widths = PROTECT(Rf_allocVector(REALSXP, n_strings));
  double* widths_p = REAL(widths);
  bool success = false;
  long width = 0;
  
  BEGIN_CPP
  FreetypeShaper shaper;
  
  for (int i = 0; i < n_strings; ++i) {
    success = shaper.single_line_width(
      Rf_translateCharUTF8(STRING_ELT(string, i)),
      one_path ? first_path : Rf_translateCharUTF8(STRING_ELT(path, i)),
      one_path ? first_index : INTEGER(index)[i], 
      one_size ? first_size : REAL(size)[i],
      one_res ? first_res : REAL(res)[i],
      one_bear ? first_bear : LOGICAL(include_bearing)[0],
      width
    );
    if (!success) {
      Rf_error("Failed to calculate width of string (%s) with font file (%s) with freetype error %i", Rf_translateCharUTF8(STRING_ELT(string, i)), Rf_translateCharUTF8(STRING_ELT(path, i)), shaper.error_code);
    }
    widths_p[i] = (double) width / 64.0;
  }
  END_CPP
  
  UNPROTECT(1);
  return widths;
}

int string_width(const char* string, const char* fontfile, int index, 
                    double size, double res, int include_bearing, double* width) {
  
  BEGIN_CPP
  FreetypeShaper shaper;
  long width_tmp = 0;
  bool success = shaper.single_line_width(
    string, fontfile, index, size, res, (bool) include_bearing, width_tmp
  );
  if (success) {
    *width = (double) width_tmp / 64.0;
    return 0;
  }
  return shaper.error_code;
  
  END_CPP
}
int string_shape(const char* string, const char* fontfile, int index, 
                  double size, double res, double* x, double* y, unsigned int max_length) {
  
  BEGIN_CPP
  FreetypeShaper shaper;
  bool success = shaper.shape_string(string, fontfile, index, size, res, 0.0, 0, 
                                     0.0, 0.0, -1.0, 0.0, 0.0, 0.0, 0.0, 0.0);
  if (!success) {
    return shaper.error_code;
  }
  success = shaper.finish_string();
  if (!success) {
    return shaper.error_code;
  }
  max_length = max_length < shaper.x_pos.size() ? max_length : shaper.x_pos.size();
  for (unsigned int i = 0; i < max_length; ++i) {
    x[i] = shaper.x_pos[i];
    y[i] = shaper.y_pos[i];
  }
  return 0;
  
  END_CPP
}
