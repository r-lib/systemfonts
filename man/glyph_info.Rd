% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/font_info.R
\name{glyph_info}
\alias{glyph_info}
\title{Query glyph-specific information from fonts}
\usage{
glyph_info(
  glyphs,
  family = "",
  italic = FALSE,
  weight = "normal",
  width = "undefined",
  size = 12,
  res = 72,
  path = NULL,
  index = 0,
  variation = font_variation(),
  bold = deprecated()
)
}
\arguments{
\item{glyphs}{A vector of glyphs. Strings will be split into separate glyphs
automatically}

\item{family}{The name of the font families to match}

\item{italic}{logical indicating the font slant}

\item{weight}{The weight to query for, either in numbers (\code{0}, \code{100}, \code{200},
\code{300}, \code{400}, \code{500}, \code{600}, \code{700}, \code{800}, or \code{900}) or strings (\code{"undefined"},
\code{"thin"}, \code{"ultralight"}, \code{"light"}, \code{"normal"}, \code{"medium"}, \code{"semibold"},
\code{"bold"}, \code{"ultrabold"}, or \code{"heavy"}). \code{NA} will be interpreted as
\code{"undefined"}/\code{0}}

\item{width}{The width to query for either in numbers (\code{0}, \code{1}, \code{2},
\code{3}, \code{4}, \code{5}, \code{6}, \code{7}, \code{8}, or \code{9}) or strings (\code{"undefined"},
\code{"ultracondensed"}, \code{"extracondensed"}, \code{"condensed"}, \code{"semicondensed"},
\code{"normal"}, \code{"semiexpanded"}, \code{"expanded"}, \code{"extraexpanded"}, or
\code{"ultraexpanded"}). \code{NA} will be interpreted as \code{"undefined"}/\code{0}}

\item{size}{The pointsize of the font to use for size related measures}

\item{res}{The ppi of the size related measures}

\item{path, index}{path an index of a font file to circumvent lookup based on
family and style}

\item{variation}{A \code{font_variation} object or a list of them to control
variable fonts}

\item{bold}{\ifelse{html}{\href{https://lifecycle.r-lib.org/articles/stages.html#deprecated}{\figure{lifecycle-deprecated.svg}{options: alt='[Deprecated]'}}}{\strong{[Deprecated]}} Use \code{weight = "bold"} instead}
}
\value{
A data.frame with information about each glyph, containing the following
columns:

\describe{
\item{glyph}{The glyph as a character}
\item{index}{The index of the glyph in the font file}
\item{width}{The width of the glyph}
\item{height}{The height of the glyph}
\item{x_bearing}{The horizontal distance from the origin to the leftmost part of the glyph}
\item{y_bearing}{The vertical distance from the origin to the top part of the glyph}
\item{x_advance}{The horizontal distance to move the cursor after adding the glyph}
\item{y_advance}{The vertical distance to move the cursor after adding the glyph}
\item{bbox}{The tight bounding box surrounding the glyph}
}
}
\description{
This function allows you to extract information about the individual glyphs
in a font, based on a specified size. All size related measures are in
pixel-units. The function is vectorised to the length of the \code{glyphs} vector.
}
