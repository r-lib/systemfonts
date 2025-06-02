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
#' @param variation A `font_variation` object or a list of them to control
#' variable fonts
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
  variation = font_variation(),
  verbose = FALSE
) {
  if (is_font_variation(variation)) variation <- list(variation)
  n_glyphs <- length(glyph)
  glyph <- as.integer(glyph)
  path <- rep_len(as.character(path), n_glyphs)
  index <- rep_len(as.integer(index), n_glyphs)
  size <- rep_len(as.numeric(size), n_glyphs)
  variation <- rep_len(variation, n_glyphs)
  tolerance <- as.numeric(tolerance)

  get_glyph_outlines(
    glyph,
    path,
    index,
    size,
    variation,
    tolerance,
    as.logical(verbose)
  )
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
  variation = font_variation(),
  col = "black",
  verbose = FALSE
) {
  if (is_font_variation(variation)) variation <- list(variation)
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
  variation <- rep_len(variation, n_glyphs)

  get_glyph_bitmap(glyph, path, index, size, res, variation, col, verbose)
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

#' Create a visual representation of what the various glyph stats mean
#'
#' This function helps you understand the concepts of width, height, bearing,
#' and advance by annotating a glyph with the various measures
#'
#' @param glyph The character to plot
#' @inheritParams glyph_info
#'
#' @return This function is called for its side effects
#'
#' @export
#'
#' @examples
#' plot_glyph_stats("g")
#'
plot_glyph_stats <- function(
  glyph,
  family = '',
  italic = FALSE,
  weight = "normal",
  width = "undefined",
  size = 12,
  res = 72,
  variation = font_variation(),
  path = NULL,
  index = 0
) {
  font <- font_info(
    family = family,
    italic = italic,
    weight = weight,
    width = width,
    path = path,
    index = index
  )
  info <- glyph_info(
    glyphs = glyph,
    family = family,
    italic = italic,
    weight = weight,
    width = width,
    size = size,
    res = res,
    variation = variation,
    path = path,
    index = index
  )
  outline <- glyph_outline(
    glyph = info$index[1],
    path = font$path[1],
    index = font$index[1],
    size = size,
    variation = variation,
    tolerance = 0.1
  )
  x_scale <- c(min(0, info$x_bearing), info$x_advance)
  y_scale <- c(min(0, info$bbox[[1]][3]), info$bbox[[1]][4])
  max_dim <- max(diff(x_scale), diff(y_scale))
  gutter <- max_dim * 0.1 * c(-1, 1)
  x_scale <- x_scale + gutter
  y_scale <- y_scale + gutter
  max_dim <- max_dim * 1.2
  vp <- grid::viewport(
    width = grid::unit(diff(x_scale) / max_dim, "snpc"),
    height = grid::unit(diff(y_scale) / max_dim, "snpc"),
    xscale = x_scale,
    yscale = y_scale,
    clip = "off"
  )
  bbox <- grid::rectGrob(
    x = info$bbox[[1]][1],
    y = info$bbox[[1]][3],
    width = info$bbox[[1]][2] - info$bbox[[1]][1],
    height = info$bbox[[1]][4] - info$bbox[[1]][3],
    hjust = 0,
    vjust = 0,
    gp = grid::gpar(col = NA, fill = "grey90"),
    default.units = "native",
    vp = vp
  )
  glyph <- grid::pathGrob(
    outline$x,
    outline$y,
    id = outline$contour,
    gp = grid::gpar(col = "black", fill = "white"),
    default.units = "native",
    vp = vp
  )
  coord <- grid::segmentsGrob(
    x0 = grid::unit(c(0, 0), c("npc", "native")),
    y0 = grid::unit(c(0, 0), c("native", "npc")),
    x1 = grid::unit(c(1, 0), c("npc", "native")),
    y1 = grid::unit(c(0, 1), c("native", "npc")),
    vp = vp,
    gp = grid::gpar(lty = 3)
  )
  origin <- grid::pointsGrob(
    0,
    0,
    vp = vp,
    pch = 19,
    size = grid::unit(0.03, "snpc")
  )
  arrows <- grid::segmentsGrob(
    x0 = c(
      info$x_bearing,
      info$bbox[[1]][2] + max_dim * 0.02,
      info$bbox[[1]][1] - max_dim * 0.02,
      0,
      0
    ),
    y0 = c(
      info$y_bearing + max_dim * 0.02,
      info$y_bearing,
      0,
      info$y_bearing + max_dim * 0.05,
      info$bbox[[1]][3] - max_dim * 0.02
    ),
    x1 = c(
      info$x_bearing + info$width,
      info$bbox[[1]][2] + max_dim * 0.02,
      info$bbox[[1]][1] - max_dim * 0.02,
      info$x_bearing,
      info$x_advance
    ),
    y1 = c(
      info$y_bearing + max_dim * 0.02,
      info$y_bearing - info$height,
      info$y_bearing,
      info$y_bearing + max_dim * 0.05,
      info$bbox[[1]][3] - max_dim * 0.02
    ),
    default.units = "native",
    arrow = grid::arrow(
      length = grid::unit(0.015, "snpc"),
      ends = c("both", "both", "last", "last", "last")
    ),
    vp = vp
  )
  labels <- grid::textGrob(
    label = c(
      "width",
      "height",
      "x bearing",
      "y bearing",
      "x advance",
      "(0,0)"
    ),
    x = c(
      mean(info$bbox[[1]][1:2]),
      info$bbox[[1]][2] + max_dim * 0.03,
      min(0, info$x_bearing) - max_dim * 0.01,
      info$bbox[[1]][1] - max_dim * 0.03,
      info$x_advance / 2,
      -max_dim * 0.01
    ),
    y = c(
      info$y_bearing + max_dim * 0.03,
      mean(info$bbox[[1]][3:4]),
      info$y_bearing + max_dim * 0.05,
      info$y_bearing / 2,
      info$bbox[[1]][3] - max_dim * 0.03,
      -max_dim * 0.01
    ),
    hjust = c(0.5, 0, 1, 1, 0.5, 1),
    vjust = c(0, 0.5, 0.5, 0.5, 1, 1),
    default.units = "native",
    vp = vp
  )
  grid::grid.newpage()
  grid::grid.draw(grid::gList(bbox, coord, origin, glyph, arrows, labels))
}
