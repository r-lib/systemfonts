#include "emoji.h"
#include "utils.h"

bool is_emoji(uint32_t* codepoints, int n, int* result, const char* fontpath, int index) {
  
  BEGIN_CPP
  EmojiMap& emoji_map = get_emoji_map();
  FreetypeCache& cache = get_font_cache();
  bool loaded = cache.load_font(fontpath, index, 12.0, 72.0); // We don't care about sizing
  
  if (!loaded) {
    return false;
  }
  
  for (int i = 0; i < n; ++i) {
    EmojiMap::iterator it = emoji_map.find(codepoints[i]);
    if (it == emoji_map.end()) { // Not an emoji
      result[i] = 0;
      continue;
    }
    switch (it->second) {
    case 0: // Fully qualified emoji codepoint
      result[i] = 1;
      break;
    case 1: // Emoji with text presentation default
      if (i == n - 1) {
        result[i] = 0;
        break;
      }
      if (codepoints[i + 1] == 0xFE0F) {
        result[i] = 1;
        result[i++] = 1;
      } else if (cache.has_glyph(codepoints[i])) {
        result[i] = 0;
      } else {
        result[i] = 1;
      }
      break;
    case 2: // Emoji with text presentation default that can take modifier
      if (i == n - 1) {
        result[i] = 0;
        break;
      }
      if (codepoints[i + 1] >= 0x1F3FB && codepoints[i + 1] <= 0x1F3FF) {
        result[i] = 1;
        result[i++] = 1;
      } else if (cache.has_glyph(codepoints[i])) {
        result[i] = 0;
      } else {
        result[i] = 1;
      }
      break;
    default: // should not be reached
      result[i] = 0;
    }
  }
  
  return true;
  
  END_CPP
}

SEXP load_emoji_codes(SEXP all, SEXP default_text, SEXP base_mod) {
  
  BEGIN_CPP
  EmojiMap& emoji_map = get_emoji_map();
  
  int* all_p = INTEGER(all);
  int* text_p = INTEGER(default_text);
  int* base_p = INTEGER(base_mod);
  
  for (int i = 0; i < LENGTH(all); ++i) {
    emoji_map[all_p[i]] = 0;
  }
  for (int i = 0; i < LENGTH(default_text); ++i) {
    emoji_map[text_p[i]] = 1;
  }
  for (int i = 0; i < LENGTH(base_mod); ++i) {
    emoji_map[base_p[i]] = 2;
  }
  
  return R_NilValue;
  
  END_CPP
}

SEXP emoji_split(SEXP string, SEXP path, SEXP index) {
  int n_strings = LENGTH(string);
  bool one_path = LENGTH(path) == 1;
  const char* first_path = Rf_translateCharUTF8(STRING_ELT(path, 0));
  int first_index = INTEGER(index)[0];
  
  int cur_size = 0;
  int it = 0;
  SEXP glyph, id, emoji;
  PROTECT_INDEX pr_gl, pr_id, pr_em;
  PROTECT_WITH_INDEX(glyph = Rf_allocVector(INTSXP, cur_size), &pr_gl);
  PROTECT_WITH_INDEX(id = Rf_allocVector(INTSXP, cur_size), &pr_id);
  PROTECT_WITH_INDEX(emoji = Rf_allocVector(LGLSXP, cur_size), &pr_em);
  int* glyph_p = INTEGER(glyph);
  int* id_p = INTEGER(id);
  int* emoji_p = LOGICAL(emoji);
  
  BEGIN_CPP
  for (int i = 0; i < n_strings; ++i) {
    int n_glyphs = 0;
    
    UTF_UCS utf_converter;
    uint32_t* glyphs = utf_converter.convert(Rf_translateCharUTF8(STRING_ELT(string, i)), n_glyphs);
    
    int req_size = it + n_glyphs;
    if (cur_size < req_size) {
      int new_size = cur_size * 2;
      new_size = new_size < req_size ? req_size : new_size;
      REPROTECT(glyph = Rf_lengthgets(glyph, new_size), pr_gl);
      glyph_p = INTEGER(glyph);
      REPROTECT(id = Rf_lengthgets(id, new_size), pr_id);
      id_p = INTEGER(id);
      REPROTECT(emoji = Rf_lengthgets(emoji, new_size), pr_em);
      emoji_p = INTEGER(emoji);
      cur_size = new_size;
    }
    
    is_emoji(glyphs, n_glyphs, &emoji_p[it], one_path ? first_path : Rf_translateCharUTF8(STRING_ELT(path, i)), one_path ? first_index : INTEGER(index)[i]);
    
    for (int j = 0; j < n_glyphs; j++) {
      glyph_p[it] = glyphs[j];
      id_p[it] = i;
      it++;
    }
  }
  END_CPP
  
  SEXP res = PROTECT(Rf_allocVector(VECSXP, 3));
  SET_VECTOR_ELT(res, 0, Rf_lengthgets(glyph, it));
  SET_VECTOR_ELT(res, 1, Rf_lengthgets(id, it));
  SET_VECTOR_ELT(res, 2, Rf_lengthgets(emoji, it));
  UNPROTECT(4);
  return res;
}
