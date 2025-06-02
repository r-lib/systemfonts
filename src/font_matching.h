#pragma once

#include <cpp11/list.hpp>
#include <cpp11/strings.hpp>
#include <cpp11/integers.hpp>
#include <cpp11/logicals.hpp>
#include <cpp11/data_frame.hpp>
#include <R_ext/Rdynload.h>

#include "types.h"
#include "caches.h"

// Default fonts based on browser behaviour
#if defined _WIN32
#define SANS "Arial"
#define SERIF "Times New Roman"
#define MONO "Courier New"
#define EMOJI "Segoe UI Emoji"
#define SYMBOL "Segoe UI Symbol"
#elif defined __APPLE__
#define SANS "Helvetica"
#define SERIF "Times"
#define MONO "Courier New"
#define EMOJI "Apple Color Emoji"
#define SYMBOL "Symbol"
#else
#define SANS "sans"
#define SERIF "serif"
#define MONO "mono"
#define EMOJI "emoji"
#define SYMBOL "symbol"
#endif

int locate_font(const char *family, int italic, int bold, char *path, int max_path_length);
FontSettings locate_font_with_features(const char *family, int italic, int bold);
FontSettings2 locate_font_with_features2(const char *family, double italic, double weight, double width, const int* axes, const int* coords, int n_axes);

[[cpp11::register]]
cpp11::list match_font_c(cpp11::strings family, cpp11::logicals italic,
                         cpp11::logicals bold);

[[cpp11::register]]
cpp11::writable::data_frame locate_fonts_c(cpp11::strings family,
                                           cpp11::doubles italic,
                                           cpp11::doubles weight,
                                           cpp11::doubles width);

[[cpp11::register]]
cpp11::writable::data_frame system_fonts_c();

[[cpp11::register]]
void reset_font_cache_c();

[[cpp11::init]]
void export_font_matching(DllInfo* dll);
