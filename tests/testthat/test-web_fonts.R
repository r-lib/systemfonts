test_that("require_font probes repositories, not an unrelated host", {
  probed <- character(0)
  tried <- character(0)
  local_mocked_bindings(
    has_internet = function(url) {
      probed <<- c(probed, url)
      TRUE
    },
    get_from_google_fonts = function(family, dir) {
      tried <<- c(tried, "google fonts")
      FALSE
    },
    get_from_font_squirrel = function(family, dir) {
      tried <<- c(tried, "font squirrel")
      FALSE
    },
    system_fonts = function() {
      data.frame(
        path = "/system/fonts/sans.ttf",
        family = "sans",
        stringsAsFactors = FALSE
      )
    }
  )

  expect_error(
    require_font(
      "NotARealFont",
      repositories = c("Google Fonts", "Font Squirrel"),
      verbose = FALSE
    ),
    "NotARealFont"
  )

  # 8.8.8.8 must never be probed
  expect_false(any(grepl("8\\.8\\.8\\.8", probed)))
  # Probes target the repositories that will actually be used
  expect_true(any(grepl("googleapis\\.com", probed)))
  expect_true(any(grepl("fontsquirrel\\.com", probed)))
  expect_equal(tried, c("google fonts", "font squirrel"))
})

test_that("require_font skips unreachable repositories but tries the rest", {
  tried <- character(0)
  local_mocked_bindings(
    has_internet = function(url) !grepl("googleapis\\.com", url),
    get_from_google_fonts = function(family, dir) {
      tried <<- c(tried, "google fonts")
      FALSE
    },
    get_from_font_squirrel = function(family, dir) {
      tried <<- c(tried, "font squirrel")
      TRUE
    },
    system_fonts = function() {
      data.frame(
        path = "/system/fonts/sans.ttf",
        family = "sans",
        stringsAsFactors = FALSE
      )
    }
  )

  expect_message(
    res <- require_font(
      "NotARealFont",
      repositories = c("Google Fonts", "Font Squirrel"),
      verbose = TRUE
    ),
    "Cannot reach Google Fonts"
  )

  # Google Fonts was skipped, Font Squirrel was still tried
  expect_equal(tried, "font squirrel")
  expect_true(res)
})

test_that("require_font reports no internet when all repositories unreachable", {
  local_mocked_bindings(
    has_internet = function(url) FALSE,
    system_fonts = function() {
      data.frame(
        path = "/system/fonts/sans.ttf",
        family = "sans",
        stringsAsFactors = FALSE
      )
    }
  )

  expect_message(
    expect_error(
      require_font(
        "NotARealFont",
        repositories = c("Google Fonts", "Font Squirrel"),
        verbose = TRUE
      ),
      "NotARealFont"
    ),
    "No internet connection"
  )
})
