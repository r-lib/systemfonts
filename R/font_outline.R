#' Get the outline of glyphs
#'
#' This function allows you to retrieve the outline of glyphs as polygon
#' coordinates. The glyphs are given as indexes into a font file and not as
#' characters allowing you to retrieve outlines for glyphs that doesn't have a
#' character counterpoint. Glyphs that are given as bitmaps are ignored.
#'
#' @param glyph The index of the glyph in the font file
#' @param path The path to the font file encoding the glyph
#' @param index The index of the font in the font file
#' @param size The size of the font in big points (1/72 inch)
#' @param tolerance The deviation tolerance for decomposing bezier curves of the
#' glyph. Given in the same unit as size. Smaller values give more detailed
#' polygons
#' @param verbose Should font and glyph loading errors be reported as warnings
#'
#' @return A data frame giving the outlines of the glyphs provide in `glyph`. It
#' contains the columns `glyph` pointing to the element in the input it relates
#' to, `contour` enumerating the contours the glyph consists of, and `x` and `y`
#' giving the coordinates in big points
#'
#' @export
#'
#' @examples
#' # Get the shape of s in the default font
#' font <- font_info()
#' glyph <- glyph_info("s", path = font$path, index = font$index)
#'
#' s <- glyph_outline(glyph$index, font$path, font$index, size = 150)
#'
#' plot(s$x, s$y, type = 'l')
#'
glyph_outline <- function(
  glyph,
  path,
  index = 0,
  size = 12,
  tolerance = 0.2,
  verbose = FALSE
) {
  n_glyphs <- length(glyph)
  glyph <- as.integer(glyph)
  path <- rep_len(as.character(path), n_glyphs)
  index <- rep_len(as.integer(index), n_glyphs)
  size <- rep_len(as.numeric(size), n_glyphs)
  tolerance <- as.numeric(tolerance)

  get_glyph_outlines(glyph, path, index, size, tolerance, verbose)
}

#' Render glyphs to raster image
#'
#' Not all glyphs are encoded as vector outlines (emojis often not). Even for
#' fonts that provide an outline you might be interested in a raster version.
#' This function gives you just that. It converts a glyph into an optimized
#' raster object that can be plotted with e.g. [graphics::rasterImage()] or
#' [grid::grid.raster()]. For convenience, you can also use
#' [glyph_raster_grob()] for plotting the result.
#'
#' @inheritParams glyph_outline
#' @param res The resolution to render the glyphs to
#' @param col The color of the glyph assuming the glyph doesn't have a native
#' coloring
#'
#' @return A list of nativeRaster objects (or `NULL` if it failed to render a
#' given glyph). The nativeRasters have additional attributes attached. `"size"`
#' will give the size of the glyph in big points and `"offset"` will give the
#' location of the top-left corner of the raster with respect to where it should
#' be rendered.
#'
#' @export
#'
#' @examples
#' font <- font_info()
#' glyph <- glyph_info("R", path = font$path, index = font$index)
#'
#' R <- glyph_raster(glyph$index, font$path, font$index, size = 150)
#'
#' plot.new()
#' plot.window(c(0,150), c(0, 150), asp = 1)
#' rasterImage(R[[1]], 0, 0, attr(R[[1]], "size")[2], attr(R[[1]], "size")[1])
#'
glyph_raster <- function(
  glyph,
  path,
  index = 0,
  size = 12,
  res = 300,
  col = "black",
  verbose = FALSE
) {
  n_glyphs <- length(glyph)

  if (all(col == "black" | col == "#000000")) {
    col <- rep_len(-16777216L, n_glyphs)
  } else {
    if (!requireNamespace("farver", quietly = TRUE)) {
      stop("The farver package is required to tint glyphs with a color")
    }
    col <- rep_len(farver::encode_native(col), n_glyphs)
  }

  glyph <- as.integer(glyph)
  path <- rep_len(as.character(path), n_glyphs)
  index <- rep_len(as.integer(index), n_glyphs)
  size <- rep_len(as.numeric(size), n_glyphs)
  res <- rep_len(as.numeric(res), n_glyphs)

  get_glyph_bitmap(glyph, path, index, size, res, col, verbose)
}

#' Convert an extracted glyph raster to a grob
#'
#' This is a convenience function that helps in creating [rasterGrob] with the
#' correct settings for the glyph. It takes inot account the sizing and offset
#' returned by [glyph_raster()] and allows you to only consider the baseline
#' position of the glyph.
#'
#' @param glyph The nativeRaster object returned as one of the elements by
#' [glyph_raster()]
#' @param x,y The baseline location of the glyph
#' @inheritDotParams grid::rasterGrob -x -y
#' @inheritParams grid::rasterGrob
#'
#' @return A rasterGrob object
#'
#' @export
#'
#' @examples
#' font <- font_info()
#' glyph <- glyph_info("R", path = font$path, index = font$index)
#'
#' R <- glyph_raster(glyph$index, font$path, font$index, size = 150)
#'
#' grob <- glyph_raster_grob(R[[1]], 50, 50)
#'
#' grid::grid.newpage()
#' # Mark the baseline location
#' grid::grid.points(50, 50, default.units = "bigpts")
#' # Draw the glyph
#' grid::grid.draw(grob)
#'
glyph_raster_grob <- function(glyph, x, y, ..., default.units = "bigpts") {
  if (length(glyph) == 0) return(grid::nullGrob())
  if (!grid::is.unit(x)) x <- grid::unit(x, default.units)
  if (!grid::is.unit(y)) y <- grid::unit(y, default.units)
  size <- attr(glyph, "size")
  offset <- attr(glyph, "offset")
  grid::rasterGrob(
    glyph,
    x = x + grid::unit(offset[2], "bigpts"),
    y = y + grid::unit(offset[1], "bigpts"),
    width = grid::unit(size[2], "bigpts"),
    height = grid::unit(size[1], "bigpts"),
    just = c("left", "top"),
    ...
  )
}
