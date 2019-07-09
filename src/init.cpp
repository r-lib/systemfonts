#include <R.h>
#include <Rinternals.h>
#include <stdlib.h> // for NULL
#include <R_ext/Rdynload.h>

#include <map>

#include "systemfonts.h"
#include "FontDescriptor.h"

static ResultSet* fonts;

ResultSet& get_font_list() {
  return *fonts;
}

static const R_CallMethodDef CallEntries[] = {
  {"match_font_c", (DL_FUNC) &match_font, 3},
  {"system_fonts_c", (DL_FUNC) &system_fonts, 0},
  {"dev_string_widths_c", (DL_FUNC) &dev_string_widths, 6},
  {"dev_string_metrics_c", (DL_FUNC) &dev_string_metrics, 6},
  {"register_font_c", (DL_FUNC) &register_font, 3},
  {NULL, NULL, 0}
};

extern "C" void R_init_systemfonts(DllInfo *dll) {
  R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
  R_useDynamicSymbols(dll, FALSE);

  fonts = new ResultSet();
  font_registry = new FontReg();

  R_RegisterCCallable("systemfonts", "locate_font", (DL_FUNC)locate_font);
}

extern "C" void R_unload_systemfonts(DllInfo *dll) {
  delete fonts;
  delete font_registry;
}
