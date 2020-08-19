#' Get string widths as measured by the current device
#' 
#' For certain composition tasks it is beneficial to get the width of a string 
#' as interpreted by the device that is going to plot it. grid provides this
#' through construction of a `textGrob` and then converting the corresponding 
#' grob width to e.g. cm, but this comes with a huge overhead. 
#' `string_widths_dev()` provides direct, vectorised, access to the graphic
#' device for as high performance as possible.
#' 
#' @param strings A character vector of strings to measure
#' @param family The font families to use. Will get recycled
#' @param face The font faces to use. Will get recycled
#' @param size The font size to use. Will get recycled
#' @param cex The cex multiplier to use. Will get recycled
#' @param unit The unit to return the width in. Either `"cm"`, `"inches"`, 
#' `"device"`, or `"relative"`
#' 
#' @return A numeric vector with the width of each of the strings given in 
#' `strings` in the unit given in `unit`
#' 
#' @export
#' 
#' @family device metrics
#' 
#' @examples 
#' # Get the widths as measured in cm (default)
#' string_widths_dev(c('a string', 'an even longer string'))
#' 
string_widths_dev <- function(strings, family = '', face = 1, size = 12, cex = 1, unit = 'cm') {
  unit <- match.arg(unit, possible_units)
  unit <- match(unit, possible_units) - 1L
  n_total <- length(strings)
  if (length(family) != 1) family <- rep_len(family, n_total)
  if (any(c(length(face), length(size), length(cex)) != 1)) {
    face <- rep_len(face, n_total)
    size <- rep_len(size, n_total)
    cex <- rep_len(cex, n_total)
  }
  dev_string_widths_c(as.character(strings), as.character(family), 
    as.integer(face), as.numeric(size), as.numeric(cex), unit)
}
#' Get string metrics as measured by the current device
#' 
#' This function is much like [string_widths_dev()] but also returns the ascent 
#' and descent of the string making it possible to construct a tight bounding
#' box around the string.
#' 
#' @inheritParams string_widths_dev
#' 
#' @return A data.frame with `width`, `ascent`, and `descent` columns giving the
#' metrics in the requested unit.
#' 
#' @family device metrics
#' 
#' @export
#' 
#' @examples 
#' # Get the metrics as measured in cm (default)
#' string_metrics_dev(c('some text', 'a string with descenders'))
#' 
string_metrics_dev <- function(strings, family = '', face = 1, size = 12, cex = 1, unit = 'cm') {
  unit <- match.arg(unit, possible_units)
  unit <- match(unit, possible_units) - 1L
  n_total <- length(strings)
  if (length(family) != 1) family <- rep_len(family, n_total)
  if (any(c(length(face), length(size), length(cex)) != 1)) {
    face <- rep_len(face, n_total)
    size <- rep_len(size, n_total)
    cex <- rep_len(cex, n_total)
  }
  dev_string_metrics_c(as.character(strings), as.character(family), 
    as.integer(face), as.numeric(size), as.numeric(cex), unit)
}

# Order important. Will get converted to 0-indexed unit identity for C code
possible_units <- c('cm', 'inches', 'device', 'relative')
