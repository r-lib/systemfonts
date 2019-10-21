# systemfonts (development version)

* Add `string_widths_dev()` and `string_metrics_dev()` to request the current 
  graphic device for string widths and metrics.
* Add system for registering non-system fonts for look-up.
* systemfonts will now detect user-installed fonts on Windows (possible after the 1806 update)

# systemfonts 0.1.1

* Fix compilation on systems with a very old fontconfig version (Solaris)

# systemfonts 0.1.0

* First version with `match_font()` and `system_fonts()` capabilities. More to
  come.
* Added a `NEWS.md` file to track changes to the package.
