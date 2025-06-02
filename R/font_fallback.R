#' Get the fallback font for a given string
#'
#' A fallback font is a font to use as a substitute if the chosen font does not
#' contain the requested characters. Using font fallbacks means that the user
#' doesn't have to worry about mixing characters from different scripts or
#' mixing text and emojies. Fallback is calculated for the full string and the
#' result is platform specific. If no font covers all the characters in the
#' string an undefined "best match" is returned. The best approach is to figure
#' out which characters are not covered by your chosen font and figure out
#' fallbacks for these, rather than just request a fallback for the full string.
#'
#' @param string The strings to find fallbacks for
#' @inheritParams font_info
#'
#' @return A data frame with a `path` and `index` column giving fallback for the
#' specified string and font combinations
#'
#' @export
#'
#' @examples
#' font_fallback("\U0001f604") # Smile emoji
#'
font_fallback <- function(
  string,
  family = '',
  italic = FALSE,
  weight = "normal",
  width = "undefined",
  path = NULL,
  index = 0,
  variation = font_variation(),
  bold = deprecated()
) {
  if (lifecycle::is_present(bold)) {
    lifecycle::deprecate_soft(
      "1.2.4",
      "font_fallback(bold)",
      "font_fallback(weight)"
    )
    weight <- ifelse(bold, "bold", "normal")
  }
  if (is_font_variation(variation)) variation <- list(variation)
  full_length <- length(string)
  if (is.null(path)) {
    fonts <- match_fonts(
      family = rep_len(family, full_length),
      italic = rep_len(italic, full_length),
      weight = rep_len(weight, full_length),
      width = rep_len(width, full_length)
    )
    path <- fonts$path
    index <- fonts$index
  } else {
    full_length <- max(length(path), length(index), full_length)
    if (!all(c(length(path), length(index)) == 1)) {
      path <- rep_len(path, full_length)
      index <- rep_len(index, full_length)
    }
  }
  if (length(string) != 1) string <- rep_len(string, full_length)
  variation <- rep_len(variation, full_length)
  if (!all(file.exists(path)))
    stop("path must point to a valid file", call. = FALSE)
  get_fallback_c(path, as.integer(index), as.character(string), variation)
}
