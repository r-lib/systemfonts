#ifndef SYSTEMFONTS_INCLUDED
#define SYSTEMFONTS_INCLUDED

#include <map>
#include <string>

#include <R.h>
#include <Rinternals.h>

typedef std::pair<std::string, unsigned int> FontLoc;
typedef std::vector<FontLoc> FontCollection;
typedef std::map<std::string, FontCollection> FontReg;

// Defined in init.cpp
FontReg& get_font_registry();


int locate_font(const char *family, int italic, int bold, char *path, int max_path_length);
SEXP match_font(SEXP family, SEXP italic, SEXP bold);
SEXP system_fonts();
SEXP dev_string_widths(SEXP strings, SEXP family, SEXP face, SEXP size, SEXP cex, SEXP unit);
SEXP dev_string_metrics(SEXP strings, SEXP family, SEXP face, SEXP size, SEXP cex, SEXP unit);
SEXP register_font(SEXP family, SEXP paths, SEXP indices);
  
#endif
