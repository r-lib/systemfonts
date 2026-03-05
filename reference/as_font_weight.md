# Convert weight and width to numerics

It is often more natural to describe font weight and width with names
rather than numbers (e.g. "bold" or "condensed"), but underneath these
names are matched to numeric values. These two functions are used to
retrieve the numeric counterparts to names

## Usage

``` r
as_font_weight(weight)

as_font_width(width)
```

## Arguments

- weight, width:

  character vectors with valid names for weight or width

## Value

An integer vector matching the length of the input

## Examples

``` r
as_font_weight(
  c("undefined", "thin", "ultralight", "light", "normal", "medium", "semibold",
    "bold", "ultrabold", "heavy")
)
#>  [1]   0 100 200 300 400 500 600 700 800 900

as_font_width(
  c("undefined", "ultracondensed", "extracondensed", "condensed", "semicondensed",
  "normal", "semiexpanded", "expanded", "extraexpanded", "ultraexpanded")
)
#>  [1] 0 1 2 3 4 5 6 7 8 9
```
