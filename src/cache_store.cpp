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
  face = cache.get_face();
  
  END_CPP
  
  *error = 0;
  return face;
}

void export_cache_store(DllInfo* dll) {
  R_RegisterCCallable("systemfonts", "get_cached_face", (DL_FUNC)get_cached_face);
}
