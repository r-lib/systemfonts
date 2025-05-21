#' Define axis coordinates for variable fonts
#'
#' @export
#'
font_variation <- function(
  italic = NULL,
  weight = NULL,
  width = NULL,
  slant = NULL,
  optical_sizing = NULL,
  ...
) {
  variations <- list(...)
  if (is.null(variations$ital) && !is.null(italic)) {
    variations$ital <- as.numeric(italic)
  }
  if (is.null(variations$wght) && !is.null(weight)) {
    variations$wght <- as_font_weight(weight)
    if (variations$wght == 0) variations$wght <- NULL
  }
  if (is.null(variations$wdth) && !is.null(width)) {
    variations$wdth <- as_font_width(width)
    if (variations$wdth == 0) {
      variations$wdth <- NULL
    } else {
      variations$wdth <- 2^((variations$wdth - 5) / 4)
    }
  }
  if (is.null(variations$slnt) && !is.null(slant)) {
    variations$slnt <- as.numeric(slant)
  }
  if (is.null(variations$opsz) && !is.null(optical_sizing)) {
    variations$optz <- as.numeric(optical_sizing)
  }
  variations <- variations[lengths(variations) != 0]
  axes <- names(variations) %||% character(0)
  values <- vapply(variations, identity, numeric(1))
  structure(
    list(axis = axes_to_tags(axes), value = values_to_fixed(values)),
    class = "font_variation"
  )
}
is_font_variation <- function(x) inherits(x, "font_variation")
#' @export
print.font_variation <- function(x, ...) {
  if (length(x$axis) == 0) {
    cat("An empty font_variation object\n")
  } else {
    cat("A list of font variation axis coordinates\n")
    cat(paste(paste0("- ", tags_to_axes(x$axis), ": ", fixed_to_values(x$value)), collapse = "\n"))
  }
  invisible(x)
}
#' @export
format.font_variation <- function(x, ...) {
  if (length(x$axis) == 0) {
    return("<empty>")
  }
  paste(tags_to_axes(x$axis), ': ', fixed_to_values(x$value), collapse = '; ', sep = '')
}
combine_variations <- function(a, b) {
  new <- setdiff(b$axis, a$axis)
  current <- intersect(b$axis, a$axis)
  a$axis <- c(a$axis, new)
  a$value[match(b$axis, a$axis)] <- b$value
  a
}
add_standard_to_variations <- function(variations, italic, weight, width) {
   mapply(
    function(orig_var, italic, weight, width) {
      combine_variations(
        font_variation(italic = italic, weight = weight, width = width),
        orig_var
      )
    },
    orig_var = variations,
    italic = italic,
    weight = weight,
    width = width,
    SIMPLIFY = FALSE
  )
}
