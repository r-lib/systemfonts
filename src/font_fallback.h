#pragma once

#include <cpp11/data_frame.hpp>
#include <cpp11/strings.hpp>
#include <cpp11/integers.hpp>
#include <R_ext/Rdynload.h>

#include "types.h"

[[cpp11::register]]
cpp11::writable::data_frame get_fallback_c(cpp11::strings path, cpp11::integers index, cpp11::strings string);

FontSettings request_fallback(const char *string, const char *path, int index);

[[cpp11::init]]
void export_font_fallback(DllInfo* dll);
