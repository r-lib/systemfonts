# Search font repositories for a font based on family name

While it is often advisable to visit the webpage for a font repository
when looking for a font, in order to see examples etc,
`search_web_fonts()` provide a quick lookup based on family name in the
repositories supported by systemfonts (currently [Google
Fonts](https://fonts.google.com) and [Font
Squirrel](https://www.fontsquirrel.com) - [Bunny
Fonts](https://fonts.bunny.net/) provide the same fonts as Google Fonts
but doesn't have a search API). The lookup is based on fuzzy matching
provided by [`utils::adist()`](https://rdrr.io/r/utils/adist.html) and
the matching parameters can be controlled through `...`

## Usage

``` r
search_web_fonts(family, n_max = 10, ...)
```

## Arguments

- family:

  The font family name to look for

- n_max:

  The maximum number of matches to return

- ...:

  Arguments passed on to
  [`utils::adist`](https://rdrr.io/r/utils/adist.html)

  `costs`

  :   a numeric vector or list with names partially matching
      `insertions`, `deletions` and `substitutions` giving the
      respective costs for computing the Levenshtein distance, or `NULL`
      (default) indicating using unit cost for all three possible
      transformations.

  `counts`

  :   a logical indicating whether to optionally return the
      transformation counts (numbers of insertions, deletions and
      substitutions) as the `"counts"` attribute of the return value.

  `fixed`

  :   a logical. If `TRUE` (default), the `x` elements are used as
      string literals. Otherwise, they are taken as regular expressions
      and `partial = TRUE` is implied (corresponding to the approximate
      string distance used by
      [`agrep`](https://rdrr.io/r/base/agrep.html) with
      `fixed = FALSE`).

  `partial`

  :   a logical indicating whether the transformed `x` elements must
      exactly match the complete `y` elements, or only substrings of
      these. The latter corresponds to the approximate string distance
      used by [`agrep`](https://rdrr.io/r/base/agrep.html) (by default).

  `ignore.case`

  :   a logical. If `TRUE`, case is ignored for computing the distances.

  `useBytes`

  :   a logical. If `TRUE` distance computations are done byte-by-byte
      rather than character-by-character.

## Value

A data.frame with the columns `family`, giving the family name of the
matched font, and `repository` giving the repository it was found in.

## Examples

``` r
# Requires an internet connection

# search_web_fonts("Spectral")
```
