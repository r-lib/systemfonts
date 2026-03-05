# Register a font as a variant as an existing one

This function is a wrapper around
[`register_font()`](https://systemfonts.r-lib.org/reference/register_font.md)
that allows you to easily create variants of existing system fonts, e.g.
to target different weights and/or widths, or for attaching OpenType
features to a font.

## Usage

``` r
register_variant(
  name,
  family,
  weight = NULL,
  width = NULL,
  features = font_feature()
)
```

## Arguments

- name:

  The new family name the variant should respond to

- family:

  The name of an existing font family that this is a variant of

- weight:

  One or two of `"thin"`, `"ultralight"`, `"light"`, `"normal"`,
  `"medium"`, `"semibold"`, `"bold"`, `"ultrabold"`, or `"heavy"`. If
  one is given it sets the weight for the whole variant. If two is given
  the first one defines the plain weight and the second the bold weight.
  If `NULL` then the variants of the given family closest to `"normal"`
  and `"bold"` will be chosen.

- width:

  One of `"ultracondensed"`, `"extracondensed"`, `"condensed"`,
  `"semicondensed"`, `"normal"`, `"semiexpanded"`, `"expanded"`,
  `"extraexpanded"`, or `"ultraexpanded"` giving the width of the
  variant. If `NULL` then the width closest to `"normal"` will be
  chosen.

- features:

  A
  [`font_feature`](https://systemfonts.r-lib.org/reference/font_feature.md)
  object describing the specific OpenType font features to turn on for
  the registered font variant.

## Font matching

During font matching, systemfonts has to look in three different
locations. The font registry (populated by
[`register_font()`](https://systemfonts.r-lib.org/reference/register_font.md)/`register_variant()`),
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
# Get the default "sans" family
sans <- match_fonts("sans")$path
sans <- system_fonts()$family[system_fonts()$path == sans][1]

# Register a variant of it:
register_variant(
  "sans_ligature",
  sans,
  features = font_feature(ligatures = "discretionary")
)

registry_fonts()
#> # A tibble: 4 × 7
#>   path                        index family style weight italic features
#>   <chr>                       <int> <chr>  <chr> <ord>  <lgl>  <list>  
#> 1 /usr/share/fonts/truetype/…     0 sans_… Regu… normal FALSE  <int>   
#> 2 /usr/share/fonts/truetype/…     0 sans_… Bold  bold   FALSE  <int>   
#> 3 /usr/share/fonts/truetype/…     0 sans_… Ital… normal TRUE   <int>   
#> 4 /usr/share/fonts/truetype/…     0 sans_… Bold… bold   TRUE   <int>   

# clean up
clear_registry()
```
