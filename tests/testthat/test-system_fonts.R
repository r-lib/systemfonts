context("Font listing")

test_that("System fonts can be listed", {
  fonts <- system_fonts()
  expect_is(fonts, 'data.frame')
  expect_more_than(nrow(fonts), 0)
  expect_named(fonts, c("path", "name", "family", "style", "weight", "width",
                        "italic", "monospace"))
})
