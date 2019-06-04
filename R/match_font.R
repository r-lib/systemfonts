#' Find a system font by name and style
#'
#' This function locates the font file best matching a name and optional style
#' (italic/bold).
#'
#' @return A path locating the font file
#'
#' @export
#'
match_font <- function(family, italic = FALSE, bold = FALSE) {
  if (!is.character(family)) stop("family must be a string", call. = FALSE)
  .Call("match_font_c", family, as.logical(italic), as.logical(bold))
}