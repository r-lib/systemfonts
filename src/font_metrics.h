#pragma once

#include <cpp11/strings.hpp>
#include <cpp11/integers.hpp>
#include <cpp11/doubles.hpp>
#include <cpp11/data_frame.hpp>
#include <R_ext/Rdynload.h>
#include <cstdint>

[[cpp11::register]]
cpp11::writable::data_frame get_font_info_c(cpp11::strings path, cpp11::integers index, cpp11::doubles size, cpp11::doubles res);

[[cpp11::register]]
cpp11::writable::data_frame get_glyph_info_c(cpp11::strings glyphs, cpp11::strings path, cpp11::integers index, cpp11::doubles size, cpp11::doubles res);

int glyph_metrics(uint32_t code, const char* fontfile, int index, double size, 
                  double res, double* ascent, double* descent, double* width);

int font_weight(const char* fontfile, int index);
int font_family(const char* fontfile, int index, char* family, int max_length);

[[cpp11::init]]
void export_font_metrics(DllInfo* dll);
