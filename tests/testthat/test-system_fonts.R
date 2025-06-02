context("Font listing")

test_that("System fonts can be listed", {
  fonts <- system_fonts()
  expect_is(fonts, 'data.frame')
  expect_gt(nrow(fonts), 0)
  expect_named(
    fonts,
    c(
      "path",
      "index",
      "name",
      "family",
      "style",
      "weight",
      "width",
      "italic",
      "monospace",
      "variable"
    )
  )
})
