# Add local font files to the search path

systemfonts is mainly about getting system native access to the fonts
installed on the OS you are executing the code on. However, you may want
to access fonts without doing a full installation, either because you
want your project to be reproducible on all systems, because you don't
have administrator privileges on the system, or for a different reason
entirely. `add_fonts()` provide a way to side load font files so that
they are found during font matching. The function differs from
[`register_font()`](https://systemfonts.r-lib.org/reference/register_font.md)
and
[`register_variant()`](https://systemfonts.r-lib.org/reference/register_variant.md)
in that they add the font file as-is using the family name etc that are
provided by the font. `scan_local_fonts()` is run when systemfonts is
loaded and will automatically add font files stored in `./fonts`
(project local) and `~/fonts` (user local).

## Usage

``` r
add_fonts(files)

scan_local_fonts()

clear_local_fonts()
```

## Arguments

- files:

  A character vector of font file paths or urls to add

## Value

This function is called for its sideeffects

## Font matching

During font matching, systemfonts has to look in three different
locations. The font registry (populated by
[`register_font()`](https://systemfonts.r-lib.org/reference/register_font.md)/[`register_variant()`](https://systemfonts.r-lib.org/reference/register_variant.md)),
the local fonts (populated with `add_fonts()`/`scan_local_fonts()`), and
the fonts installed on the system. It does so in that order: registry \>
local \> installed.

The matching performed at each step also differs. The fonts in the
registry is only matched by family name. The local fonts are matched
based on all the provided parameters (family, weight, italic, etc) in a
way that is local to systemfonts, but try to emulate the system native
matching. The installed fonts are matched using the system native
matching functionality on macOS and Linux. On Windows the installed
fonts are read from the system registry and matched using the same
approach as for local fonts. Matching will always find a font no matter
what you throw at it, defaulting to "sans" if nothing else is found.

## Examples

``` r
# example code
empty_font <- system.file("unfont.ttf", package = "systemfonts")

add_fonts(empty_font)

clear_local_fonts()
#> Run `scan_local_fonts()` in order to re-add automatically added fonts
```
