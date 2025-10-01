#' Define axis coordinates for variable fonts
#'
#' Variable fonts is a technology that allows font designers to encode the full,
#' continuous font space of a typeface into a single font file and then have the
#' user set the coordinates of the variable axes to define the font. So, rather
#' than having a font file for bold, bold + italic, thin, and thin + italic,
#' etc. it is all encoded in a single file with a continuous range of all axes
#' (e.g. weight doesn't have to be one of the 9 standard weights but can be
#' anything in between). There are 5 standard axes that fonts can use, but font
#' designers are free to define their own completely arbitrary axes as well. You
#' can use [font_info()] to see which axes a font defines along with their value
#' range and default setting. Values given as `font_variation()` will always win
#' over the conventional setting *if* the axis is present in the font. For
#' example, setting `weight = "bold"` along with
#' `variation = font_variation(weight = 650)` will eventually request a weight
#' of `650` (helfway between semibold and bold), assuming the weight-axis is
#' present in the font. For clarity however, it is advised that `font_variation()`
#' is only used for axes that can otherwise not be accessed by "top-level"
#' arguments.
#'
#' @param italic Value between 0 and 1. Usually treated as a boolean rather than
#' a continuous axis
#' @param weight Usually a value between 100 and 900 though fonts can limit or
#' expand the supported range. Weight names are also allowed (see
#' [as_font_weight()])
#' @param width Usually a value between 1 and 9 though fonts can limit or
#' expand the supported range. Width names are also allowed (see
#' [as_font_width()])
#' @param slant The angular slant of the font, usually between -90 and 90
#' (negative values tilt in the "standard" italic way)
#' @param optical_sizing Stroke thickness compensation for the glyphs. During
#' rendering the thickness of the stroke is usually increased when the font is
#' set at small sizes to increase readability. Set this to the size of the font
#' to get the "expected" look at that size.
#' @param ... Further axes and coordinate settings
#'
#' @return A `font_variation` object with coordinates for the provided axes
#'
#' @note systemfonts uses a scale of width values ranging from 1-9 while the
#' width axis uses a different scale (0.5 - 2.0) going from half as wide to
#' twice as wide as "normal". When using the `width` argument the coordinate
#' values is automatically converted. If you set the value based on the width
#' tag (`wdth`) then no conversion will happen.
#'
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
  if (
    length(variations) == 0 &&
      is.null(italic) &&
      is.null(weight) &&
      is.null(width) &&
      is.null(slant) &&
      is.null(optical_sizing)
  ) {
    return(structure(
      list(axis = integer(0), value = integer(0)),
      class = "font_variation"
    ))
  }
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
  # TODO: Support "auto" (i.e. set it to the size of the font)
  if (is.null(variations$opsz) && !is.null(optical_sizing)) {
    variations$optz <- as.numeric(optical_sizing)
  }
  variations <- variations[lengths(variations) != 0]
  axes <- names(variations) %||% character(0)
  values <- vapply(variations, identity, numeric(1))
  structure(
    list(
      axis = axes_to_tags(axes),
      value = values_to_fixed(values)
    ),
    class = "font_variation"
  )
}
is_font_variation <- function(x) inherits(x, "font_variation")
#' @export
print.font_variation <- function(x, ...) {
  if (length(x) == 0) {
    cat("An empty font_variation object\n")
  } else {
    cat("A list of font variation axis coordinates\n")
    cat(paste(paste0("- ", axes(x), ": ", coords(x)), collapse = "\n"))
  }
  invisible(x)
}
#' @export
format.font_variation <- function(x, ...) {
  if (length(x) == 0) {
    return("<empty>")
  }
  paste(axes(x), ': ', coords(x), collapse = '; ', sep = '')
}
combine_variations <- function(a, b) {
  a <- unclass(a)
  b <- unclass(b)
  new <- setdiff(b$axis, a$axis)
  current <- intersect(b$axis, a$axis)
  a$axis <- c(a$axis, new)
  a$value[match(b$axis, a$axis)] <- b$value
  `class<-`(a, "font_variation")
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

axes <- function(x) {
  tags_to_axes(.subset2(x, "axis"))
}
coords <- function(x) {
  fixed_to_values(.subset2(x, "value"))
}

#' @export
length.font_variation <- function(x) {
  length(.subset2(x, "axis"))
}
#' @export
names.font_variation <- function(x) {
  axes(x)
}
#' @export
`[[.font_variation` <- function(x, i, ...) {
  if (is.character(i)) {
    coords(x)[[match(i, axes(x))]]
  } else {
    coords(x)[[i]]
  }
}
#' @export
`[[<-.font_variation` <- function(x, i, value) {
  axes <- .subset2(x, "axis")
  values <- .subset2(x, "value")
  if (is.character(i)) {
    if (i == "width") {
      i <- "wdth"
      value <- 2^((value - 5) / 4)
    } else if (i == "weight") {
      i <- "wght"
    } else if (i == "italic") {
      i <- "ital"
    } else if (i == "slant") {
      i <- "slnt"
    } else if (i == "optical_sizing") {
      i <- "optz"
    }
    i <- axes_to_tags(i)
    ind <- match(i, axes)
  } else if (i > length(axes)) {
    stop("subscript out of bounds", call. = FALSE)
  } else {
    ind <- i
  }
  value <- values_to_fixed(value)
  if (is.na(ind)) {
    axes <- c(axes, i)
    values <- c(values, value)
  } else {
    values[ind] <- value
  }
  structure(
    list(axis = axes, value = values),
    class = "font_variation"
  )
}
#' @export
`$.font_variation` <- function(x, name) {
  x[[name]]
}
#' @export
`$<-.font_variation` <- function(x, name, value) {
  `[[<-`(x, name, value)
}
#' @export
`[.font_variation` <- function(x, i, ...) {
  if (is.character(i)) {
    coords(x)[match(i, axes(x))]
  } else {
    coords(x)[i]
  }
}
#' @export
`[<-.font_variation` <- function(x, i, value) {
  if (length(value) != length(i)) {
    stop("`i` and `value` must be the same length")
  }
  for (j in seq_along(i)) {
    x[[i[[j]]]] <- value[[j]]
  }
  x
}
