#include <cpp11/R.hpp>
#include "caches.h"

extern "C" void R_unload_systemfonts(DllInfo *dll) {
  unload_caches(dll);
}
