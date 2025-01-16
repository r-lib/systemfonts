#pragma once

#include "types.h"
#include "caches.h"
#include "utils.h"

#include <cpp11/strings.hpp>

FontDescriptor *find_first_match(FontDescriptor *desc, ResultSet& font_list);
FontDescriptor *match_local_fonts(FontDescriptor *desc);

[[cpp11::register]]
int add_local_fonts(cpp11::strings paths);

[[cpp11::register]]
void clear_local_fonts_c();
