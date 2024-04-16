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
  system_fonts_c()
}

#' Reset the system font cache
#' 
#' Building the list of system fonts is time consuming and is therefore cached.
#' This, in turn, means that changes to the system fonts (i.e. installing new 
#' fonts), will not propagate to systemfonts. The solution is to reset the 
#' cache, which will result in the next call to e.g. [match_fonts()] will 
#' trigger a rebuild of the cache.
#' 
#' @export
#' 
#' @examples 
#' all_fonts <- system_fonts()
#' 
#' ##-- Install a new font on the system --##
#' 
#' all_fonts_new <- system_fonts()
#' 
#' ## all_fonts_new will be equal to all_fonts
#' 
#' reset_font_cache()
#' 
#' all_fonts_new <- system_fonts()
#' 
#' ## all_fonts_new will now contain the new font
#' 
reset_font_cache <- function() {
  reset_font_cache_c()
}
