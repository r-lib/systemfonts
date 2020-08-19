#include "dev_metrics.h"

#include <cpp11/named_arg.hpp>

#include <R_ext/GraphicsEngine.h>

using namespace cpp11;

doubles dev_string_widths_c(strings string, strings family, integers face, doubles size, doubles cex, integers unit) {
  GEUnit u = GE_INCHES;
  switch (INTEGER(unit)[0]) {
  case 0:
    u = GE_CM;
    break;
  case 1:
    u = GE_INCHES;
    break;
  case 2:
    u = GE_DEVICE;
    break;
  case 3:
    u = GE_NDC;
    break;
  }
  pGEDevDesc dev = GEcurrentDevice();
  R_GE_gcontext gc = {};
  double width = 0;
  int n_total = string.size();
  int scalar_family = family.size() == 1;
  int scalar_rest = face.size() == 1;
  strcpy(gc.fontfamily, Rf_translateCharUTF8(family[0]));
  gc.fontface = face[0];
  gc.ps = size[0];
  gc.cex = cex[0];
  writable::doubles res(n_total);
  
  for (int i = 0; i < n_total; ++i) {
    if (i > 0 && !scalar_family) {
      strcpy(gc.fontfamily, Rf_translateCharUTF8(family[i]));
    }
    if (i > 0 && !scalar_rest) {
      gc.fontface = face[i];
      gc.ps = size[i];
      gc.cex = cex[i];
    }
    width = GEStrWidth(
      CHAR(string[i]), 
      Rf_getCharCE(string[i]), 
      &gc, 
      dev
    );
    res[i] = GEfromDeviceWidth(width, u, dev);
  }
  return res;
}

writable::data_frame dev_string_metrics_c(strings string, strings family, integers face, doubles size, doubles cex, integers unit) {
  GEUnit u = GE_INCHES;
  switch (INTEGER(unit)[0]) {
  case 0:
    u = GE_CM;
    break;
  case 1:
    u = GE_INCHES;
    break;
  case 2:
    u = GE_DEVICE;
    break;
  case 3:
    u = GE_NDC;
    break;
  }
  pGEDevDesc dev = GEcurrentDevice();
  R_GE_gcontext gc = {};
  double width = 0, ascent = 0, descent = 0;
  int n_total = string.size();
  int scalar_family = family.size() == 1;
  int scalar_rest = face.size() == 1;
  strcpy(gc.fontfamily, Rf_translateCharUTF8(family[0]));
  gc.fontface = face[0];
  gc.ps = size[0];
  gc.cex = cex[0];
  writable::doubles w(n_total);
  writable::doubles a(n_total);
  writable::doubles d(n_total);
  
  for (int i = 0; i < n_total; ++i) {
    if (i > 0 && !scalar_family) {
      strcpy(gc.fontfamily, Rf_translateCharUTF8(family[i]));
    }
    if (i > 0 && !scalar_rest) {
      gc.fontface = face[i];
      gc.ps = size[i];
      gc.cex = cex[i];
    }
    GEStrMetric(
      CHAR(string[i]), 
      Rf_getCharCE(string[i]), 
      &gc,
      &ascent, &descent, &width,
      dev
    );
    w[i] = GEfromDeviceWidth(width, u, dev);
    a[i] = GEfromDeviceWidth(ascent, u, dev);
    d[i] = GEfromDeviceWidth(descent, u, dev);
  }
  writable::data_frame res({
    "width"_nm = w,
    "ascent"_nm = a,
    "descent"_nm = d
  });
  res.attr("class") = writable::strings({"tbl_df", "tbl", "data.frame"});
  
  return res;
}
