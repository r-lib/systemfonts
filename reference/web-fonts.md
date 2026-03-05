# Download and add web font

In order to use a font in R it must first be made available locally.
These functions facilitate the download and registration of fonts from
online repositories.

## Usage

``` r
get_from_google_fonts(family, dir = "~/fonts", woff2 = FALSE)

get_from_font_squirrel(family, dir = "~/fonts")
```

## Arguments

- family:

  The font family to download (case insensitive)

- dir:

  Where to download the font to. The default places it in your user
  local font folder so that the font will be available automatically in
  new R sessions. Set to
  [`tempdir()`](https://rdrr.io/r/base/tempfile.html) to only keep the
  font for the session.

- woff2:

  Should the font be downloaded in the woff2 format (smaller and more
  optimized)? Defaults to FALSE as the format is not supported on all
  systems

## Value

A logical invisibly indicating whether a font was found and downloaded
or not
