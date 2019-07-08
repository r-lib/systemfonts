#include "ft_cache.h"

FreetypeCache::FreetypeCache() {
  FT_Error err = FT_Init_FreeType(&library);
}
FreetypeCache::~FreetypeCache() {
  FT_Done_FreeType(library);
}

bool FreetypeCache::load_font(const char* file, int index, double size) {
  std::string font_id = file;
  font_id += "-";
  font_id += index;
  if (font_id.compare(current_facename) == 0) {
    if (size == current_size) {
      return true;
    }
    FT_Set_Char_Size(current_face, 0, size * 64, 300, 300);
    current_size = size;
    return true;
  }
  
  FT_Face new_face;
  FT_New_Face(library, file, index, &new_face);
  FT_Set_Char_Size(new_face, 0, size * 64, 300, 300);
  current_face = new_face;
  current_size = size;
  current_facename = font_id;
  faces
}
