#include <R.h>
#include <Rinternals.h>

void locate_font(const char *family, int italic, int bold, char *path, int max_path_length);
SEXP match_font(SEXP family, SEXP italic, SEXP bold);
SEXP system_fonts();