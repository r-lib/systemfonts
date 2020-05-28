#ifndef FTCACHE_INCLUDED
#define FTCACHE_INCLUDED
#include <vector>
#include <string>
#include <set>
#include <map>
#include <unordered_set>
#include <memory>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_TYPES_H
#include FT_SIZES_H
#include "utils.h"
#include "cache_lru.h"

struct FaceID {
  std::string file;
  unsigned int index;
  
  FaceID() : file(""), index(-1) {}
  FaceID(std::string f) : file(f), index(0) {}
  FaceID(std::string f, unsigned int i) : file(f), index(i) {}
  FaceID(FaceID& face) : file(face.file), index(face.index) {}
  
  bool operator==(const FaceID &other) const { 
    return (index == other.index && file == other.file);
  }
};
struct FaceIDHasher {
  std::size_t operator()(const FaceID& k) const {
    return std::hash<std::string>()(k.file) ^ std::hash<unsigned int>()(k.index);
  }
};
struct SizeID {
  FaceID face;
  double size;
  double res;
  
  SizeID() : face(), size(-1.0), res(-1.0) {}
  SizeID(FaceID f, double s, double r) : face(f), size(s), res(r) {}
  
  bool operator==(const SizeID &other) const { 
    return (size == other.size && res == other.res && face == other.face);
  }
};
struct SizeIDHasher {
  std::size_t operator()(const SizeID& k) const {
    return FaceIDHasher()(k.face) ^ std::hash<double>()(k.size) ^ std::hash<double>()(k.res);
  }
};
struct FaceStore {
  FT_Face face;
  std::unordered_set<SizeID> sizes;
  
  FaceStore() : sizes() {};
  FaceStore(FT_Face f) : face(f), sizes() {}
};

struct FontInfo {
  std::string family;
  std::string style;
  bool is_italic;
  bool is_bold;
  bool is_monospace;
  bool is_vertical;
  bool has_kerning;
  bool has_color;
  bool is_scalable;
  int n_glyphs;
  int n_sizes;
  int n_charmaps;
  std::vector<long> bbox;
  long max_ascend;
  long max_descend;
  long max_advance_h;
  long max_advance_w;
  long lineheight;
  long underline_pos;
  long underline_size;
};

struct GlyphInfo {
  unsigned index;
  long x_bearing;
  long y_bearing;
  long width;
  long height;
  long x_advance;
  long y_advance;
  std::vector<long> bbox;
};

class FaceCache : public LRU_Cache<FaceID, FaceStore, FaceIDHasher> {
public:
  FaceCache() : 
  LRU_Cache<FaceID, FaceStore, FaceIDHasher>() {
    
  }
  FaceCache(size_t max_size) :
  LRU_Cache<FaceID, FaceStore, FaceIDHasher>(max_size) {
    
  }
private:
  virtual void value_dtor(FaceStore& value) {
    FT_Done_Face(value.face);
  }
};

class SizeCache : public LRU_Cache<SizeID, FT_Size, SizeIDHasher> {
public:
  SizeCache() : 
  LRU_Cache<SizeID, FT_Size, SizeIDHasher>() {
    
  }
  SizeCache(size_t max_size) :
  LRU_Cache<SizeID, FT_Size, SizeIDHasher>(max_size) {
    
  }
private:
  virtual void value_dtor(FT_Size& value) {
    FT_Done_Size(value);
  }
};

class FreetypeCache {
public:
  FreetypeCache();
  ~FreetypeCache();

  bool load_font(const char* file, int index, double size, double res);
  FontInfo font_info();
  bool has_glyph(uint32_t index);
  bool load_glyph(uint32_t index);
  GlyphInfo glyph_info();
  GlyphInfo cached_glyph_info(uint32_t index, int& error);
  double string_width(uint32_t* string, int length, bool add_kern);
  long cur_lineheight();
  long cur_ascender();
  long cur_descender();
  bool get_kerning(uint32_t left, uint32_t right, long &x, long &y);
  bool apply_kerning(uint32_t left, uint32_t right, long &x, long &y);
  double tracking_diff(double tracking);
  int error_code;
  
private:
  FT_Library library;
  std::map<uint32_t, GlyphInfo> glyphstore;
  FaceCache face_cache;
  SizeCache size_cache;
  
  FaceID cur_id;
  double cur_size;
  double cur_res;
  bool cur_can_kern;
  unsigned int cur_glyph;
  bool cur_is_scalable;
  double unscaled_scaling;
  
  FT_Face face;
  FT_Size size;
  
  bool load_face(FaceID face);
  bool load_size(FaceID face, double size, double res);
  
  inline bool current_face(FaceID id, double size, double res) {
    return size == cur_size && res == cur_res && id == cur_id;
  };
  
};

#endif
