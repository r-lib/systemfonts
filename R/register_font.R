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
register_font <- function(name, plain, bold = plain, italic = plain, bolditalic = plain) {
  if (is.character(plain)) plain <- list(plain, 0)
  if (is.character(bold)) bold <- list(bold, 0)
  if (is.character(italic)) italic <- list(italic, 0)
  if (is.character(bolditalic)) bolditalic <- list(bolditalic, 0)
  files <- c(plain[[1]], bold[[1]], italic[[1]], bolditalic[[1]])
  indices <- c(plain[[2]], bold[[2]], italic[[2]], bolditalic[[2]])
  if (!all(file.exists(files))) {
    stop("reference to non-existing font file", call. = FALSE)
  }
  
  invisible(.Call("register_font_c", as.character(name), as.character(files), as.integer(indices), PACKAGE = "systemfonts"))
}
#' @rdname register_font
#' @export
registry_fonts <- function() {
  .Call("registry_fonts_c", PACKAGE = "systemfonts")
}
#' @rdname register_font
#' @export
clear_registry <- function() {
  invisible(.Call("clear_registry_c", PACKAGE = "systemfonts"))
}
