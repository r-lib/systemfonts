# Get string widths as measured by the current device

For certain composition tasks it is beneficial to get the width of a
string as interpreted by the device that is going to plot it. grid
provides this through construction of a `textGrob` and then converting
the corresponding grob width to e.g. cm, but this comes with a huge
overhead. `string_widths_dev()` provides direct, vectorised, access to
the graphic device for as high performance as possible.

## Usage

``` r
string_widths_dev(
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

A numeric vector with the width of each of the strings given in
`strings` in the unit given in `unit`

## See also

Other device metrics:
[`string_metrics_dev()`](https://systemfonts.r-lib.org/reference/string_metrics_dev.md)

## Examples

``` r
# Get the widths as measured in cm (default)
string_widths_dev(c('a string', 'an even longer string'))
#> [1] 1.609549 4.536502
```
