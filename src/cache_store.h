#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H
#include <cpp11/R.hpp>
#include <R_ext/Rdynload.h>

FT_Face get_cached_face(const char* file, int index, double size, double res, int* error);

[[cpp11::init]]
void export_cache_store(DllInfo* dll);
