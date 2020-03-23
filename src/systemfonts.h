#ifndef SYSTEMFONTS_INCLUDED
#define SYSTEMFONTS_INCLUDED

#include <unordered_map>
#include <string>
#include <vector>
#include <utility>

#if defined(_WIN32)
#include <stdint.h>
typedef uint8_t u_int8_t;
typedef uint16_t u_int16_t;
typedef uint32_t u_int32_t;
#endif

#include <R.h>
#include <Rinternals.h>

typedef std::pair<std::string, unsigned int> FontLoc;
typedef std::vector<FontLoc> FontCollection;
typedef std::unordered_map<std::string, FontCollection> FontReg;
typedef std::unordered_map<u_int32_t, u_int8_t> EmojiMap;

// Defined in init.cpp
FontReg& get_font_registry();

// Will be set by sf_init
extern SEXP sf_ns_env;

void sf_init(SEXP ns);
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
