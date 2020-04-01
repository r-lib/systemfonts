#include "ft_cache.h"
#include "systemfonts.h"

static FT_Error face_requester(FTC_FaceID face_id, FT_Library library,
                               FT_Pointer request_data, FT_Face *aface ) {
  FaceID* face = (FaceID*) face_id;   // simple typecase
    
  return FT_New_Face(library, face->first.c_str(), face->second, aface);
}

FreetypeCache::FreetypeCache() : cached_unscaled_loaded(false) {
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
  if (cached_unscaled_loaded) {
    FT_Done_Face(cached_unscaled_face);
  }
  FTC_Manager_Done(manager);
  FT_Done_FreeType(library);
}

bool FreetypeCache::load_font(const char* file, int index, double size, double res) {
  FaceID id = {std::string(file), index};
  
  if (current_face(id, size, res)) {
    return true;
  }
  
  cur_is_scaled = true;
  
  if (id == cached_unscaled_id) {
    cur_is_scaled = false;
    return load_cached_unscaled(size, res);
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
  if (!FT_IS_SCALABLE(temp_face)) {
    cur_is_scaled = false;
    return load_new_unscaled(id, size, res);
  }
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

bool FreetypeCache::load_cached_unscaled(double req_size, double req_res) {
  face = cached_unscaled_face;
  if (req_size != cur_cached_unscaled_size || req_res != cur_cached_unscaled_res) {
    if (face->num_fixed_sizes == 0) {
      error_code = 23;
      return false;
    }
    int best_match = 0;
    int diff = 1e6;
    for (int i = 0; i < face->num_fixed_sizes; ++i) {
      int ndiff = face->available_sizes[i].size / 64 - req_size;
      if (ndiff >= 0 && ndiff < diff) {
        best_match = i;
        diff = ndiff;
      }
    }
    
    FT_Error err = FT_Select_Size(face, best_match);
    error_code = err;
    if (err != 0) {
      return false;
    }
    cur_cached_unscaled_size = req_size;
    cur_cached_unscaled_res = req_res;
    cached_unscaled_scaling = req_size / (face->available_sizes[best_match].size / 64);
    unscaled_glyphstore.clear();
  }
  size = face->size;
  cur_size = req_size;
  cur_res = req_res;
  cur_can_kern = FT_HAS_KERNING(face);
  cur_has_size = false;
  return true;
}

bool FreetypeCache::load_new_unscaled(FaceID id, double req_size, double req_res) {
  if (cached_unscaled_loaded) {
    FT_Done_Face(cached_unscaled_face);
  }
  FT_Error err = FT_New_Face(library, id.first.c_str(), id.second, &cached_unscaled_face);
  error_code = err;
  if (err != 0) {
    return false;
  }
  cached_unscaled_loaded = true;
  cur_cached_unscaled_res = 0.0;
  cur_cached_unscaled_res = 0.0;
  cur_id = id;
  return load_cached_unscaled(req_size, req_res);
}

bool FreetypeCache::has_glyph(u_int32_t index) {
  FT_UInt glyph_id;
  if (cur_is_scaled) {
    glyph_id = FTC_CMapCache_Lookup(charmaps, (FTC_FaceID) &cur_id, -1, index);
  } else {
    glyph_id = FT_Get_Char_Index(face, index);
  }
  return glyph_id != 0;
}

bool FreetypeCache::load_glyph(u_int32_t index) {
  FT_UInt glyph_id;
  if (cur_is_scaled) {
    glyph_id = FTC_CMapCache_Lookup(charmaps, (FTC_FaceID) &cur_id, -1, index);
  } else {
    glyph_id = FT_Get_Char_Index(face, index);
  }
  FT_Error err;
  err = FT_Load_Glyph(face, glyph_id, cur_is_scaled ? FT_LOAD_NO_BITMAP : FT_LOAD_DEFAULT);
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
#ifdef FT_HAS_COLOR
  res.has_color = FT_HAS_COLOR(face);
#else
  res.has_color = false;
#endif
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
  
  if (res.y_advance != 0) { // Vertical
    res.x_bearing = face->glyph->metrics.vertBearingX;
    res.y_bearing = face->glyph->metrics.vertBearingY;
  } else {
    res.x_bearing = face->glyph->metrics.horiBearingX;
    res.y_bearing = face->glyph->metrics.horiBearingY;
  }
  
  res.bbox = {res.x_bearing, res.x_bearing + res.width,
              res.y_bearing - res.height, res.y_bearing};
  
  if (!cur_is_scaled) {
    res.width *= cached_unscaled_scaling;
    res.height *= cached_unscaled_scaling;
    res.x_advance *= cached_unscaled_scaling;
    res.y_advance *= cached_unscaled_scaling;
    res.x_bearing *= cached_unscaled_scaling;
    res.y_bearing *= cached_unscaled_scaling;
    res.bbox[0] *= cached_unscaled_scaling;
    res.bbox[1] *= cached_unscaled_scaling;
    res.bbox[2] *= cached_unscaled_scaling;
    res.bbox[3] *= cached_unscaled_scaling;
  }
  
  return res;
}

GlyphInfo FreetypeCache::cached_glyph_info(u_int32_t index, int& error) {
  std::map<u_int32_t, GlyphInfo>* active_store = cur_is_scaled ? &glyphstore : &unscaled_glyphstore;
  
  std::map<u_int32_t, GlyphInfo>::iterator cached_gi = active_store->find(index);
  GlyphInfo info;
  error = 0;
  
  if (cached_gi == active_store->end()) {
    if (load_glyph(index)) {
      info = glyph_info();
      (*active_store)[index] = info;
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

double FreetypeCache::tracking_diff(double tracking) {
  if (cur_has_size) {
    return (double) FT_MulFix(face->units_per_EM, size->metrics.x_scale) * tracking / 1000;
  } else {
    return (double) face->units_per_EM * tracking / 1000;
  }
};
