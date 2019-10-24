#' @export
font_info <- function(family = '', italic = FALSE, bold = FALSE, size = 12, 
                      res = 300, path = NULL, index = 0) {
  if (is.null(path)) {
    loc <- match_font(family, italic, bold)
    path <- loc$path
    index <- loc$index
  }
  if (!file.exists(path)) stop("path must point to a valid file", call. = FALSE)
  .Call("get_font_info_c", path, as.integer(index), as.numeric(size), as.numeric(res), PACKAGE = "systemfonts")
}
#' @export
glyph_info <- function(glyphs, family = '', italic = FALSE, bold = FALSE, 
                       size = 12, res = 300, path = NULL, index = 0) {
  if (is.null(path)) {
    loc <- match_font(family, italic, bold)
    path <- loc$path
    index <- loc$index
  }
  if (!file.exists(path)) stop("path must point to a valid file", call. = FALSE)
  glyphs <- unlist(strsplit(glyphs, ''))
  .Call("get_glyph_info_c", glyphs, path, as.integer(index), as.numeric(size), as.numeric(res), PACKAGE = "systemfonts")
}
