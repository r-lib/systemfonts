#include "font_metrics.h"
#include "types.h"
#include "caches.h"
#include "utils.h"

#include <cpp11/named_arg.hpp>
#include <cpp11/logicals.hpp>
#include <cpp11/list.hpp>

using list_t = cpp11::list;
using list_w = cpp11::writable::list;
using data_frame_w = cpp11::writable::data_frame;
using strings_t = cpp11::strings;
using strings_w = cpp11::writable::strings;
using integers_t = cpp11::integers;
using integers_w = cpp11::writable::integers;
using logicals_t = cpp11::logicals;
using logicals_w = cpp11::writable::logicals;
using doubles_t = cpp11::doubles;
using doubles_w = cpp11::writable::doubles;

using namespace cpp11::literals;

data_frame_w get_font_info_c(strings_t path, integers_t index, doubles_t size, doubles_t res) {
  bool one_path = path.size() == 1;
  const char* first_path = Rf_translateCharUTF8(path[0]);
  int first_index = index[0];
  bool one_size = size.size() == 1;
  double first_size = size[0];
  bool one_res = res.size() == 1;
  double first_res = res[0];
  int full_length = 1;
  if (!one_path) full_length = path.size();
  else if (!one_size) full_length = size.size();
  else if (!one_res) full_length = res.size();
  
  FreetypeCache& cache = get_font_cache();
  
  strings_w path_col(full_length);
  integers_w index_col(full_length);
  strings_w family(full_length);
  strings_w style(full_length);
  logicals_w italic(full_length);
  logicals_w bold(full_length);
  logicals_w monospace(full_length);
  logicals_w kerning(full_length);
  logicals_w color(full_length);
  logicals_w scalable(full_length);
  logicals_w vertical(full_length);
  integers_w nglyphs(full_length);
  integers_w nsizes(full_length);
  integers_w ncharmaps(full_length);
  list_w bbox(full_length);
  doubles_w ascend(full_length);
  doubles_w descend(full_length);
  doubles_w advance_w(full_length);
  doubles_w advance_h(full_length);
  doubles_w lineheight(full_length);
  doubles_w u_pos(full_length);
  doubles_w u_size(full_length);
  
  for (int i = 0; i < full_length; ++i) {
    bool success = cache.load_font(
      one_path ? first_path : Rf_translateCharUTF8(path[i]),
      one_path ? first_index : index[i],
      one_size ? first_size : size[i],
      one_res ? first_res : res[i]
    );
    if (!success) {
      cpp11::stop("Failed to open font file (%s) with freetype error %i", Rf_translateCharUTF8(path[i]), cache.error_code);
    }
    FontInfo info = cache.font_info();
    
    path_col[i] = one_path ? first_path : path[i];
    index_col[i] = one_path ? first_index : index[i];
    family[i] = info.family;
    style[i] = info.style;
    italic[i] = (Rboolean) info.is_italic;
    bold[i] = (Rboolean) info.is_bold;
    monospace[i] = (Rboolean) info.is_monospace;
    kerning[i] = (Rboolean) info.has_kerning;
    color[i] = (Rboolean) info.has_color;
    scalable[i] = (Rboolean) info.is_scalable;
    vertical[i] = (Rboolean) info.is_vertical;
    nglyphs[i] = info.n_glyphs;
    nsizes[i] = info.n_sizes;
    ncharmaps[i] = info.n_charmaps;
    
    bbox[i] = doubles_w({
      "xmin"_nm = double(info.bbox[0]) / 64.0,
      "xmax"_nm = double(info.bbox[1]) / 64.0,
      "ymin"_nm = double(info.bbox[2]) / 64.0,
      "ymax"_nm = double(info.bbox[3]) / 64.0
    });
    
    ascend[i] = info.max_ascend / 64.0;
    descend[i] = info.max_descend / 64.0;
    advance_w[i] = info.max_advance_w / 64.0;
    advance_h[i] = info.max_advance_h / 64.0;
    lineheight[i] = info.lineheight / 64.0;
    u_pos[i] = info.underline_pos / 64.0;
    u_size[i] = info.underline_size / 64.0;
  }
  
  data_frame_w info({
    "path"_nm = path_col,
    "index"_nm = index_col,
    "family"_nm = family,
    "style"_nm = style,
    "italic"_nm = italic,
    "bold"_nm = bold,
    "monospace"_nm = monospace,
    "kerning"_nm = kerning,
    "color"_nm = color,
    "scalable"_nm = scalable,
    "vertical"_nm = vertical,
    "n_glyphs"_nm = nglyphs,
    "n_sizes"_nm = nsizes,
    "n_charmaps"_nm = ncharmaps,
    "bbox"_nm = bbox,
    "max_ascend"_nm = ascend,
    "max_descend"_nm = descend,
    "max_advance_width"_nm = advance_w,
    "max_advance_height"_nm = advance_h,
    "lineheight"_nm = lineheight,
    "underline_pos"_nm = u_pos,
    "underline_size"_nm = u_size
  });
  info.attr("class") = {"tbl_df", "tbl", "data.frame"};
  
  return info;
}

data_frame_w get_glyph_info_c(strings_t glyphs, strings_t path, integers_t index, doubles_t size, doubles_t res) {
  int n_glyphs = glyphs.size();
  
  bool one_path = path.size() == 1;
  const char* first_path = Rf_translateCharUTF8(path[0]);
  int first_index = index[0];
  bool one_size = size.size() == 1;
  double first_size = size[0];
  bool one_res = res.size() == 1;
  double first_res = res[0];
  
  FreetypeCache& cache = get_font_cache();
  
  integers_w glyph_ids(n_glyphs);
  doubles_w widths(n_glyphs);
  doubles_w heights(n_glyphs);
  doubles_w x_bearings(n_glyphs);
  doubles_w y_bearings(n_glyphs);
  doubles_w x_advances(n_glyphs);
  doubles_w y_advances(n_glyphs);
  list_w bboxes(n_glyphs);
  
  UTF_UCS utf_converter;
  int length = 0;
  int error_c = 0;
  
  for (int i = 0; i < n_glyphs; ++i) {
    bool success = cache.load_font(
      one_path ? first_path : Rf_translateCharUTF8(path[i]),
      one_path ? first_index : index[i],
      one_size ? first_size : size[i],
      one_res ? first_res : res[i]
    );
    if (!success) {
      cpp11::stop("Failed to open font file (%s) with freetype error %i", Rf_translateCharUTF8(path[i]), cache.error_code);
    }
    const char* glyph = Rf_translateCharUTF8(glyphs[i]);
    uint32_t* glyph_code = utf_converter.convert(glyph, length);
    GlyphInfo glyph_info = cache.cached_glyph_info(glyph_code[0], error_c);
    if (error_c != 0) {
      cpp11::stop("Failed to load `%s` from font (%s) with freetype error %i", glyph, Rf_translateCharUTF8(path[i]), error_c);
    }
    
    glyph_ids[i] = glyph_info.index;
    widths[i] = glyph_info.width / 64.0;
    heights[i] = glyph_info.height / 64.0;
    x_bearings[i] = glyph_info.x_bearing / 64.0;
    y_bearings[i] = glyph_info.y_bearing / 64.0;
    x_advances[i] = glyph_info.x_advance / 64.0;
    y_advances[i] = glyph_info.y_advance / 64.0;
    bboxes[i] = doubles_w({
      "xmin"_nm = double(glyph_info.bbox[0]) / 64.0,
      "xmax"_nm = double(glyph_info.bbox[1]) / 64.0,
      "ymin"_nm = double(glyph_info.bbox[2]) / 64.0,
      "ymax"_nm = double(glyph_info.bbox[3]) / 64.0
    });
  }
  
  data_frame_w info({
    "glyph"_nm = glyphs,
    "index"_nm = glyph_ids,
    "width"_nm = widths,
    "height"_nm = heights,
    "x_bearing"_nm = x_bearings,
    "y_bearing"_nm = y_bearings,
    "x_advance"_nm = x_advances,
    "y_advance"_nm = y_advances,
    "bbox"_nm = bboxes
  });
  info.attr("class") = {"tbl_df", "tbl", "data.frame"};
  
  return info;
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
  *ascent = metrics.bbox[3] / 64.0;
  *descent = -metrics.bbox[2] / 64.0;
  
  END_CPP
  
  return 0;
}

void export_font_metrics(DllInfo* dll) {
  R_RegisterCCallable("systemfonts", "glyph_metrics", (DL_FUNC)glyph_metrics);
}
