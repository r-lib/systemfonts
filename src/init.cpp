#include <R.h>
#include <Rinternals.h>
#include <stdlib.h> // for NULL
#include <R_ext/Rdynload.h>

#include <map>

#include "systemfonts.h"
#include "FontDescriptor.h"
#include "ft_cache.h"
#include "font_metrics.h"
#include "string_metrics.h"

static ResultSet* fonts;

ResultSet& get_font_list() {
  return *fonts;
}

static FontReg* font_registry;

FontReg& get_font_registry() {
  return *font_registry;
}

static FreetypeCache* font_cache;

FreetypeCache& get_font_cache() {
  return *font_cache;
}

static const R_CallMethodDef CallEntries[] = {
  {"match_font_c", (DL_FUNC) &match_font, 3},
  {"system_fonts_c", (DL_FUNC) &system_fonts, 0},
  {"dev_string_widths_c", (DL_FUNC) &dev_string_widths, 6},
  {"dev_string_metrics_c", (DL_FUNC) &dev_string_metrics, 6},
  {"register_font_c", (DL_FUNC) &register_font, 3},
  {"clear_registry_c", (DL_FUNC) &clear_registry, 0},
  {"registry_fonts_c", (DL_FUNC) &registry_fonts, 0},
  {"get_font_info_c", (DL_FUNC) &get_font_info, 4},
  {"get_glyph_info_c", (DL_FUNC) &get_glyph_info, 5},
  {"get_string_shape_c", (DL_FUNC) &get_string_shape, 10},
  {"get_line_width_c", (DL_FUNC) &get_line_width, 6},
  {NULL, NULL, 0}
};

extern "C" void R_init_systemfonts(DllInfo *dll) {
  R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
  R_useDynamicSymbols(dll, FALSE);

  fonts = new ResultSet();
  font_registry = new FontReg();
  font_cache = new FreetypeCache();

  R_RegisterCCallable("systemfonts", "locate_font", (DL_FUNC)locate_font);
  R_RegisterCCallable("systemfonts", "glyph_metrics", (DL_FUNC)glyph_metrics);
  R_RegisterCCallable("systemfonts", "string_width", (DL_FUNC)string_width);
  R_RegisterCCallable("systemfonts", "string_shape", (DL_FUNC)string_shape);
}

extern "C" void R_unload_systemfonts(DllInfo *dll) {
  delete fonts;
  delete font_registry;
  delete font_cache;
}
