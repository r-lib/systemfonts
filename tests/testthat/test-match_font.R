context("Font Matching")

sysname <- tolower(Sys.info()[["sysname"]])
font <- switch(
  sysname,
  darwin = ,
  linux = "Helvetica.ttc",
  windows = "arial.ttf"
)
family = switch(
  sysname,
  darwin = ,
  linux = "Helvetica",
  windows = "Arial"
)

test_that("Font files can be found", {
  font_path <- match_font(basename(family))

  expect_true(file.exists(font_path))
  expect_equal(basename(font_path), font)
})

test_that("Default font is correct", {
  font_path <- match_font("")

  expect_true(file.exists(font_path))
  expect_equal(basename(font_path), font)
})
