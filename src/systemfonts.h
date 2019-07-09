#include <R.h>
#include <Rinternals.h>

int locate_font(const char *family, int italic, int bold, char *path, int max_path_length);
SEXP match_font(SEXP family, SEXP italic, SEXP bold);
SEXP system_fonts();
SEXP dev_string_widths(SEXP strings, SEXP family, SEXP face, SEXP size, SEXP cex, SEXP unit);
SEXP dev_string_metrics(SEXP strings, SEXP family, SEXP face, SEXP size, SEXP cex, SEXP unit);
