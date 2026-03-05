# Calculate the width of a string, ignoring new-lines

This is a very simple alternative to
[`shape_string()`](https://systemfonts.r-lib.org/reference/shape_string.md)
that simply calculates the width of strings without taking any newline
into account. As such it is suitable to calculate the width of words or
lines that has already been splitted by `\n`. Input is recycled to the
length of `strings`.

## Usage

``` r
string_width(
  strings,
  family = "",
  italic = FALSE,
  weight = "normal",
  width = "undefined",
  size = 12,
  res = 72,
  include_bearing = TRUE,
  path = NULL,
  index = 0,
  bold = deprecated()
)
```

## Arguments

- strings:

  A character vector of strings

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

- include_bearing:

  Logical, should left and right bearing be included in the string
  width?

- path, index:

  path and index of a font file to circumvent lookup based on family and
  style

- bold:

  **\[deprecated\]** Use `weight = "bold"` instead

## Value

A numeric vector giving the width of the strings in pixels. Use the
provided `res` value to convert it into absolute values.

## Examples

``` r
strings <- c('A short string', 'A very very looong string')
string_width(strings)
#> [1]  82 151
```
