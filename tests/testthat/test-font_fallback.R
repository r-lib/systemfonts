test_that("empty string input returns empty results", {
  expect_equal(nrow(font_fallback(character())), 0)
})
