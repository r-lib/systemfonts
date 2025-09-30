#include "cache_store.h"

#include "types.h"
#include "caches.h"
#include "utils.h"

FT_Face get_cached_face(const char* file, int index, double size, double res, int* error) {
  FT_Face face = nullptr;
  BEGIN_CPP

  FreetypeCache& cache = get_font_cache();
  if (!cache.load_font(file, index, size, res)) {
    *error = cache.error_code;
    return face;
  }
  face = cache.get_referenced_face();

  END_CPP

  *error = 0;
  return face;
}

FT_Face get_cached_face2(const FontSettings2& font, double size, double res, int* error) {
  FT_Face face = nullptr;
  BEGIN_CPP

  FreetypeCache& cache = get_font_cache();
  if (!cache.load_font(font.file, font.index, size, res)) {
    *error = cache.error_code;
    return face;
  }
  cache.set_axes(font.axes, font.coords, font.n_axes);
  face = cache.get_referenced_face();

  END_CPP

  *error = 0;
  return face;
}

bool check_ft_version(int major, int minor, int patch) {
  return major == FREETYPE_MAJOR && minor == FREETYPE_MINOR && patch == FREETYPE_PATCH;
}

void export_cache_store(DllInfo* dll) {
  R_RegisterCCallable("systemfonts", "get_cached_face", (DL_FUNC)get_cached_face);
  R_RegisterCCallable("systemfonts", "get_cached_face2", (DL_FUNC)get_cached_face2);
  R_RegisterCCallable("systemfonts", "check_ft_version", (DL_FUNC)check_ft_version);
}
