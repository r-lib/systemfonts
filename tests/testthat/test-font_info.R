test_that("failed font load does not corrupt the font cache", {
  expect_error(
    font_info(path = tempfile(fileext = ".ttf"), index = 0),
    "valid file"
  )
  # A valid load after the failure must still work
  expect_equal(nrow(font_info("sans")), 1)
})
