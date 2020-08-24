#include "string_metrics.h"
#include "string_shape.h"
#include "utils.h"

#include <cpp11/data_frame.hpp>
#include <cpp11/named_arg.hpp>

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

list_t get_string_shape_c(strings_t string, integers_t id, strings_t path, integers_t index, 
                        doubles_t size, doubles_t res, doubles_t lineheight, integers_t align, 
                        doubles_t hjust, doubles_t vjust, doubles_t width, doubles_t tracking, 
                        doubles_t indent, doubles_t hanging, doubles_t space_before, 
                        doubles_t space_after) {
  int n_strings = string.size();
  bool one_path = path.size() == 1;
  const char* first_path = Rf_translateCharUTF8(path[0]);
  int first_index = index[0];
  bool one_size = size.size() == 1;
  double first_size = size[0];
  bool one_res = res.size() == 1;
  double first_res = res[0];
  bool one_lht = lineheight.size() == 1;
  double first_lht = lineheight[0];
  bool one_align = align.size() == 1;
  int first_align = align[0];
  bool one_hjust = hjust.size() == 1;
  double first_hjust = hjust[0];
  bool one_vjust = vjust.size() == 1;
  double first_vjust = vjust[0];
  bool one_width = width.size() == 1;
  double first_width = width[0] * 64;
  bool one_tracking = tracking.size() == 1;
  double first_tracking = tracking[0];
  bool one_indent = indent.size() == 1;
  double first_indent = indent[0] * 64;
  bool one_hanging = hanging.size() == 1;
  double first_hanging = hanging[0] * 64;
  bool one_before = space_before.size() == 1;
  double first_before = space_before[0] * 64;
  bool one_after = space_after.size() == 1;
  double first_after = space_after[0] * 64;
  
  integers_w glyph;
  integers_w glyph_id;
  integers_w metric_id;
  integers_w string_id;
  doubles_w x_offset;
  doubles_w y_offset;
  doubles_w x_midpoint;
  
  doubles_w widths;
  doubles_w heights;
  doubles_w left_bearings;
  doubles_w right_bearings;
  doubles_w top_bearings;
  doubles_w bottom_bearings;
  doubles_w left_border;
  doubles_w top_border;
  doubles_w pen_x;
  doubles_w pen_y;
  
  // Shape the text
  int cur_id = id[0] - 1; // make sure it differs from first
  bool success = false;
  
  FreetypeShaper shaper;
  for (int i = 0; i < n_strings; ++i) {
    const char* this_string = Rf_translateCharUTF8(string[i]);
    int this_id = id[i];
    if (cur_id == this_id) {
      success = shaper.add_string(
        this_string,
        one_path ? first_path : Rf_translateCharUTF8(path[i]),
        one_path ? first_index : index[i], 
        one_size ? first_size : size[i],
        one_tracking ? first_tracking : tracking[i]
      );
      if (!success) {
        cpp11::stop("Failed to shape string (%s) with font file (%s) with freetype error %i", this_string, Rf_translateCharUTF8(path[i]), shaper.error_code);
      }
    } else {
      cur_id = this_id;
      success = shaper.shape_string(
        this_string, 
        one_path ? first_path : Rf_translateCharUTF8(path[i]), 
        one_path ? first_index : index[i], 
        one_size ? first_size : size[i], 
        one_res ? first_res : res[i],
        one_lht ? first_lht : lineheight[i],
        one_align ? first_align : align[i],
        one_hjust ? first_hjust : hjust[i],
        one_vjust ? first_vjust : vjust[i],
        one_width ? first_width : width[i] * 64,
        one_tracking ? first_tracking : tracking[i],
        one_indent ? first_indent : indent[i] * 64,
        one_hanging ? first_hanging : hanging[i] * 64,
        one_before ? first_before : space_before[i] * 64,
        one_after ? first_after : space_after[i] * 64
      );
      if (!success) {
        cpp11::stop("Failed to shape string (%s) with font file (%s) with freetype error %i", this_string, Rf_translateCharUTF8(path[i]), shaper.error_code);
      }
    }
    bool store_string = i == n_strings - 1 || cur_id != id[i + 1];
    if (store_string) {
      success = shaper.finish_string();
      if (!success) {
        cpp11::stop("Failed to finalise string shaping");
      }
      int n_glyphs = shaper.glyph_id.size();
      for (int j = 0; j < n_glyphs; j++) {
        glyph.push_back((int) shaper.glyph_uc[j]);
        glyph_id.push_back((int) shaper.glyph_id[j]);
        metric_id.push_back(widths.size());
        string_id.push_back(shaper.string_id[j] + 1);
        x_offset.push_back(double(shaper.x_pos[j]) / 64.0);
        y_offset.push_back(double(shaper.y_pos[j]) / 64.0);
        x_midpoint.push_back(double(shaper.x_mid[j]) / 64.0);
      }
      widths.push_back(double(shaper.width) / 64.0);
      heights.push_back(double(shaper.height) / 64.0);
      left_bearings.push_back(double(shaper.left_bearing) / 64.0);
      right_bearings.push_back(double(shaper.right_bearing) / 64.0);
      top_bearings.push_back(double(shaper.top_bearing) / 64.0);
      bottom_bearings.push_back(double(shaper.bottom_bearing) / 64.0);
      left_border.push_back(double(shaper.left_border) / 64.0);
      top_border.push_back(double(shaper.top_border) / 64.0);
      pen_x.push_back(double(shaper.pen_x) / 64.0);
      pen_y.push_back(double(shaper.pen_y) / 64.0);
    }
  }
  
  data_frame_w shape_df({
    "glyph"_nm = (SEXP) glyph,
    "index"_nm = (SEXP) glyph_id,
    "metric_id"_nm = (SEXP) metric_id,
    "string_id"_nm = (SEXP) string_id,
    "x_offset"_nm = (SEXP) x_offset,
    "y_offset"_nm = (SEXP) y_offset,
    "x_midpoint"_nm = (SEXP) x_midpoint
  });
  shape_df.attr("class") = {"tbl_df", "tbl", "data.frame"};
  
  data_frame_w metrics_df({
    "string"_nm = (SEXP) strings_w(widths.size()),
    "width"_nm = (SEXP) widths,
    "height"_nm = (SEXP) heights,
    "left_bearing"_nm = (SEXP) left_bearings,
    "right_bearing"_nm = (SEXP) right_bearings,
    "top_bearing"_nm = (SEXP) top_bearings,
    "bottom_bearing"_nm = (SEXP) bottom_bearings,
    "left_border"_nm = (SEXP) left_border,
    "top_border"_nm = (SEXP) top_border,
    "pen_x"_nm = (SEXP) pen_x,
    "pen_y"_nm = (SEXP) pen_y
  });
  metrics_df.attr("class") = {"tbl_df", "tbl", "data.frame"};
  
  return list_w({
    "shape"_nm = shape_df,
    "metrics"_nm = metrics_df
  });
}

doubles_t get_line_width_c(strings_t string, strings_t path, integers_t index, doubles_t size, 
                         doubles_t res, logicals_t include_bearing) {
  int n_strings = string.size();
  bool one_path = path.size() == 1;
  const char* first_path = Rf_translateCharUTF8(path[0]);
  int first_index = index[0];
  bool one_size = size.size() == 1;
  double first_size = size[0];
  bool one_res = res.size() == 1;
  double first_res = res[0];
  bool one_bear = include_bearing.size() == 1;
  int first_bear = include_bearing[0];
  
  doubles_w widths(n_strings);
  bool success = false;
  long width = 0;
  
  FreetypeShaper shaper;
  
  for (int i = 0; i < n_strings; ++i) {
    success = shaper.single_line_width(
      Rf_translateCharUTF8(string[i]),
      one_path ? first_path : Rf_translateCharUTF8(path[i]),
      one_path ? first_index : index[i], 
      one_size ? first_size : size[i],
      one_res ? first_res : res[i],
      one_bear ? first_bear : include_bearing[0],
      width
    );
    if (!success) {
      cpp11::stop("Failed to calculate width of string (%s) with font file (%s) with freetype error %i", Rf_translateCharUTF8(string[i]), Rf_translateCharUTF8(path[i]), shaper.error_code);
    }
    widths[i] = (double) width / 64.0;
  }
  
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
  if (!success) {
    return shaper.error_code;
  }
  *width = (double) width_tmp / 64.0;
  
  END_CPP
  
  return 0;
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
  
  END_CPP
  
  return 0;
}

void export_string_metrics(DllInfo* dll){
  R_RegisterCCallable("systemfonts", "string_width", (DL_FUNC)string_width);
  R_RegisterCCallable("systemfonts", "string_shape", (DL_FUNC)string_shape);
}
