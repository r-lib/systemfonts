#' Define OpenType font feature settings
#' 
#' This function encapsulates the specification of OpenType font features. Some
#' specific features have named arguments, but all available features can be
#' set by using its specific 4-letter tag For a list of the 4-letter tags
#' available see e.g. the overview on 
#' [Wikipedia](https://en.wikipedia.org/wiki/List_of_typographic_features).
#' 
#' @param ligatures Settings related to ligatures. One or more types of 
#' ligatures to turn on (see details).
#' @param letters Settings related to the appearance of single 
#' letters (as opposed to ligatures that substitutes multiple letters). See 
#' details for supported values.
#' @param numbers Settings related to the appearance of numbers. See details for
#' supported values. 
#' @param ... key-value pairs with the key being the 4-letter tag and the value
#' being the setting (usually `TRUE` to turn it on).
#' 
#' @details 
#' OpenType features are defined by a 4-letter tag along with an integer value.
#' Often that value is a simple `0` (off) or `1` (on), but some features support
#' additional values, e.g. stylistic alternates (`salt`) where a font may 
#' provide multiple variants of a letter and the value will be used to chose 
#' which one to use.
#' 
#' Common features related to appearance may be given with a long form name to
#' either the `ligatures`, `letters`, or `numbers` argument to avoid remembering
#' the often arbitrary 4-letter tag. Providing a long form name is the same as
#' setting the tag to `1` and can thus not be used to set tags to other values.
#' 
#' The possible long form names are given below with the tag in parenthesis:
#' 
#' **Ligatures**  
#' - `standard` (*liga*): Turns on standard multiple letter substitution
#' - `historical` (*hlig*): Use obsolete historical ligatures
#' - `contextual` (*clig*): Apply secondary ligatures based on the character 
#'   patterns surrounding the potential ligature
#' - `discretionary` (*dlig*): Use ornamental ligatures
#' 
#' **Letters**  
#' - `swash` (*cswh*): Use contextual swashes (ornamental decorations)
#' - `alternates` (*calt*): Use alternate letter forms based on the sourrounding
#'   pattern
#' - `historical` (*hist*): Use obsolete historical forms of the letters
#' - `localized` (*locl*): Use alternate forms preferred by the script language 
#' - `randomize` (*rand*): Use random variants of the letters (e.g. to mimick
#'   handwriting)
#' - `alt_annotation` (*nalt*): Use alternate annotations (e.g. circled digits)
#' - `stylistic` (*salt*): Use a stylistic alternative form of the letter
#' - `subscript` (*subs*): Set letter in subscript
#' - `superscript` (*sups*): Set letter in superscript
#' - `titling` (*titl*): Use letter forms well suited for large text and titles
#' - `small_caps` (*smcp*): Use small caps variants of the letters
#' 
#' **Numbers**
#' - `lining` (*lnum*): Use number variants that rest on the baseline
#' - `oldstyle` (*onum*): Use old style numbers that use descender and ascender
#'   for various numbers
#' - `proportional` (*pnum*): Let numbers take up width based on the visual 
#'   width of the glyph
#' - `tabular` (*tnum*): Enforce all numbers to take up the same width
#' - `fractions` (*frac*): Convert numbers separated by `/` into a fraction 
#'   glyph
#' - `fractions_alt` (*afrc*): Use alternate fraction form with a horizontal 
#'   divider
#' 
#' @return A `font_feature` object
#' 
#' @export
#' 
#' @examples 
#' font_feature(letters = "stylistic", numbers = c("lining", "tabular"))
#' 
#' # Use the tag directly to access additional stylistic variants
#' font_feature(numbers = c("lining", "tabular"), salt = 2)
#' 
font_feature <- function(ligatures = NULL, letters = NULL, numbers = NULL , ...) {
  features <- list(...)
  if (!is.null(ligatures)) {
    for (lig in ligatures) {
      features[get_ligature_tag(lig)] <- 1L
    }
  }
  if (!is.null(letters)) {
    for (let in letters) {
      features[get_letter_tag(let)] <- 1L
    }
  }
  if (!is.null(numbers)) {
    for (num in numbers) {
      features[get_number_tag(num)] <- 1L
    }
  }
  if (length(features) == 0) {
    features <- list(character(), integer())
  } else {
    feature_names <- names(features)
    if (any(vapply(features, length, integer(1)) != 1)) {
      stop("A feature setting can only be of length 1", call. = FALSE)
    }
    if (anyDuplicated(feature_names)) {
      stop("Features can only be given once", call. = FALSE)
    }
    if (any(nchar(feature_names) != 4)) {
      stop("Feature tags must be 4 character long", call. = FALSE)
    }
    feature_settings <- vapply(features, as.integer, integer(1))
    features <- list(feature_names, feature_settings)
  }
  class(features) <- "font_feature"
  features
}
is_font_feature <- function(x) inherits(x, "font_feature")

#' @export
length.font_feature <- function(x) {
  length(x[[1]])
}
#' @export
print.font_feature <- function(x, ...) {
  if (length(x) == 0) {
    cat("An empty font_feature object\n")
  } else {
    cat("A list of OpenType font feature settings\n")
    cat(paste(paste0("- ", x[[1]], ": ", x[[2]]), collapse = "\n"))
  }
  invisible(x)
}
#' @export
format.font_feature <- function(x, ...) {
  if (length(x) == 0) {
    return("<empty>")
  }
  paste(x[[1]], ': ', x[[2]], collapse = '; ', sep = '')
}
#' @export
c.font_feature <- function(x, ...) {
  features <- rev(list(x, ...))
  names <- unlist(lapply(features, `[[`, 1))
  keep <- !duplicated(names)
  values <- unlist(lapply(features, `[[`, 2))[keep]
  structure(list(names[keep], values), class = "font_feature")
}

get_ligature_tag <- function(x) {
  name <- c("standard", "historical", "contextual", "discretionary")
  tags <- c("liga",     "hlig",       "clig",       "dlig"         )
  ind <- match(tolower(x), name)
  if (is.na(ind)) {
    stop("No ligature setting called '", x, "'. Use one of ", paste(name, collapse = ", "), call. = FALSE)
  }
  tags[ind]
}
get_letter_tag <- function(x) {
  name <- c("swash", "alternates", "historical", "localized", "randomize", "alt_annotation", "stylistic", "subscript", "superscript", "titling", "small_caps")
  tags <- c("cswh",  "calt",       "hist",       "locl",      "rand",      "nalt",           "salt",      "subs",      "sups",        "titl",    "smcp"      )
  ind <- match(tolower(x), name)
  if (is.na(ind)) {
    stop("No letter setting called '", x, "'. Use one of ", paste(name, collapse = ", "), call. = FALSE)
  }
  tags[ind]
}
get_number_tag <- function(x) {
  name <- c("lining", "oldstyle", "proportional", "tabular", "fractions", "fractions_alt")
  tags <- c("lnum",   "onum",     "pnum",         "tnum",    "frac",      "afrc"         )
  ind <- match(tolower(x), name)
  if (is.na(ind)) {
    stop("No number setting called '", x, "'. Use one of ", paste(name, collapse = ", "), call. = FALSE)
  }
  tags[ind]
}
