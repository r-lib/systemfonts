#include <vector>
#include <string>
#include <map>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_TYPES_H
#include "utils.h"

#ifndef FTCACHE_INCLUDED
#define FTCACHE_INCLUDED

struct FontInfo {

};

struct GlyphInfo {

};

class FreetypeCache {
public:
  FreetypeCache();
  ~FreetypeCache();

  bool load_font(const char* file, int index, double size);
  FontInfo font_info();
  bool load_glyph(u_int32_t index);
  GlyphInfo glyph_info();
  double string_width(u_int32_t* string, int length, bool add_kern);
private:
  FT_Library library;
  std::map<std::string, FT_Face> faces;
  FT_Face current_face;
  std::string current_facename;
  double current_size;


  FT_GlyphSlot glyph;
  u_int32_t glyph_index;
};

#endif