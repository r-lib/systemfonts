#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_TYPES_H
#include FT_CACHE_H

#ifdef __EMSCRIPTEN__
#undef TYPEOF
#endif

#include "utils.h"
#include "ft_cache.h"

class FreetypeShaper {
public:
  FreetypeShaper() :
    width(0),
    height(0),
    left_bearing(0),
    right_bearing(0),
    top_bearing(0),
    bottom_bearing(0),
    top_border(0),
    left_border(0),
    pen_x(0),
    pen_y(0),
    error_code(0),
    cur_lineheight(0.0),
    cur_align(0),
    cur_string(0),
    cur_hjust(0.0),
    cur_vjust(0.0),
    cur_res(0.0),
    line_left_bear(),
    line_right_bear(),
    line_width(),
    line_id(),
    top(0),
    bottom(0),
    ascend(0),
    descend(0),
    max_width(0),
    indent(0),
    hanging(0),
    space_before(0),
    space_after(0)
  {};
  ~FreetypeShaper() {};
  
  static std::vector<uint32_t> glyph_uc;
  static std::vector<unsigned int> glyph_id;
  static std::vector<unsigned int> string_id;
  static std::vector<long> x_pos;
  static std::vector<long> y_pos;
  static std::vector<long> x_mid;
  long width;
  long height;
  long left_bearing;
  long right_bearing;
  long top_bearing;
  long bottom_bearing;
  long top_border;
  long left_border;
  long pen_x;
  long pen_y;
  
  int error_code;
  
  bool shape_string(const char* string, const char* fontfile, int index, 
                    double size, double res, double lineheight,
                    int align, double hjust, double vjust, double width,
                    double tracking, double ind, double hang, double before, 
                    double after);
  bool add_string(const char* string, const char* fontfile, int index, 
                  double size, double tracking);
  bool finish_string();
  
  bool single_line_width(const char* string, const char* fontfile, int index, 
                         double size, double res, bool include_bearing, long& width);
  
private:
  static UTF_UCS utf_converter;
  double cur_lineheight;
  int cur_align;
  unsigned int cur_string;
  double cur_hjust;
  double cur_vjust;
  double cur_res;
  static std::vector<long> x_advance; 
  static std::vector<long> x_offset; 
  static std::vector<long> left_bear; 
  static std::vector<long> right_bear; 
  static std::vector<long> top_extend; 
  static std::vector<long> bottom_extend; 
  static std::vector<long> ascenders; 
  static std::vector<long> descenders; 
  std::vector<long> line_left_bear; 
  std::vector<long> line_right_bear;
  std::vector<long> line_width;
  std::vector<long> line_id;
  
  long top;
  long bottom;
  long ascend;
  long descend;
  long max_width;
  long indent;
  long hanging;
  long space_before;
  long space_after;
  
  void reset();
  bool shape_glyphs(uint32_t* glyphs, int n_glyphs, FreetypeCache& cache, double tracking);
  
  inline bool glyph_is_linebreak(int id) {
    switch (id) {
    case 10: return true;
    case 11: return true;
    case 12: return true;
    case 13: return true;
    case 133: return true;
    case 8232: return true;
    case 8233: return true;
    }
    return false;
  }
  
  inline bool glyph_is_breaker(int id) {
    switch (id) {
    case 9: return true;
    case 32: return true;
    case 5760: return true;
    case 6158: return true;
    case 8192: return true;
    case 8193: return true;
    case 8194: return true;
    case 8195: return true;
    case 8196: return true;
    case 8197: return true;
    case 8198: return true;
    case 8200: return true;
    case 8201: return true;
    case 8202: return true;
    case 8203: return true;
    case 8204: return true;
    case 8205: return true;
    case 8287: return true;
    case 12288: return true;
    }
    return false;
  }
};
