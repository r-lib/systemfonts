#' Find a system font by name and style
#'
#' This function locates the font file (and index) best matching a name and
#' optional style. A font file will be returned even if a perfect match
#' isn't found, but it is not necessarily similar to the requested family and
#' it should not be relied on for font substitution. The aliases `"sans"`,
#' `"serif"`, `"mono"`, `"symbol"`, and `"emoji"` match to their respective
#' system defaults (`""` is equivalent to `"sans"`). `match_font()` has been
#' deprecated in favour of `match_fonts()` which provides vectorisation, as well
#' as querying for different weights (rather than just "normal" and "bold") as
#' well as different widths.
#'
#' # Font matching
#' During font matching, systemfonts has to look in three different locations.
#' The font registry (populated by [register_font()]/[register_variant()]), the
#' local fonts (populated with [add_fonts()]/[scan_local_fonts()]), and the
#' fonts installed on the system. It does so in that order: registry > local >
#' installed.
#'
#' The matching performed at each step also differs. The fonts in the registry
#' is only matched by family name. The local fonts are matched based on all the
#' provided parameters (family, weight, italic, etc) in a way that is local to
#' systemfonts, but try to emulate the system native matching. The installed
#' fonts are matched using the system native matching functionality on macOS and
#' Linux. On Windows the installed fonts are read from the system registry and
#' matched using the same approach as for local fonts. Matching will always find
#' a font no matter what you throw at it, defaulting to "sans" if nothing else
#' is found.
#'
#' @param family The name of the font families to match
#' @param italic logical indicating the font slant
#' @param weight The weight to query for, either in numbers (`0`, `100`, `200`,
#' `300`, `400`, `500`, `600`, `700`, `800`, or `900`) or strings (`"undefined"`,
#' `"thin"`, `"ultralight"`, `"light"`, `"normal"`, `"medium"`, `"semibold"`,
#' `"bold"`, `"ultrabold"`, or `"heavy"`). `NA` will be interpreted as
#' `"undefined"`/`0`
#' @param width The width to query for either in numbers (`0`, `1`, `2`,
#' `3`, `4`, `5`, `6`, `7`, `8`, or `9`) or strings (`"undefined"`,
#' `"ultracondensed"`, `"extracondensed"`, `"condensed"`, `"semicondensed"`,
#' `"normal"`, `"semiexpanded"`, `"expanded"`, `"extraexpanded"`, or
#' `"ultraexpanded"`). `NA` will be interpreted as `"undefined"`/`0`
#' @param bold logical indicating whether the font weight
#'
#' @return A list containing the paths locating the font files, the 0-based
#' index of the font in the files and the features for the font in case a
#' registered font was located.
#'
#' @export
#'
#' @examples
#' # Get the system default sans-serif font in italic
#' match_fonts('sans', italic = TRUE)
#'
#' # Try to match it to a thin variant
#' match_fonts(c('sans', 'serif'), weight = "thin")
#'
match_fonts <- function(
  family,
  italic = FALSE,
  weight = "normal",
  width = "undefined"
) {
  if (!is.character(family) || anyNA(family))
    stop("`family` must be a character vector without NA values", call. = FALSE)
  weight <- as_font_weight(weight)
  width <- as_font_width(width)
  n_max <- max(length(family), length(italic), length(weight), length(width))
  locate_fonts_c(
    rep_len(family, n_max),
    rep_len(as.numeric(italic), n_max),
    rep_len(weight, n_max),
    rep_len(width, n_max)
  )
}
#' @rdname match_fonts
#' @export
match_font <- function(family, italic = FALSE, bold = FALSE) {
  lifecycle::deprecate_soft("1.1.0", "match_font()", "match_fonts()")
  if (!is.character(family)) stop("family must be a string", call. = FALSE)
  match_font_c(family, as.logical(italic), as.logical(bold))
}


weights <- c(
  "undefined",
  "thin",
  "ultralight",
  "light",
  "normal",
  "medium",
  "semibold",
  "bold",
  "ultrabold",
  "heavy"
)
#' Convert weight and width to numerics
#'
#' It is often more natural to describe font weight and width with names rather
#' than numbers (e.g. "bold" or "condensed"), but underneath these names are
#' matched to numeric values. These two functions are used to retrieve the
#' numeric counterparts to names
#'
#' @param weight,width character vectors with valid names for weight or width
#'
#' @return An integer vector matching the length of the input
#'
#' @keywords internal
#'
#' @export
#'
#' @examples
#' as_font_weight(
#'   c("undefined", "thin", "ultralight", "light", "normal", "medium", "semibold",
#'     "bold", "ultrabold", "heavy")
#' )
#'
as_font_weight <- function(weight) {
  if (is.factor(weight)) weight <- as.character(weight)
  if (is.logical(weight) && all(is.na(weight))) weight <- "undefined"
  if (is.character(weight)) {
    weight[is.na(weight)] <- "undefined"
    weight <- match(tolower(weight), weights)
    if (anyNA(weight)) {
      stop(
        paste0(
          "`weight` must be one of ",
          paste('"', weights[1:9], '"', collapse = ", ", sep = ''),
          ', or "',
          weights[10],
          '"'
        ),
        call. = FALSE
      )
    }
    weight <- (weight - 1) * 100
  } else if (is.numeric(weight) || is.integer(weight)) {
    weight[is.na(weight)] <- 0
  } else {
    stop("Can't convert this object to a font weight", call. = FALSE)
  }
  as.numeric(weight)
}
widths <- c(
  "undefined",
  "ultracondensed",
  "extracondensed",
  "condensed",
  "semicondensed",
  "normal",
  "semiexpanded",
  "expanded",
  "extraexpanded",
  "ultraexpanded"
)
#' @rdname as_font_weight
#' @export
#'
#' @examples
#' as_font_width(
#'   c("undefined", "ultracondensed", "extracondensed", "condensed", "semicondensed",
#'   "normal", "semiexpanded", "expanded", "extraexpanded", "ultraexpanded")
#' )
#'
as_font_width <- function(width) {
  if (is.factor(width)) width <- as.character(width)
  if (is.logical(width) && all(is.na(width))) width <- "undefined"
  if (is.character(width)) {
    width[is.na(width)] <- "undefined"
    width <- match(tolower(width), widths)
    if (anyNA(width)) {
      stop(
        paste0(
          "`width` must be one of ",
          paste('"', widths[1:9], '"', collapse = ", ", sep = ''),
          ', or "',
          widths[10],
          '"'
        ),
        call. = FALSE
      )
    }
    width <- width - 1
  } else if (is.numeric(width) || is.integer(width)) {
    width[is.na(width)] <- 0
  } else {
    stop("Can't convert this object to a font width", call. = FALSE)
  }
  as.numeric(width)
}
