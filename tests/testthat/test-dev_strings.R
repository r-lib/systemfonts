test_that("dev string functions tolerate long family names", {
  skip_if_not(
    capabilities("aqua") ||
      capabilities("X11") ||
      .Platform$OS.type == "windows"
  )
  grDevices::pdf(NULL)
  on.exit(grDevices::dev.off(), add = TRUE)
  # The point is the absence of a crash/overflow, not of device warnings
  expect_error(
    suppressWarnings(string_widths_dev("test", family = strrep("a", 500))),
    NA
  )
})
