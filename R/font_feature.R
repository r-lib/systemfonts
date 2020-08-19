#' Define OpenType font feature settings
#' 
#' This function encapsulates the specification of OpenType font features. Some
#' specific features have named arguments, but all available features can be
#' set by using its specific 4-letter tag For a list of the 4-letter tags
#' available see e.g. the overview on 
#' [Wikipedia](https://en.wikipedia.org/wiki/List_of_typographic_features).
#' 
#' @param ... key-value pairs with the key being the 4-letter tag and the value
#' being the setting (usually `TRUE` to turn it on).
#' 
#' @return A `font_feature` object
#' 
#' @export
font_feature <- function(...) {
  list(character(), integer())
}
