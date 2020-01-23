#ifndef EMOJI_INCLUDED
#define EMOJI_INCLUDED

#include <unordered_map>
#include <R.h>
#include <Rinternals.h>
#include "utils.h"
#include "systemfonts.h"
#include "ft_cache.h"

typedef std::unordered_map<u_int32_t, u_int8_t> EmojiMap;

// Defined in init.cpp
EmojiMap& get_emoji_map();
FreetypeCache& get_font_cache();

SEXP load_emoji_codes(SEXP all, SEXP default_text, SEXP base_mod);
SEXP emoji_split(SEXP string, SEXP path, SEXP index);

#endif
