#pragma once

#include "types.h"

#include <cpp11/strings.hpp>
#include <cpp11/integers.hpp>
#include <cpp11/data_frame.hpp>

[[cpp11::register]]
void register_font_c(cpp11::strings family, cpp11::strings paths, cpp11::integers indices, cpp11::strings features, cpp11::integers settings);

[[cpp11::register]]
void clear_registry_c();

[[cpp11::register]]
cpp11::writable::data_frame registry_fonts_c();

bool locate_in_registry(const char *family, int italic, int bold, FontSettings& res);
