#include <limits.h>
#include <algorithm>
#include <cstdint>
#include "string_shape.h"
#include "types.h"
#include "caches.h"

UTF_UCS FreetypeShaper::utf_converter = UTF_UCS();
std::vector<uint32_t> FreetypeShaper::glyph_uc = {};
std::vector<unsigned int> FreetypeShaper::glyph_id = {};
std::vector<unsigned int> FreetypeShaper::string_id = {};
std::vector<long> FreetypeShaper::x_pos = {};
std::vector<long> FreetypeShaper::y_pos = {};
std::vector<long> FreetypeShaper::x_mid = {};
std::vector<long> FreetypeShaper::x_advance = {}; 
std::vector<long> FreetypeShaper::x_offset = {}; 
std::vector<long> FreetypeShaper::left_bear = {}; 
std::vector<long> FreetypeShaper::right_bear = {}; 
std::vector<long> FreetypeShaper::top_extend = {}; 
std::vector<long> FreetypeShaper::bottom_extend = {}; 
std::vector<long> FreetypeShaper::ascenders = {}; 
std::vector<long> FreetypeShaper::descenders = {}; 

bool FreetypeShaper::shape_string(const char* string, const char* fontfile, 
                                  int index, double size, double res, double lineheight,
                                  int align, double hjust, double vjust, double width,
                                  double tracking, double ind, double hang, double before, 
                                  double after) {
  reset();
  
  FreetypeCache& cache = get_font_cache();
  bool success = cache.load_font(fontfile, index, size, res);
  if (!success) {
    error_code = cache.error_code;
    return false;
  }
  
  int n_glyphs = 0;
  uint32_t* glyphs = utf_converter.convert(string, n_glyphs);
  
  if (n_glyphs == 0) return true;
  
  max_width = width;
  indent = ind;
  pen_x = indent;
  hanging = hang;
  space_before = before;
  space_after = after;
  
  glyph_uc.reserve(n_glyphs);
  glyph_id.reserve(n_glyphs);
  string_id.reserve(n_glyphs);
  x_pos.reserve(n_glyphs);
  y_pos.reserve(n_glyphs);
  
  cur_res = res;
  cur_lineheight = lineheight;
  cur_align = align;
  cur_hjust = hjust;
  cur_vjust = vjust;
  
  ascend = cache.cur_ascender();
  descend = cache.cur_descender();
  
  success = shape_glyphs(glyphs, n_glyphs, cache, tracking);
  return success;
}

bool FreetypeShaper::add_string(const char* string, const char* fontfile, 
                                int index, double size, double tracking) {
  cur_string++;
  int n_glyphs = 0;
  uint32_t* glyphs = utf_converter.convert(string, n_glyphs);
  
  if (n_glyphs == 0) return true;
  
  FreetypeCache& cache = get_font_cache();
  bool success = cache.load_font(fontfile, index, size, cur_res);
  if (!success) {
    error_code = cache.error_code;
    return false;
  }
  ascend = cache.cur_ascender();
  descend = cache.cur_descender();
  success = shape_glyphs(glyphs, n_glyphs, cache, tracking);
  return success;
}
  
bool FreetypeShaper::finish_string() {
  if (glyph_id.size() == 0) {
    return true;
  }
  bool first_char = true;
  bool first_line = true;
  pen_x += indent;
  int last_space = -1;
  long last_nonspace_width = 0;
  long last_nonspace_bear = 0;
  int cur_line = 0;
  double line_height = 0;
  size_t glyph_counter = 0;
  long max_descend = 0;
  long max_ascend = 0;
  long max_top_extend = 0;
  long max_bottom_extend = 0;
  long last_max_descend = 0;
  bool no_break_last = true;
  
  for (unsigned int i = 0; i < glyph_id.size(); ++i) {
    bool linebreak = glyph_is_linebreak(glyph_uc[i]);
    bool may_break = glyph_is_breaker(glyph_uc[i]);
    bool last = i == glyph_id.size() - 1;
    
    bool soft_wrap = false;
    
    if (may_break || linebreak) {
      last_space = i; 
      if (no_break_last) {
        last_nonspace_width = pen_x;
        last_nonspace_bear = i == 0 ? 0 : right_bear[i - 1];
      }
    }
    no_break_last = !may_break;
    
    // Apply kerning if not the first glyph on the line
    if (!first_char) {
      pen_x += x_offset[i];
    }
    
    // Calculate top and bottom extend and ascender/descender
    
    
    // Soft wrapping?
    if (max_width > 0 && !first_char && pen_x + x_advance[i] > max_width && !may_break && !linebreak) {
      // Rewind to last breaking char and set the soft_wrap flag
      i = last_space >= 0 ? last_space : i - 1;
      x_pos.resize(i + 1);
      x_mid.resize(i + 1);
      line_id.resize(i + 1);
      soft_wrap = true;
      last = false;
    } else {
      // No soft wrap, record pen position
      x_pos.push_back(pen_x);
      x_mid.push_back(x_advance[i] / 2);
      line_id.push_back(cur_line);
    }
    
    // If last char update terminal line info
    if (last) {
      last_nonspace_width = pen_x + x_advance[i];
      last_nonspace_bear = right_bear[i];
    }
    if (first_char) {
      line_left_bear.push_back(left_bear[i]);
      pen_y -= space_before;
    }
    
    // Handle new lines
    if (linebreak || soft_wrap || last) {
      // Record and reset line dim info
      line_right_bear.push_back(last_nonspace_bear);
      line_width.push_back(last_nonspace_width);
      last_nonspace_bear = 0;
      last_nonspace_width = 0;
      last_space = -1;
      no_break_last = true;
      
      // Calculate line dimensions
      for (size_t j = glyph_counter; j < x_pos.size(); ++j) {
        if (max_ascend < ascenders[j]) {
          max_ascend = ascenders[j];
        }
        if (max_top_extend < top_extend[j]) {
          max_top_extend = top_extend[j];
        }
        if (max_descend > descenders[j]) {
          max_descend = descenders[j];
        }
        if (max_bottom_extend > bottom_extend[j]) {
          max_bottom_extend = bottom_extend[j];
        }
      }
      
      // Move pen based on indent and line height
      line_height = (max_ascend - last_max_descend) * cur_lineheight;
      if (last) {
        pen_x = (linebreak || soft_wrap) ? 0 : pen_x + x_advance[i];
      } else {
        pen_x = soft_wrap ? hanging : indent;
      }
      pen_y = first_line ? 0 : pen_y - line_height;
      bottom -= line_height;
      // Fill up y_pos based on calculated pen position
      for (; glyph_counter < x_pos.size(); ++glyph_counter) {
        y_pos.push_back(pen_y);
      }
      // Move pen_y further down based on paragraph spacing
      // TODO: Add per string paragraph spacing
      if (linebreak) {
        pen_y -= space_after;
        if (last) {
          pen_y -= line_height;
          bottom -= line_height;
        }
      }
      if (first_line) {
        top_border = max_ascend;
        top_bearing = top_border - max_top_extend;
      }
      // Reset flags and counters
      last_max_descend = max_descend;
      if (!last) {
        max_ascend = 0;
        max_descend = 0;
        max_top_extend = 0;
        max_bottom_extend = 0;
        first_line = false;
        cur_line++;
        first_char = true;
      }
    } else {
      // No line break - advance the pen
      pen_x += x_advance[i];
      first_char = false;
    }
  }
  height = top_border - bottom - max_descend;
  bottom_bearing = max_bottom_extend - max_descend;
  int max_width_ind = std::max_element(line_width.begin(), line_width.end()) - line_width.begin();
  width = max_width < 0 ? line_width[max_width_ind] : max_width;
  if (cur_align != 0) {
    for (unsigned int i = 0; i < x_pos.size(); ++i) {
      int index = line_id[i];
      int lwd = line_width[index];
      x_pos[i] = cur_align == 1 ? x_pos[i] + width/2 - lwd/2 : x_pos[i] + width - lwd;
    }
  }
  double width_diff = width - line_width[max_width_ind];
  if (cur_align == 1) {
    width_diff /= 2;
  }
  left_bearing = cur_align == 0 ? *std::min_element(line_left_bear.begin(), line_left_bear.end()) : line_left_bear[max_width_ind] + width_diff;
  right_bearing = cur_align == 2 ? *std::min_element(line_right_bear.begin(), line_right_bear.end()) : line_right_bear[max_width_ind] + width_diff;
  if (cur_hjust != 0.0) {
    left_border = - cur_hjust * width;
    pen_x += left_border;
    for (unsigned int i = 0; i < x_pos.size(); ++i) {
      x_pos[i] += left_border;
    }
  }
  if (cur_vjust != 1.0) {
    long just_height = top_border - pen_y;
    for (unsigned int i = 0; i < x_pos.size(); ++i) {
      y_pos[i] += - pen_y - cur_vjust * just_height;
    }
    top_border += - pen_y - cur_vjust * just_height;
    pen_y += - pen_y - cur_vjust * just_height;
  }
  return true;
}

bool FreetypeShaper::single_line_width(const char* string, const char* fontfile, 
                                       int index, double size, double res, 
                                       bool include_bearing, long& width) {
  long x = 0;
  long y = 0;
  long left_bear = 0;
  int error_c = 0;
  GlyphInfo metrics = {};
  
  int n_glyphs = 0;
  uint32_t* glyphs = utf_converter.convert(string, n_glyphs);
  
  if (n_glyphs == 0) {
    width = x;
    return true;
  }
  
  FreetypeCache& cache = get_font_cache();
  bool success = cache.load_font(fontfile, index, size, res);
  if (!success) {
    error_code = cache.error_code;
    return false;
  }
  
  for (int i = 0; i < n_glyphs; ++i) {
    metrics = cache.cached_glyph_info(glyphs[i], error_c);
    if (error_c != 0) {
      error_code = error_c;
      return false;
    }
    if (i != 0) {
      success = cache.apply_kerning(glyphs[i - 1], glyphs[i], x, y);
      if (!success) {
        error_code = cache.error_code;
        return false;
      }
    } else {
      left_bear = metrics.x_bearing;
    }
    x += metrics.x_advance;
  }
  
  if (!include_bearing) {
    x -= left_bear;
    x -= metrics.x_advance - metrics.bbox[1];
  }
  width = x;
  return true;
}

void FreetypeShaper::reset() {
  glyph_uc.clear();
  glyph_id.clear();
  string_id.clear();
  x_pos.clear();
  y_pos.clear();
  x_mid.clear();
  x_advance.clear(); 
  x_offset.clear(); 
  left_bear.clear(); 
  right_bear.clear(); 
  top_extend.clear(); 
  bottom_extend.clear(); 
  line_left_bear.clear();
  line_right_bear.clear();
  line_width.clear();
  line_id.clear();
  ascenders.clear();
  descenders.clear();
  
  pen_x = 0;
  pen_y = 0;
  
  top = 0;
  bottom = 0;
  ascend = 0;
  descend = 0;
  
  left_bearing = 0;
  right_bearing = 0;
  width = 0;
  height = 0;
  top_border = 0;
  left_border = 0;
  
  cur_string = 0;
}

bool FreetypeShaper::shape_glyphs(uint32_t* glyphs, int n_glyphs, FreetypeCache& cache, double tracking) {
  if (n_glyphs == 0) return true;
  int error_c = 0; 
  bool success = false;
  GlyphInfo old_metrics = cache.cached_glyph_info(glyphs[0], error_c);
  if (error_c != 0) {
    error_code = error_c;
    return false;
  }
  GlyphInfo metrics = old_metrics;
  
  tracking = cache.tracking_diff(tracking);
  
  long delta_x = 0;
  long delta_y = 0;
  
  for (int i = 0; i < n_glyphs; ++i) {
    x_advance.push_back(metrics.x_advance + tracking);
    left_bear.push_back(metrics.bbox[0]);
    right_bear.push_back(old_metrics.x_advance - old_metrics.bbox[1]);
    top_extend.push_back(metrics.bbox[3]);
    bottom_extend.push_back(metrics.bbox[2]);
    ascenders.push_back(ascend);
    descenders.push_back(descend);
    if (i == 0) {
      x_offset.push_back(0);
    } else {
      success = cache.get_kerning(glyphs[i - 1], glyphs[i], delta_x, delta_y);
      if (!success) {
        error_code = cache.error_code;
        return false;
      }
      x_offset.push_back(delta_x);
    }
    glyph_uc.push_back(glyphs[i]);
    glyph_id.push_back(metrics.index);
    string_id.push_back(cur_string);
    
    if (i != n_glyphs - 1) {
      old_metrics = metrics;
      metrics = cache.cached_glyph_info(glyphs[i + 1], error_c);
      if (error_c != 0) {
        error_code = error_c;
        return false;
      }
    }
  }
  return true;
}
