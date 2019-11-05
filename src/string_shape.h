#ifndef FTSHAPE_INCLUDED
#define FTSHAPE_INCLUDED
#include <vector>
#include <string>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_TYPES_H
#include FT_CACHE_H
#include "utils.h"
#include "ft_cache.h"

// Defined in init.cpp
FreetypeCache& get_font_cache();

class FreetypeShaper {
public:
  FreetypeShaper() {};
  ~FreetypeShaper() {};
  
  static std::vector<u_int32_t> glyph_uc;
  static std::vector<unsigned int> glyph_id;
  static std::vector<unsigned int> string_id;
  static std::vector<long> x_pos;
  static std::vector<long> y_pos;
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
                    int align, double hjust, double vjust);
  bool add_string(const char* string, const char* fontfile, int index, 
                  double size);
  bool finish_string();
  
  bool single_line_width(const char* string, const char* fontfile, int index, 
                         double size, double res, bool include_bearing, long& width);
  
private:
  static UTF_UCS utf_converter;
  double cur_lineheight;
  double cur_full_lineheight;
  int cur_align;
  unsigned int cur_string;
  double cur_hjust;
  double cur_vjust;
  double cur_res;
  int cur_line;
  int first_glyph;
  bool kern;
  bool firstline;
  std::vector<long> line_left_bear; 
  std::vector<long> line_right_bear;
  std::vector<long> line_width;
  std::vector<long> line_id;
  
  long top;
  long bottom;
  long ascend;
  long descend;
  
  void reset();
  bool shape_glyphs(u_int32_t* glyphs, int n_glyphs, FreetypeCache& cache);
};

#endif
