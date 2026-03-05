# Find a system font by name and style

This function locates the font file (and index) best matching a name and
optional style. A font file will be returned even if a perfect match
isn't found, but it is not necessarily similar to the requested family
and it should not be relied on for font substitution. The aliases
`"sans"`, `"serif"`, `"mono"`, `"symbol"`, and `"emoji"` match to their
respective system defaults (`""` is equivalent to `"sans"`).
`match_font()` has been deprecated in favour of `match_fonts()` which
provides vectorisation, as well as querying for different weights
(rather than just "normal" and "bold") as well as different widths.

## Usage

``` r
match_fonts(family, italic = FALSE, weight = "normal", width = "undefined")

match_font(family, italic = FALSE, bold = FALSE)
```

## Arguments

- family:

  The name of the font families to match

- italic:

  logical indicating the font slant

- weight:

  The weight to query for, either in numbers (`0`, `100`, `200`, `300`,
  `400`, `500`, `600`, `700`, `800`, or `900`) or strings
  (`"undefined"`, `"thin"`, `"ultralight"`, `"light"`, `"normal"`,
  `"medium"`, `"semibold"`, `"bold"`, `"ultrabold"`, or `"heavy"`). `NA`
  will be interpreted as `"undefined"`/`0`

- width:

  The width to query for either in numbers (`0`, `1`, `2`, `3`, `4`,
  `5`, `6`, `7`, `8`, or `9`) or strings (`"undefined"`,
  `"ultracondensed"`, `"extracondensed"`, `"condensed"`,
  `"semicondensed"`, `"normal"`, `"semiexpanded"`, `"expanded"`,
  `"extraexpanded"`, or `"ultraexpanded"`). `NA` will be interpreted as
  `"undefined"`/`0`

- bold:

  logical indicating whether the font weight

## Value

A list containing the paths locating the font files, the 0-based index
of the font in the files and the features for the font in case a
registered font was located.

## Font matching

During font matching, systemfonts has to look in three different
locations. The font registry (populated by
[`register_font()`](https://systemfonts.r-lib.org/reference/register_font.md)/[`register_variant()`](https://systemfonts.r-lib.org/reference/register_variant.md)),
the local fonts (populated with
[`add_fonts()`](https://systemfonts.r-lib.org/reference/add_fonts.md)/[`scan_local_fonts()`](https://systemfonts.r-lib.org/reference/add_fonts.md)),
and the fonts installed on the system. It does so in that order:
registry \> local \> installed.

The matching performed at each step also differs. The fonts in the
registry is only matched by family name. The local fonts are matched
based on all the provided parameters (family, weight, italic, etc) in a
way that is local to systemfonts, but try to emulate the system native
matching. The installed fonts are matched using the system native
matching functionality on macOS and Linux. On Windows the installed
fonts are read from the system registry and matched using the same
approach as for local fonts. Matching will always find a font no matter
what you throw at it, defaulting to "sans" if nothing else is found.

## Examples

``` r
# Get the system default sans-serif font in italic
match_fonts('sans', italic = TRUE)
#> # A tibble: 1 × 4
#>   path                                      index features   variations
#>   <chr>                                     <int> <list>     <list>    
#> 1 /usr/share/fonts/truetype/dejavu/DejaVuS…     0 <font_ftr> <fnt_vrtn>

# Try to match it to a thin variant
match_fonts(c('sans', 'serif'), weight = "thin")
#> # A tibble: 2 × 4
#>   path                                      index features   variations
#>   <chr>                                     <int> <list>     <list>    
#> 1 /usr/share/fonts/truetype/dejavu/DejaVuS…     0 <font_ftr> <fnt_vrtn>
#> 2 /usr/share/fonts/truetype/dejavu/DejaVuS…     0 <font_ftr> <fnt_vrtn>
```
