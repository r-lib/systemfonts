#include "ft_cache.h"
#include "FontDescriptor.h"
#include "R_ext/Print.h"
#include "utils.h"
#include <cmath>
#include <cstdint>
#include <cpp11/protect.hpp>
#include <cpp11/list_of.hpp>
#include <cpp11/integers.hpp>
#include <functional>
#include <string>
#include <vector>
#include <cstring>

FreetypeCache::FreetypeCache()
  : error_code(0),
    glyphstore(),
    face_cache(16),
    size_cache(32),
    cur_id(),
    cur_var(0),
    cur_size(-1),
    cur_res(-1),
    cur_can_kern(false),
    cur_glyph(0),
    cur_has_variations(false)
  {
  FT_Error err = FT_Init_FreeType(&library);
  if (err != 0) {
    cpp11::stop("systemfonts failed to initialise the freetype font cache");
  }
}
FreetypeCache::~FreetypeCache() {
  FT_Done_FreeType(library);
}

bool FreetypeCache::load_font(const char* file, int index, double size, double res) {
  FaceID id(std::string(file), index);

  if (current_face(id, size, res)) {
    return true;
  }

  if (!load_face(id)) {
    return false;
  }

  if (!load_size(id, size, res)) {
    return false;
  }

  cur_id = id;
  cur_var = 0;
  cur_size = size;
  cur_res = res;
  glyphstore.clear();

  cur_can_kern = FT_HAS_KERNING(face);
  cur_has_variations = is_variable();

  return true;
}

bool FreetypeCache::load_font(const char* file, int index) {
  std::string file_str(file);
  FaceID id(file_str, index);

  if (id == cur_id) {
    return true;
  }

  if (!load_face(id)) {
    return false;
  }

  cur_id = id;
  cur_var = 0;
  cur_size = -1;
  cur_res = -1;
  glyphstore.clear();

  cur_can_kern = FT_HAS_KERNING(face);

  return true;
}

bool FreetypeCache::load_face(FaceID face) {
  if (face == cur_id) {
    return true;
  }

  FaceStore cached_face;
  if (face_cache.get(face, cached_face)) {
    this->face = cached_face.face;
    cur_is_scalable = FT_IS_SCALABLE(this->face);
    return true;
  }
  FT_Face new_face;
  FT_Error err = FT_New_Face(this->library, face.file.c_str(), face.index, &new_face);
  if (err != 0) {
    error_code = err;
    err = FT_New_Face(this->library, face.file.c_str(), 0, &new_face);
    if (err != 0) {
      return false;
    }

  }
  this->face = new_face;
  cur_is_scalable = FT_IS_SCALABLE(new_face);
  if (face_cache.add(face, FaceStore(new_face), cached_face)) {
    for(std::unordered_set<SizeID>::iterator it = cached_face.sizes.begin(); it != cached_face.sizes.end(); ++it) {
      size_cache.remove(*it);
    }
    FT_Done_Face(cached_face.face);
  }
  return true;
}

bool FreetypeCache::load_size(FaceID face, double size, double res) {
  SizeID id(face, size, res);
  FT_Size cached_size;
  SizeID cached_id;
  FaceStore cached_face;
  if (size_cache.get(id, cached_size)) {
    FT_Activate_Size(cached_size);
    this->size = cached_size;
    return true;
  }
  FT_Size new_size;
  FT_Error err = FT_New_Size(this->face, &new_size);
  if (err != 0) {
    error_code = err;
    return false;
  }
  FT_Size old_size = this->face->size;
  FT_Activate_Size(new_size);

  if (cur_is_scalable) {
    err = FT_Set_Char_Size(this->face, 0, size * 64, res, res);
    if (err != 0) {
      error_code = err;
      FT_Activate_Size(old_size);
      return false;
    }
  } else {
    if (this->face->num_fixed_sizes == 0) {
      error_code = 23;
      FT_Activate_Size(old_size);
      return false;
    }
    int best_match = 0;
    int diff = 1e6;
    int scaled_size = 64 * size * res / 72;
    int largest_size = 0;
    int largest_ind = -1;
    bool found_match = false;
    for (int i = 0; i < this->face->num_fixed_sizes; ++i) {
      if (this->face->available_sizes[i].size > largest_size) {
        largest_ind = i;
      }
      int ndiff = this->face->available_sizes[i].size - scaled_size;
      if (ndiff >= 0 && ndiff < diff) {
        best_match = i;
        diff = ndiff;
        found_match = true;
      }
    }
    if (!found_match && scaled_size >= largest_size) {
      best_match = largest_ind;
    }

    err = FT_Select_Size(this->face, best_match);
    if (err != 0) {
      error_code = err;
      FT_Activate_Size(old_size);
      return false;
    }
    unscaled_scaling = 1;
  }
  if (size_cache.add(id, new_size, cached_id)) {
    if (face_cache.get(cached_id.face, cached_face)) {
      cached_face.sizes.erase(cached_id);
    }
  }

  face_cache.add_size_id(face, id);

  this->size = new_size;
  return true;
}

bool FreetypeCache::has_glyph(uint32_t index) {
  FT_UInt glyph_id = FT_Get_Char_Index(face, index);
  return glyph_id != 0;
}

bool FreetypeCache::load_unicode(uint32_t index) {
  FT_UInt glyph_id = FT_Get_Char_Index(face, index);
  return load_glyph(glyph_id);
}

bool FreetypeCache::load_glyph(FT_UInt id, int flags) {
  FT_Error err = 0;
  err = FT_Load_Glyph(face, id, flags);
  error_code = err;
  if (err == 0) {
    cur_glyph = id;
  }
  return err == 0;
}

std::string enc_to_string(FT_Encoding_ enc) {
  switch(enc) {
    case FT_ENCODING_NONE: return "none";
    case FT_ENCODING_UNICODE: return "unicode";
    case FT_ENCODING_MS_SYMBOL: return "microsoft symbol";
    case FT_ENCODING_SJIS: return "shift jis";
    case FT_ENCODING_PRC: return "prc";
    case FT_ENCODING_BIG5: return "big5";
    case FT_ENCODING_WANSUNG: return "extended wansung";
    case FT_ENCODING_JOHAB: return "johab";
    case FT_ENCODING_ADOBE_LATIN_1: return "abobe latin-1";
    case FT_ENCODING_ADOBE_STANDARD: return "adobe standard";
    case FT_ENCODING_ADOBE_EXPERT: return "adobe expert";
    case FT_ENCODING_ADOBE_CUSTOM: return "adobe custom";
    default: return "";
  }
}

FontInfo FreetypeCache::font_info() {
  FontInfo res = {};
  res.family = std::string(face->family_name);
  res.style = std::string(face->style_name);
  res.name = cur_name();
  res.is_italic = face->style_flags & FT_STYLE_FLAG_ITALIC;
  res.is_bold = face->style_flags & FT_STYLE_FLAG_BOLD;
  res.weight = get_weight();
  res.width = get_width();
  res.is_monospace = FT_IS_FIXED_WIDTH(face);
  res.is_vertical = FT_HAS_VERTICAL(face);
  res.has_kerning = cur_can_kern;
#ifdef FT_HAS_COLOR
  res.has_color = FT_HAS_COLOR(face);
#else
  res.has_color = false;
#endif
  res.is_scalable = cur_is_scalable;
  res.n_glyphs = face->num_glyphs;
  res.n_sizes = face->num_fixed_sizes;
  res.n_charmaps = face->num_charmaps;
  for (size_t i = 0; i < face->num_charmaps; ++i) {
    res.charmaps.push_back(enc_to_string(face->charmaps[i]->encoding));
  }
  res.bbox = {
    FT_MulFix(face->bbox.xMin, size->metrics.x_scale),
    FT_MulFix(face->bbox.xMax, size->metrics.x_scale),
    FT_MulFix(face->bbox.yMin, size->metrics.y_scale),
    FT_MulFix(face->bbox.yMax, size->metrics.y_scale)
  };
  res.max_ascend = FT_MulFix(face->ascender, size->metrics.y_scale);
  res.max_descend = FT_MulFix(face->descender, size->metrics.y_scale);
  res.max_advance_h = FT_MulFix(face->max_advance_height, size->metrics.y_scale);
  res.max_advance_w = FT_MulFix(face->max_advance_width, size->metrics.x_scale);
  res.lineheight = FT_MulFix(face->height, size->metrics.y_scale);
  res.underline_pos = FT_MulFix(face->underline_position, size->metrics.y_scale);
  res.underline_size = FT_MulFix(face->underline_thickness, size->metrics.y_scale);

  if (cur_has_variations) {
    FT_MM_Var* variations = nullptr;
    int error = FT_Get_MM_Var(face, &variations);
    if (error == 0) {
      std::vector<FT_Fixed> set_val(variations->num_axis);
      error = FT_Get_Var_Design_Coordinates(face, variations->num_axis, set_val.data());
      if (error == 0) {
        for (FT_UInt i = 0; i < variations->num_axis; ++i) {
          if (variations->axis[i].tag == ITAL_TAG) {
            res.is_italic = fixed_to_italic(set_val[i]);
          } else if (variations->axis[i].tag == WGHT_TAG) {
            res.weight = fixed_to_weight(set_val[i]);
            res.is_bold = res.weight >= FontWeightBold;
          } else if (variations->axis[i].tag == WDTH_TAG) {
            res.width = fixed_to_width(set_val[i]);
          }
        }
      }
      FT_Done_MM_Var(library, variations);
    }
  }

  return res;
}

GlyphInfo FreetypeCache::glyph_info() {
  static char name_buffer[50];
  GlyphInfo res = {};

  res.index = cur_glyph;
  res.width = face->glyph->metrics.width;
  res.height = face->glyph->metrics.height;
  res.x_advance = face->glyph->advance.x;
  res.y_advance = face->glyph->advance.y;

  if (res.y_advance != 0) { // Vertical
    res.x_bearing = face->glyph->metrics.vertBearingX;
    res.y_bearing = face->glyph->metrics.vertBearingY;
  } else {
    res.x_bearing = face->glyph->metrics.horiBearingX;
    res.y_bearing = face->glyph->metrics.horiBearingY;
  }

  res.bbox = {res.x_bearing, res.x_bearing + res.width,
              res.y_bearing - res.height, res.y_bearing};

  if (!cur_is_scalable) {

    res.width *= unscaled_scaling;
    res.height *= unscaled_scaling;
    res.x_advance *= unscaled_scaling;
    res.y_advance *= unscaled_scaling;
    res.x_bearing *= unscaled_scaling;
    res.y_bearing *= unscaled_scaling;
    res.bbox[0] *= unscaled_scaling;
    res.bbox[1] *= unscaled_scaling;
    res.bbox[2] *= unscaled_scaling;
    res.bbox[3] *= unscaled_scaling;
  }

  if (FT_HAS_GLYPH_NAMES(face)) {
    FT_Get_Glyph_Name(face, cur_glyph, name_buffer, 50);
    res.name = std::string(name_buffer);
  } else {
    res.name = "";
  }

  return res;
}

GlyphInfo FreetypeCache::cached_glyph_info(uint32_t index, int& error) {
  std::map<uint32_t, GlyphInfo>::iterator cached_gi = glyphstore.find(index);
  GlyphInfo info = {};
  error = 0;

  if (cached_gi == glyphstore.end()) {
    if (load_unicode(index)) {
      info = glyph_info();
      glyphstore[index] = info;
    } else {
      error = error_code;
    }
  } else {
    info = cached_gi->second;
  }

  return info;
}

long FreetypeCache::cur_lineheight() {
  return FT_MulFix(face->height, size->metrics.y_scale);
}
long FreetypeCache::cur_ascender() {
  return FT_MulFix(face->ascender, size->metrics.y_scale);
}
long FreetypeCache::cur_descender() {
  return FT_MulFix(face->descender, size->metrics.y_scale);
}
bool FreetypeCache::cur_is_variable() {
  return cur_has_variations;
}
bool FreetypeCache::get_kerning(uint32_t left, uint32_t right, long &x, long &y) {
  x = 0;
  y = 0;
  // Early exit
  if (!cur_can_kern) return true;

  FT_UInt left_id = FT_Get_Char_Index(face, left);
  FT_UInt right_id = FT_Get_Char_Index(face, right);

  FT_Vector delta = {};

  FT_Error error = FT_Get_Kerning(face, left_id, right_id, FT_KERNING_DEFAULT, &delta);

  if (error != 0) {
    error_code = error;
    return false;
  }
  x = delta.x;
  y = delta.y;

  return true;
}
bool FreetypeCache::apply_kerning(uint32_t left, uint32_t right, long &x, long &y) {
  long delta_x = 0, delta_y = 0;

  if (!get_kerning(left, right, delta_x, delta_y)) {
    return false;
  }

  x += delta_x;
  y += delta_y;

  return true;
}

double FreetypeCache::tracking_diff(double tracking) {
  return (double) FT_MulFix(face->units_per_EM, size->metrics.x_scale) * tracking / 1000;
}

FT_Face FreetypeCache::get_face() {
  return face;
}

FT_Face FreetypeCache::get_referenced_face() {
  FT_Reference_Face(face);
  return face;
}

std::string FreetypeCache::cur_name() {
  const char* ps_name = FT_Get_Postscript_Name(face);
  if (ps_name == NULL) {
    const char* f_name = face->family_name;
    if (f_name == NULL) f_name = "";
    return {f_name};
  }
  return {ps_name};
}

std::vector<VariationInfo> FreetypeCache::cur_axes() {
  std::vector<VariationInfo> axes;

  if (!cur_has_variations) return axes;

  FT_MM_Var* variations = nullptr;
  int error = FT_Get_MM_Var(face, &variations);
  if (error != 0) {
    return axes;
  }

  std::vector<FT_Fixed> set_var(variations->num_axis);
  FT_Get_Var_Design_Coordinates(face, set_var.size(), set_var.data());

  for (FT_UInt i = 0; i < variations->num_axis; ++i) {
    axes.push_back({
      tag_to_axis(variations->axis[i].tag),
      variations->axis[i].minimum / FIXED_MOD,
      variations->axis[i].maximum / FIXED_MOD,
      variations->axis[i].def / FIXED_MOD,
      set_var[i] / FIXED_MOD
    });
  }
  FT_Done_MM_Var(library, variations);
  return axes;
}

void FreetypeCache::has_axes(bool& weight, bool& width, bool& italic) {
  width = false;
  weight = false;
  italic = false;

  if (!cur_has_variations) return;

  FT_MM_Var* variations = nullptr;
  int error = FT_Get_MM_Var(face, &variations);
  if (error == 0) {
    for (FT_UInt i = 0; i < variations->num_axis; ++i) {
      long tag = variations->axis[i].tag;
      if (tag == WGHT_TAG) weight = true;
      else if (tag == WDTH_TAG) width = true;
      else if (tag == ITAL_TAG) italic = true;
    }
    FT_Done_MM_Var(library, variations);
  }
}

int FreetypeCache::n_axes() {
  int n = 0;

  if (!cur_has_variations) return n;

  FT_MM_Var* variations = nullptr;
  int error = FT_Get_MM_Var(face, &variations);
  if (error == 0) {
    n = variations->num_axis;
    FT_Done_MM_Var(library, variations);
  }
  return n;
}

bool FreetypeCache::is_variable() {
  bool variable = false;
  FT_MM_Var* variations = nullptr;
  int error = FT_Get_MM_Var(face, &variations);
  if (error == 0) {
    variable = variations->num_axis != 0;
    FT_Done_MM_Var(library, variations);
  }
  return variable;
}

inline int var_hash(const int* axes, const int* vals, size_t n) {
  int hash = 0;
  for (size_t i = 0; i < n; ++i) {
    hash ^= std::hash<int>()(axes[i]);
    hash ^= std::hash<int>()(vals[i]);
  }
  return hash;
}

void FreetypeCache::set_axes(const int* axes, const int* vals, size_t n) {
  if (!cur_has_variations) {
    cur_var = 0;
    return;
  }

  int this_var = var_hash(axes, vals, n);
  if (this_var == cur_var) return;

  std::vector<FT_Fixed> fvals;

  if (n == 0) {
    FT_Set_Var_Design_Coordinates(face, 0, fvals.data());
    glyphstore.clear();
    cur_var = this_var;
    return;
  }

  FT_MM_Var* variations = nullptr;
  int error = FT_Get_MM_Var(face, &variations);
  if (error != 0) {
    return;
  }
  for (FT_UInt i = 0; i < variations->num_axis; ++i) {
    auto it = std::find(axes, axes + n, (int) variations->axis[i].tag);
    if (it == axes + n) {
      fvals.push_back(variations->axis[i].def);
    } else {
      fvals.push_back(std::min(variations->axis[i].maximum, std::max(variations->axis[i].minimum, FT_Fixed(vals[it - axes]))));
    }
  }
  FT_Done_MM_Var(library, variations);
  FT_Set_Var_Design_Coordinates(face, fvals.size(), fvals.data());
  glyphstore.clear();
  cur_var = this_var;
}

int FreetypeCache::get_weight() {
  // Support for variations
  if (cur_has_variations) {
    FT_MM_Var* variations = nullptr;
    int error = FT_Get_MM_Var(face, &variations);
    if (error == 0) {
      unsigned wght_index = 0;
      for (; wght_index < variations->num_axis; ++wght_index) {
        if (variations->axis[wght_index].tag == WGHT_TAG) {
          break;
        }
      }
      if (wght_index != variations->num_axis) {
        std::vector<FT_Fixed> set_var(variations->num_axis);
        FT_Get_Var_Design_Coordinates(face, set_var.size(), set_var.data());
        return fixed_to_weight(set_var[wght_index]);
      }
    }
  }

  // Classic reading from OS table
  void* table = FT_Get_Sfnt_Table(face, ft_sfnt_os2); // [1] ft_sfnt_os2 is deprecated and should be replaced by FT_SFNT_OS2 (only) in the remote future for compatibility (2021-03-04)
  if (table == NULL) {
    return 0;
  }
  TT_OS2* os2_table = (TT_OS2*) table;
  return os2_table->usWeightClass;
}

int FreetypeCache::get_width() {
  void* table = FT_Get_Sfnt_Table(face, ft_sfnt_os2); // see comment [1] above
  if (table == NULL) {
    return 0;
  }
  TT_OS2* os2_table = (TT_OS2*) table;
  return os2_table->usWidthClass;
}
void FreetypeCache::get_family_name(char* family, int max_length) {
  strncpy(family, face->family_name, max_length);
}

static FreetypeCache* font_cache;

FreetypeCache& get_font_cache() {
  return *font_cache;
}

void init_ft_caches(DllInfo* dll) {
  font_cache = new FreetypeCache();
}

void unload_ft_caches(DllInfo* dll) {
  delete font_cache;
}
