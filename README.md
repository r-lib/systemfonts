
<!-- README.md is generated from README.Rmd. Please edit that file -->

# systemfonts

<!-- badges: start -->

[![Codecov test
coverage](https://codecov.io/gh/r-lib/systemfonts/branch/master/graph/badge.svg)](https://app.codecov.io/gh/r-lib/systemfonts?branch=master)
[![CRAN
status](https://www.r-pkg.org/badges/version/systemfonts)](https://cran.r-project.org/package=systemfonts)
[![Lifecycle:
stable](https://img.shields.io/badge/lifecycle-stable-brightgreen.svg)](https://lifecycle.r-lib.org/articles/stages.html)
[![R-CMD-check](https://github.com/r-lib/systemfonts/workflows/R-CMD-check/badge.svg)](https://github.com/r-lib/systemfonts/actions)
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

match_fonts('Avenir', italic = TRUE)
#>                               path index features
#> 1 /System/Library/Fonts/Avenir.ttc     1
```

This function returns the path to the file holding the font, as well as
the 0-based index of the font in the file.

It is also possible to get a data.frame of all available fonts:

``` r
system_fonts()
#> # A tibble: 843 × 9
#>    path                   index name  family style weight width italic monospace
#>    <chr>                  <int> <chr> <chr>  <chr> <ord>  <ord> <lgl>  <lgl>    
#>  1 /System/Library/Fonts…     2 Rock… Rockw… Bold  bold   norm… FALSE  FALSE    
#>  2 /Users/thomas/Library…     0 Open… Open … Ligh… normal norm… TRUE   FALSE    
#>  3 /Users/thomas/Library…     0 Open… Open … Semi… semib… semi… TRUE   FALSE    
#>  4 /System/Library/Fonts…     0 Note… Notew… Light normal norm… FALSE  FALSE    
#>  5 /System/Library/Fonts…     1 Deva… Devan… Bold  bold   norm… FALSE  FALSE    
#>  6 /System/Library/Fonts…     0 Kann… Kanna… Regu… normal norm… FALSE  FALSE    
#>  7 /System/Library/Fonts…     0 Verd… Verda… Bold  bold   norm… FALSE  FALSE    
#>  8 /System/Library/Fonts…     8 Aria… Arial… Light light  norm… FALSE  FALSE    
#>  9 /Users/thomas/Library…     0 Open… Open … Medi… medium norm… TRUE   FALSE    
#> 10 /System/Library/Fonts…    10 Appl… Apple… Thin  thin   norm… FALSE  FALSE    
#> # ℹ 833 more rows
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
- `"serif"` return *Times* on Mac, *Times New Roman* on Windows, and the
  default serif font on Linux (*DejaVu Serif* on Ubuntu)
- `"mono"` return *Courier* on Mac, *Courier New* on Windows, and the
  default mono font on Linux (*DejaVu Mono* on Ubuntu)
- `"emoji"` return *Apple Color Emoji* on Mac, *Segoe UI Emoji* on
  Windows, and the default emoji font on Linux (*Noto Color* on Ubuntu)

## Code of Conduct

Please note that the ‘systemfonts’ project is released with a
[Contributor Code of
Conduct](https://github.com/r-lib/systemfonts/blob/master/CODE_OF_CONDUCT.md).
By contributing to this project, you agree to abide by its terms.
