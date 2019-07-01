context("Font Matching")

sysname <- tolower(Sys.info()[["sysname"]])
font <- switch(
  sysname,
  darwin = "Helvetica",
  linux = "DejaVuSans",
  windows = "arial"
)

test_that("Font files can be found", {
  font_path <- match_font("sans")$path

  expect_true(file.exists(font_path))

  skip_on_os("linux") # Different fonts for different distros
  skip_on_os("solaris") # Have no idea what it is supposed to give
  expect_equal(tools::file_path_sans_ext(basename(font_path)), font)
})

test_that("Default font is correct", {
  font_path <- match_font("sans")$path

  expect_true(file.exists(font_path))

  skip_on_os("linux")
  skip_on_os("solaris")
  expect_equal(tools::file_path_sans_ext(basename(font_path)), font)
})
