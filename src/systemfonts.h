#ifndef SYSTEMFONTS_INCLUDED
#define SYSTEMFONTS_INCLUDED

#include <unordered_map>
#include <string>
#include <vector>
#include <utility>

#include <R.h>
#include <Rinternals.h>

typedef std::pair<std::string, unsigned int> FontLoc;
typedef std::vector<FontLoc> FontCollection;
typedef std::unordered_map<std::string, FontCollection> FontReg;
typedef std::unordered_map<u_int32_t, u_int8_t> EmojiMap;

// Defined in init.cpp
FontReg& get_font_registry();


int locate_font(const char *family, int italic, int bold, char *path, int max_path_length);
SEXP match_font(SEXP family, SEXP italic, SEXP bold);
SEXP system_fonts();
SEXP dev_string_widths(SEXP strings, SEXP family, SEXP face, SEXP size, SEXP cex, SEXP unit);
SEXP dev_string_metrics(SEXP strings, SEXP family, SEXP face, SEXP size, SEXP cex, SEXP unit);
SEXP register_font(SEXP family, SEXP paths, SEXP indices);
SEXP clear_registry();
SEXP registry_fonts();

#endif
