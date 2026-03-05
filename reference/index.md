# Package index

## Font Lookup

systemfonts is primarily a package to facilitate the lookup of fonts
based on typeface and style. These functions facilitate this

- [`match_fonts()`](https://systemfonts.r-lib.org/reference/match_fonts.md)
  [`match_font()`](https://systemfonts.r-lib.org/reference/match_fonts.md)
  : Find a system font by name and style
- [`font_fallback()`](https://systemfonts.r-lib.org/reference/font_fallback.md)
  : Get the fallback font for a given string
- [`system_fonts()`](https://systemfonts.r-lib.org/reference/system_fonts.md)
  : List all fonts installed on your system
- [`reset_font_cache()`](https://systemfonts.r-lib.org/reference/reset_font_cache.md)
  : Reset the system font cache

## Shaping

While text shaping is better handed off to the
[textshaping](https://github.com/r-lib/textshaping) package, systemfonts
does provide basic shaping functionality

- [`shape_string()`](https://systemfonts.r-lib.org/reference/shape_string.md)
  : Calculate glyph positions for strings
- [`string_width()`](https://systemfonts.r-lib.org/reference/string_width.md)
  : Calculate the width of a string, ignoring new-lines
- [`string_metrics_dev()`](https://systemfonts.r-lib.org/reference/string_metrics_dev.md)
  : Get string metrics as measured by the current device
- [`string_widths_dev()`](https://systemfonts.r-lib.org/reference/string_widths_dev.md)
  : Get string widths as measured by the current device
- [`str_split_emoji()`](https://systemfonts.r-lib.org/reference/str_split_emoji.md)
  : Split a string into emoji and non-emoji glyph runs

## Font file information

The following functions allow you to extract various kind of information
from font files

- [`font_info()`](https://systemfonts.r-lib.org/reference/font_info.md)
  : Query font-specific information
- [`glyph_info()`](https://systemfonts.r-lib.org/reference/glyph_info.md)
  : Query glyph-specific information from fonts
- [`glyph_outline()`](https://systemfonts.r-lib.org/reference/glyph_outline.md)
  : Get the outline of glyphs
- [`glyph_raster()`](https://systemfonts.r-lib.org/reference/glyph_raster.md)
  : Render glyphs to raster image
- [`glyph_raster_grob()`](https://systemfonts.r-lib.org/reference/glyph_raster_grob.md)
  : Convert an extracted glyph raster to a grob
- [`plot_glyph_stats()`](https://systemfonts.r-lib.org/reference/plot_glyph_stats.md)
  : Create a visual representation of what the various glyph stats mean

## Local fonts

While systemfonts was mainly created to access fonts installed on the
system it also has rich support for using non-installed (local) font
files from R. Further, it provides facilities for accessing system fonts
that are otherwise inaccessible from R

- [`add_fonts()`](https://systemfonts.r-lib.org/reference/add_fonts.md)
  [`scan_local_fonts()`](https://systemfonts.r-lib.org/reference/add_fonts.md)
  [`clear_local_fonts()`](https://systemfonts.r-lib.org/reference/add_fonts.md)
  : Add local font files to the search path
- [`register_variant()`](https://systemfonts.r-lib.org/reference/register_variant.md)
  : Register a font as a variant as an existing one
- [`register_font()`](https://systemfonts.r-lib.org/reference/register_font.md)
  [`registry_fonts()`](https://systemfonts.r-lib.org/reference/register_font.md)
  [`clear_registry()`](https://systemfonts.r-lib.org/reference/register_font.md)
  : Register font collections as families
- [`font_feature()`](https://systemfonts.r-lib.org/reference/font_feature.md)
  : Define OpenType font feature settings
- [`font_variation()`](https://systemfonts.r-lib.org/reference/font_variation.md)
  : Define axis coordinates for variable fonts

## Web fonts

Many fonts are now available online through font repositories.
systemfonts provide access to these, both for installing and using
directly, but also for embedding inside SVGs

- [`search_web_fonts()`](https://systemfonts.r-lib.org/reference/search_web_fonts.md)
  : Search font repositories for a font based on family name
- [`require_font()`](https://systemfonts.r-lib.org/reference/require_font.md)
  : Ensure font availability in a script
- [`get_from_google_fonts()`](https://systemfonts.r-lib.org/reference/web-fonts.md)
  [`get_from_font_squirrel()`](https://systemfonts.r-lib.org/reference/web-fonts.md)
  : Download and add web font
- [`fonts_as_import()`](https://systemfonts.r-lib.org/reference/fonts_as_import.md)
  : Create import specifications for web content
