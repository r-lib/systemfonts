#include "font_variation.h"
#include "Rinternals.h"
#include "cpp11/integers.hpp"
#include "utils.h"

cpp11::writable::integers axes_to_tags(cpp11::strings axes) {
  cpp11::writable::integers tags(axes.size());
  int* tags_data = INTEGER(tags.data());
  for (R_xlen_t i = 0; i < axes.size(); ++i) {
    tags_data[i] = axis_to_tag(axes[i]);
  }
  return tags;
}

cpp11::writable::strings tags_to_axes(cpp11::integers tags) {
  cpp11::writable::strings axes(tags.size());
  int* tags_data = INTEGER(tags.data());
  for (R_xlen_t i = 0; i < axes.size(); ++i) {
    axes[i] = tag_to_axis(tags_data[i]);
  }
  return axes;
}

cpp11::writable::integers values_to_fixed(cpp11::doubles values) {
  cpp11::writable::integers fixed(values.size());
  for (R_xlen_t i = 0; i < values.size(); ++i) {
    fixed[i] = values[i] * FIXED_MOD;
  }
  return fixed;
}

cpp11::writable::doubles fixed_to_values(cpp11::integers fixed) {
  cpp11::writable::doubles values(fixed.size());
  for (R_xlen_t i = 0; i < fixed.size(); ++i) {
    values[i] = fixed[i] / FIXED_MOD;
  }
  return values;
}
