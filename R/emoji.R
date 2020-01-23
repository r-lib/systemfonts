#' Split a string into emoji and non-emoji glyph runs
#' 
#' In order to do correct text rendering, the font needed must be figured out. A
#' common case is rendering of emojis within a string where the system emoji 
#' font is used rather than the requested font. This function will inspect the
#' provided strings and split them up in runs that must be rendered with the
#' emoji font, and the rest. Arguments are recycled to the length of the `string`
#' vector.
#' 
#' @param string A character vector of strings that should be splitted.
#' @inheritParams match_font
#' @param path,index path an index of a font file to circumvent lookup based on 
#' family and style
#' 
#' @return A data.frame containing the following columns:
#' \describe{
#'   \item{string}{The substring containing a consecutive run of glyphs}
#'   \item{id}{The index into the original `string` vector that the substring is part of}
#'   \item{emoji}{A logical vector giving if the substring is a run of emojis or not}
#' }
#' 
#' @export
#' 
#' @examples 
#' emoji_string <- "This is a joke\U0001f642. It should be obvious from the smiley"
#' str_split_emoji(emoji_string)
#' 
str_split_emoji <- function(string, family = '', italic = FALSE, bold = FALSE, 
                            path = NULL, index = 0) {
  n_strings <- length(string)
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
      index <- vapply(loc, `[[`, integer(1L), 2, USE.NAMES = FALSE)
    }
  } else {
    if (!all(c(length(path), length(index)) == 1)) {
      path <- rep_len(path, n_strings)
      index <- rep_len(index, n_strings)
    }
  }
  if (!all(file.exists(path))) stop("path must point to a valid file", call. = FALSE)
  emoji_splitted <- .Call("emoji_split_c", as.character(string), path, index, PACKAGE = "systemfonts")
  groups <- diff(c(0, which(diff(emoji_splitted[[3]]) != 0), length(emoji_splitted[[3]])))
  groups <- rep(seq_along(groups), groups)
  groups <- paste0(emoji_splitted[[2]], '_', groups)
  string <- vapply(split(emoji_splitted[[1]], groups), intToUtf8, character(1))
  id <- vapply(split(emoji_splitted[[2]], groups), `[[`, integer(1), 1)
  emoji <- vapply(split(emoji_splitted[[3]], groups), `[[`, logical(1), 1)
  res <- data.frame(string = string, id = id + 1, emoji = emoji, stringsAsFactors = FALSE, row.names = NULL)
  class(res) <- c("tbl_df", "tbl", "data.frame")
  res
}

load_emoji_codes <- function() {
  .Call("load_emoji_codes_c", as.integer(all_emoji), as.integer(text_pres_emoji), 
        as.integer(base_mod_emoji), PACKAGE = "systemfonts")
  invisible(NULL)
}
