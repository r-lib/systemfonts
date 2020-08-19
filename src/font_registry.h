#pragma once

#include "types.h"

#include <cpp11/strings.hpp>
#include <cpp11/integers.hpp>
#include <cpp11/data_frame.hpp>

using namespace cpp11;

[[cpp11::register]]
void register_font_c(strings family, strings paths, integers indices, strings features, integers settings);

[[cpp11::register]]
void clear_registry_c();

[[cpp11::register]]
writable::data_frame registry_fonts_c();

bool locate_in_registry(const char *family, int italic, int bold, FontSettings& res);
