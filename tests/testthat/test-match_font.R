context("Font Matching")

sysname <- tolower(Sys.info()[["sysname"]])
font <- switch(
  sysname,
  darwin = "Helvetica.ttc",
  linux = "DejaVuSans.ttf",
  windows = "arial.ttf"
)

test_that("Font files can be found", {
  font_path <- match_font("sans")$path

  expect_true(file.exists(font_path))

  skip_on_os("linux") # Different fonts for different distros
  expect_equal(basename(font_path), font)
})

test_that("Default font is correct", {
  font_path <- match_font("sans")$path

  expect_true(file.exists(font_path))

  skip_on_os("linux")
  expect_equal(basename(font_path), font)
})
