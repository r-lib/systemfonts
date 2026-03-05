# Get the fallback font for a given string

A fallback font is a font to use as a substitute if the chosen font does
not contain the requested characters. Using font fallbacks means that
the user doesn't have to worry about mixing characters from different
scripts or mixing text and emojies. Fallback is calculated for the full
string and the result is platform specific. If no font covers all the
characters in the string an undefined "best match" is returned. The best
approach is to figure out which characters are not covered by your
chosen font and figure out fallbacks for these, rather than just request
a fallback for the full string.

## Usage

``` r
font_fallback(
  string,
  family = "",
  italic = FALSE,
  weight = "normal",
  width = "undefined",
  path = NULL,
  index = 0,
  variation = font_variation(),
  bold = deprecated()
)
```

## Arguments

- string:

  The strings to find fallbacks for

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

- path, index:

  path and index of a font file to circumvent lookup based on family and
  style

- variation:

  A `font_variation` object or a list of them to control variable fonts

- bold:

  **\[deprecated\]** Use `weight = "bold"` instead

## Value

A data frame with a `path` and `index` column giving fallback for the
specified string and font combinations

## Examples

``` r
font_fallback("\U0001f604") # Smile emoji
#>                                              path index
#> 1 /usr/share/fonts/truetype/dejavu/DejaVuSans.ttf     0
```
