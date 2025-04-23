release_bullets <- function() {
  c(
    '`rhub::check_on_solaris(env_vars = c("_R_CHECK_FORCE_SUGGESTS_" = "false"))`',
    '`rhub::check_with_valgrind(env_vars = c(VALGRIND_OPTS = "--leak-check=full --track-origins=yes"))`'
  )
}

.onLoad <- function(...) {
  load_emoji_codes()
  scan_local_fonts()
}

warn_env <- new.env(parent = emptyenv())
warn_env$warned <- FALSE

#' Get location of the fallback font
#'
#' @export
#' @keywords internal
get_fallback <- function() {
  if (!warn_env$warned) {
    warning(
      "No fonts detected on your system. Using an empty font.",
      call. = FALSE
    )
    warn_env$warned <- TRUE
  }
  list(
    system.file("unfont.ttf", package = "systemfonts"),
    0L
  )
}
