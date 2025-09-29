#include "font_outlines.h"
#include "Rinternals.h"
#include "caches.h"
#include "cpp11/data_frame.hpp"
#include "cpp11/doubles.hpp"
#include "cpp11/integers.hpp"
#include "cpp11/list.hpp"
#include "types.h"
#include <cpp11/matrix.hpp>
#include <cpp11/protect.hpp>
#include <cstdint>
#include <cstdlib>

#include <ft2build.h>
#include <string>
#include FT_OUTLINE_H

#include <R_ext/GraphicsEngine.h>

using namespace cpp11::literals;

struct Outline {
  cpp11::writable::integers glyph;
  cpp11::writable::integers contour;
  cpp11::writable::doubles x;
  cpp11::writable::doubles y;

  double last_x;
  double last_y;

  int current_glyph;
  int current_contour;
  double tolerance;

  cpp11::writable::data_frame to_df() {
    return {
      "glyph"_nm = glyph,
      "contour"_nm = contour,
      "x"_nm = x,
      "y"_nm = y
    };
  }
};

void recurse_conic(double x0, double y0, double x1, double y1, double x2, double y2,
                   cpp11::writable::doubles& x, cpp11::writable::doubles& y,
                   double tolerance) {
  double td2x = std::abs(x0 + x2 - x1 - x1);
  double td2y = std::abs(y0 + y2 - y1 - y1);
  double dist = td2x + td2y;

  if (dist * 2.0 <= tolerance) {
    x.push_back(x2 / 64.0);
    y.push_back(y2 / 64.0);
    return;
  }

  double x01 = (x0 + x1) * 0.5;
  double y01 = (y0 + y1) * 0.5;
  double x12 = (x1 + x2) * 0.5;
  double y12 = (y1 + y2) * 0.5;

  double x012 = (x01 + x12) * 0.5;
  double y012 = (y01 + y12) * 0.5;

  recurse_conic(x0, y0, x01, y01, x012, y012, x, y, tolerance);
  recurse_conic(x012, y012, x12, y12, x2, y2, x, y, tolerance);
}

void recurse_cubic(double x0, double y0, double x1, double y1,
                       double x2, double y2, double x3, double y3,
                       cpp11::writable::doubles& x, cpp11::writable::doubles& y,
                       double tolerance) {
  double td2x = std::abs(x0 + x0 + x3 - x1 - x1 - x1);
  double td2y = std::abs(y0 + y0 + y3 - y1 - y1 - y1);
  double td3x = std::abs(x0 + x3 + x3 - x2 - x2 - x2);
  double td3y = std::abs(y0 + y3 + y3 - y2 - y2 - y2);
  double dist = td2x + td2y + td3x + td3y;

  if (dist <= tolerance) {
    x.push_back(x3 / 64.0);
    y.push_back(y3 / 64.0);
    return;
  }

  double x01 = (x0 + x1) * 0.5;
  double y01 = (y0 + y1) * 0.5;
  double x12 = (x1 + x2) * 0.5;
  double y12 = (y1 + y2) * 0.5;
  double x23 = (x2 + x3) * 0.5;
  double y23 = (y2 + y3) * 0.5;

  double x012 = (x01 + x12) * 0.5;
  double y012 = (y01 + y12) * 0.5;
  double x123 = (x12 + x23) * 0.5;
  double y123 = (y12 + y23) * 0.5;

  double x0123 = (x012 + x123) * 0.5;
  double y0123 = (y012 + y123) * 0.5;

  recurse_cubic(x0, y0, x01, y01, x012, y012, x0123, y0123, x, y, tolerance);
  recurse_cubic(x0123, y0123, x123, y123, x23, y23, x3, y3, x, y, tolerance);
}

static int move_func(const FT_Vector *to, void *user) {
  Outline *outlines = static_cast<Outline *>(user);

  outlines->current_contour++;

  outlines->last_x = to->x;
  outlines->last_y = to->y;

  return 0;
}

static int line_func(const FT_Vector *to, void *user) {
  Outline *outlines = static_cast<Outline *>(user);

  outlines->last_x = to->x;
  outlines->last_y = to->y;

  outlines->glyph.push_back(outlines->current_glyph);
  outlines->contour.push_back(outlines->current_contour);
  outlines->x.push_back(double(to->x) / 64.0);
  outlines->y.push_back(double(to->y) / 64.0);

  return 0;
}

static int conic_func(const FT_Vector *control, const FT_Vector *to, void *user) {
  Outline *outlines = static_cast<Outline *>(user);

  R_xlen_t last = outlines->x.size();

  recurse_conic(outlines->last_x, outlines->last_y, control->x, control->y, to->x, to->y, outlines->x, outlines->y, outlines->tolerance);

  for (R_xlen_t i = last; i < outlines->x.size(); ++i) {
    outlines->glyph.push_back(outlines->current_glyph);
    outlines->contour.push_back(outlines->current_contour);
  }

  outlines->last_x = to->x;
  outlines->last_y = to->y;

  return 0;
}

static int cubic_func(const FT_Vector *controlOne, const FT_Vector *controlTwo, const FT_Vector *to, void *user) {
  Outline *outlines = static_cast<Outline *>(user);

  R_xlen_t last = outlines->x.size();

  recurse_cubic(outlines->last_x, outlines->last_y, controlOne->x, controlOne->y, controlTwo->x, controlTwo->y, to->x, to->y, outlines->x, outlines->y, outlines->tolerance);

  for (R_xlen_t i = last; i < outlines->x.size(); ++i) {
    outlines->glyph.push_back(outlines->current_glyph);
    outlines->contour.push_back(outlines->current_contour);
  }

  outlines->last_x = to->x;
  outlines->last_y = to->y;

  return 0;
}

cpp11::writable::data_frame get_glyph_outlines(cpp11::integers glyph, cpp11::strings path, cpp11::integers index, cpp11::doubles size, cpp11::list_of<cpp11::list> variations, double tolerance, bool verbose) {
  Outline outlines;
  // We double the tolerance rather than halve it in the quadratic bezier
  // as that is the most common curve: 64*2 (the 64 takes care of scaling it to
  // glyph units)
  outlines.tolerance = tolerance * 128.0;

  FreetypeCache& cache = get_font_cache();

  FT_Outline_Funcs callbacks;

  callbacks.move_to = move_func;
  callbacks.line_to = line_func;
  callbacks.conic_to = conic_func;
  callbacks.cubic_to = cubic_func;
  callbacks.delta = 0;
  callbacks.shift = 0;

  cpp11::writable::integers unscallable;

  for (R_xlen_t i = 0; i < glyph.size(); ++i) {
    if (!cache.load_font(std::string(path[i]).c_str(), index[i], size[i], 72.0)) {
      if (verbose) {
        cpp11::warning("Failed to load %s:%i with freetype error %i", std::string(path[i]).c_str(), index[i], cache.error_code);
      }
      continue;
    }
    if (!FT_IS_SCALABLE(cache.get_face())) {
      if (verbose) {
        cpp11::warning("%s:%i does not provide outlines", std::string(path[i]).c_str(), index[i]);
      }
      unscallable.push_back(i+1);
      continue;
    }

    cache.set_axes(INTEGER(variations[i]["axis"]), INTEGER(variations[i]["value"]), Rf_xlength(variations[i]["axis"]));

    if (!cache.load_glyph(glyph[i])) {
      if (verbose) {
        cpp11::warning("Failed to load glyph %i in %s:%i with freetype error %i", glyph[i], std::string(path[i]).c_str(), index[i], cache.error_code);
      }
      continue;
    }

    FT_GlyphSlot& slot = cache.get_face()->glyph;
    FT_Outline& outline = slot->outline;

    if (slot->format != FT_GLYPH_FORMAT_OUTLINE) {
      if (verbose) {
        cpp11::warning("Glyph %i in %s:%i does not provide an outline", glyph[i], std::string(path[i]).c_str(), index[i]);
      }
      unscallable.push_back(i+1);
      continue;
    }
    if (outline.n_contours <= 0 || outline.n_points <= 0) {
      continue;
    }

    outlines.current_glyph = i + 1;
    outlines.current_contour = 0;

    R_xlen_t last_size = outlines.glyph.size();

    FT_Error error = FT_Outline_Decompose(&outline, &callbacks, &outlines);
    if (error) {
      if (verbose) {
        cpp11::warning("Couldn't extract outline from glyph %i in %s:%i with freetype error %i", glyph[i], std::string(path[i]).c_str(), index[i], error);
      }
      outlines.glyph.resize(last_size);
      outlines.contour.resize(last_size);
      outlines.x.resize(last_size);
      outlines.y.resize(last_size);
      unscallable.push_back(i+1);
      continue;
    }
    if (outlines.contour[outlines.contour.size() - 1] != outlines.contour[outlines.contour.size() - 2]) {
      // Terminal point is singular
      outlines.glyph.pop_back();
      outlines.contour.pop_back();
      outlines.x.pop_back();
      outlines.y.pop_back();
    }
  }

  cpp11::writable::data_frame outlines_df = outlines.to_df();
  outlines_df.attr("missing") = unscallable;

  return outlines_df;
}

inline uint8_t demultiply(uint8_t a, uint8_t b) {
  if (a * b == 0) return 0;
  if (a > b) return 255;
  return (a * 255 + (b >> 1)) / b;
}
inline uint8_t multiply(uint8_t a, uint8_t b) {
  uint32_t t = a * b + 127;
  return ((t >> 8) + t) >> 8;
}

double set_font_size(FT_Face face, int size) {
  int best_match = 0;
  int diff = 1e6;
  int largest_size = 0;
  int largest_ind = -1;
  bool found_match = false;
  for (int i = 0; i < face->num_fixed_sizes; ++i) {
    if (face->available_sizes[i].size > largest_size) {
      largest_ind = i;
    }
    int ndiff = face->available_sizes[i].size - size;
    if (ndiff >= 0 && ndiff < diff) {
      best_match = i;
      diff = ndiff;
      found_match = true;
    }
  }
  if (!found_match && size >= largest_size) {
    best_match = largest_ind;
  }
  FT_Select_Size(face, best_match);
  return double(size) / double(face->size->metrics.height);
}

SEXP one_glyph_bitmap(int glyph, const char* path, int index, double size, double res, const int* axes, const int* coords, int n_axes, int color, FreetypeCache& cache, bool verbose) {
  if (!cache.load_font(path, index, size, res)) {
    if (verbose) {
      cpp11::warning("Failed to load %s:%i with freetype error %i", path, index, cache.error_code);
    }
    return R_NilValue;
  }

  cache.set_axes(axes, coords, n_axes);

  double scaling = 72.0 / res;

  if (!FT_IS_SCALABLE(cache.get_face())) {
    scaling *= set_font_size(cache.get_face(), size * res * 64.0 / 72.0);
  }

  if (!cache.load_glyph(glyph, FT_HAS_COLOR(cache.get_face()) ? FT_LOAD_COLOR : FT_LOAD_DEFAULT)) {
    if (verbose) {
      cpp11::warning("Failed to load glyph %i in %s:%i with freetype error %i", glyph, path, index, cache.error_code);
    }
    return R_NilValue;
  }

  FT_GlyphSlot& slot = cache.get_face()->glyph;

  FT_Error error = FT_Render_Glyph(slot, FT_RENDER_MODE_NORMAL);
  if (error != 0) {
    if (verbose) {
      cpp11::warning("Failed to render glyph %i in %s:%i with freetype error %i", glyph, path, index, error);
    }
    return R_NilValue;
  }

  int red = R_RED(color);
  int green = R_GREEN(color);
  int blue = R_BLUE(color);
  int alpha = R_ALPHA(color);

  FT_Bitmap& bitmap = slot->bitmap;

  if (bitmap.pixel_mode != FT_PIXEL_MODE_GRAY && bitmap.pixel_mode != FT_PIXEL_MODE_BGRA) {
    if (verbose) {
      cpp11::warning("Unsupported pixel mode for glyph %i in %s:%i", glyph, path, index);
    }
    return R_NilValue;
  }

  const unsigned char* buffer = bitmap.buffer;
  SEXP raster = PROTECT(Rf_allocMatrix(INTSXP, bitmap.width, bitmap.rows));
  int* raster_p = INTEGER(raster);
  size_t offset = 0;
  for (size_t j = 0; j < bitmap.rows; ++j) {
    for (int k = 0; k < bitmap.pitch; ++k) {
      size_t index = k + j*bitmap.width;
      switch (bitmap.pixel_mode) {
        case FT_PIXEL_MODE_GRAY: {
          if (buffer[offset + k] == 0) raster_p[index] = R_RGBA(0, 0, 0, 0);
          else raster_p[index] = R_RGBA(red, green, blue, multiply(alpha, buffer[offset + k]));
          break;
        };
        case FT_PIXEL_MODE_BGRA: {
          size_t index = offset + k * 4;
          raster_p[index] = R_RGBA(
            demultiply(buffer[index + 2], buffer[index + 3]),
            demultiply(buffer[index + 1], buffer[index + 3]),
            demultiply(buffer[index + 0], buffer[index + 3]),
            buffer[index + 3]
          );
          break;
        };
      }
    }
    offset += bitmap.pitch;
  }
  double offset_top = slot->bitmap_top;
  if (!strcmp("Apple Color Emoji", cache.get_face()->family_name)) {
    offset_top -= bitmap.rows * 0.1;
  }
  SEXP dims = PROTECT(Rf_allocVector(INTSXP, 2));
  INTEGER(dims)[0] = bitmap.rows;
  INTEGER(dims)[1] = bitmap.width;
  Rf_setAttrib(raster, R_DimSymbol, dims);
  SEXP channels = PROTECT(Rf_ScalarInteger(4));
  Rf_setAttrib(raster, Rf_mkString("channels"), channels);
  Rf_classgets(raster, Rf_mkString("nativeRaster"));
  SEXP raster_offset = PROTECT(Rf_allocVector(REALSXP, 2));
  REAL(raster_offset)[0] = offset_top * scaling;
  REAL(raster_offset)[1] = double(slot->bitmap_left) * scaling;
  Rf_setAttrib(raster, Rf_mkString("offset"), raster_offset);
  SEXP raster_size = PROTECT(Rf_allocVector(REALSXP, 2));
  REAL(raster_size)[0] = double(bitmap.rows) * scaling;
  REAL(raster_size)[1] = double(bitmap.width) * scaling;
  Rf_setAttrib(raster, Rf_mkString("size"), raster_size);
  UNPROTECT(5);
  return raster;
}

cpp11::writable::list get_glyph_bitmap(cpp11::integers glyph, cpp11::strings path, cpp11::integers index, cpp11::doubles size, cpp11::doubles res, cpp11::list_of<cpp11::list> variations, cpp11::integers color, bool verbose) {
  cpp11::writable::list bitmaps;

  FreetypeCache& cache = get_font_cache();

  for (R_xlen_t i = 0; i < glyph.size(); ++i) {
    SEXP bitmap = PROTECT(
      one_glyph_bitmap(
        glyph[i],
        std::string(path[i]).c_str(),
        index[i],
        size[i],
        res[i],
        INTEGER(variations[i]["axis"]),
        INTEGER(variations[i]["value"]),
        Rf_xlength(variations[i]["axis"]),
        color[i],
        cache,
        verbose
      )
    );
    bitmaps.push_back(bitmap);
  }

  UNPROTECT(glyph.size());

  return bitmaps;
}

struct Path {
  std::string path;

  double* transformation;

  Path(double* t) : path(""), transformation(t) {}

  void add_point(double _x, double _y) {
    _x *= 0.015625;
    _y *= 0.015625;
    double x = transformation[0] * _x + transformation[2] * _y + transformation[4];
    double y = transformation[1] * _x + transformation[3] * _y + transformation[5];
    path += std::to_string(x) + " ";
    path += std::to_string(y) + " ";
  }
};

static int move_func_a(const FT_Vector *to, void *user) {
  Path *outline = static_cast<Path *>(user);

  if (!outline->path.empty()) {
    outline->path += "Z M ";
  } else {
    outline->path += "M ";
  }

  outline->add_point(to->x, to->y);

  return 0;
}

static int line_func_a(const FT_Vector *to, void *user) {
  Path *outline = static_cast<Path *>(user);

  outline->path += "L ";
  outline->add_point(to->x, to->y);

  return 0;
}

static int conic_func_a(const FT_Vector *control, const FT_Vector *to, void *user) {
  Path *outline = static_cast<Path *>(user);

  outline->path += "Q ";
  outline->add_point(control->x, control->y);
  outline->add_point(to->x, to->y);

  return 0;
}

static int cubic_func_a(const FT_Vector *controlOne, const FT_Vector *controlTwo, const FT_Vector *to, void *user) {
  Path *outline = static_cast<Path *>(user);

  outline->path += "C ";
  outline->add_point(controlOne->x, controlOne->y);
  outline->add_point(controlTwo->x, controlTwo->y);
  outline->add_point(to->x, to->y);

  return 0;
}

std::string get_glyph_path_impl(int glyph, double* t, bool* no_outline, const char* path, int index) {
  Path path_outline(t);
  *no_outline = false;

  FreetypeCache& cache = get_font_cache();

  FT_Outline_Funcs callbacks;

  callbacks.move_to = move_func_a;
  callbacks.line_to = line_func_a;
  callbacks.conic_to = conic_func_a;
  callbacks.cubic_to = cubic_func_a;
  callbacks.delta = 0;
  callbacks.shift = 0;

  if (!FT_IS_SCALABLE(cache.get_face())) {
    *no_outline = true;
    return "";
  }
  if (!cache.load_glyph(glyph)) {
    cpp11::warning("Failed to load glyph %i in %s:%i with freetype error %i", glyph, path, index, cache.error_code);
    return "";
  }

  FT_GlyphSlot& slot = cache.get_face()->glyph;
  FT_Outline& outline = slot->outline;

  if (slot->format != FT_GLYPH_FORMAT_OUTLINE) {
    *no_outline = true;
    return "";
  }
  if (outline.n_contours <= 0 || outline.n_points <= 0) {
    return "";
  }

  FT_Error error = FT_Outline_Decompose(&outline, &callbacks, &path_outline);
  if (error) {
    cpp11::warning("Couldn't extract outline from glyph %i in %s:%i with freetype error %i", glyph, path, index, error);
    return "";
  }

  return path_outline.path;
}
std::string get_glyph_path(int glyph, double* t, const char* path, int index, double size, bool* no_outline) {
  FreetypeCache& cache = get_font_cache();
  if (!cache.load_font(path, index, size, 72.0)) {
    cpp11::warning("Failed to load %s:%i with freetype error %i", path, index, cache.error_code);
    return "";
  }
  return get_glyph_path_impl(glyph, t, no_outline, path, index);
}
std::string get_glyph_path2(int glyph, double* t, const FontSettings2& font, double size, bool* no_outline) {
  FreetypeCache& cache = get_font_cache();
  if (!cache.load_font(font.file, font.index, size, 72.0)) {
    cpp11::warning("Failed to load %s:%i with freetype error %i", font.file, font.index, cache.error_code);
    return "";
  }
  cache.set_axes(font.axes, font.coords, font.n_axes);
  return get_glyph_path_impl(glyph, t, no_outline, font.file, font.index);
}
SEXP get_glyph_raster(int glyph, const char* path, int index, double size, double res, int color) {
  FreetypeCache& cache = get_font_cache();
  return one_glyph_bitmap(
    glyph,
    path,
    index,
    size,
    res,
    nullptr,
    nullptr,
    0,
    color,
    cache,
    true
  );
}

SEXP get_glyph_raster2(int glyph, const FontSettings2& font, double size, double res, int color) {
  FreetypeCache& cache = get_font_cache();
  return one_glyph_bitmap(
    glyph,
    font.file,
    font.index,
    size,
    res,
    font.axes,
    font.coords,
    font.n_axes,
    color,
    cache,
    true
  );
}

void export_font_outline(DllInfo* dll) {
  R_RegisterCCallable("systemfonts", "get_glyph_path", (DL_FUNC)get_glyph_path);
  R_RegisterCCallable("systemfonts", "get_glyph_path2", (DL_FUNC)get_glyph_path2);
  R_RegisterCCallable("systemfonts", "get_glyph_raster", (DL_FUNC)get_glyph_raster);
  R_RegisterCCallable("systemfonts", "get_glyph_raster2", (DL_FUNC)get_glyph_raster2);
}

