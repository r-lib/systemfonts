.onLoad <- function(...) {
  .Call("sf_init_c", asNamespace("systemfonts"))
  load_emoji_codes()
}

warn_env <- new.env(parent = emptyenv())
warn_env$warned <- FALSE

get_fallback <- function() {
  if (!warn_env$warned) {
    warning("No fonts detected on your system. Using an empty font.", call. = FALSE)
    warn_env$warned <- TRUE
  }
  list(
    system.file("unfont.ttf", package = "systemfonts"),
    0L
  )
}
