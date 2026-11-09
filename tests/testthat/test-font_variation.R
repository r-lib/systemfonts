test_that("optical_sizing maps to the opsz axis", {
  var <- font_variation(optical_sizing = 12)

  expect_equal(systemfonts:::axes(var), "opsz")
  expect_equal(systemfonts:::coords(var), 12)
  expect_equal(var[["opsz"]], 12)
})

test_that("optical_sizing can be set via subassignment", {
  var <- font_variation(weight = 500)
  var[["optical_sizing"]] <- 14

  expect_equal(systemfonts:::axes(var), c("wght", "opsz"))
  expect_equal(var[["opsz"]], 14)
})

test_that("explicit opsz tag is not overridden by optical_sizing", {
  var <- font_variation(optical_sizing = 12, opsz = 30)

  expect_equal(systemfonts:::coords(var), 30)
})

test_that("out-of-range variation values error cleanly", {
  expect_error(font_variation(NA_real_, wght = 400), "out of range")
  expect_error(font_variation(italic = 1e6), "out of range")
})

test_that("malformed variation objects error cleanly", {
  bad <- structure(
    list(axis = 2003265652L, value = 400),
    class = "font_variation"
  )
  expect_error(
    glyph_info("H", family = "sans", variation = bad),
    "Invalid font_variation"
  )
})

test_that("variation axes apply independently and reset to default", {
  vf <- subset(system_fonts(), variable)
  axes <- lapply(seq_len(nrow(vf)), function(i) {
    tryCatch(
      names(font_info(path = vf$path[i], index = vf$index[i])$variation_axes[[
        1
      ]]),
      error = function(e) NULL
    )
  })
  has_wght <- which(vapply(
    axes,
    function(a) "wght" %in% a,
    logical(1)
  ))[1]
  skip_if(is.na(has_wght), "No variable font with a weight axis available")

  f <- vf[has_wght, ]
  advance <- function(var) {
    glyph_info(
      "H",
      path = f$path,
      index = f$index,
      size = 100,
      variation = var
    )$x_advance
  }
  default <- advance(font_variation())
  heavy <- advance(font_variation(weight = 900))
  expect_false(isTRUE(all.equal(default, heavy)))
  # Asking for no axes afterwards must restore the default coordinates
  expect_equal(advance(font_variation()), default)
})
