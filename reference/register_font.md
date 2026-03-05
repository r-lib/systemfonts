# Register font collections as families

By design, systemfonts searches the fonts installed natively on the
system. It is possible, however, to register other fonts from e.g. font
packages or local font files, that will get searched before searching
any installed fonts. You can always get an overview over all registered
fonts with the `registry_fonts()` function that works as a registry
focused analogue to
[`system_fonts()`](https://systemfonts.r-lib.org/reference/system_fonts.md).
If you wish to clear out the registry, you can either restart the R
session or call `clear_registry()`.

## Usage

``` r
register_font(
  name,
  plain,
  bold = plain,
  italic = plain,
  bolditalic = plain,
  features = font_feature()
)

registry_fonts()

clear_registry()
```

## Arguments

- name:

  The name the collection will be known under (i.e. *family*)

- plain, bold, italic, bolditalic:

  Fontfiles for the different faces of the collection. can either be a
  filepath or a list containing a filepath and an index (only for font
  files containing multiple fonts). If not given it will default to the
  `plain` specification.

- features:

  A
  [`font_feature`](https://systemfonts.r-lib.org/reference/font_feature.md)
  object describing the specific OpenType font features to turn on for
  the registered font.

## Value

`register_font()` and `clear_registry()` returns `NULL` invisibly.
`registry_fonts()` returns a data table in the same style as
[`system_fonts()`](https://systemfonts.r-lib.org/reference/system_fonts.md)
though less detailed and not based on information in the font file.

## Details

`register_font` also makes it possible to use system fonts with traits
that is not covered by the graphic engine in R. In plotting operations
it is only possible to specify a family name and whether or not the font
should be bold and/or italic. There are numerous fonts that will never
get matched to this, especially because bold is only one of many
weights.

Apart from granting a way to use new varieties of fonts, font
registration also allows you to override the default `sans`, `serif`,
and `mono` mappings, simply by registering a collection to the relevant
default name. As registered fonts are searched first it will take
precedence over the default.

## Font matching

During font matching, systemfonts has to look in three different
locations. The font registry (populated by
`register_font()`/[`register_variant()`](https://systemfonts.r-lib.org/reference/register_variant.md)),
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
# Create a random font collection
fonts <- system_fonts()
random_fonts <- sample(nrow(fonts), 4)
register_font(
  'random',
  plain = list(fonts$path[random_fonts[1]], fonts$index[random_fonts[1]]),
  bold = list(fonts$path[random_fonts[2]], fonts$index[random_fonts[2]]),
  italic = list(fonts$path[random_fonts[3]], fonts$index[random_fonts[3]]),
  bolditalic = list(fonts$path[random_fonts[4]], fonts$index[random_fonts[4]])
)

# Look at your creation
registry_fonts()
#> # A tibble: 4 × 7
#>   path                        index family style weight italic features
#>   <chr>                       <int> <chr>  <chr> <ord>  <lgl>  <list>  
#> 1 /usr/share/fonts/truetype/…     0 random Regu… normal FALSE  <int>   
#> 2 /usr/share/fonts/truetype/…     0 random Bold  bold   FALSE  <int>   
#> 3 /usr/share/fonts/type1/urw…     0 random Ital… normal TRUE   <int>   
#> 4 /usr/share/fonts/X11/Type1…     0 random Bold… bold   TRUE   <int>   

# Reset
clear_registry()
```
