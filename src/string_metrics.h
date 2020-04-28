#ifndef STRING_METRICS_INCLUDED
#define STRING_METRICS_INCLUDED

#define R_NO_REMAP

#include <Rinternals.h>

SEXP get_string_shape(SEXP string, SEXP id, SEXP path, SEXP index, SEXP size, 
                      SEXP res, SEXP lineheight, SEXP align, SEXP hjust, 
                      SEXP vjust, SEXP width, SEXP tracking, SEXP indent, 
                      SEXP hanging, SEXP space_before, SEXP space_after);
SEXP get_line_width(SEXP string, SEXP path, SEXP index, SEXP size, SEXP res, 
                    SEXP include_bearing);
int string_width(const char* string, const char* fontfile, int index, 
                 double size, double res, int include_bearing, double* width);
int string_shape(const char* string, const char* fontfile, int index, 
                 double size, double res, double* x, double* y, unsigned int max_length);

#endif
