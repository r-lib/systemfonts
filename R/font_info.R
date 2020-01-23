#' Query font-specific information
#' 
#' Get general information about a font, relative to a given size. Size specific
#' measures will be returned in pixel units. The function is vectorised to the 
#' length of the longest argument.
#' 
#' @inheritParams match_font
#' @param size The pointsize of the font to use for size related measures
#' @param res The ppi of the size related mesures
#' @param path,index path an index of a font file to circumvent lookup based on 
#' family and style
#' 
#' @return 
#' A data.frame giving info on the requested font + size combinations. The 
#' data.frame will contain the following columns:
#' 
#' \describe{
#'   \item{path}{The path to the font file}
#'   \item{index}{The 0-based index of the font in the fontfile}
#'   \item{family}{The family name of the font}
#'   \item{style}{The style name of the font}
#'   \item{italic}{A logical giving if the font is italic}
#'   \item{bold}{A logical giving if the font is bold}
#'   \item{monospace}{A logical giving if the font is monospace}
#'   \item{kerning}{A logical giving if the font supports kerning}
#'   \item{color}{A logical giving if the font has color glyphs}
#'   \item{scalable}{A logical giving if the font is scalable}
#'   \item{vertical}{A logical giving if the font is vertical}
#'   \item{n_glyphs}{The number of glyphs in the font}
#'   \item{n_sizes}{The number of predefined sizes in the font}
#'   \item{n_charmaps}{The number of character mappings in the font file}
#'   \item{bbox}{A bounding box large enough to contain any of the glyphs in the font}
#'   \item{max_ascend}{The maximum ascend of the tallest glyph in the font}
#'   \item{max_descent}{The maximum descend of the most descending glyph in the font}
#'   \item{max_advance_width}{The maximum horizontal advance a glyph can make}
#'   \item{max_advance_height}{The maximum vertical advance a glyph can make}
#'   \item{lineheight}{The height of a single line of text in the font}
#'   \item{underline_pos}{The position of a potential underlining segment}
#'   \item{underline_size}{The width the the underline}
#' }
#' 
#' @export
#' 
#' @examples 
#' font_info('serif')
#' 
#' # Avoid lookup if font file is already known
#' sans <- match_font('sans')
#' font_info(path = sans$path, index = sans$index)
#' 
font_info <- function(family = '', italic = FALSE, bold = FALSE, size = 12, 
                      res = 72, path = NULL, index = 0) {
  full_length <- max(length(size), length(res))
  if (is.null(path)) {
    full_length <- max(length(family), length(italic), length(bold), full_length)
    if (all(c(length(family), length(italic), length(bold)) == 1)) {
      loc <- match_font(family, italic, bold)
      path <- loc$path
      index <- loc$index
    } else {
      family <- rep_len(family, full_length)
      italic <- rep_len(italic, full_length)
      bold <- rep_len(bold, full_length)
      loc <- Map(match_font, family = family, italic = italic, bold = bold)
      path <- vapply(loc, `[[`, character(1L), 1, USE.NAMES = FALSE)
      index <- vapply(loc, `[[`, integer(1L), 2, USE.NAMES = FALSE)
    }
    
  } else {
    full_length <- max(length(path), length(index), full_length)
    if (!all(c(length(path), length(index)) == 1)) {
      path <- rep_len(path, full_length)
      index <- rep_len(index, full_length)
    }
  }
  if (length(size) != 1) size <- rep_len(size, full_length)
  if (length(res) != 1) res <- rep_len(res, full_length)
  if (!all(file.exists(path))) stop("path must point to a valid file", call. = FALSE)
  .Call("get_font_info_c", path, as.integer(index), as.numeric(size), as.numeric(res), PACKAGE = "systemfonts")
}
#' Query glyph-specific information from fonts
#' 
#' This function allows you to extract information about the individual glyphs 
#' in a font, based on a specified size. All size related measures are in 
#' pixel-units. The function is vectorised to the length of the `glyphs` vector.
#' 
#' @param glyphs A vector of glyphs. Strings will be split into separate glyphs
#' automatically
#' @inheritParams font_info
#' @param path,index path an index of a font file to circumvent lookup based on 
#' family and style
#' 
#' @return 
#' A data.frame with information about each glyph, containing the following 
#' columns:
#' 
#' \describe{
#'   \item{glyph}{The glyph as a character}
#'   \item{index}{The index of the glyph in the font file}
#'   \item{width}{The width of the glyph}
#'   \item{height}{The height of the glyph}
#'   \item{x_bearing}{The horizontal distance from the origin to the leftmost part of the glyph}
#'   \item{y_bearing}{The vertical distance from the origin to the top part of the glyph}
#'   \item{x_advance}{The horizontal distance to move the cursor after adding the glyph}
#'   \item{y_advance}{The vertical distance to move the cursor after adding the glyph}
#'   \item{bbox}{The tight bounding box surrounding the glyph}
#' }
#' 
#' @export
glyph_info <- function(glyphs, family = '', italic = FALSE, bold = FALSE, 
                       size = 12, res = 72, path = NULL, index = 0) {
  n_strings <- length(glyphs)
  glyphs <- strsplit(glyphs, '')
  n_glyphs <- lengths(glyphs)
  glyphs <- unlist(glyphs)
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
      path <- rep(vapply(loc, `[[`, character(1L), 1, USE.NAMES = FALSE), n_glyphs)
      index <- rep(vapply(loc, `[[`, integer(1L), 2, USE.NAMES = FALSE), n_glyphs)
    }
    
  } else {
    if (!all(c(length(path), length(index)) == 1)) {
      path <- rep(rep_len(path, n_strings), n_glyphs)
      index <- rep(rep_len(index, n_strings), n_glyphs)
    }
  }
  if (length(size) != 1) size <- rep(rep_len(size, n_strings), n_glyphs)
  if (length(res) != 1) res <- rep(rep_len(res, n_strings), n_glyphs)
  if (!all(file.exists(path))) stop("path must point to a valid file", call. = FALSE)
  .Call("get_glyph_info_c", glyphs, path, as.integer(index), as.numeric(size), as.numeric(res), PACKAGE = "systemfonts")
}
