# Get string metrics as measured by the current device

This function is much like
[`string_widths_dev()`](https://systemfonts.r-lib.org/reference/string_widths_dev.md)
but also returns the ascent and descent of the string making it possible
to construct a tight bounding box around the string.

## Usage

``` r
string_metrics_dev(
  strings,
  family = "",
  face = 1,
  size = 12,
  cex = 1,
  unit = "cm"
)
```

## Arguments

- strings:

  A character vector of strings to measure

- family:

  The font families to use. Will get recycled

- face:

  The font faces to use. Will get recycled

- size:

  The font size to use. Will get recycled

- cex:

  The cex multiplier to use. Will get recycled

- unit:

  The unit to return the width in. Either `"cm"`, `"inches"`,
  `"device"`, or `"relative"`

## Value

A data.frame with `width`, `ascent`, and `descent` columns giving the
metrics in the requested unit.

## See also

Other device metrics:
[`string_widths_dev()`](https://systemfonts.r-lib.org/reference/string_widths_dev.md)

## Examples

``` r
# Get the metrics as measured in cm (default)
string_metrics_dev(c('some text', 'a string with descenders'))
#> # A tibble: 2 × 3
#>   width ascent descent
#>   <dbl>  <dbl>   <dbl>
#> 1  2.12  0.318  0.0353
#> 2  5.21  0.353  0.106 
```
