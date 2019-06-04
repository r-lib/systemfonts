#include <R.h>
#include <Rinternals.h>

#include "systemfonts.h"
#include "FontDescriptor.h"

// these functions are implemented by the platform
ResultSet *getAvailableFonts();
ResultSet *findFonts(FontDescriptor *);
FontDescriptor *findFont(FontDescriptor *);
FontDescriptor *substituteFont(char *, char *);

SEXP match_font(SEXP fontname, SEXP italic, SEXP bold) {
  FontDescriptor font_desc;
  font_desc.family = Rf_translateCharUTF8(STRING_ELT(fontname, 0));;
  font_desc.italic = LOGICAL(italic)[0];
  font_desc.weight = LOGICAL(bold)[0] ? FontWeightBold : FontWeightNormal;

  FontDescriptor* font_loc = findFont(&font_desc);

  SEXP path = PROTECT(Rf_mkString(font_loc->path));
  delete font_loc;
  UNPROTECT(1);
  return path;
}
