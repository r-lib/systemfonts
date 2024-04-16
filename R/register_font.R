#' Register font collections as families
#' 
#' By design, systemfonts searches the fonts installed natively on the system.
#' It is possible, however, to register other fonts from e.g. font packages or
#' local font files, that will get searched before searching any installed 
#' fonts. You can always get an overview over all registered fonts with the 
#' `registry_fonts()` function that works as a registry focused analogue to 
#' [system_fonts()]. If you wish to clear out the registry, you can either 
#' restart the R session or call `clear_registry()`.
#' 
#' @param name The name the collection will be known under (i.e. *family*)
#' @param plain,bold,italic,bolditalic Fontfiles for the different faces of the
#' collection. can either be a filepath or a list containing a filepath and an
#' index (only for font files containing multiple fonts). If not given it will
#' default to the `plain` specification.
#' @param features A [`font_feature`] object describing the specific OpenType
#' font features to turn on for the registered font.
#' 
#' @return `register_font()` and `clear_registry()` returns `NULL` invisibly.
#' `registry_fonts()` returns a data table in the same style as [system_fonts()]
#' though less detailed and not based on information in the font file.
#' 
#' @details 
#' `register_font` also makes it possible to use system fonts with traits that 
#' is not covered by the graphic engine in R. In plotting operations it is only
#' possible to specify a family name and whether or not the font should be bold
#' and/or italic. There are numerous fonts that will never get matched to this,
#' especially because bold is only one of many weights.
#' 
#' Apart from granting a way to use new varieties of fonts, font registration
#' also allows you to override the default `sans`, `serif`, and `mono` mappings,
#' simply by registering a collection to the relevant default name. As 
#' registered fonts are searched first it will take precedence over the default.
#' 
#' @export
#' 
#' @examples 
#' # Create a random font collection
#' fonts <- system_fonts()
#' plain <- sample(which(!fonts$italic & fonts$weight <= 'normal'), 1)
#' bold <- sample(which(!fonts$italic & fonts$weight > 'normal'), 1)
#' italic <- sample(which(fonts$italic & fonts$weight <= 'normal'), 1)
#' bolditalic <- sample(which(fonts$italic & fonts$weight > 'normal'), 1)
#' register_font(
#'   'random', 
#'   plain = list(fonts$path[plain], fonts$index[plain]), 
#'   bold = list(fonts$path[bold], fonts$index[bold]), 
#'   italic = list(fonts$path[italic], fonts$index[italic]),
#'   bolditalic = list(fonts$path[bolditalic], fonts$index[bolditalic])
#' )
#' 
#' # Look at your creation
#' registry_fonts()
#' 
#' # Reset
#' clear_registry()
#' 
register_font <- function(name, plain, bold = plain, italic = plain, bolditalic = plain, features = font_feature()) {
  if (name %in% system_fonts()$family) {
    stop("A system font with that family name already exists", call. = FALSE)
  }
  if (is.character(plain)) plain <- list(plain, 0)
  if (is.character(bold)) bold <- list(bold, 0)
  if (is.character(italic)) italic <- list(italic, 0)
  if (is.character(bolditalic)) bolditalic <- list(bolditalic, 0)
  files <- c(plain[[1]], bold[[1]], italic[[1]], bolditalic[[1]])
  indices <- c(plain[[2]], bold[[2]], italic[[2]], bolditalic[[2]])
  if (!all(file.exists(files))) {
    stop("reference to non-existing font file", call. = FALSE)
  }
  
  register_font_c(as.character(name), as.character(files), as.integer(indices), features[[1]], features[[2]])
}
#' @rdname register_font
#' @export
registry_fonts <- function() {
  registry_fonts_c()
}
#' @rdname register_font
#' @export
clear_registry <- function() {
  clear_registry_c()
}

#' Register a font as a variant as an existing one
#' 
#' This function is a wrapper around [register_font()] that allows you to easily
#' create variants of existing system fonts, e.g. to target different weights
#' and/or widths, or for attaching OpenType features to a font.
#' 
#' @param name The new family name the variant should respond to
#' @param family The name of an existing font family that this is a variant of
#' @param weight One or two of `"thin"`, `"ultralight"`, `"light"`, `"normal"`, 
#' `"medium"`, `"semibold"`, `"bold"`, `"ultrabold"`, or `"heavy"`. If one is 
#' given it sets the weight for the whole variant. If two is given the first
#' one defines the plain weight and the second the bold weight. If `NULL` then
#' the variants of the given family closest to `"normal"` and `"bold"` will be
#' chosen.
#' @param width One of `"ultracondensed"`, `"extracondensed"`, `"condensed"`, 
#' `"semicondensed"`, `"normal"`, `"semiexpanded"`, `"expanded"`, 
#' `"extraexpanded"`, or `"ultraexpanded"` giving the width of the variant. If
#' `NULL` then the width closest to `"normal"` will be chosen.
#' @param features A [`font_feature`] object describing the specific OpenType
#' font features to turn on for the registered font variant.
#' 
#' @export
#' 
#' @examples
#' # Get the default "sans" family
#' sans <- match_fonts("sans")$path
#' sans <- system_fonts()$family[system_fonts()$path == sans][1]
#' 
#' # Register a variant of it:
#' register_variant(
#'   "sans_ligature", 
#'   sans, 
#'   features = font_feature(ligatures = "discretionary")
#' )
#' 
#' registry_fonts()
#' 
#' # clean up
#' clear_registry()
register_variant <- function(name, family, weight = NULL, width = NULL, features = font_feature()) {
  sys_fonts <- system_fonts()
  sys_fonts <- sys_fonts[grepl(tolower(family), tolower(sys_fonts$family)), , drop = FALSE]
  if (!is.null(width)) {
    sys_fonts <- sys_fonts[sys_fonts$width == tolower(width), , drop = FALSE]
  }
  if (!is.null(weight)) {
    sys_fonts <- sys_fonts[sys_fonts$weight %in% tolower(weight), , drop = FALSE]
  }
  if (nrow(sys_fonts) == 0) {
    stop("No font with the given family name and weight/width settings available", call. = FALSE)
  }
  if (any(tolower(family) == tolower(sys_fonts$family))) {
    sys_fonts <- sys_fonts[tolower(family) == tolower(sys_fonts$family), , drop = FALSE]
  }
  if (is.null(width)) {
    width <- sys_fonts$width[which.min(abs(as.integer(sys_fonts$width) - 5))]
    sys_fonts <- sys_fonts[sys_fonts$width == tolower(width), , drop = FALSE]
  }
  if (is.null(weight)) {
    normal <- which.min(abs(as.integer(sys_fonts$weight) - 4))
    bold <- which.min(abs(as.integer(sys_fonts$weight) - 7))
    weight <- sys_fonts$weight[unique(c(normal, bold))]
  }
  plain <- sys_fonts[which(sys_fonts$weight == weight[1] & !sys_fonts$italic), , drop = FALSE]
  bold <- if (length(weight) == 2) sys_fonts[which(sys_fonts$weight == weight[2] & !sys_fonts$italic), , drop = FALSE] else plain
  italic <- sys_fonts[which(sys_fonts$weight == weight[1] & sys_fonts$italic), , drop = FALSE]
  bolditalic <- if (length(weight) == 2) sys_fonts[which(sys_fonts$weight == weight[2] & sys_fonts$italic), , drop = FALSE] else italic
  if (nrow(plain) == 0) plain <- italic
  if (nrow(bold) == 0) bold <- plain
  if (nrow(italic) == 0) italic <- plain
  if (nrow(bolditalic) == 0) bolditalic <- if (length(weight) == 2) bold else italic
  register_font(
    name, 
    as.list(plain[1, c('path', 'index')]), 
    as.list(bold[1, c('path', 'index')]), 
    as.list(italic[1, c('path', 'index')]), 
    as.list(bolditalic[1, c('path', 'index')]), 
    features
  )
}
