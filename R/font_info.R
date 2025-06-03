#' Query font-specific information
#'
#' Get general information about a font, relative to a given size. Size specific
#' measures will be returned in pixel units. The function is vectorised to the
#' length of the longest argument.
#'
#' @inheritParams match_font
#' @param size The pointsize of the font to use for size related measures
#' @param res The ppi of the size related measures
#' @param variation A `font_variation` object or a list of them to control
#' variable fonts
#' @param path,index path and index of a font file to circumvent lookup based on
#' family and style
#' @param bold `r lifecycle::badge("deprecated")` Use `weight = "bold"` instead
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
#'   \item{name}{The name of the font, if present, otherwise the family name}
#'   \item{italic}{A logical giving if the font is italic}
#'   \item{bold}{A logical giving if the font is bold}
#'   \item{monospace}{A logical giving if the font is monospace}
#'   \item{weight}{A factor giving the weight of the font}
#'   \item{width}{A factor giving the width of the font}
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
#' sans <- match_fonts('sans')
#' font_info(path = sans$path, index = sans$index)
#'
font_info <- function(
  family = '',
  italic = FALSE,
  weight = "normal",
  width = "undefined",
  size = 12,
  res = 72,
  path = NULL,
  index = 0,
  variation = font_variation(),
  bold = deprecated()
) {
  if (is_font_variation(variation)) variation <- list(variation)
  full_length <- max(
    length(size),
    length(res),
    length(italic),
    length(weight),
    length(width),
    length(variation)
  )
  if (is.null(path)) {
    if (lifecycle::is_present(bold)) {
      lifecycle::deprecate_soft("1.2.4", "font_info(bold)", "font_info(weight)")
      weight <- ifelse(bold, "bold", "normal")
    }
    full_length <- max(
      length(family),
      full_length
    )
    italic <- rep_len(italic, full_length)
    weight <- rep_len(weight, full_length)
    width <- rep_len(width, full_length)
    fonts <- match_fonts(
      family = rep_len(family, full_length),
      italic = italic,
      weight = weight,
      width = width
    )
    path <- fonts$path
    index <- fonts$index
  } else {
    full_length <- max(length(path), length(index), full_length)
    italic <- rep_len(italic, full_length)
    weight <- rep_len(weight, full_length)
    width <- rep_len(width, full_length)
    if (!all(c(length(path), length(index)) == 1)) {
      path <- rep_len(path, full_length)
      index <- rep_len(index, full_length)
    }
  }
  if (length(size) != 1) size <- rep_len(size, full_length)
  if (length(res) != 1) res <- rep_len(res, full_length)
  if (!all(file.exists(path))) {
    stop("path must point to a valid file", call. = FALSE)
  }
  variation <- add_standard_to_variations(
    rep_len(variation, full_length),
    italic = italic,
    weight = weight,
    width = width
  )
  get_font_info_c(
    path,
    as.integer(index),
    as.numeric(size),
    as.numeric(res),
    variation
  )
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
glyph_info <- function(
  glyphs,
  family = '',
  italic = FALSE,
  weight = "normal",
  width = "undefined",
  size = 12,
  res = 72,
  path = NULL,
  index = 0,
  variation = font_variation(),
  bold = deprecated()
) {
  if (is_font_variation(variation)) variation <- list(variation)
  n_strings <- length(glyphs)
  glyphs <- strsplit(glyphs, '')
  n_glyphs <- lengths(glyphs)
  glyphs <- unlist(glyphs)
  if (is.null(path)) {
    if (lifecycle::is_present(bold)) {
      lifecycle::deprecate_soft(
        "1.2.4",
        "glyph_info(bold)",
        "glyph_info(weight)"
      )
      weight <- ifelse(bold, "bold", "normal")
    }
    italic <- rep_len(italic, n_strings)
    weight <- rep_len(weight, n_strings)
    width <- rep_len(width, n_strings)
    fonts <- match_fonts(
      family = rep_len(family, n_strings),
      italic = italic,
      weight = weight,
      width = width
    )
    path <- rep(fonts$path, n_glyphs)
    index <- rep(fonts$index, n_glyphs)
    italic <- rep(italic, n_glyphs)
    weight <- rep(weight, n_glyphs)
    width <- rep(width, n_glyphs)
  } else {
    if (!all(c(length(path), length(index)) == 1)) {
      path <- rep(rep_len(path, n_strings), n_glyphs)
      index <- rep(rep_len(index, n_strings), n_glyphs)
    }
    italic <- rep(rep_len(italic, n_strings), n_glyphs)
    weight <- rep(rep_len(weight, n_strings), n_glyphs)
    width <- rep(rep_len(width, n_strings), n_glyphs)
  }
  if (length(size) != 1) size <- rep(rep_len(size, n_strings), n_glyphs)
  if (length(res) != 1) res <- rep(rep_len(res, n_strings), n_glyphs)
  if (!all(file.exists(path))) {
    stop("path must point to a valid file", call. = FALSE)
  }
  variation <- add_standard_to_variations(
    rep(rep_len(variation, n_strings), n_glyphs),
    italic = italic,
    weight = weight,
    width = width
  )
  get_glyph_info_c(
    glyphs,
    path,
    as.integer(index),
    as.numeric(size),
    as.numeric(res),
    variation
  )
}
