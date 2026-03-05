# Reset the system font cache

Building the list of system fonts is time consuming and is therefore
cached. This, in turn, means that changes to the system fonts (i.e.
installing new fonts), will not propagate to systemfonts. The solution
is to reset the cache, which will result in the next call to e.g.
[`match_fonts()`](https://systemfonts.r-lib.org/reference/match_fonts.md)
will trigger a rebuild of the cache.

## Usage

``` r
reset_font_cache()
```

## Examples

``` r
all_fonts <- system_fonts()

##-- Install a new font on the system --##

all_fonts_new <- system_fonts()

## all_fonts_new will be equal to all_fonts

reset_font_cache()

all_fonts_new <- system_fonts()

## all_fonts_new will now contain the new font
```
