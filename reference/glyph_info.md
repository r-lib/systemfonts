# Query glyph-specific information from fonts

This function allows you to extract information about the individual
glyphs in a font, based on a specified size. All size related measures
are in pixel-units. The function is vectorised to the length of the
`glyphs` vector.

## Usage

``` r
glyph_info(
  glyphs,
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

- glyphs:

  A vector of glyphs. Strings will be split into separate glyphs
  automatically

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

  path an index of a font file to circumvent lookup based on family and
  style

- variation:

  A `font_variation` object or a list of them to control variable fonts

- bold:

  **\[deprecated\]** Use `weight = "bold"` instead

## Value

A data.frame with information about each glyph, containing the following
columns:

- glyph:

  The glyph as a character

- index:

  The index of the glyph in the font file

- width:

  The width of the glyph

- height:

  The height of the glyph

- x_bearing:

  The horizontal distance from the origin to the leftmost part of the
  glyph

- y_bearing:

  The vertical distance from the origin to the top part of the glyph

- x_advance:

  The horizontal distance to move the cursor after adding the glyph

- y_advance:

  The vertical distance to move the cursor after adding the glyph

- bbox:

  The tight bounding box surrounding the glyph
