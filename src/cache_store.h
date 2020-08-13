#ifndef CACHESTORE_INCLUDED
#define CACHESTORE_INCLUDED

#include "ft_cache.h"

// Defined in init.cpp
FreetypeCache& get_font_cache();

int get_cached_face(const char* file, int index, double size, double res, void * face);

#endif
