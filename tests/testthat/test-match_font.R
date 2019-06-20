context("Font Matching")

sysname <- tolower(Sys.info()[["sysname"]])
font <- switch(
  sysname,
  darwin = "Helvetica.ttc",
  linux = "DejaVuSans.ttf",
  windows = "arial.ttf"
)
family = switch(
  sysname,
  darwin = "Helvetica",
  linux = "DejaVu Sans",
  windows = "Arial"
)

test_that("Font files can be found", {
  font_path <- match_font(basename(family))$path

  expect_true(file.exists(font_path))
  expect_equal(basename(font_path), font)
})

test_that("Default font is correct", {
  font_path <- match_font("")$path

  expect_true(file.exists(font_path))
  expect_equal(basename(font_path), font)
})
