#ifndef SYSTEMFONTS_INCLUDED
#define SYSTEMFONTS_INCLUDED

#include <unordered_map>
#include <string>
#include <vector>
#include <cstdint>
#include <utility>

#define R_NO_REMAP

#include <Rinternals.h>

typedef std::pair<std::string, unsigned int> FontLoc;
typedef std::vector<FontLoc> FontCollection;
typedef std::unordered_map<std::string, FontCollection> FontReg;
typedef std::unordered_map<uint32_t, uint8_t> EmojiMap;

typedef std::tuple<std::string, int, int> FontKey;
struct key_hash : public std::unary_function<FontKey, std::size_t> {
  inline std::size_t operator()(const FontKey& k) const
  {
    return std::get<0>(k)[0] ^ std::get<1>(k) ^ std::get<2>(k);
  }
};
struct key_equal : public std::binary_function<FontKey, FontKey, bool> {
  inline bool operator()(const FontKey& v0, const FontKey& v1) const
  {
    return (
        std::get<0>(v0) == std::get<0>(v1) &&
          std::get<1>(v0) == std::get<1>(v1) &&
          std::get<2>(v0) == std::get<2>(v1)
    );
  }
};
typedef std::unordered_map<FontKey, FontLoc, key_hash, key_equal> FontMap;


// Defined in init.cpp
FontReg& get_font_registry();

// Will be set by sf_init
extern SEXP sf_ns_env;

SEXP sf_init(SEXP ns);
int locate_font(const char *family, int italic, int bold, char *path, int max_path_length);
SEXP match_font(SEXP family, SEXP italic, SEXP bold);
SEXP system_fonts();
SEXP reset_font_cache();
SEXP dev_string_widths(SEXP strings, SEXP family, SEXP face, SEXP size, SEXP cex, SEXP unit);
SEXP dev_string_metrics(SEXP strings, SEXP family, SEXP face, SEXP size, SEXP cex, SEXP unit);
SEXP register_font(SEXP family, SEXP paths, SEXP indices);
SEXP clear_registry();
SEXP registry_fonts();

#endif
