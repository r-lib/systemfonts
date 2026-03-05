# Split a string into emoji and non-emoji glyph runs

In order to do correct text rendering, the font needed must be figured
out. A common case is rendering of emojis within a string where the
system emoji font is used rather than the requested font. This function
will inspect the provided strings and split them up in runs that must be
rendered with the emoji font, and the rest. Arguments are recycled to
the length of the `string` vector.

## Usage

``` r
str_split_emoji(
  string,
  family = "",
  italic = FALSE,
  bold = FALSE,
  path = NULL,
  index = 0
)
```

## Arguments

- string:

  A character vector of strings that should be splitted.

- family:

  The name of the font families to match

- italic:

  logical indicating the font slant

- bold:

  logical indicating whether the font weight

- path, index:

  path an index of a font file to circumvent lookup based on family and
  style

## Value

A data.frame containing the following columns:

- string:

  The substring containing a consecutive run of glyphs

- id:

  The index into the original `string` vector that the substring is part
  of

- emoji:

  A logical vector giving if the substring is a run of emojis or not

## Examples

``` r
emoji_string <- "This is a joke\U0001f642. It should be obvious from the smiley"
str_split_emoji(emoji_string)
#> # A tibble: 3 × 3
#>   string                                    id emoji
#>   <chr>                                  <dbl> <lgl>
#> 1 This is a joke                             1 FALSE
#> 2 🙂                                         1 TRUE 
#> 3 . It should be obvious from the smiley     1 FALSE
```
