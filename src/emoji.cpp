#include "emoji.h"
#include "types.h"
#include "caches.h"
#include "utils.h"

#include <cpp11/logicals.hpp>

using list_t = cpp11::list;
using list_w = cpp11::writable::list;
using strings_t = cpp11::strings;
using strings_w = cpp11::writable::strings;
using integers_t = cpp11::integers;
using integers_w = cpp11::writable::integers;
using logicals_t = cpp11::logicals;
using logicals_w = cpp11::writable::logicals;

using namespace cpp11::literals;

bool has_emoji(const char* string) {
  UTF_UCS utf_converter;
  int n_glyphs = 0;
  uint32_t* codepoints = utf_converter.convert(string, n_glyphs);
  EmojiMap& emoji_map = get_emoji_map();
  
  for (int i = 0; i < n_glyphs; ++i) {
    EmojiMap::iterator it = emoji_map.find(codepoints[i]);
    if (it == emoji_map.end()) { // Not an emoji
      continue;
    }
    switch (it->second) {
    case 0: // Fully qualified emoji codepoint
      return true;
    case 1: // Emoji with text presentation default
      if (i != n_glyphs - 1 && codepoints[i + 1] == 0xFE0F) {
        return true;
      }
      break;
    case 2: // Emoji with text presentation default that can take modifier
      if (i != n_glyphs - 1 && codepoints[i + 1] >= 0x1F3FB && codepoints[i + 1] <= 0x1F3FF) {
        return true;
      }
      break;
    }
  }
  
  return false;
}

void detect_emoji_embedding(const uint32_t* codepoints, int n, int* embedding, const char* fontpath, int index) {
  EmojiMap& emoji_map = get_emoji_map();
  FreetypeCache& cache = get_font_cache();
  bool loaded = cache.load_font(fontpath, index, 12.0, 72.0); // We don't care about sizing
  
  for (int i = 0; i < n; ++i) {
    EmojiMap::iterator it = emoji_map.find(codepoints[i]);
    if (it == emoji_map.end()) { // Not an emoji
      embedding[i] = 0;
      continue;
    }
    switch (it->second) {
    case 0: // Fully qualified emoji codepoint
      embedding[i] = 1;
      break;
    case 1: // Emoji with text presentation default
      if (i == n - 1) {
        embedding[i] = 0;
        break;
      }
      if (codepoints[i + 1] == 0xFE0F) {
        embedding[i] = 1;
        embedding[i + 1] = 1;
        ++i;
      } else if (loaded && cache.has_glyph(codepoints[i])) {
        embedding[i] = 0;
      } else {
        embedding[i] = 1;
      }
      break;
    case 2: // Emoji with text presentation default that can take modifier
      if (i == n - 1) {
        embedding[i] = 0;
        break;
      }
      if (codepoints[i + 1] >= 0x1F3FB && codepoints[i + 1] <= 0x1F3FF) {
        embedding[i] = 1;
        embedding[i + 1] = 1;
        ++i;
      } else if (loaded && cache.has_glyph(codepoints[i])) {
        embedding[i] = 0;
      } else {
        embedding[i] = 1;
      }
      break;
    default: // should not be reached
      embedding[i] = 0;
    }
  }
}

bool is_emoji(uint32_t* codepoints, int n, logicals_w &result, const char* fontpath, int index) {
  EmojiMap& emoji_map = get_emoji_map();
  FreetypeCache& cache = get_font_cache();
  bool loaded = cache.load_font(fontpath, index, 12.0, 72.0); // We don't care about sizing
  
  if (!loaded) {
    return false;
  }
  
  for (int i = 0; i < n; ++i) {
    EmojiMap::iterator it = emoji_map.find(codepoints[i]);
    if (it == emoji_map.end()) { // Not an emoji
      result.push_back(FALSE);
      continue;
    }
    switch (it->second) {
    case 0: // Fully qualified emoji codepoint
      result.push_back(TRUE);
      break;
    case 1: // Emoji with text presentation default
      if (i == n - 1) {
        result.push_back(FALSE);
        break;
      }
      if (codepoints[i + 1] == 0xFE0F) {
        result.push_back(TRUE);
        result.push_back(TRUE);
        ++i;
      } else if (cache.has_glyph(codepoints[i])) {
        result.push_back(FALSE);
      } else {
        result.push_back(TRUE);
      }
      break;
    case 2: // Emoji with text presentation default that can take modifier
      if (i == n - 1) {
        result.push_back(FALSE);
        break;
      }
      if (codepoints[i + 1] >= 0x1F3FB && codepoints[i + 1] <= 0x1F3FF) {
        result.push_back(TRUE);
        result.push_back(TRUE);
        ++i;
      } else if (cache.has_glyph(codepoints[i])) {
        result.push_back(FALSE);
      } else {
        result.push_back(TRUE);
      }
      break;
    default: // should not be reached
      result.push_back(FALSE);
    }
  }
  
  return true;
}

void load_emoji_codes_c(integers_t all, integers_t default_text, integers_t base_mod) {
  EmojiMap& emoji_map = get_emoji_map();
  
  for (int i = 0; i < all.size(); ++i) {
    emoji_map[all[i]] = 0;
  }
  for (int i = 0; i < default_text.size(); ++i) {
    emoji_map[default_text[i]] = 1;
  }
  for (int i = 0; i < base_mod.size(); ++i) {
    emoji_map[base_mod[i]] = 2;
  }
}

list_t emoji_split_c(strings_t string, strings_t path, integers_t index) {
  int n_strings = string.size();
  bool one_path = path.size() == 1;
  const char* first_path = Rf_translateCharUTF8(path[0]);
  int first_index = index[0];
  
  integers_w glyph;
  integers_w id;
  logicals_w emoji;
  
  UTF_UCS utf_converter;
  
  for (int i = 0; i < n_strings; ++i) {
    int n_glyphs = 0;
    
    uint32_t* glyphs = utf_converter.convert(Rf_translateCharUTF8(string[i]), n_glyphs);
    
    is_emoji(glyphs, n_glyphs, emoji, one_path ? first_path : Rf_translateCharUTF8(path[i]), one_path ? first_index : index[i]);
    
    for (int j = 0; j < n_glyphs; j++) {
      glyph.push_back(glyphs[j]);
      id.push_back(i);
    }
  }
  
  return list_w({(SEXP) glyph, (SEXP) id, (SEXP) emoji});
}

void export_emoji_detection(DllInfo* dll){
  R_RegisterCCallable("systemfonts", "detect_emoji_embedding", (DL_FUNC)detect_emoji_embedding);
}
