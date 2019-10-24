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
  
  std::vector<u_int32_t> glyph_uc;
  std::vector<unsigned int> glyph_id;
  std::vector<long> x_pos;
  std::vector<long> y_pos;
  long width;
  long height;
  long left_bearing;
  long right_bearing;
  
  void shape_string(const char* string, const char* fontfile, int index, 
                    double size, double res);
  
private:
  UTF_UCS utf_converter;
};

#endif
