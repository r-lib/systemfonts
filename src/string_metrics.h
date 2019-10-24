#ifndef STRING_METRICS_INCLUDED
#define STRING_METRICS_INCLUDED

#include <R.h>
#include <Rinternals.h>

SEXP get_string_shape(SEXP string, SEXP path, SEXP index, SEXP size, SEXP res);

#endif
