#pragma once

#include <cpp11/strings.hpp>
#include <cpp11/integers.hpp>
#include <cpp11/doubles.hpp>
#include <cpp11/data_frame.hpp>
#include <cstdint>

using namespace cpp11;

[[cpp11::register]]
writable::data_frame get_font_info_c(strings path, integers index, doubles size, doubles res);

[[cpp11::register]]
writable::data_frame get_glyph_info_c(strings glyphs, strings path, integers index, doubles size, doubles res);

int glyph_metrics(uint32_t code, const char* fontfile, int index, double size, 
                  double res, double* ascent, double* descent, double* width);

[[cpp11::init]]
void export_font_metrics(DllInfo* dll);
