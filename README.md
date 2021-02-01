
<!-- README.md is generated from README.Rmd. Please edit that file -->

# systemfonts

<!-- badges: start -->

[![R build
status](https://github.com/r-lib/systemfonts/workflows/R-CMD-check/badge.svg)](https://github.com/r-lib/systemfonts/actions)
[![Codecov test
coverage](https://codecov.io/gh/r-lib/systemfonts/branch/master/graph/badge.svg)](https://codecov.io/gh/r-lib/systemfonts?branch=master)
[![CRAN
status](https://www.r-pkg.org/badges/version/systemfonts)](https://cran.r-project.org/package=systemfonts)
[![Lifecycle:
stable](https://img.shields.io/badge/lifecycle-stable-brightgreen.svg)](https://lifecycle.r-lib.org/articles/stages.html)
<!-- badges: end -->

systemfonts is a package that locates installed fonts. It uses the
system-native libraries on Mac (CoreText) and Linux (FontConfig), and
uses Freetype to parse the fonts in the registry on Windows.

## Installation

systemfonts is available from CRAN using
`install.packages('systemfonts')`. It is however still under development
and you can install the development version using devtools.

``` r
# install.packages('devtools')
devtools::install_github('r-lib/systemfonts')
```

## Examples

The main use of this package is to locate font files based on family and
style:

``` r
library(systemfonts)

match_font('Avenir', italic = TRUE)
#> $path
#> [1] "/System/Library/Fonts/Avenir.ttc"
#> 
#> $index
#> [1] 1
#> 
#> $features
#> NULL
```

This function returns the path to the file holding the font, as well as
the 0-based index of the font in the file.

It is also possible to get a data.frame of all available fonts:

``` r
system_fonts()
#> # A tibble: 830 x 9
#>    path             index name     family   style  weight width italic monospace
#>    <chr>            <int> <chr>    <chr>    <chr>  <ord>  <ord> <lgl>  <lgl>    
#>  1 /System/Library…     2 Rockwel… Rockwell Bold   bold   norm… FALSE  FALSE    
#>  2 /System/Library…     0 Notewor… Notewor… Light  normal norm… FALSE  FALSE    
#>  3 /Users/thomas/L…     0 SourceS… Source … Bold … bold   norm… TRUE   FALSE    
#>  4 /System/Library…     1 Devanag… Devanag… Bold   bold   norm… FALSE  FALSE    
#>  5 /System/Library…     0 Kannada… Kannada… Regul… normal norm… FALSE  FALSE    
#>  6 /System/Library…     0 Verdana… Verdana  Bold   bold   norm… FALSE  FALSE    
#>  7 /System/Library…     8 ArialHe… Arial H… Light  light  norm… FALSE  FALSE    
#>  8 /Users/thomas/L…     0 FiraSan… Fira Sa… Light… normal norm… TRUE   FALSE    
#>  9 /System/Library…    10 AppleSD… Apple S… Thin   thin   norm… FALSE  FALSE    
#> 10 /System/Library…     0 DecoTyp… DecoTyp… Regul… normal norm… FALSE  FALSE    
#> # … with 820 more rows
```

Further, you can query additional information about fonts and specific
glyphs, if that is of interest using the `font_info()` and
`glyph_info()` functions.

## C API

While getting this information in R is nice, the intended use is mostly
through compiled code so that graphic devices can easily locate relevant
font files etc.

In order to use functions from systemfonts in C(++) code your package
should list systemfonts in the `LinkingTo` field in the `DESCRIPTION`
file. Once this is done you can now `#include <systemfonts.h>` in your
code and use the provided functions. Look into the
[`inst/include/systemfonts.h`](https://github.com/r-lib/systemfonts/blob/master/inst/include/systemfonts.h)
file to familiarise yourself with the C API.

## System Defaults

systemfonts will always try to find a font for you, even if none exist
with the given family name or style. How it resolves this is system
specific and should not be relied on, but it can be expected that a
valid font file is always returned no matter the input.

A few special aliases exist that behaves predictably but system
dependent:

  - `""` and `"sans"` return *Helvetica* on Mac, *Arial* on Windows, and
    the default sans-serif font on Linux (*DejaVu Sans* on Ubuntu)
  - `"serif"` return *Times* on Mac, *Times New Roman* on Windows, and
    the default serif font on Linux (*DejaVu Serif* on Ubuntu)
  - `"mono"` return *Courier* on Mac, *Courier New* on Windows, and the
    default mono font on Linux (*DejaVu Mono* on Ubuntu)
  - `"emoji"` return *Apple Color Emoji* on Mac, *Segoe UI Emoji* on
    Windows, and the default emoji font on Linux (*Noto Color* on
    Ubuntu)

## Code of Conduct

Please note that the ‘systemfonts’ project is released with a
[Contributor Code of
Conduct](https://github.com/r-lib/systemfonts/blob/master/CODE_OF_CONDUCT.md).
By contributing to this project, you agree to abide by its terms.
