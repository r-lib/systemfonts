#ifndef STRING_METRICS_INCLUDED
#define STRING_METRICS_INCLUDED

#include <R.h>
#include <Rinternals.h>

SEXP get_string_shape(SEXP string, SEXP id, SEXP path, SEXP index, SEXP size, 
                      SEXP res, SEXP lineheight, SEXP align, SEXP hjust, 
                      SEXP vjust);
SEXP get_line_width(SEXP string, SEXP path, SEXP index, SEXP size, SEXP res, 
                    SEXP include_bearing);
double string_width(const char* string, const char* fontfile, int index, 
                    double size, double res, int include_bearing);
void string_shape(const char* string, const char* fontfile, int index, 
                  double size, double res, double* x, double* y, int max_length);

#endif
