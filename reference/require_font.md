# Ensure font availability in a script

When running a script on a different machine you are not always in
control of which fonts are installed on the system and thus how graphics
created by the script ends up looking. `require_font()` is a way to
specify your font requirements for a script. It will look at the
available fonts and if the required font family is not present it will
attempt to fetch it from one of the given repositories (in the order
given). If that fails, it will either throw an error or, if `fallback`
is given, provide an alias for the fallback so it maps to the required
font.

## Usage

``` r
require_font(
  family,
  fallback = NULL,
  dir = tempdir(),
  repositories = c("Google Fonts", "Font Squirrel", "Font Library"),
  error = TRUE,
  verbose = TRUE
)
```

## Arguments

- family:

  The font family to require

- fallback:

  An available font to fall back to if `family` cannot be found or
  downloaded

- dir:

  The location to put the font file downloaded from repositories

- repositories:

  The repositories to search for the font in case it is not available on
  the system. They will be tried in the order given. Currently
  `"Google Fonts"`, `"Font Squirrel"`, and `"Font Library"` is
  available.

- error:

  Should the function throw an error if unsuccessful?

- verbose:

  Should status messages be emitted?

## Value

Invisibly `TRUE` if the font is available or `FALSE` if not (this can
only be returned if `error = FALSE`)

## Examples

``` r
# Should always work
require_font("sans")
```
