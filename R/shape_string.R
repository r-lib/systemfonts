#' @export
shape_string <- function(strings, family = '', italic = FALSE, bold = FALSE, 
                         size = 12, res = 300, path = NULL, index = 0) {
  n_strings = length(strings)
  if (is.null(path)) {
    if (all(c(length(family), length(italic), length(bold)) == 1)) {
      loc <- match_font(family, italic, bold)
      path <- loc$path
      index <- loc$index
    } else {
      family <- rep_len(family, n_strings)
      italic <- rep_len(italic, n_strings)
      bold <- rep_len(bold, n_strings)
      loc <- Map(match_font, family = family, italic = italic, bold = bold)
      path <- vapply(loc, `[[`, character(1L), 1, USE.NAMES = FALSE)
      index <- vapply(loc, `[[`, character(1L), 2, USE.NAMES = FALSE)
    }
  } else {
    if (!all(c(length(path), length(index)) == 1)) {
      path <- rep_len(path, n_strings)
      index <- rep_len(index, n_strings)
    }
  }
  if (length(size) != 1) size <- rep_len(size, n_strings)
  if (length(res) != 1) res <- rep_len(res, n_strings)
  if (!all(file.exists(path))) stop("path must point to a valid file", call. = FALSE)
  .Call("get_string_shape_c", as.character(strings), path, as.integer(index), as.numeric(size), as.numeric(res), PACKAGE = "systemfonts")
}
