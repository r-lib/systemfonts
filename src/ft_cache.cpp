#include "ft_cache.h"
#include "systemfonts.h"

static FT_Error face_requester(FTC_FaceID face_id, FT_Library library,
                               FT_Pointer request_data, FT_Face *aface ) {
  FaceID* face = (FaceID*) face_id;   // simple typecase
    
  return FT_New_Face(library, face->first.c_str(), face->second, aface);
}

FreetypeCache::FreetypeCache() {
  FT_Error err = FT_Init_FreeType(&library);
  if (err == 0) {
    err = FTC_Manager_New(library, 0, 0, 0, &face_requester, NULL, &manager);
  }
  if (err == 0) {
    err = FTC_CMapCache_New(manager, &charmaps);
  }
  if (err != 0) {
    Rf_error("systemfonts failed to initialise the freetype font cache");
  }
}
FreetypeCache::~FreetypeCache() {
  FTC_Manager_Done(manager);
  FT_Done_FreeType(library);
}

bool FreetypeCache::load_font(const char* file, int index, double size, double res) {
  FaceID id = {std::string(file), index};
  
  if (current_face(id, size, res)) {
    return true;
  }
  
  if (id_lookup.find(id) == id_lookup.end()) {
    // First time using this font
    id_lookup.insert(id);
    std::unique_ptr<FaceID> id_p(new FaceID(id.first, id.second));
    id_pool.push_back(std::move(id_p));
    scaler.face_id = (FTC_FaceID) id_pool.back().get();
  } else {
    scaler.face_id = (FTC_FaceID) &id;
  }
  scaler.width = size * 64;
  scaler.height = size * 64;
  scaler.pixel = false;
  scaler.x_res = res;
  scaler.y_res = res;
  
  FT_Error err;
  FT_Face temp_face;
  err = FTC_Manager_LookupFace(manager, scaler.face_id, &temp_face);
  error_code = err;
  if (err != 0) {
    return false;
  }
  if (!FT_IS_SCALABLE(temp_face)) {
    scaler.pixel = true;
    scaler.width = temp_face->available_sizes[0].x_ppem / 64;
    scaler.height = temp_face->available_sizes[0].y_ppem / 64;
  }
  
  err = FTC_Manager_LookupSize(manager, &scaler, &this->size);
  error_code = err;
  if (err != 0) {
    return false;
    cur_has_size = false;
    face = temp_face;
    this->size = face->size;
  } else {
    cur_has_size = true;
    face = this->size->face;
  }
  
  cur_id = id;
  cur_size = size;
  cur_res = res;
  glyphstore.clear();
  
  cur_can_kern = FT_HAS_KERNING(face);
  return true;
}
bool FreetypeCache::has_glyph(u_int32_t index) {
  FT_UInt glyph_id = FTC_CMapCache_Lookup(charmaps, (FTC_FaceID) &cur_id, -1, index);
  return glyph_id != 0;
}

bool FreetypeCache::load_glyph(u_int32_t index) {
  FT_UInt glyph_id = FTC_CMapCache_Lookup(charmaps, (FTC_FaceID) &cur_id, -1, index);
  FT_Error err;
  err = FT_Load_Glyph(face, glyph_id, FT_LOAD_NO_BITMAP);
  error_code = err;
  if (err == 0) {
    cur_glyph = glyph_id;
  }
  return err == 0;
}

FontInfo FreetypeCache::font_info() {
  FontInfo res;
  res.family = std::string(face->family_name);
  res.style = std::string(face->style_name);
  res.is_italic = face->style_flags & FT_STYLE_FLAG_ITALIC;
  res.is_bold = face->style_flags & FT_STYLE_FLAG_BOLD;
  res.is_monospace = FT_IS_FIXED_WIDTH(face);
  res.is_vertical = FT_HAS_VERTICAL(face);
  res.has_kerning = cur_can_kern;
  res.has_color = FT_HAS_COLOR(face);
  res.is_scalable = FT_IS_SCALABLE(face);
  res.n_glyphs = face->num_glyphs;
  res.n_sizes = face->num_fixed_sizes;
  res.n_charmaps = face->num_charmaps;
  if (cur_has_size) {
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
  } else {
    res.bbox = {
      face->bbox.xMin, 
      face->bbox.xMax, 
      face->bbox.yMin, 
      face->bbox.yMax
    };
    res.max_ascend = face->ascender;
    res.max_descend = face->descender;
    res.max_advance_h = face->max_advance_height;
    res.max_advance_w = face->max_advance_width;
    res.lineheight = face->height;
    res.underline_pos = face->underline_position;
    res.underline_size = face->underline_thickness;
  }
  
  return res;
}

GlyphInfo FreetypeCache::glyph_info() {
  GlyphInfo res;
  
  res.index = cur_glyph;
  res.width = face->glyph->metrics.width;
  res.height = face->glyph->metrics.height;
  res.x_advance = face->glyph->advance.x;
  res.y_advance = face->glyph->advance.y;
  
  if (FT_HAS_VERTICAL(face)) {
    res.x_bearing = face->glyph->metrics.vertBearingX;
    res.y_bearing = face->glyph->metrics.vertBearingY;
  } else {
    res.x_bearing = face->glyph->metrics.horiBearingX;
    res.y_bearing = face->glyph->metrics.horiBearingY;
  }
  
  res.bbox = {res.x_bearing, res.x_bearing + res.width,
              res.y_bearing - res.height, res.y_bearing};
  
  return res;
}

GlyphInfo FreetypeCache::cached_glyph_info(u_int32_t index, int& error) {
  std::map<u_int32_t, GlyphInfo>::iterator cached_gi = glyphstore.find(index);
  GlyphInfo info;
  error = 0;
  
  if (cached_gi == glyphstore.end()) {
    if (load_glyph(index)) {
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

bool FreetypeCache::apply_kerning(u_int32_t left, u_int32_t right, long &x, long &y) {
  // Early exit
  if (!cur_can_kern) return true;
  
  FT_UInt left_id = FTC_CMapCache_Lookup(charmaps, (FTC_FaceID) &cur_id, -1, left);
  FT_UInt right_id = FTC_CMapCache_Lookup(charmaps, (FTC_FaceID) &cur_id, -1, right);
  
  FT_Vector delta;
  
  FT_Error error = FT_Get_Kerning(face, left_id, right_id, FT_KERNING_DEFAULT, &delta);
  
  if (error != 0) {
    error_code = error;
    return false;
  }
  
  x += delta.x;
  y += delta.y;
  
  return true;
}
