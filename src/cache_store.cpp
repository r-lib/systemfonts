#include "cache_store.h"

int get_cached_face(const char* file, int index, double size, double res, void * face) {
  FreetypeCache& cache = get_font_cache();
  if (!cache.load_font(file, index, size, res)) {
    return cache.error_code;
  }
  face = cache.get_face();
  return 0;
}
