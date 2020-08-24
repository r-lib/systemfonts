#include "dev_metrics.h"

#include <cstring>
#include <cpp11/named_arg.hpp>

#include <R_ext/GraphicsEngine.h>

using doubles_t = cpp11::doubles;
using doubles_w = cpp11::writable::doubles;
using strings_t = cpp11::strings;
using integers_t = cpp11::integers;
using data_frame_w = cpp11::writable::data_frame;

using namespace cpp11::literals;

doubles_t dev_string_widths_c(strings_t string, strings_t family, integers_t face, doubles_t size, doubles_t cex, integers_t unit) {
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
  doubles_w res(n_total);
  
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

data_frame_w dev_string_metrics_c(strings_t string, strings_t family, integers_t face, doubles_t size, doubles_t cex, integers_t unit) {
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
  doubles_w w(n_total);
  doubles_w a(n_total);
  doubles_w d(n_total);
  
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
  data_frame_w res({
    "width"_nm = w,
    "ascent"_nm = a,
    "descent"_nm = d
  });
  res.attr("class") = {"tbl_df", "tbl", "data.frame"};
  
  return res;
}
