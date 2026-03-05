# Changelog

## systemfonts (development version)

- Fixed a compilation error when compiling against very old versions of
  Freetype
- Avoid compiler warnings when comparing doubles to enum
- Work around broken PingFangUI.ttc on macOS
  ([\#147](https://github.com/r-lib/systemfonts/issues/147))
- Guard against zero-length input
  ([\#151](https://github.com/r-lib/systemfonts/issues/151))

## systemfonts 1.3.1

CRAN release: 2025-10-01

- Fixed a sanitizer issue with converting variation axis names to tags
- Avoid spurious build issues on old macOS systems
- Avoid writing font files to the user directory during test

## systemfonts 1.3.0

CRAN release: 2025-09-30

- Fixed a bug in the URL generation for Google Font imports
- Added support for Bunny Fonts imports
  ([\#132](https://github.com/r-lib/systemfonts/issues/132))
- Begin deprecation of `bold` argument in favour of `weight` throughout
  package
- Improve messaging in
  [`require_font()`](https://systemfonts.r-lib.org/reference/require_font.md)
- Fonts are automatically added to the session when an import is created
  ([\#131](https://github.com/r-lib/systemfonts/issues/131))
- Fixed a bug in converting font weights as reported by macOS into
  ISO-style weight used by systemfonts and FreeType
- [`require_font()`](https://systemfonts.r-lib.org/reference/require_font.md)
  now better handles lack of internet access
- Added
  [`plot_glyph_stats()`](https://systemfonts.r-lib.org/reference/plot_glyph_stats.md)
  to provide visual explanation for various glyph measures
- [`font_info()`](https://systemfonts.r-lib.org/reference/font_info.md)
  now returns the PostScript name of the font in the `name` column
- Added support for variable fonts throughout the package. Other
  packages will need to be upgraded to take advantage of this. A new
  function
  [`font_variation()`](https://systemfonts.r-lib.org/reference/font_variation.md)
  can be used to define coords for the variation axes
- Fixed a bug in webfont download on Windows where fontfiles would
  become corrupted
  ([\#134](https://github.com/r-lib/systemfonts/issues/134))
- Fixed an issue in textshaping where conflicting DLL names resulted in
  the R process inability to render text if textshaping was loaded first
  (textshaping#36)
- Add a way to test if the freetype version matches between systemfonts
  and another package match
- Added a `name` column to the output of
  [`glyph_info()`](https://systemfonts.r-lib.org/reference/glyph_info.md)
  to report the name of the glyph is provided by the font
- Added a `charmaps` column to the output of
  [`font_info()`](https://systemfonts.r-lib.org/reference/font_info.md)
  to report the name of the character maps provided by the font
- Cached faces are now reference counted when they are handed off to
  another package and it is the other packages’ responsibility to
  decrement the reference by calling `FT_Done_Face()` when finished with
  it.

## systemfonts 1.2.3

CRAN release: 2025-04-30

- Added
  [`fonts_as_import()`](https://systemfonts.r-lib.org/reference/fonts_as_import.md)
  to create stylesheet urls for embedding of fonts in HTML and SVG
- Added two C-level functions for getting glyph outline and bitmap
  information

## systemfonts 1.2.2

CRAN release: 2025-04-04

- Fix compilation on macOS when the obj-c++ compiler fails to pick up
  the right obj-c++ version
  ([\#122](https://github.com/r-lib/systemfonts/issues/122))
- [`add_fonts()`](https://systemfonts.r-lib.org/reference/add_fonts.md)
  now supports urls as well as file paths
  ([\#124](https://github.com/r-lib/systemfonts/issues/124))

## systemfonts 1.2.1

CRAN release: 2025-01-20

- Fix a memory issue when adding new fonts with
  [`add_fonts()`](https://systemfonts.r-lib.org/reference/add_fonts.md)
- Default to not downloading woff2 files from Google Fonts since it is
  poorly supported on many systems
- Fixed a bug in
  [`get_from_font_squirrel()`](https://systemfonts.r-lib.org/reference/web-fonts.md)
  where the font wasn’t placed in the user specified location

## systemfonts 1.2.0

CRAN release: 2025-01-16

- Providing the font name as the family should now result in better
  matching
- Improved the fallback options for Windows so that as many scripts are
  now covered
- Add infrastructure to add uninstalled font files to the search path
  used for font matching
- Add facilities to download and register fonts from web repositories
  such as Google Fonts and Font Squirrel
- Add
  [`require_font()`](https://systemfonts.r-lib.org/reference/require_font.md)
  that does it’s best to ensure that a given font is available after
  being called.
- Added functions for extracting outline and raster representation of
  glyphs

## systemfonts 1.1.0

CRAN release: 2024-05-15

- [`match_fonts()`](https://systemfonts.r-lib.org/reference/match_fonts.md)
  have been added as a vectorized and generalized version of
  [`match_font()`](https://systemfonts.r-lib.org/reference/match_fonts.md).
  In the process
  [`match_font()`](https://systemfonts.r-lib.org/reference/match_fonts.md)
  has been deprecated in favour of
  [`match_fonts()`](https://systemfonts.r-lib.org/reference/match_fonts.md)
- Two internal functions for converting weight and width names to
  integers have been exported
- Fix a segfault on macOS when the system encounters a corrupted font
  collection ([\#113](https://github.com/r-lib/systemfonts/issues/113))

## systemfonts 1.0.6

CRAN release: 2024-03-07

- Fix a bug in
  [`shape_string()`](https://systemfonts.r-lib.org/reference/shape_string.md)
  using `vjust = 1`
  ([\#85](https://github.com/r-lib/systemfonts/issues/85))

## systemfonts 1.0.4

CRAN release: 2022-02-11

- Use Courier New as default mono font on macOS instead of Courier to
  avoid issues between FreeType and Courier
  ([\#105](https://github.com/r-lib/systemfonts/issues/105))

## systemfonts 1.0.4

CRAN release: 2022-02-11

- Provide a fallback solution to the setup of the CRAN windows builder
  so that fonts can be discovered
  ([\#87](https://github.com/r-lib/systemfonts/issues/87))

## systemfonts 1.0.3

CRAN release: 2021-10-13

- Avoid warning when including the systemfonts header
  ([\#77](https://github.com/r-lib/systemfonts/issues/77))
- Fix size selection of non-scalable fonts when the requested size is
  bigger than the available
- Fix compilation bug when systemfont is used in C packages
  ([\#76](https://github.com/r-lib/systemfonts/issues/76))

## systemfonts 1.0.2

CRAN release: 2021-05-11

- Ensure compitability with freetype \<= 2.4.11
  ([\#70](https://github.com/r-lib/systemfonts/issues/70),
  [@jan-glx](https://github.com/jan-glx))
- Prepare for UCRT compilation

## systemfonts 1.0.1

CRAN release: 2021-02-09

- Fix a bug in font matching on Windows when matching monospace fonts
- Fix a bug in
  [`reset_font_cache()`](https://systemfonts.r-lib.org/reference/reset_font_cache.md)
  on mac that would cause a system crash if the cache was not filled in
  advance ([\#67](https://github.com/r-lib/systemfonts/issues/67))

## systemfonts 1.0.0

CRAN release: 2021-02-01

- Tweak size determination for non-scalable fonts
- Fix bug when switching between scalable and non-scalable fonts in the
  cache
- Add utility for querying font fallbacks at both the R and C level
- Add C-level API for finding emoji embeddings in strings
- Add utility for getting weight of font from C code
- Add utility for getting family name of font from C code
- Add font weight and width to the output of
  [`font_info()`](https://systemfonts.r-lib.org/reference/font_info.md)

## systemfonts 0.3.2

CRAN release: 2020-09-29

- Fix compiled code for old R versions
- Changes to comply with next cpp11 version

## systemfonts 0.3.1

CRAN release: 2020-09-08

- Fixed warnings on CRAN LTO machine

## systemfonts 0.3.0

CRAN release: 2020-09-01

- Added `get_cached_face()` so that other packages might retrieve
  FT_Face objects from the cache.
- Adapted cpp11
- Add infrastructure for setting OpenType font features on a registered
  font with either
  [`register_font()`](https://systemfonts.r-lib.org/reference/register_font.md)
  or the new
  [`register_variant()`](https://systemfonts.r-lib.org/reference/register_variant.md),
  along with the
  [`font_feature()`](https://systemfonts.r-lib.org/reference/font_feature.md)
  function.

## systemfonts 0.2.3

CRAN release: 2020-06-09

- Replace the buggy Freetype cache subsystem with own implementation
- Fix indexing bug in `glyph_metrics()`

## systemfonts 0.2.2

CRAN release: 2020-05-14

- Fix remaining valgrind issues by fixing the included font-manager code
- Rewrite the text shaping algorithm to make it more future proof
- Work around a nasty freetype bug in their cache subsystem

## systemfonts 0.2.1

CRAN release: 2020-04-29

- Various fixes to the correctness of compiled code

## systemfonts 0.2.0

CRAN release: 2020-04-16

- Add
  [`string_widths_dev()`](https://systemfonts.r-lib.org/reference/string_widths_dev.md)
  and
  [`string_metrics_dev()`](https://systemfonts.r-lib.org/reference/string_metrics_dev.md)
  to request the current graphic device for string widths and metrics.
- Add system for registering non-system fonts for look-up.
- systemfonts will now detect user-installed fonts on Windows (possible
  after the 1806 update)
- Font lookup is now cached for faster performance. The caching will get
  flushed when new fonts are added to the registry, or manually with
  [`reset_font_cache()`](https://systemfonts.r-lib.org/reference/reset_font_cache.md)
- Systemfonts now provide querying of font information with
  [`font_info()`](https://systemfonts.r-lib.org/reference/font_info.md)
  and
  [`glyph_info()`](https://systemfonts.r-lib.org/reference/glyph_info.md)
- Basic string shaping is now provided with
  [`shape_string()`](https://systemfonts.r-lib.org/reference/shape_string.md)
- Line width calculation is now available with
  [`string_width()`](https://systemfonts.r-lib.org/reference/string_width.md)
  (ignores presence of newlines, use
  [`shape_string()`](https://systemfonts.r-lib.org/reference/shape_string.md)
  for more complicated strings)
- Added
  [`str_split_emoji()`](https://systemfonts.r-lib.org/reference/str_split_emoji.md)
  for splitting of strings into substrings of emoji and non-emoji glyphs
- Provide a header file for easy use from within C in other packages
- Fix memory management issues on Mac
- Fix handling of erroneous font files on windows

## systemfonts 0.1.1

CRAN release: 2019-07-01

- Fix compilation on systems with a very old fontconfig version
  (Solaris)

## systemfonts 0.1.0

CRAN release: 2019-06-28

- First version with
  [`match_font()`](https://systemfonts.r-lib.org/reference/match_fonts.md)
  and
  [`system_fonts()`](https://systemfonts.r-lib.org/reference/system_fonts.md)
  capabilities. More to come.
- Added a `NEWS.md` file to track changes to the package.
