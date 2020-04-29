#define R_NO_REMAP

#include <Rinternals.h>

#include <cstdint>

#include "font_metrics.h"
#include "utils.h"

SEXP get_font_info(SEXP path, SEXP index, SEXP size, SEXP res) {
  bool one_path = Rf_length(path) == 1;
  const char* first_path = Rf_translateCharUTF8(STRING_ELT(path, 0));
  int first_index = INTEGER(index)[0];
  bool one_size = Rf_length(size) == 1;
  double first_size = REAL(size)[0];
  bool one_res = Rf_length(res) == 1;
  double first_res = REAL(res)[0];
  int full_length = 1;
  if (!one_path) full_length = Rf_length(path);
  else if (!one_size) full_length = Rf_length(size);
  else if (!one_res) full_length = Rf_length(res);
  FreetypeCache& cache = get_font_cache();
  
  SEXP info_df = PROTECT(Rf_allocVector(VECSXP, 22));
  SEXP names = PROTECT(Rf_allocVector(STRSXP, 22));
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
  SET_STRING_ELT(names, 10, Rf_mkChar("vertical"));
  SET_STRING_ELT(names, 11, Rf_mkChar("n_glyphs"));
  SET_STRING_ELT(names, 12, Rf_mkChar("n_sizes"));
  SET_STRING_ELT(names, 13, Rf_mkChar("n_charmaps"));
  SET_STRING_ELT(names, 14, Rf_mkChar("bbox"));
  SET_STRING_ELT(names, 15, Rf_mkChar("max_ascend"));
  SET_STRING_ELT(names, 16, Rf_mkChar("max_descend"));
  SET_STRING_ELT(names, 17, Rf_mkChar("max_advance_width"));
  SET_STRING_ELT(names, 18, Rf_mkChar("max_advance_height"));
  SET_STRING_ELT(names, 19, Rf_mkChar("lineheight"));
  SET_STRING_ELT(names, 20, Rf_mkChar("underline_pos"));
  SET_STRING_ELT(names, 21, Rf_mkChar("underline_size"));
  Rf_setAttrib(info_df, Rf_install("names"), names);
  
  SEXP path_col = SET_VECTOR_ELT(info_df, 0, Rf_allocVector(STRSXP, full_length));
  SEXP index_col = SET_VECTOR_ELT(info_df, 1, Rf_allocVector(INTSXP, full_length));
  SEXP family = SET_VECTOR_ELT(info_df, 2, Rf_allocVector(STRSXP, full_length));
  SEXP style = SET_VECTOR_ELT(info_df, 3, Rf_allocVector(STRSXP, full_length));
  SEXP italic = SET_VECTOR_ELT(info_df, 4, Rf_allocVector(LGLSXP, full_length));
  SEXP bold = SET_VECTOR_ELT(info_df, 5, Rf_allocVector(LGLSXP, full_length));
  SEXP monospace = SET_VECTOR_ELT(info_df, 6, Rf_allocVector(LGLSXP, full_length));
  SEXP kerning = SET_VECTOR_ELT(info_df, 7, Rf_allocVector(LGLSXP, full_length));
  SEXP color = SET_VECTOR_ELT(info_df, 8, Rf_allocVector(LGLSXP, full_length));
  SEXP scalable = SET_VECTOR_ELT(info_df, 9, Rf_allocVector(LGLSXP, full_length));
  SEXP vertical = SET_VECTOR_ELT(info_df, 10, Rf_allocVector(LGLSXP, full_length));
  SEXP nglyphs = SET_VECTOR_ELT(info_df, 11, Rf_allocVector(INTSXP, full_length));
  SEXP nsizes = SET_VECTOR_ELT(info_df, 12, Rf_allocVector(INTSXP, full_length));
  SEXP ncharmaps = SET_VECTOR_ELT(info_df, 13, Rf_allocVector(INTSXP, full_length));
  SEXP bbox = SET_VECTOR_ELT(info_df, 14, Rf_allocVector(VECSXP, full_length));
  SEXP ascend = SET_VECTOR_ELT(info_df, 15, Rf_allocVector(REALSXP, full_length));
  SEXP descend = SET_VECTOR_ELT(info_df, 16, Rf_allocVector(REALSXP, full_length));
  SEXP advance_w = SET_VECTOR_ELT(info_df, 17, Rf_allocVector(REALSXP, full_length));
  SEXP advance_h = SET_VECTOR_ELT(info_df, 18, Rf_allocVector(REALSXP, full_length));
  SEXP lineheight = SET_VECTOR_ELT(info_df, 19, Rf_allocVector(REALSXP, full_length));
  SEXP u_pos = SET_VECTOR_ELT(info_df, 20, Rf_allocVector(REALSXP, full_length));
  SEXP u_size = SET_VECTOR_ELT(info_df, 21, Rf_allocVector(REALSXP, full_length));
  
  SEXP cl = PROTECT(Rf_allocVector(STRSXP, 3));
  SET_STRING_ELT(cl, 0, Rf_mkChar("tbl_df"));
  SET_STRING_ELT(cl, 1, Rf_mkChar("tbl"));
  SET_STRING_ELT(cl, 2, Rf_mkChar("data.frame"));
  Rf_classgets(info_df, cl);
  
  SEXP row_names = PROTECT(Rf_allocVector(REALSXP, 2));
  REAL(row_names)[0] = NA_REAL;
  REAL(row_names)[1] = -full_length;
  Rf_setAttrib(info_df, Rf_install("row.names"), row_names);
  
  
  SEXP bbox_names = PROTECT(Rf_allocVector(STRSXP, 4));
  SET_STRING_ELT(bbox_names, 0, Rf_mkChar("xmin"));
  SET_STRING_ELT(bbox_names, 1, Rf_mkChar("xmax"));
  SET_STRING_ELT(bbox_names, 2, Rf_mkChar("ymin"));
  SET_STRING_ELT(bbox_names, 3, Rf_mkChar("ymax"));
  
  BEGIN_CPP
  for (int i = 0; i < full_length; ++i) {
    bool success = cache.load_font(
      one_path ? first_path : Rf_translateCharUTF8(STRING_ELT(path, i)),
      one_path ? first_index : INTEGER(index)[i],
      one_size ? first_size : REAL(size)[i],
      one_res ? first_res : REAL(res)[i]
    );
    if (!success) {
      Rf_error("Failed to open font file (%s) with freetype error %i", Rf_translateCharUTF8(STRING_ELT(path, i)), cache.error_code);
    }
    FontInfo info = cache.font_info();
    
    SET_STRING_ELT(path_col, i, one_path ? Rf_mkChar(first_path) : STRING_ELT(path, i));
    INTEGER(index_col)[i] = one_path ? first_index : INTEGER(index)[i];
    SET_STRING_ELT(family, i, Rf_mkChar(info.family.c_str()));
    SET_STRING_ELT(style, i, Rf_mkChar(info.style.c_str()));
    LOGICAL(italic)[i] = info.is_italic ? TRUE : FALSE;
    LOGICAL(bold)[i] = info.is_bold ? TRUE : FALSE;
    LOGICAL(monospace)[i] = info.is_monospace ? TRUE : FALSE;
    LOGICAL(kerning)[i] = info.has_kerning ? TRUE : FALSE;
    LOGICAL(color)[i] = info.has_color ? TRUE : FALSE;
    LOGICAL(scalable)[i] = info.is_scalable ? TRUE : FALSE;
    LOGICAL(vertical)[i] = info.is_vertical ? TRUE : FALSE;
    INTEGER(nglyphs)[i] = info.n_glyphs;
    INTEGER(nsizes)[i] = info.n_sizes;
    INTEGER(ncharmaps)[i] = info.n_charmaps;
    
    SEXP bbox_el = SET_VECTOR_ELT(bbox, i, Rf_allocVector(REALSXP, 4));
    REAL(bbox_el)[0] = double(info.bbox[0]) / 64.0;
    REAL(bbox_el)[1] = double(info.bbox[1]) / 64.0;
    REAL(bbox_el)[2] = double(info.bbox[2]) / 64.0;
    REAL(bbox_el)[3] = double(info.bbox[3]) / 64.0;
    Rf_setAttrib(bbox_el, Rf_install("names"), bbox_names);
    
    REAL(ascend)[i] = info.max_ascend / 64.0;
    REAL(descend)[i] = info.max_descend / 64.0;
    REAL(advance_w)[i] = info.max_advance_w / 64.0;
    REAL(advance_h)[i] = info.max_advance_h / 64.0;
    REAL(lineheight)[i] = info.lineheight / 64.0;
    REAL(u_pos)[i] = info.underline_pos / 64.0;
    REAL(u_size)[i] = info.underline_size / 64.0;
  }
  END_CPP
  
  UNPROTECT(5);
  return info_df;
}

SEXP get_glyph_info(SEXP glyphs, SEXP path, SEXP index, SEXP size, SEXP res) {
  int n_glyphs = Rf_length(glyphs);
  
  bool one_path = Rf_length(path) == 1;
  const char* first_path = Rf_translateCharUTF8(STRING_ELT(path, 0));
  int first_index = INTEGER(index)[0];
  bool one_size = Rf_length(size) == 1;
  double first_size = REAL(size)[0];
  bool one_res = Rf_length(res) == 1;
  double first_res = REAL(res)[0];
  
  BEGIN_CPP
  FreetypeCache& cache = get_font_cache();
  
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
  Rf_setAttrib(info_df, Rf_install("names"), names);
  
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
  Rf_setAttrib(info_df, Rf_install("row.names"), row_names);
  
  UTF_UCS utf_converter;
  int length = 0;
  int error_c = 0;
  SEXP bbox_names = PROTECT(Rf_allocVector(STRSXP, 4));
  SET_STRING_ELT(bbox_names, 0, Rf_mkChar("xmin"));
  SET_STRING_ELT(bbox_names, 1, Rf_mkChar("xmax"));
  SET_STRING_ELT(bbox_names, 2, Rf_mkChar("ymin"));
  SET_STRING_ELT(bbox_names, 3, Rf_mkChar("ymax"));
  
  for (int i = 0; i < n_glyphs; ++i) {
    bool success = cache.load_font(
      one_path ? first_path : Rf_translateCharUTF8(STRING_ELT(path, i)),
      one_path ? first_index : INTEGER(index)[i],
      one_size ? first_size : REAL(size)[i],
      one_res ? first_res : REAL(res)[i]
    );
    if (!success) {
      Rf_error("Failed to open font file (%s) with freetype error %i", Rf_translateCharUTF8(STRING_ELT(path, i)), cache.error_code);
    }
    const char* glyph = Rf_translateCharUTF8(STRING_ELT(glyphs, i));
    uint32_t* glyph_code = utf_converter.convert(glyph, length);
    GlyphInfo glyph_info = cache.cached_glyph_info(glyph_code[0], error_c);
    if (error_c != 0) {
      Rf_error("Failed to load `%s` from font (%s) with freetype error %i", glyph, Rf_translateCharUTF8(STRING_ELT(path, i)), error_c);
    }
    
    glyph_ids_p[i] = glyph_info.index;
    widths_p[i] = glyph_info.width / 64.0;
    heights_p[i] = glyph_info.height / 64.0;
    x_bearings_p[i] = glyph_info.x_bearing / 64.0;
    y_bearings_p[i] = glyph_info.y_bearing / 64.0;
    x_advances_p[i] = glyph_info.x_advance / 64.0;
    y_advances_p[i] = glyph_info.y_advance / 64.0;
    SEXP bbox_el = SET_VECTOR_ELT(bboxes, i, Rf_allocVector(REALSXP, 4));
    REAL(bbox_el)[0] = double(glyph_info.bbox[0]) / 64.0;
    REAL(bbox_el)[1] = double(glyph_info.bbox[1]) / 64.0;
    REAL(bbox_el)[2] = double(glyph_info.bbox[2]) / 64.0;
    REAL(bbox_el)[3] = double(glyph_info.bbox[3]) / 64.0;
    Rf_setAttrib(bbox_el, Rf_install("names"), bbox_names);
  }
  
  UNPROTECT(5);
  return info_df;
  
  END_CPP
}

int glyph_metrics(uint32_t code, const char* fontfile, int index, double size, 
                   double res, double* ascent, double* descent, double* width) {
  
  BEGIN_CPP
  FreetypeCache& cache = get_font_cache();
  if (!cache.load_font(fontfile, index, size, res)) {
    return cache.error_code;
  }
  int error = 0;
  GlyphInfo metrics = cache.cached_glyph_info(code, error);
    
  if (error != 0) {
    return error;
  }
  *width = metrics.x_advance / 64.0;
  *ascent = metrics.bbox[4] / 64.0;
  *descent = -metrics.bbox[3] / 64.0;
  
  return 0;
  
  END_CPP
}
