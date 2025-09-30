context("Font Matching")

sysname <- tolower(Sys.info()[["sysname"]])
font <- switch(
  sysname,
  darwin = "Helvetica",
  linux = "DejaVuSans",
  freebsd = "DejaVuSans",
  windows = "arial"
)

test_that("Font files can be found", {
  font_path <- match_fonts("sans")$path

  expect_true(file.exists(font_path))

  skip_if_not(sysname %in% c("mac", "windows")) # Not deterministic if not
  expect_equal(tools::file_path_sans_ext(basename(font_path)), font)
})

test_that("Default font is correct", {
  font_path <- match_fonts("sans")$path

  expect_true(file.exists(font_path))

  skip_if_not(sysname %in% c("mac", "windows")) # Not deterministic if not
  expect_equal(tools::file_path_sans_ext(basename(font_path)), font)
})
