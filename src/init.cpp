#include <R.h>
#include <Rinternals.h>
#include <stdlib.h> // for NULL
#include <R_ext/Rdynload.h>
#include "systemfonts.h"


static const R_CallMethodDef CallEntries[] = {
  {"match_font_c", (DL_FUNC) &match_font, 3},
  {NULL, NULL, 0}
};

extern "C" void R_init_systemfonts(DllInfo *dll) {
  R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
  R_useDynamicSymbols(dll, FALSE);
}
