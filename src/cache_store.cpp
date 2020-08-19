#include "cache_store.h"

#include "types.h"
#include "caches.h"
#include "utils.h"

int get_cached_face(const char* file, int index, double size, double res, void * face) {
  BEGIN_CPP
  
  FreetypeCache& cache = get_font_cache();
  if (!cache.load_font(file, index, size, res)) {
    return cache.error_code;
  }
  face = cache.get_face();
  
  END_CPP
  
  return 0;
}

void export_cache_store(DllInfo* dll) {
  R_RegisterCCallable("systemfonts", "get_cached_face", (DL_FUNC)get_cached_face);
}
