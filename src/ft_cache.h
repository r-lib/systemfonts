#pragma once

#include <cpp11/R.hpp>
#include <R_ext/Rdynload.h>
#include <cstdint>
#include <vector>
#include <string>
#include <map>
#include <unordered_set>
#include <memory>
#include <functional>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_TYPES_H
#include FT_SIZES_H
#include FT_TRUETYPE_TABLES_H
#include FT_MULTIPLE_MASTERS_H

#ifdef __EMSCRIPTEN__
#undef TYPEOF
#endif

#include "cache_lru.h"


struct FaceID {
  std::string file;
  unsigned int index;

  inline FaceID() : file(""), index(0) {}
  inline FaceID(std::string f) : file(f), index(0) {}
  inline FaceID(std::string f, unsigned int i) : file(f), index(i) {}
  inline FaceID(const FaceID& face) : file(face.file), index(face.index) {}

  inline bool operator==(const FaceID &other) const {
    return (index == other.index && file == other.file);
  }
};
struct SizeID {
  FaceID face;
  double size;
  double res;

  inline SizeID() : face(), size(-1.0), res(-1.0) {}
  inline SizeID(FaceID f, double s, double r) : face(f), size(s), res(r) {}
  inline SizeID(const SizeID& s) : face(s.face), size(s.size), res(s.res) {}

  inline bool operator==(const SizeID &other) const {
    return (size == other.size && res == other.res && face == other.face);
  }
};

namespace std {
template <>
struct hash<FaceID> {
  size_t operator()(const FaceID & x) const {
    return std::hash<std::string>()(x.file) ^ std::hash<unsigned int>()(x.index);
  }
};
template<>
struct hash<SizeID> {
  size_t operator()(const SizeID & x) const {
    return std::hash<FaceID>()(x.face) ^ std::hash<double>()(x.size) ^ std::hash<double>()(x.res);
  }
};
}

struct FaceStore {
  FT_Face face;
  std::unordered_set<SizeID> sizes;

  FaceStore() : sizes() {};
  FaceStore(FT_Face f) : face(f), sizes() {}
};

struct FontFaceInfo {
  std::string family;
  std::string style;
  std::string name;
  bool is_italic;
  bool is_bold;
  bool is_monospace;
  int weight;
  int width;
  bool is_vertical;
  bool has_kerning;
  bool has_color;
  bool is_scalable;
  int n_glyphs;
  int n_sizes;
  int n_charmaps;
  std::vector<std::string> charmaps;
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
  std::string name;
  long x_bearing;
  long y_bearing;
  long width;
  long height;
  long x_advance;
  long y_advance;
  std::vector<long> bbox;
};

struct VariationInfo {
  std::string name;
  double min;
  double max;
  double def;
  double set;
};

class FaceCache : public LRU_Cache<FaceID, FaceStore> {
  using typename LRU_Cache<FaceID, FaceStore>::key_value_t;
  using typename LRU_Cache<FaceID, FaceStore>::list_t;
  using typename LRU_Cache<FaceID, FaceStore>::cache_list_it_t;
  using typename LRU_Cache<FaceID, FaceStore>::map_t;
  using typename LRU_Cache<FaceID, FaceStore>::cache_map_it_t;

public:
  FaceCache() :
  LRU_Cache<FaceID, FaceStore>() {

  }
  FaceCache(size_t max_size) :
  LRU_Cache<FaceID, FaceStore>(max_size) {

  }

  void add_size_id(FaceID fid, SizeID sid) {
    cache_map_it_t it = _cache_map.find(fid);
    if (it == _cache_map.end()) {
      return;
    }
    it->second->second.sizes.insert(sid);
  }
private:
  inline virtual void value_dtor(FaceStore& value) {
    FT_Done_Face(value.face);
  }
};

class SizeCache : public LRU_Cache<SizeID, FT_Size> {
public:
  SizeCache() :
  LRU_Cache<SizeID, FT_Size>() {

  }
  SizeCache(size_t max_size) :
  LRU_Cache<SizeID, FT_Size>(max_size) {

  }
private:
  inline virtual void value_dtor(FT_Size& value) {
    FT_Done_Size(value);
  }
};

class FreetypeCache {
public:
  FreetypeCache();
  ~FreetypeCache();

  bool load_font(const char* file, int index, double size, double res);
  bool load_font(const char* file, int index);
  FontFaceInfo font_info();
  bool has_glyph(uint32_t index);
  bool load_unicode(uint32_t index);
  bool load_glyph(FT_UInt index, int flags = FT_LOAD_DEFAULT);
  GlyphInfo glyph_info();
  GlyphInfo cached_glyph_info(uint32_t index, int& error);
  double string_width(uint32_t* string, int length, bool add_kern);
  long cur_lineheight();
  long cur_ascender();
  long cur_descender();
  bool cur_is_variable();
  bool get_kerning(uint32_t left, uint32_t right, long &x, long &y);
  bool apply_kerning(uint32_t left, uint32_t right, long &x, long &y);
  double tracking_diff(double tracking);
  FT_Face get_face();
  FT_Face get_referenced_face();
  int get_weight();
  int get_width();
  void get_family_name(char* family, int max_length);
  std::string cur_name();
  std::vector<VariationInfo> cur_axes();
  void has_axes(bool& weight, bool& width, bool& italic);
  int n_axes();
  void set_axes(const int* axes, const int* vals, size_t n);
  int error_code;

private:
  FT_Library library;
  std::map<uint32_t, GlyphInfo> glyphstore;
  FaceCache face_cache;
  SizeCache size_cache;

  FaceID cur_id;
  int cur_var;
  double cur_size;
  double cur_res;
  bool cur_can_kern;
  unsigned int cur_glyph;
  bool cur_is_scalable;
  bool cur_has_variations;
  double unscaled_scaling;

  FT_Face face;
  FT_Size size;

  bool load_face(FaceID face);
  bool load_size(FaceID face, double size, double res);

  inline bool current_face(FaceID id, double size, double res) {
    return size == cur_size && res == cur_res && id == cur_id;
  };

  bool is_variable();

};

FreetypeCache& get_font_cache();

[[cpp11::init]]
void init_ft_caches(DllInfo* dll);

void unload_ft_caches(DllInfo* dll);
