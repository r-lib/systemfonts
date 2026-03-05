# Query font-specific information

Get general information about a font, relative to a given size. Size
specific measures will be returned in pixel units. The function is
vectorised to the length of the longest argument.

## Usage

``` r
font_info(
  family = "",
  italic = FALSE,
  weight = "normal",
  width = "undefined",
  size = 12,
  res = 72,
  path = NULL,
  index = 0,
  variation = font_variation(),
  bold = deprecated()
)
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

- size:

  The pointsize of the font to use for size related measures

- res:

  The ppi of the size related measures

- path, index:

  path and index of a font file to circumvent lookup based on family and
  style

- variation:

  A `font_variation` object or a list of them to control variable fonts

- bold:

  **\[deprecated\]** Use `weight = "bold"` instead

## Value

A data.frame giving info on the requested font + size combinations. The
data.frame will contain the following columns:

- path:

  The path to the font file

- index:

  The 0-based index of the font in the fontfile

- family:

  The family name of the font

- style:

  The style name of the font

- name:

  The name of the font, if present, otherwise the family name

- italic:

  A logical giving if the font is italic

- bold:

  A logical giving if the font is bold

- monospace:

  A logical giving if the font is monospace

- weight:

  A factor giving the weight of the font

- width:

  A factor giving the width of the font

- kerning:

  A logical giving if the font supports kerning

- color:

  A logical giving if the font has color glyphs

- scalable:

  A logical giving if the font is scalable

- vertical:

  A logical giving if the font is vertical

- n_glyphs:

  The number of glyphs in the font

- n_sizes:

  The number of predefined sizes in the font

- n_charmaps:

  The number of character mappings in the font file

- bbox:

  A bounding box large enough to contain any of the glyphs in the font

- max_ascend:

  The maximum ascend of the tallest glyph in the font

- max_descent:

  The maximum descend of the most descending glyph in the font

- max_advance_width:

  The maximum horizontal advance a glyph can make

- max_advance_height:

  The maximum vertical advance a glyph can make

- lineheight:

  The height of a single line of text in the font

- underline_pos:

  The position of a potential underlining segment

- underline_size:

  The width the the underline

## Examples

``` r
font_info('serif')
#> # A tibble: 1 × 27
#>   path     index family style name  italic bold  monospace weight width
#>   <chr>    <int> <chr>  <chr> <chr> <lgl>  <lgl> <lgl>     <ord>  <ord>
#> 1 /usr/sh…     0 DejaV… Book  Deja… FALSE  FALSE FALSE     normal norm…
#> # ℹ 17 more variables: kerning <lgl>, color <lgl>, scalable <lgl>,
#> #   vertical <lgl>, n_glyphs <int>, n_sizes <int>, n_charmaps <int>,
#> #   charmaps <list>, bbox <list>, max_ascend <dbl>, max_descend <dbl>,
#> #   max_advance_width <dbl>, max_advance_height <dbl>,
#> #   lineheight <dbl>, underline_pos <dbl>, underline_size <dbl>,
#> #   variation_axes <list>

# Avoid lookup if font file is already known
sans <- match_fonts('sans')
font_info(path = sans$path, index = sans$index)
#> # A tibble: 1 × 27
#>   path     index family style name  italic bold  monospace weight width
#>   <chr>    <int> <chr>  <chr> <chr> <lgl>  <lgl> <lgl>     <ord>  <ord>
#> 1 /usr/sh…     0 DejaV… Book  Deja… FALSE  FALSE FALSE     normal norm…
#> # ℹ 17 more variables: kerning <lgl>, color <lgl>, scalable <lgl>,
#> #   vertical <lgl>, n_glyphs <int>, n_sizes <int>, n_charmaps <int>,
#> #   charmaps <list>, bbox <list>, max_ascend <dbl>, max_descend <dbl>,
#> #   max_advance_width <dbl>, max_advance_height <dbl>,
#> #   lineheight <dbl>, underline_pos <dbl>, underline_size <dbl>,
#> #   variation_axes <list>
```
