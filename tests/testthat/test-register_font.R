context("Font registration")

font_file <- system.file("unfont.ttf", package = "systemfonts")

test_that("fonts can be registered with variation coordinates", {
  register_font(
    "reg-var",
    plain = font_file,
    variation = font_variation(weight = 650, slant = -5)
  )
  on.exit(clear_registry())

  reg <- registry_fonts()
  expect_named(
    reg,
    c(
      "path",
      "index",
      "family",
      "style",
      "weight",
      "italic",
      "features",
      "variations"
    )
  )
  var <- reg$variations[[1]]
  expect_s3_class(var, "font_variation")
  expect_equal(var[["wght"]], 650)
  expect_equal(var[["slnt"]], -5)
})

test_that("registered variations are returned during matching", {
  register_font(
    "reg-var",
    plain = font_file,
    variation = font_variation(weight = 650)
  )
  on.exit(clear_registry())

  loc <- match_fonts("reg-var")
  expect_equal(
    loc$path,
    normalizePath(font_file, mustWork = FALSE) %||% loc$path
  )
  expect_equal(loc$variations[[1]][["wght"]], 650)
})

test_that("non-standard weight requests override registered wght coordinate", {
  register_font(
    "reg-var",
    plain = font_file,
    variation = font_variation(weight = 650, slant = -5)
  )
  on.exit(clear_registry())

  loc <- match_fonts("reg-var", weight = 800)
  # Still resolved from the registry
  expect_equal(basename(loc$path), basename(font_file))
  var <- loc$variations[[1]]
  expect_equal(var[["wght"]], 800)
  # Other axes are left untouched
  expect_equal(var[["slnt"]], -5)
})

test_that("non-standard weight requests bypass registry without wght axis", {
  register_font("reg-novar", plain = font_file)
  on.exit(clear_registry())

  loc <- match_fonts("reg-novar", weight = 800)
  expect_false(basename(loc$path) == basename(font_file))

  # Standard weights still resolve to the registry
  loc <- match_fonts("reg-novar", weight = "bold")
  expect_equal(basename(loc$path), basename(font_file))
})

test_that("register_variant forwards variation", {
  skip_if(nrow(system_fonts()) == 0)
  register_variant(
    "reg-variant",
    family = system_fonts()$family[1],
    variation = font_variation(weight = 550)
  )
  on.exit(clear_registry())

  var <- registry_fonts()$variations[[1]]
  expect_equal(var[["wght"]], 550)
})

test_that("variation must be a font_variation object", {
  expect_error(
    register_font("reg-bad", plain = font_file, variation = list(wght = 650)),
    "font_variation"
  )
})
