test_that("empty string input returns empty results", {
  expect_equal(nrow(shape_string(character())$shape), 0)
  expect_equal(length(string_width(character())), 0)
})
