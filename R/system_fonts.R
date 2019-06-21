#' List all fonts installed on your system
#'
#' @return A data frame with a row for each font and various information in each
#' column
#'
#' @export
#'
#' @examples
#' # See all monospace fonts
#' fonts <- system_fonts()
#' fonts[fonts$monospace, ]
#'
system_fonts <- function() {
  .Call("system_fonts_c", PACKAGE = "systemfonts")
}