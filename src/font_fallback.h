#pragma once

#include <cpp11/data_frame.hpp>
#include <cpp11/strings.hpp>
#include <cpp11/integers.hpp>
#include <cpp11/list_of.hpp>
#include <R_ext/Rdynload.h>

#include "types.h"

[[cpp11::register]]
cpp11::writable::data_frame get_fallback_c(cpp11::strings path, cpp11::integers index, cpp11::strings string, cpp11::list_of<cpp11::list> variations);

FontSettings request_fallback(const char *string, const char *path, int index);
FontSettings2 request_fallback2(const char *string, const FontSettings2& font);

[[cpp11::init]]
void export_font_fallback(DllInfo* dll);
