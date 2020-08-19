#' Find a system font by name and style
#'
#' This function locates the font file (and index) best matching a name and
#' optional style (italic/bold). A font file will be returned even if a match
#' isn't found, but it is not necessarily similar to the requested family and
#' it should not be relied on for font substitution. The aliases `"sans"`,
#' `"serif"`, and `"mono"` match to the system default sans-serif, serif, and
#' mono fonts respectively (`""` is equivalent to `"sans"`).
#'
#'
#' @param family The name of the font family
#' @param italic,bold logicals indicating the font style
#'
#' @return A list containing the path locating the font file and the 0-based
#' index of the font in the file.
#'
#' @export
#'
#' @examples
#' # Get the system default sans-serif font in italic
#' match_font('sans', italic = TRUE)
#'
match_font <- function(family, italic = FALSE, bold = FALSE) {
  if (!is.character(family)) stop("family must be a string", call. = FALSE)
  match_font_c(family, as.logical(italic), as.logical(bold))
}
