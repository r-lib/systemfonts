#ifndef FONT_METRICS_INCLUDED
#define FONT_METRICS_INCLUDED

#include <cstdint>

#define R_NO_REMAP

#include <Rinternals.h>

#include "ft_cache.h"

// Defined in init.cpp
FreetypeCache& get_font_cache();

SEXP get_font_info(SEXP path, SEXP index, SEXP size, SEXP res);
SEXP get_glyph_info(SEXP glyphs, SEXP path, SEXP index, SEXP size, SEXP res);
int glyph_metrics(uint32_t code, const char* fontfile, int index, double size, 
                  double res, double* ascent, double* descent, double* width);

#endif
