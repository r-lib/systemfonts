# Define OpenType font feature settings

This function encapsulates the specification of OpenType font features.
Some specific features have named arguments, but all available features
can be set by using its specific 4-letter tag For a list of the 4-letter
tags available see e.g. the overview on
[Wikipedia](https://en.wikipedia.org/wiki/List_of_typographic_features).

## Usage

``` r
font_feature(ligatures = NULL, letters = NULL, numbers = NULL, ...)
```

## Arguments

- ligatures:

  Settings related to ligatures. One or more types of ligatures to turn
  on (see details).

- letters:

  Settings related to the appearance of single letters (as opposed to
  ligatures that substitutes multiple letters). See details for
  supported values.

- numbers:

  Settings related to the appearance of numbers. See details for
  supported values.

- ...:

  key-value pairs with the key being the 4-letter tag and the value
  being the setting (usually `TRUE` to turn it on).

## Value

A `font_feature` object

## Details

OpenType features are defined by a 4-letter tag along with an integer
value. Often that value is a simple `0` (off) or `1` (on), but some
features support additional values, e.g. stylistic alternates (`salt`)
where a font may provide multiple variants of a letter and the value
will be used to chose which one to use.

Common features related to appearance may be given with a long form name
to either the `ligatures`, `letters`, or `numbers` argument to avoid
remembering the often arbitrary 4-letter tag. Providing a long form name
is the same as setting the tag to `1` and can thus not be used to set
tags to other values.

The possible long form names are given below with the tag in
parenthesis:

**Ligatures**

- `standard` (*liga*): Turns on standard multiple letter substitution

- `historical` (*hlig*): Use obsolete historical ligatures

- `contextual` (*clig*): Apply secondary ligatures based on the
  character patterns surrounding the potential ligature

- `discretionary` (*dlig*): Use ornamental ligatures

**Letters**

- `swash` (*cswh*): Use contextual swashes (ornamental decorations)

- `alternates` (*calt*): Use alternate letter forms based on the
  surrounding pattern

- `historical` (*hist*): Use obsolete historical forms of the letters

- `localized` (*locl*): Use alternate forms preferred by the script
  language

- `randomize` (*rand*): Use random variants of the letters (e.g. to
  mimic handwriting)

- `alt_annotation` (*nalt*): Use alternate annotations (e.g. circled
  digits)

- `stylistic` (*salt*): Use a stylistic alternative form of the letter

- `subscript` (*subs*): Set letter in subscript

- `superscript` (*sups*): Set letter in superscript

- `titling` (*titl*): Use letter forms well suited for large text and
  titles

- `small_caps` (*smcp*): Use small caps variants of the letters

**Numbers**

- `lining` (*lnum*): Use number variants that rest on the baseline

- `oldstyle` (*onum*): Use old style numbers that use descender and
  ascender for various numbers

- `proportional` (*pnum*): Let numbers take up width based on the visual
  width of the glyph

- `tabular` (*tnum*): Enforce all numbers to take up the same width

- `fractions` (*frac*): Convert numbers separated by `/` into a fraction
  glyph

- `fractions_alt` (*afrc*): Use alternate fraction form with a
  horizontal divider

## Examples

``` r
font_feature(letters = "stylistic", numbers = c("lining", "tabular"))
#> A list of OpenType font feature settings
#> - salt: 1
#> - lnum: 1
#> - tnum: 1

# Use the tag directly to access additional stylistic variants
font_feature(numbers = c("lining", "tabular"), salt = 2)
#> A list of OpenType font feature settings
#> - salt: 2
#> - lnum: 1
#> - tnum: 1
```
