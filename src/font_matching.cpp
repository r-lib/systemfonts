#include "font_matching.h"
#include "Rinternals.h"
#include "types.h"
#include "caches.h"
#include "utils.h"
#include "FontDescriptor.h"
#include "font_registry.h"
#include "font_local.h"

#include <cmath>
#include <string>
#include <cstring>
#include <memory>

#include <cpp11/integers.hpp>
#include <cpp11/doubles.hpp>
#include <cpp11/strings.hpp>
#include <cpp11/function.hpp>
#include <cpp11/r_string.hpp>
#include <cpp11/r_bool.hpp>

using list_t = cpp11::list;
using list_w = cpp11::writable::list;
using data_frame_w = cpp11::writable::data_frame;
using strings_t = cpp11::strings;
using strings_w = cpp11::writable::strings;
using integers_t = cpp11::integers;
using integers_w = cpp11::writable::integers;
using doubles_t = cpp11::doubles;
using doubles_w = cpp11::writable::doubles;
using logicals_t = cpp11::logicals;
using logicals_w = cpp11::writable::logicals;

using namespace cpp11::literals;

// these functions are implemented by the platform
ResultSet *getAvailableFonts();
ResultSet *findFonts(FontDescriptor *);
FontDescriptor *findFont(FontDescriptor *);
void resetFontCache();

void locate_systemfont(const char *family, int italic, int weight, int width, FontSettings2& res) {
  const char* resolved_family = family;
  if (strcmp_no_case(family, "") || strcmp_no_case(family, "sans")) {
    resolved_family = SANS;
  } else if (strcmp_no_case(family, "serif")) {
    resolved_family = SERIF;
  } else if (strcmp_no_case(family, "mono")) {
    resolved_family = MONO;
  } else if (strcmp_no_case(family, "emoji")) {
    resolved_family = EMOJI;
  } else if (strcmp_no_case(family, "symbol")) {
    resolved_family = SYMBOL;
  }
  FontMap& font_map = get_font_map();
  static FontKey key;
  key.family.assign(resolved_family);
  key.weight = weight;
  key.width = width;
  key.italic = italic;
  FontMap::iterator font_it = font_map.find(key);
  if (font_it != font_map.end()) {
    strncpy(res.file, font_it->second.file.c_str(), PATH_MAX);
    res.file[PATH_MAX] = '\0';
    res.index = font_it->second.index;
    res.axes = font_it->second.axes.data();
    res.coords = font_it->second.coords.data();
    res.n_axes = font_it->second.axes.size();
    return;
  }

  FontDescriptor font_desc(resolved_family, fixed_to_italic(italic), fixed_to_weight(weight), fixed_to_width(width));
  std::unique_ptr<FontDescriptor> font_loc(match_local_fonts(&font_desc));
  if (!font_loc) {
    font_loc = std::unique_ptr<FontDescriptor>(findFont(&font_desc));
  }

  FontLoc cached_loc;

  if (!font_loc) {
    list_t fallback = cpp11::as_cpp<list_t>(cpp11::package("systemfonts")["get_fallback"]());
    cached_loc.file = std::string(Rf_translateCharUTF8(STRING_ELT(fallback[0], 0)));
    cached_loc.index = INTEGER(fallback[1])[0];
  } else {
    cached_loc.file = std::string(font_loc->path);
    cached_loc.index = font_loc->index;
    if (font_loc->var_ital) {
      cached_loc.axes.push_back(ITAL_TAG);
      cached_loc.coords.push_back(italic);
    }
    if (font_loc->var_wght) {
      cached_loc.axes.push_back(WGHT_TAG);
      cached_loc.coords.push_back(weight);
    }
    if (font_loc->var_wdth) {
      cached_loc.axes.push_back(WDTH_TAG);
      cached_loc.coords.push_back(width);
    }
  }
  font_map[key] = cached_loc;

  strncpy(res.file, cached_loc.file.c_str(), PATH_MAX);
  res.file[PATH_MAX] = '\0';
  res.index = cached_loc.index;
  res.axes = cached_loc.axes.data();
  res.coords = cached_loc.coords.data();
  res.n_axes = cached_loc.axes.size();
}

int locate_font(const char *family, int italic, int bold, char *path, int max_path_length) {
  FontSettings2 match;

  BEGIN_CPP

  if (!locate_in_registry(family, italic, bold, match)) {
    locate_systemfont(
      family,
      italic_to_fixed(italic),
      weight_to_fixed(bold ? FontWeightBold : FontWeightNormal),
      0,
      match
    );
  }

  END_CPP

  strncpy(path, match.file, max_path_length);
  return match.index;
}

FontSettings locate_font_with_features(const char *family, int italic, int bold) {
  FontSettings2 match = {};

  BEGIN_CPP

  if (!locate_in_registry(family, italic, bold, match)) {
    locate_systemfont(
      family,
      italic_to_fixed(italic),
      weight_to_fixed(bold ? FontWeightBold : FontWeightNormal),
      0,
      match
    );
  }

  END_CPP

  return match;
}

FontSettings2 locate_font_with_features2(const char *family, double italic, double weight, double width, const int* axes, const int* coords, int n_axes) {
  FontSettings2 match = {};

  BEGIN_CPP

  int italic_int = italic_to_fixed(italic);
  int weight_int = weight_to_fixed(FontWeight(weight));
  int width_int = width_to_fixed(FontWidth(width));


  for (int i = 0; i < n_axes; ++i) {
    if (axes[i] == ITAL_TAG) {
      italic_int = coords[i];
    } else if (axes[i] == WGHT_TAG) {
      weight_int = coords[i];
    } else if (axes[i] == WDTH_TAG) {
      width_int = coords[i];
    }
  }

  int weight2 = fixed_to_weight(weight_int);

  if (!locate_in_registry(family, fixed_to_italic(italic_int), weight2 >=650 && weight2 < 750, match)) {
    locate_systemfont(
      family,
      italic_int,
      weight_int,
      width_int,
      match
    );
  }

  END_CPP

  return match;
}

list_t match_font_c(strings_t family, logicals_t italic, logicals_t bold) {
  FontSettings loc = locate_font_with_features(
    Rf_translateCharUTF8(family[0]), italic[0], bold[0]
  );
  integers_w feat(loc.n_features);
  if (loc.n_features == 0) {
    return list_w({
      "path"_nm = cpp11::r_string(loc.file),
      "index"_nm = loc.index,
      "features"_nm = feat
    });
  }
  strings_w tag(loc.n_features);
  for (int i = 0; i < loc.n_features; ++i) {
    feat[i] = loc.features[i].setting;
    tag[i] = cpp11::r_string({
      loc.features[i].feature[0],
      loc.features[i].feature[1],
      loc.features[i].feature[2],
      loc.features[i].feature[3]
    });
  }
  feat.names() = tag;

  return list_w({
    "path"_nm = cpp11::r_string(loc.file),
    "index"_nm = loc.index,
    "features"_nm = feat
  });
}

data_frame_w locate_fonts_c(strings_t family, doubles_t italic,
                            doubles_t weight, doubles_t width) {
  strings_w paths;
  integers_w indices;
  list_w features;
  list_w variations;

  FontSettings2 match = {};

  for (R_xlen_t i = 0; i < family.size(); ++i) {
    const char* fam = Rf_translateCharUTF8(family[i]);
    bool standard_width = (width[i] == FontWidthUndefined || width[i] == FontWidthNormal);
    bool standard_weight = (weight[i] == FontWeightNormal || weight[i] == FontWeightBold || weight[i] == FontWeightUndefined);
    if (!(standard_width && standard_weight && locate_in_registry(fam, italic[i], weight[i] != FontWeightNormal, match))) {
      locate_systemfont(
        fam,
        italic_to_fixed(italic[i]),
        weight_to_fixed(weight[i]),
        width_to_fixed(width[i]),
        match
      );
    }
    paths.push_back(match.file);
    indices.push_back(match.index);
    strings_w tags(match.n_features);
    integers_w vals(match.n_features);
    for (int j = 0; j < match.n_features; ++j) {
      tags[j] = cpp11::r_string({
        match.features[j].feature[0],
        match.features[j].feature[1],
        match.features[j].feature[2],
        match.features[j].feature[3]
      });
      vals[j] = match.features[j].setting;
    }
    list_w f_feat({tags, vals});
    f_feat.attr("class") = {"font_feature"};
    features.push_back(f_feat);
    list_w f_vars({
      "axis"_nm = integers_w(match.axes, match.axes + match.n_axes),
      "value"_nm = integers_w(match.coords, match.coords + match.n_axes)
    });
    f_vars.attr("class") = {"font_variation"};
    variations.push_back(f_vars);
  }

  data_frame_w res({
    "path"_nm = paths,
    "index"_nm = indices,
    "features"_nm = features,
    "variations"_nm = variations
  });
  res.attr("class") = {"tbl_df", "tbl", "data.frame"};

  return res;
}

data_frame_w system_fonts_c() {
  int n = 0;

  std::unique_ptr<ResultSet> all_fonts(getAvailableFonts());
  all_fonts->insert(all_fonts->begin(), get_local_font_list().begin(), get_local_font_list().end());
  n = all_fonts->n_fonts();

  strings_w path(n);
  integers_w index(n);
  strings_w name(n);
  strings_w family(n);
  strings_w style(n);
  integers_w weight(n);
  weight.attr("class") = {"ordered", "factor"};
  weight.attr("levels") = {
    "thin",
    "ultralight",
    "light",
    "normal",
    "medium",
    "semibold",
    "bold",
    "ultrabold",
    "heavy"
  };
  integers_w width(n);
  width.attr("class") = {"ordered", "factor"};
  width.attr("levels") = {
    "ultracondensed",
    "extracondensed",
    "condensed",
    "semicondensed",
    "normal",
    "semiexpanded",
    "expanded",
    "extraexpanded",
    "ultraexpanded"
  };
  logicals_w italic(n);
  logicals_w monospace(n);
  logicals_w variable(n);

  int i = 0;

  for (ResultSet::iterator it = all_fonts->begin(); it != all_fonts->end(); it++) {
    path[i] = (*it)->get_path();
    index[i] = (*it)->index;
    name[i] = (*it)->get_psname();
    family[i] = (*it)->get_family();
    style[i] = (*it)->get_style();
    weight[i] = (*it)->get_weight();
    if (weight[i] == 0) {
      weight[i] = NA_INTEGER;
    }
    width[i] = (*it)->get_width();
    if (width[i] == 0) {
      width[i] = NA_INTEGER;
    }
    italic[i] = (Rboolean) (*it)->italic;
    monospace[i] = (Rboolean) (*it)->monospace;
    variable[i] = (Rboolean) (*it)->variable;
    ++i;
  }

  // Remove local fonts so they don't get deleted at cleanup
  all_fonts->erase(all_fonts->begin(), all_fonts->begin() + get_local_font_list().size());

  data_frame_w res({
    "path"_nm = path,
    "index"_nm = index,
    "name"_nm = name,
    "family"_nm = family,
    "style"_nm = style,
    "weight"_nm = weight,
    "width"_nm = width,
    "italic"_nm = italic,
    "monospace"_nm = monospace,
    "variable"_nm = variable
  });
  res.attr("class") = {"tbl_df", "tbl", "data.frame"};
  return res;
}

void reset_font_cache_c() {
  resetFontCache();
  get_font_map().clear();
}

void export_font_matching(DllInfo* dll) {
  R_RegisterCCallable("systemfonts", "locate_font", (DL_FUNC)locate_font);
  R_RegisterCCallable("systemfonts", "locate_font_with_features", (DL_FUNC)locate_font_with_features);
  R_RegisterCCallable("systemfonts", "locate_font_with_features2", (DL_FUNC)locate_font_with_features2);
}
