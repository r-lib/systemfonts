#include <limits.h>
#include "string_shape.h"

void FreetypeShaper::shape_string(const char* string, const char* fontfile, 
                                  int index, double size, double res) {
  FreetypeCache& cache = get_font_cache();
  cache.load_font(fontfile, index, size, res);
  
  int n_glyphs = 0;
  u_int32_t* glyphs = utf_converter.convert(string, n_glyphs);
  
  glyph_uc.clear();
  glyph_uc.reserve(n_glyphs);
  glyph_id.clear();
  glyph_id.reserve(n_glyphs);
  x_pos.clear();
  x_pos.clear();
  x_pos.reserve(n_glyphs);
  y_pos.clear();
  y_pos.reserve(n_glyphs);
  
  int pen_x = 0, pen_y = 0;
  long lineheight = cache.cur_lineheight();
  left_bearing = 0;
  right_bearing = 0;
  width = 0;
  height = 0;
  
  if (n_glyphs == 0) return;
  
  GlyphInfo old_metrics = cache.cached_glyph_info(glyphs[0]);
  GlyphInfo metrics = old_metrics;
  
  left_bearing = metrics.bbox[0];
  right_bearing = LONG_MIN;
  height = lineheight;
  
  bool kern = false;
  
  for (int i = 0; i < n_glyphs; i++) {
    bool linebreak = glyphs[i] == 10;
    bool last = i == n_glyphs - 1;
    if (kern && !linebreak) {
      cache.apply_kerning(glyphs[i - 1], glyphs[i], pen_x, pen_y);
    }
    glyph_uc.push_back(glyphs[i]);
    glyph_id.push_back(metrics.index);
    x_pos.push_back(pen_x);
    y_pos.push_back(pen_y);
    if (linebreak) { // linebreak
      width = width < pen_x ? pen_x : width;
      pen_x = 0;
      pen_y -= lineheight >> 6;
      height += lineheight;
      long rb = old_metrics.x_advance - old_metrics.bbox[1];
      right_bearing = right_bearing < rb ? right_bearing : rb;
      kern = false;
    } else {
      pen_x += metrics.x_advance >> 6;
      kern = true;
    }
    if (!last) {
      old_metrics = metrics;
      metrics = cache.cached_glyph_info(glyphs[i + 1]);
      if (linebreak) {
        left_bearing = left_bearing < metrics.bbox[0] ? left_bearing : metrics.bbox[0];
      }
    }
  }
};
