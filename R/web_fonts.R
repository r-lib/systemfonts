#' Search font repositories for a font based on family name
#'
#' While it is often advisable to visit the webpage for a font repository when
#' looking for a font, in order to see examples etc, `search_web_fonts()`
#' provide a quick lookup based on family name in the repositories supported by
#' systemfonts (currently [Google Fonts](https://fonts.google.com) and
#' [Font Squirrel](https://www.fontsquirrel.com)). The lookup is based on fuzzy
#' matching provided by [utils::adist()] and the matching parameters can be
#' controlled through `...`
#'
#' @param family The font family name to look for
#' @param n_max The maximum number of matches to return
#' @inheritDotParams utils::adist -x -y
#'
#' @return A data.frame with the columns `family`, giving the family name of the
#' matched font, and `repository` giving the repository it was found in.
#'
#' @export
#'
#' @examples
#' # Requires an internet connection
#'
#' # search_web_fonts("Spectral")
#'
search_web_fonts <- function(family, n_max = 10, ...) {
  gf <- unique(get_google_fonts_registry()$family)
  fs <- unique(get_font_squirrel_registry()$family)
  all <- data.frame(family = c(gf, fs), repository = c(rep(c("Google Fonts", "Font Squirrel"), c(length(gf), length(fs)))))
  all[order(utils::adist(x = tolower(family), y = tolower(all$family), ...))[seq_len(min(n_max, nrow(all)))],]
}

#' Download and add web font
#'
#' In order to use a font in R it must first be made available locally. These
#' functions facilitate the download and registration of fonts from online
#' repositories.
#'
#' @param family The font family to download (case insensitive)
#' @param dir Where to download the font to. The default places it in your user
#' local font folder so that the font will be available automatically in new R
#' sessions. Set to `tempdir()` to only keep the font for the session.
#' @param woff2 Should the font be downloaded in the woff2 format (smaller and
#' more optimized)? Defaults to FALSE as the format is not supported on all
#' systems
#'
#' @return A logical invisibly indicating whether a font was found and
#' downloaded or not
#'
#' @name web-fonts
#' @rdname web-fonts
#'
NULL

#' @rdname web-fonts
#' @export
#'
get_from_google_fonts <- function(family, dir = "~/fonts", woff2 = FALSE) {
  fonts <- get_google_fonts_registry(woff2)
  match <- which(tolower(fonts$family) == tolower(family))

  if (length(match) == 0) {
    return(invisible(FALSE))
  }

  if (!dir.exists(dir)) dir.create(dir, recursive = TRUE)

  files <- fonts$url[match]
  download_name <- file.path(dir, fonts$file[match])
  success <- try({
    if (capabilities("libcurl")) {
      utils::download.file(files, download_name, method = "libcurl")
    } else {
      mapply(utils::download.file, url = files, destfile = download_name)
    }
  }, silent = TRUE)

  if (inherits(success, "try-error")) {
    return(invisible(FALSE))
  }

  add_fonts(download_name)
  invisible(TRUE)
}

#' @rdname web-fonts
#' @export
#'
get_from_font_squirrel <- function(family, dir = "~/fonts") {
  fonts <- get_font_squirrel_registry()
  match <- which(tolower(fonts$family) == tolower(family))

  if (length(match) == 0) {
    return(invisible(FALSE))
  }

  if (!dir.exists(dir)) dir.create(dir, recursive = TRUE)

  files <- fonts$url[match]
  download_name <- file.path(tempdir(check = TRUE), paste0(basename(fonts$url[match]), ".zip"))
  success <- try({
    if (capabilities("libcurl")) {
      utils::download.file(files, download_name, method = "libcurl")
    } else {
      mapply(utils::download.file, url = files, destfile = download_name)
    }
  }, silent = TRUE)

  if (inherits(success, "try-error")) {
    return(invisible(FALSE))
  }

  new_fonts <- unlist(mapply(utils::unzip, zipfile = download_name, MoreArgs = list(exdir = dir)))
  is_font <- grepl("\\.(?:ttf|ttc|otf|otc|woff|woff2)$", tolower(new_fonts))
  unlink(new_fonts[!is_font])
  add_fonts(new_fonts[is_font])

  return(invisible(TRUE))
}

#' Ensure font availability in a script
#'
#' When running a script on a different machine you are not always in control of
#' which fonts are installed on the system and thus how graphics created by the
#' script ends up looking. `require_font()` is a way to specify your font
#' requirements for a script. It will look at the available fonts and if the
#' required font family is not present it will attempt to fetch it from one of
#' the given repositories (in the order given). If that fails, it will either
#' throw an error or, if `fallback` is given, provide an alias for the fallback
#' so it maps to the required font.
#'
#' @param family The font family to require
#' @param fallback An available font to fall back to if `family` cannot be found
#' or downloaded
#' @param dir The location to put the font file downloaded from repositories
#' @param repositories The repositories to search for the font in case it is not
#' available on the system. They will be tried in the order given. Currently
#' only `"Google Fonts"` and `"Font Squirrel"` is available.
#' @param error Should the function throw an error if unsuccessful?
#'
#' @return Invisibly `TRUE` if the font is available or `FALSE` if not (this can
#' only be returned if `error = FALSE`)
#'
#' @export
#'
#' @examples
#' # Should always work
#' require_font("sans")
#'
require_font <- function(family, fallback = NULL, dir = tempdir(), repositories = c("Google Fonts", "Font Squirrel"), error = TRUE) {
  if (tolower(family) %in% c("sans", "serif", "mono", "symbol")) return(invisible(TRUE))
  if (!is.character(family) || length(family) != 1) {
    stop("`family` must be a string")
  }
  if (!is.null(fallback) && (!is.character(fallback) || length(fallback) != 1)) {
    stop("`family` must be a string")
  }
  success <- tolower(font_info(family)$family) == tolower(family)

  for (repo in repositories) {
    if (success) break
    success <- switch(
      tolower(repo),
      "google fonts" = get_from_google_fonts(family, dir),
      "font squirrel" = get_from_font_squirrel(family, dir),
      FALSE
    )
  }

  if (!success) {
    if (is.null(fallback)) {
      if (error) stop(paste0("Required font: ", family, ", is not available on the system"))
    } else {
      warning(paste0("Required font: `", family, "`, is not available on the system. Adding alias to `", fallback, "`"))
      register_variant(family, fallback)
      success <- TRUE
    }
  }

  invisible(success)
}

font_as_import <- function(family, self_contained = FALSE, may_embed = TRUE, repositories = c("Google Fonts", "Font Library"), ...) {
  import <- NULL
  if (!self_contained) {
    for (repo in repositories) {
      if (!is.null(import)) break
      import <- switch(
        tolower(repo),
        "google fonts" = import_from_google_fonts(family, ...),
        "font library" = import_from_font_library(family, ...),
        NULL
      )
    }
  }

  if (is.null(import) && may_embed) {
    font <- font_info(family)
  }

  import
}

import_from_google_fonts <- function(family, italic = NULL, weight = NULL, width = NULL, display = "swap") {
  display <- match.arg(display, c("auto", "block", "swap", "fallback", "optional"))
  url <- paste0("https://fonts.googleapis.com/css2?family=", gsub(" ", "+", family))

  settings <- list()
  if (!is.null(italic)) {
    if (!is.logical(italic)) {
      stop("`italic` must be a logical vector")
    }
    settings$ital <- italic
  }
  if (!is.null(width)) {
    settings$wdth <- as_font_weight(width)
  }
  if (!is.null(weight)) {
    if (!is.numeric(weight)) {
      stop("`italic` must be a logical vector")
    }
    settings$ital <- italic
  }
  if (length(settings) != 0) {
    settings <- lapply(settings, function(x) rep_len(as.character(x), max(lengths(settings))))
    url = paste0(
      url,
      ":",
      paste(names(settings), collapse = ","),
      "@",
      paste(
        vapply(
          seq_along(settings[[1]]),
          function(i) paste(vapply(settings, `[[`, character(1), i), collapse = ","),
          character(1)
        ),
        collapse = ";"
      )
    )
  }
  url <- paste0(url, "?display=", display)

  success <- try(suppressWarnings(readLines(url, n = 1)), silent = TRUE)
  if (inherits(success, "try-error")) {
    NULL
  } else {
    paste0('<link rel="stylesheet" href="', url, '"/>')
  }
}
import_from_font_library <- function(family, ...) {
  family_name <- gsub(" ", "-", tolower(family))
  url <- paste0("https://fontlibrary.org/face/", family_name)
  if (length(readLines(url, n = 1)) != 0) {
    paste0('<link rel="stylesheet" href="', url, '"/>')
  } else {
    NULL
  }
}

# REGISTRIES -------------------------------------------------------------------

google_fonts_registry <- new.env(parent = emptyenv())
get_google_fonts_registry <- function(woff2 = FALSE) {
  loc <- if (woff2) "woff2" else "ttf"
  if (is.null(google_fonts_registry[[loc]])) {
    url <- "https://www.googleapis.com/webfonts/v1/webfonts?key=AIzaSyBkOYsZREsyZWvbSR_d03SI5XX30cIapYo&sort=popularity"
    if (woff2) {
      url <- paste0(url, "&capability=WOFF2")
    }
    fonts <- jsonlite::read_json(url)$items
    google_fonts_registry[[loc]] <- do.call(rbind, lapply(fonts, function(x) {
      x <- data.frame(
        family = x$family,
        variant = unlist(x$variants),
        url = unlist(x$files),
        file = NA_character_,
        version = x$version,
        modified = x$lastModified,
        category = I(rep(list(x$category), length(x$variants)))
      )
      x$file <- paste0(x$family, "-", x$variant, sub("^.*(\\.\\w+)$", "\\1", x$url))
      attr(x, "row.names") <- .set_row_names(nrow(x))
      x
    }))
  }
  google_fonts_registry[[loc]]
}

font_squirrel_registry <- new.env(parent = emptyenv())
get_font_squirrel_registry <- function() {
  if (is.null(font_squirrel_registry$fonts)) {
    fonts <- jsonlite::read_json("https://www.fontsquirrel.com/api/fontlist/all")
    font_squirrel_registry$fonts <- do.call(rbind, lapply(fonts, function(x) {
      x <- data.frame(
        family = x$family_name,
        n_variants = x$family_count,
        url = paste0("https://www.fontsquirrel.com/fonts/download/", x$family_urlname),
        category = I(list(x$classification))
      )
      attr(x, "row.names") <- .set_row_names(nrow(x))
      x
    }))
  }
  font_squirrel_registry$fonts
}
