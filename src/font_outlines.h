#pragma once

#include <cpp11/data_frame.hpp>
#include <cpp11/doubles.hpp>
#include <cpp11/integers.hpp>
#include <cpp11/strings.hpp>

[[cpp11::register]]
cpp11::writable::data_frame get_glyph_outlines(cpp11::integers glyph, cpp11::strings path, cpp11::integers index, cpp11::doubles size, double tolerance, bool verbose);

[[cpp11::register]]
cpp11::writable::list get_glyph_bitmap(cpp11::integers glyph, cpp11::strings path, cpp11::integers index, cpp11::doubles size, cpp11::doubles res, cpp11::integers color, bool verbose);

[[cpp11::init]]
void export_font_outline(DllInfo* dll);
