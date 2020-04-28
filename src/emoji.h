#ifndef EMOJI_INCLUDED
#define EMOJI_INCLUDED

#include <unordered_map>

#define R_NO_REMAP

#include <Rinternals.h>
#include "utils.h"
#include "systemfonts.h"
#include "ft_cache.h"

typedef std::unordered_map<uint32_t, uint8_t> EmojiMap;

// Defined in init.cpp
EmojiMap& get_emoji_map();
FreetypeCache& get_font_cache();

SEXP load_emoji_codes(SEXP all, SEXP default_text, SEXP base_mod);
SEXP emoji_split(SEXP string, SEXP path, SEXP index);

#endif
