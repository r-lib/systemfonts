#include <R.h>
#include <Rinternals.h>

#include "font_metrics.h"

SEXP get_font_info(SEXP path, SEXP index, SEXP size, SEXP res) {
  FreetypeCache& cache = get_font_cache();
  cache.load_font(Rf_translateCharUTF8(STRING_ELT(path, 0)), 
                  INTEGER(index)[0],
                  REAL(size)[0],
                  REAL(res)[0]);
  FontInfo info = cache.font_info();
  
  SEXP info_df = PROTECT(Rf_allocVector(VECSXP, 21));
  SEXP names = PROTECT(Rf_allocVector(STRSXP, 21));
  SET_STRING_ELT(names, 0, Rf_mkChar("path"));
  SET_STRING_ELT(names, 1, Rf_mkChar("index"));
  SET_STRING_ELT(names, 2, Rf_mkChar("family"));
  SET_STRING_ELT(names, 3, Rf_mkChar("style"));
  SET_STRING_ELT(names, 4, Rf_mkChar("italic"));
  SET_STRING_ELT(names, 5, Rf_mkChar("bold"));
  SET_STRING_ELT(names, 6, Rf_mkChar("monospace"));
  SET_STRING_ELT(names, 7, Rf_mkChar("kerning"));
  SET_STRING_ELT(names, 8, Rf_mkChar("color"));
  SET_STRING_ELT(names, 9, Rf_mkChar("scalable"));
  SET_STRING_ELT(names, 10, Rf_mkChar("n_glyphs"));
  SET_STRING_ELT(names, 11, Rf_mkChar("n_sizes"));
  SET_STRING_ELT(names, 12, Rf_mkChar("n_charmaps"));
  SET_STRING_ELT(names, 13, Rf_mkChar("bbox"));
  SET_STRING_ELT(names, 14, Rf_mkChar("max_ascend"));
  SET_STRING_ELT(names, 15, Rf_mkChar("max_descend"));
  SET_STRING_ELT(names, 16, Rf_mkChar("max_advance_width"));
  SET_STRING_ELT(names, 17, Rf_mkChar("max_advance_height"));
  SET_STRING_ELT(names, 18, Rf_mkChar("lineheight"));
  SET_STRING_ELT(names, 19, Rf_mkChar("underline_pos"));
  SET_STRING_ELT(names, 20, Rf_mkChar("underline_size"));
  setAttrib(info_df, Rf_install("names"), names);
  
  SET_VECTOR_ELT(info_df, 0, path);
  SET_VECTOR_ELT(info_df, 1, index);
  SET_VECTOR_ELT(info_df, 2, Rf_mkString(info.family.c_str()));
  SET_VECTOR_ELT(info_df, 3, Rf_mkString(info.style.c_str()));
  SET_VECTOR_ELT(info_df, 4, Rf_ScalarLogical(info.is_italic));
  SET_VECTOR_ELT(info_df, 5, Rf_ScalarLogical(info.is_bold));
  SET_VECTOR_ELT(info_df, 6, Rf_ScalarLogical(info.is_monospace));
  SET_VECTOR_ELT(info_df, 7, Rf_ScalarLogical(info.has_kerning));
  SET_VECTOR_ELT(info_df, 8, Rf_ScalarLogical(info.has_color));
  SET_VECTOR_ELT(info_df, 9, Rf_ScalarLogical(info.is_scalable));
  SET_VECTOR_ELT(info_df, 10, Rf_ScalarInteger(info.n_glyphs));
  SET_VECTOR_ELT(info_df, 11, Rf_ScalarInteger(info.n_sizes));
  SET_VECTOR_ELT(info_df, 12, Rf_ScalarInteger(info.n_charmaps));
  
  SEXP bbox = SET_VECTOR_ELT(info_df, 13, Rf_allocVector(VECSXP, 1));
  SEXP bbox_el = SET_VECTOR_ELT(bbox, 0, Rf_allocVector(REALSXP, 4));
  REAL(bbox_el)[0] = double(info.bbox[0]) / 26.6;
  REAL(bbox_el)[1] = double(info.bbox[1]) / 26.6;
  REAL(bbox_el)[2] = double(info.bbox[2]) / 26.6;
  REAL(bbox_el)[3] = double(info.bbox[3]) / 26.6;
  SEXP bbox_names = PROTECT(Rf_allocVector(STRSXP, 4));
  SET_STRING_ELT(bbox_names, 0, Rf_mkChar("xmin"));
  SET_STRING_ELT(bbox_names, 1, Rf_mkChar("xmax"));
  SET_STRING_ELT(bbox_names, 2, Rf_mkChar("ymin"));
  SET_STRING_ELT(bbox_names, 3, Rf_mkChar("ymax"));
  setAttrib(bbox_el, Rf_install("names"), bbox_names);
  
  SET_VECTOR_ELT(info_df, 14, Rf_ScalarReal(info.max_ascend / 26.6));
  SET_VECTOR_ELT(info_df, 15, Rf_ScalarReal(info.max_descend / 26.6));
  SET_VECTOR_ELT(info_df, 16, Rf_ScalarReal(info.max_advance_w / 26.6));
  SET_VECTOR_ELT(info_df, 17, Rf_ScalarReal(info.max_advance_h / 26.6));
  SET_VECTOR_ELT(info_df, 18, Rf_ScalarReal(info.lineheight / 26.6));
  SET_VECTOR_ELT(info_df, 19, Rf_ScalarReal(info.underline_pos / 26.6));
  SET_VECTOR_ELT(info_df, 20, Rf_ScalarReal(info.underline_size));
  
  SEXP cl = PROTECT(Rf_allocVector(STRSXP, 3));
  SET_STRING_ELT(cl, 0, Rf_mkChar("tbl_df"));
  SET_STRING_ELT(cl, 1, Rf_mkChar("tbl"));
  SET_STRING_ELT(cl, 2, Rf_mkChar("data.frame"));
  Rf_classgets(info_df, cl);
  
  SEXP row_names = PROTECT(Rf_allocVector(REALSXP, 2));
  REAL(row_names)[0] = NA_REAL;
  REAL(row_names)[1] = -1;
  setAttrib(info_df, Rf_install("row.names"), row_names);
  
  UNPROTECT(5);
  return info_df;
}

SEXP get_glyph_info(SEXP glyphs, SEXP path, SEXP index, SEXP size, SEXP res) {
  int n_glyphs = LENGTH(glyphs);
  
  FreetypeCache& cache = get_font_cache();
  cache.load_font(Rf_translateCharUTF8(STRING_ELT(path, 0)), 
                  INTEGER(index)[0],
                  REAL(size)[0],
                  REAL(res)[0]);
  
  SEXP info_df = PROTECT(Rf_allocVector(VECSXP, 9));
  SEXP names = PROTECT(Rf_allocVector(STRSXP, 9));
  SET_STRING_ELT(names, 0, Rf_mkChar("glyph"));
  SET_STRING_ELT(names, 1, Rf_mkChar("index"));
  SET_STRING_ELT(names, 2, Rf_mkChar("width"));
  SET_STRING_ELT(names, 3, Rf_mkChar("height"));
  SET_STRING_ELT(names, 4, Rf_mkChar("x_bearing"));
  SET_STRING_ELT(names, 5, Rf_mkChar("y_bearing"));
  SET_STRING_ELT(names, 6, Rf_mkChar("x_advance"));
  SET_STRING_ELT(names, 7, Rf_mkChar("y_advance"));
  SET_STRING_ELT(names, 8, Rf_mkChar("bbox"));
  setAttrib(info_df, Rf_install("names"), names);
  
  SET_VECTOR_ELT(info_df, 0, glyphs);
  SEXP glyph_ids = SET_VECTOR_ELT(info_df, 1, Rf_allocVector(INTSXP, n_glyphs));
  SEXP widths = SET_VECTOR_ELT(info_df, 2, Rf_allocVector(REALSXP, n_glyphs));
  SEXP heights = SET_VECTOR_ELT(info_df, 3, Rf_allocVector(REALSXP, n_glyphs));
  SEXP x_bearings = SET_VECTOR_ELT(info_df, 4, Rf_allocVector(REALSXP, n_glyphs));
  SEXP y_bearings = SET_VECTOR_ELT(info_df, 5, Rf_allocVector(REALSXP, n_glyphs));
  SEXP x_advances = SET_VECTOR_ELT(info_df, 6, Rf_allocVector(REALSXP, n_glyphs));
  SEXP y_advances = SET_VECTOR_ELT(info_df, 7, Rf_allocVector(REALSXP, n_glyphs));
  SEXP bboxes = SET_VECTOR_ELT(info_df, 8, Rf_allocVector(VECSXP, n_glyphs));
  
  int* glyph_ids_p = INTEGER(glyph_ids);
  double* widths_p = REAL(widths);
  double* heights_p = REAL(heights);
  double* x_bearings_p = REAL(x_bearings);
  double* y_bearings_p = REAL(y_bearings);
  double* x_advances_p = REAL(x_advances);
  double* y_advances_p = REAL(y_advances);
  
  SEXP cl = PROTECT(Rf_allocVector(STRSXP, 3));
  SET_STRING_ELT(cl, 0, Rf_mkChar("tbl_df"));
  SET_STRING_ELT(cl, 1, Rf_mkChar("tbl"));
  SET_STRING_ELT(cl, 2, Rf_mkChar("data.frame"));
  Rf_classgets(info_df, cl);
  
  SEXP row_names = PROTECT(Rf_allocVector(REALSXP, 2));
  REAL(row_names)[0] = NA_REAL;
  REAL(row_names)[1] = -n_glyphs;
  setAttrib(info_df, Rf_install("row.names"), row_names);
  
  UTF_UCS utf_converter;
  int length;
  SEXP bbox_names = PROTECT(Rf_allocVector(STRSXP, 4));
  SET_STRING_ELT(bbox_names, 0, Rf_mkChar("xmin"));
  SET_STRING_ELT(bbox_names, 1, Rf_mkChar("xmax"));
  SET_STRING_ELT(bbox_names, 2, Rf_mkChar("ymin"));
  SET_STRING_ELT(bbox_names, 3, Rf_mkChar("ymax"));
  
  for (int i = 0; i < n_glyphs; i++) {
    const char* glyph = Rf_translateCharUTF8(STRING_ELT(glyphs, i));
    u_int32_t* glyph_code = utf_converter.convert(glyph, length);
    GlyphInfo glyph_info = cache.cached_glyph_info(glyph_code[0]);
    glyph_ids_p[i] = glyph_info.index;
    widths_p[i] = glyph_info.width / 26.6;
    heights_p[i] = glyph_info.height / 26.6;
    x_bearings_p[i] = glyph_info.x_bearing / 26.6;
    y_bearings_p[i] = glyph_info.y_bearing / 26.6;
    x_advances_p[i] = glyph_info.x_advance / 26.6;
    y_advances_p[i] = glyph_info.y_advance / 26.6;
    SEXP bbox_el = SET_VECTOR_ELT(bboxes, i, Rf_allocVector(REALSXP, 4));
    REAL(bbox_el)[0] = double(glyph_info.bbox[0]) / 26.6;
    REAL(bbox_el)[1] = double(glyph_info.bbox[1]) / 26.6;
    REAL(bbox_el)[2] = double(glyph_info.bbox[2]) / 26.6;
    REAL(bbox_el)[3] = double(glyph_info.bbox[3]) / 26.6;
    setAttrib(bbox_el, Rf_install("names"), bbox_names);
  }
  
  UNPROTECT(5);
  return info_df;
}

void glyph_metrics(u_int32_t code, const char* fontfile, int index, double size, 
                   double res, double* ascent, double* descent, double* width) {
  FreetypeCache& cache = get_font_cache();
  cache.load_font(fontfile, index, size, res);
  GlyphInfo metrics = cache.cached_glyph_info(code);
  *width = metrics.x_advance / 26.6;
  *ascent = metrics.bbox[4] / 26.6;
  *descent = -metrics.bbox[3] / 26.6;
}
