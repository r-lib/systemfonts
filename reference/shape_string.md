# Calculate glyph positions for strings

Do basic text shaping of strings. This function will use freetype to
calculate advances, doing kerning if possible. It will not perform any
font substitution or ligature resolving and will thus be much in line
with how the standard graphic devices does text shaping. Inputs are
recycled to the length of `strings`.

## Usage

``` r
shape_string(
  strings,
  id = NULL,
  family = "",
  italic = FALSE,
  weight = "normal",
  width = "undefined",
  size = 12,
  res = 72,
  lineheight = 1,
  align = "left",
  hjust = 0,
  vjust = 0,
  max_width = NA,
  tracking = 0,
  indent = 0,
  hanging = 0,
  space_before = 0,
  space_after = 0,
  path = NULL,
  index = 0,
  bold = deprecated()
)
```

## Arguments

- strings:

  A character vector of strings to shape

- id:

  A vector grouping the strings together. If strings share an id the
  shaping will continue between strings

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

- lineheight:

  A multiplier for the lineheight

- align:

  Within text box alignment, either `'left'`, `'center'`, or `'right'`

- hjust, vjust:

  The justification of the textbox surrounding the text

- max_width:

  The requested with of the string in inches. Setting this to something
  other than `NA` will turn on word wrapping.

- tracking:

  Tracking of the glyphs (space adjustment) measured in 1/1000 em.

- indent:

  The indent of the first line in a paragraph measured in inches.

- hanging:

  The indent of the remaining lines in a paragraph measured in inches.

- space_before, space_after:

  The spacing above and below a paragraph, measured in points

- path, index:

  path an index of a font file to circumvent lookup based on family and
  style

- bold:

  **\[deprecated\]** Use `weight = "bold"` instead

## Value

A list with two element: `shape` contains the position of each glyph,
relative to the origin in the enclosing textbox. `metrics` contain
metrics about the full strings.

`shape` is a data.frame with the following columns:

- glyph:

  The glyph as a character

- index:

  The index of the glyph in the font file

- metric_id:

  The index of the string the glyph is part of (referencing a row in the
  `metrics` data.frame)

- string_id:

  The index of the string the glyph came from (referencing an element in
  the `strings` input)

- x_offset:

  The x offset in pixels from the origin of the textbox

- y_offset:

  The y offset in pixels from the origin of the textbox

- x_mid:

  The x offset in pixels to the middle of the glyph, measured from the
  origin of the glyph

`metrics` is a data.frame with the following columns:

- string:

  The text the string consist of

- width:

  The width of the string

- height:

  The height of the string

- left_bearing:

  The distance from the left edge of the textbox and the leftmost glyph

- right_bearing:

  The distance from the right edge of the textbox and the rightmost
  glyph

- top_bearing:

  The distance from the top edge of the textbox and the topmost glyph

- bottom_bearing:

  The distance from the bottom edge of the textbox and the bottommost
  glyph

- left_border:

  The position of the leftmost edge of the textbox related to the origin

- top_border:

  The position of the topmost edge of the textbox related to the origin

- pen_x:

  The horizontal position of the next glyph after the string

- pen_y:

  The vertical position of the next glyph after the string

## Examples

``` r
string <- "This is a long string\nLook; It spans multiple lines\nand all"

# Shape with default settings
shape_string(string)
#> $shape
#> # A tibble: 59 × 7
#>    glyph index metric_id string_id x_offset y_offset x_midpoint
#>    <chr> <int>     <int>     <int>    <dbl>    <dbl>      <dbl>
#>  1 "T"      55         0         1        0        0        3.5
#>  2 "h"      75         0         1        7        0        4  
#>  3 "i"      76         0         1       15        0        1.5
#>  4 "s"      86         0         1       18        0        3  
#>  5 " "       3         0         1       24        0        2  
#>  6 "i"      76         0         1       28        0        1.5
#>  7 "s"      86         0         1       31        0        3  
#>  8 " "       3         0         1       37        0        2  
#>  9 "a"      68         0         1       41        0        3.5
#> 10 " "       3         0         1       48        0        2  
#> # ℹ 49 more rows
#> 
#> $metrics
#> # A tibble: 1 × 11
#>   string            width height left_bearing right_bearing top_bearing
#>   <chr>             <dbl>  <dbl>        <dbl>         <dbl>       <dbl>
#> 1 "This is a long …   168   53.0           -1             0        1.14
#> # ℹ 5 more variables: bottom_bearing <dbl>, left_border <dbl>,
#> #   top_border <dbl>, pen_x <dbl>, pen_y <dbl>
#> 

# Mix styles within the same string
string <- c(
  "This string will have\na ",
  "very large",
  " text style\nin the middle"
)

shape_string(string, id = c(1, 1, 1), size = c(12, 24, 12))
#> $shape
#> # A tibble: 59 × 7
#>    glyph index metric_id string_id x_offset y_offset x_midpoint
#>    <chr> <int>     <int>     <int>    <dbl>    <dbl>      <dbl>
#>  1 "T"      55         0         1        0        0        3.5
#>  2 "h"      75         0         1        7        0        4  
#>  3 "i"      76         0         1       15        0        1.5
#>  4 "s"      86         0         1       18        0        3  
#>  5 " "       3         0         1       24        0        2  
#>  6 "s"      86         0         1       28        0        3  
#>  7 "t"      87         0         1       34        0        2.5
#>  8 "r"      85         0         1       39        0        2.5
#>  9 "i"      76         0         1       44        0        1.5
#> 10 "n"      81         0         1       47        0        4  
#> # ℹ 49 more rows
#> 
#> $metrics
#> # A tibble: 1 × 11
#>   string            width height left_bearing right_bearing top_bearing
#>   <chr>             <dbl>  <dbl>        <dbl>         <dbl>       <dbl>
#> 1 "This string wil…   194   67.0           -1             0        1.14
#> # ℹ 5 more variables: bottom_bearing <dbl>, left_border <dbl>,
#> #   top_border <dbl>, pen_x <dbl>, pen_y <dbl>
#> 
```
