# Convert an extracted glyph raster to a grob

This is a convenience function that helps in creating rasterGrob with
the correct settings for the glyph. It takes inot account the sizing and
offset returned by
[`glyph_raster()`](https://systemfonts.r-lib.org/reference/glyph_raster.md)
and allows you to only consider the baseline position of the glyph.

## Usage

``` r
glyph_raster_grob(glyph, x, y, ..., default.units = "bigpts")
```

## Arguments

- glyph:

  The nativeRaster object returned as one of the elements by
  [`glyph_raster()`](https://systemfonts.r-lib.org/reference/glyph_raster.md)

- x, y:

  The baseline location of the glyph

- ...:

  Arguments passed on to
  [`grid::rasterGrob`](https://rdrr.io/r/grid/grid.raster.html)

  `image`

  :   Any R object that can be coerced to a raster object.

  `width`

  :   A numeric vector or unit object specifying width.

  `height`

  :   A numeric vector or unit object specifying height.

  `just`

  :   The justification of the rectangle relative to its (x, y)
      location. If there are two values, the first value specifies
      horizontal justification and the second value specifies vertical
      justification. Possible string values are: `"left"`, `"right"`,
      `"centre"`, `"center"`, `"bottom"`, and `"top"`. For numeric
      values, 0 means left alignment and 1 means right alignment.

  `hjust`

  :   A numeric vector specifying horizontal justification. If
      specified, overrides the `just` setting.

  `vjust`

  :   A numeric vector specifying vertical justification. If specified,
      overrides the `just` setting.

  `name`

  :   A character identifier.

  `gp`

  :   An object of class `"gpar"`, typically the output from a call to
      the function [`gpar`](https://rdrr.io/r/grid/gpar.html). This is
      basically a list of graphical parameter settings.

  `vp`

  :   A Grid viewport object (or NULL).

  `interpolate`

  :   A logical value indicating whether to linearly interpolate the
      image (the alternative is to use nearest-neighbour interpolation,
      which gives a more blocky result).

- default.units:

  A string indicating the default units to use if `x`, `y`, `width`, or
  `height` are only given as numeric vectors.

## Value

A rasterGrob object

## Examples

``` r
font <- font_info()
glyph <- glyph_info("R", path = font$path, index = font$index)

R <- glyph_raster(glyph$index, font$path, font$index, size = 150)

grob <- glyph_raster_grob(R[[1]], 50, 50)

grid::grid.newpage()
# Mark the baseline location
grid::grid.points(50, 50, default.units = "bigpts")
# Draw the glyph
grid::grid.draw(grob)

```
