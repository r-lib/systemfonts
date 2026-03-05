# Define axis coordinates for variable fonts

Variable fonts is a technology that allows font designers to encode the
full, continuous font space of a typeface into a single font file and
then have the user set the coordinates of the variable axes to define
the font. So, rather than having a font file for bold, bold + italic,
thin, and thin + italic, etc. it is all encoded in a single file with a
continuous range of all axes (e.g. weight doesn't have to be one of the
9 standard weights but can be anything in between). There are 5 standard
axes that fonts can use, but font designers are free to define their own
completely arbitrary axes as well. You can use
[`font_info()`](https://systemfonts.r-lib.org/reference/font_info.md) to
see which axes a font defines along with their value range and default
setting. Values given as `font_variation()` will always win over the
conventional setting *if* the axis is present in the font. For example,
setting `weight = "bold"` along with
`variation = font_variation(weight = 650)` will eventually request a
weight of `650` (helfway between semibold and bold), assuming the
weight-axis is present in the font. For clarity however, it is advised
that `font_variation()` is only used for axes that can otherwise not be
accessed by "top-level" arguments.

## Usage

``` r
font_variation(
  italic = NULL,
  weight = NULL,
  width = NULL,
  slant = NULL,
  optical_sizing = NULL,
  ...
)
```

## Arguments

- italic:

  Value between 0 and 1. Usually treated as a boolean rather than a
  continuous axis

- weight:

  Usually a value between 100 and 900 though fonts can limit or expand
  the supported range. Weight names are also allowed (see
  [`as_font_weight()`](https://systemfonts.r-lib.org/reference/as_font_weight.md))

- width:

  Usually a value between 1 and 9 though fonts can limit or expand the
  supported range. Width names are also allowed (see
  [`as_font_width()`](https://systemfonts.r-lib.org/reference/as_font_weight.md))

- slant:

  The angular slant of the font, usually between -90 and 90 (negative
  values tilt in the "standard" italic way)

- optical_sizing:

  Stroke thickness compensation for the glyphs. During rendering the
  thickness of the stroke is usually increased when the font is set at
  small sizes to increase readability. Set this to the size of the font
  to get the "expected" look at that size.

- ...:

  Further axes and coordinate settings

## Value

A `font_variation` object with coordinates for the provided axes

## Note

systemfonts uses a scale of width values ranging from 1-9 while the
width axis uses a different scale (0.5 - 2.0) going from half as wide to
twice as wide as "normal". When using the `width` argument the
coordinate values is automatically converted. If you set the value based
on the width tag (`wdth`) then no conversion will happen.
