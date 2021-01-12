#pragma once

#include <cpp11/R.hpp>
#include <R_ext/Rdynload.h>
#include "types.h"
#include "FontDescriptor.h"
#include "ft_cache.h"

ResultSet& get_font_list();

FontReg& get_font_registry();

FreetypeCache& get_font_cache();

EmojiMap& get_emoji_map();

FontMap& get_font_map();

WinLinkMap& get_win_link_map();

[[cpp11::init]]
void init_caches(DllInfo* dll);

void unload_caches(DllInfo* dll);
