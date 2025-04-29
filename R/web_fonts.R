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
  all <- data.frame(
    family = c(gf, fs),
    repository = c(rep(
      c("Google Fonts", "Font Squirrel"),
      c(length(gf), length(fs))
    ))
  )
  all[
    order(utils::adist(
      x = tolower(family),
      y = tolower(all$family),
      ...
    ))[seq_len(min(n_max, nrow(all)))],
  ]
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
  success <- try(
    {
      if (capabilities("libcurl")) {
        utils::download.file(files, download_name, method = "libcurl")
      } else {
        mapply(utils::download.file, url = files, destfile = download_name)
      }
    },
    silent = TRUE
  )

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
  download_name <- file.path(
    tempdir(check = TRUE),
    paste0(basename(fonts$url[match]), ".zip")
  )
  success <- try(
    {
      if (capabilities("libcurl")) {
        utils::download.file(files, download_name, method = "libcurl")
      } else {
        mapply(utils::download.file, url = files, destfile = download_name)
      }
    },
    silent = TRUE
  )

  if (inherits(success, "try-error")) {
    return(invisible(FALSE))
  }

  new_fonts <- unlist(mapply(
    utils::unzip,
    zipfile = download_name,
    MoreArgs = list(exdir = dir)
  ))
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
require_font <- function(
  family,
  fallback = NULL,
  dir = tempdir(),
  repositories = c("Google Fonts", "Font Squirrel"),
  error = TRUE
) {
  if (tolower(family) %in% c("sans", "serif", "mono", "symbol"))
    return(invisible(TRUE))
  if (!is.character(family) || length(family) != 1) {
    stop("`family` must be a string")
  }
  if (
    !is.null(fallback) && (!is.character(fallback) || length(fallback) != 1)
  ) {
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
      if (error)
        stop(paste0(
          "Required font: ",
          family,
          ", is not available on the system"
        ))
    } else {
      warning(paste0(
        "Required font: `",
        family,
        "`, is not available on the system. Adding alias to `",
        fallback,
        "`"
      ))
      register_variant(family, fallback)
      success <- TRUE
    }
  }

  invisible(success)
}

#' Create import specifications for web content
#'
#' If you create content in a text-based format such as HTML or SVG you need to
#' make sure that the font is available on the computer where it is viewed. This
#' can be achieved through the use of stylesheets that can either be added with
#' a `<link>` tag or inserted with an `@import` statement. This function
#' facilitates the creation of either of these (or the bare URL to the
#' stylesheet). It can rely on the Google Fonts or Font Library repository for
#' serving the fonts. If the requested font is not found it can optionally hard
#' code the data into the stylesheet.
#'
#' @inheritParams match_fonts
#' @param ... Additional arguments passed on to the specific functions for the
#' repositories. Currently:
#' * **Google Fonts:**
#'   - `text` A piece of text containing the glyphs required. Using this can
#'     severely cut down on the size of the required download
#'   - `display` One of `"auto"`, `"block"`, `"swap"`, `"fallback"`, or
#'     `"optional"`. Controls how the text is displayed while the font is
#'     downloading.
#' @param type The type of return value. `"url"` returns the bare url pointing
#' to the style sheet. `"import"` returns the stylesheet as an import statement
#' (`@import url(<url>)`). `"link"` returns the stylesheet as a link tag
#' (`<link rel="stylesheet" href="<url>"/>`)
#' @param may_embed Logical. Should fonts that can't be found in the provided
#' repositories be embedded as data-URLs. This is only possible if the font is
#' available locally and in a `woff2`, `woff`, `otf`, or `ttf` file.
#' @param repositories The repositories to try looking for the font. Currently
#' `"Google Fonts"` and `"Font Library"` are supported. Set this to `NULL`
#' together with `may_embed = TRUE` to force embedding of the font data.
#'
#' @return A character vector with stylesheet specifications according to `type`
#' @export
#'
fonts_as_import <- function(
  family,
  italic = NULL,
  weight = NULL,
  width = NULL,
  ...,
  type = c("url", "import", "link"),
  may_embed = TRUE,
  repositories = c("Google Fonts", "Font Library")
) {
  import <- character(0)
  type <- match.arg(type)
  if (may_embed) repositories <- c(repositories, "local")

  for (repo in repositories) {
    fonts <- switch(
      tolower(repo),
      "google fonts" = import_from_google_fonts(
        family,
        italic = italic,
        weight = weight,
        width = width,
        ...
      ),
      "font library" = import_from_font_library(family, ...),
      "local" = import_embedded(
        family,
        italic = italic,
        weight = weight,
        width = width,
        ...
      ),
      NULL
    )
    if (!is.null(fonts)) {
      import <- c(import, fonts)
      missing <- attr(fonts, "no_match")

      family <- family[missing]
      if (!is.null(italic)) italic <- italic[missing]
      if (!is.null(width)) width <- width[missing]
      if (!is.null(weight)) weight <- weight[missing]
    }
    if (length(family) == 0) break
  }

  if (length(family) != 0) {
    warning("No import found for ", paste(family, collapse = ", "))
  }

  switch(
    type,
    import = paste0("@import url('", import, "');"),
    link = paste0('<link rel="stylesheet" href="', import, '"/>'),
    url = import
  )
}

import_from_google_fonts <- function(
  family,
  italic = NULL,
  weight = NULL,
  width = NULL,
  ...,
  text = NULL,
  display = "swap"
) {
  n_fonts <- max(length(family), length(italic), length(weight), length(width))
  family <- rep_len(family, n_fonts)
  if (!is.null(italic)) italic <- rep_len(as.integer(italic), n_fonts)
  if (!is.null(weight))
    weight <- rep_len(systemfonts::as_font_weight(weight), n_fonts)
  if (!is.null(width))
    width <- rep_len(systemfonts::as_font_width(width), n_fonts)

  clusters <- split(seq_along(family), family)

  possible_fonts <- unique(get_google_fonts_registry()$family)

  fonts <- lapply(clusters, function(i) {
    fam <- family[i[1]]
    fam <- match(tolower(fam), tolower(possible_fonts))
    if (is.na(fam)) return()
    fam <- possible_fonts[fam]
    fam <- paste0("family=", gsub(" ", "+", family[i[1]]))

    spec <- list()
    spec$ital <- if (!is.null(italic)) unique(range(italic[i]))
    if (isTRUE(spec$ital == 0L)) spec$ital <- NULL
    spec$wdth <- if (!is.null(width))
      paste0(unique(range(width[i])), collapse = "..")
    spec$wght <- if (!is.null(weight))
      paste0(unique(range(weight[i])), collapse = "..")
    spec <- spec[lengths(spec) != 0]
    if (length(spec) != 0) {
      spec$sep <- ","
      val <- paste0(do.call(paste0, spec), collapse = ";")
      spec <- paste0(names(spec), collapse = ",")
      fam <- paste0(fam, ":", spec, "@", val)
    }
    fam
  })
  missing <- unlist(clusters[lengths(fonts) == 0]) %||% integer()
  fonts <- paste0(unlist(fonts), collapse = "&")

  display <- match.arg(
    display,
    c("auto", "block", "swap", "fallback", "optional")
  )

  url <- paste0("https://fonts.googleapis.com/css2?", fonts)

  if (display != "auto") {
    url <- paste0(url, "&display=", display)
  }

  if (!is.null(text)) {
    url <- paste0(url, "&text=", utils::URLencode(text))
  }

  success <- try(suppressWarnings(readLines(url, n = 1)), silent = TRUE)
  if (inherits(success, "try-error")) {
    url <- character(0)
    missing <- seq_along(family)
  }
  structure(url %||% character(), no_match = missing)
}
import_from_font_library <- function(family, ...) {
  family <- gsub(" ", "-", tolower(family))
  u_fam <- unique(family)
  names(u_fam) <- u_fam
  fonts <- lapply(u_fam, function(name) {
    url <- paste0("https://fontlibrary.org/face/", name)
    if (length(readLines(url, n = 1)) != 0) {
      url
    } else {
      NULL
    }
  })
  missing <- which(family %in% names(fonts[lengths(fonts) == 0]))
  fonts <- unlist(fonts)

  structure(fonts %||% character(), no_match = missing, names = NULL)
}
import_embedded <- function(
  family,
  italic = NULL,
  weight = NULL,
  width = NULL,
  ...
) {
  fonts <- match_fonts(
    family,
    italic = italic %||% FALSE,
    weight = weight %||% "normal",
    width = width %||% "undefined"
  )
  fonts <- lapply(seq_along(family), function(i) {
    found <- font_info(path = fonts$path[i], index = fonts$index[i])
    if (tolower(family[i]) != tolower(found$family)) return()
    ext <- tools::file_ext(fonts$path[i])
    if (!ext %in% c("woff2", "woff", "otf", "ttf")) {
      warning(ext, "-files cannot be embedded (", family[i], ")")
      return()
    }
    format <- switch(
      ext,
      woff2 = 'format("woff2")',
      woff = 'format("woff")',
      otf = 'format("opentype")',
      ttf = 'format("truetype")'
    )
    src <- paste0(
      "url(data:font/",
      ext,
      ";charset=utf-8;base64,",
      base64enc::base64encode(fonts$path[i]),
      ") ",
      format
    )
    features <- paste0(
      '"',
      fonts$features[[i]][[1]],
      '" ',
      fonts$features[[i]][[2]],
      collapse = ", "
    )
    # fmt: skip
    x <- paste0(
      '@font-face {\n',
      '  font-family: "', family[i], '";\n',
      '  src: ', src, ';\n',
      if (length(fonts$features[[i]]) != 0) paste0(
      '  font-feature-settings: ', features, ';\n'),
      if (!is.na(found$width)) paste0(
      '  font-stretch: ', sub("(ra|mi)", "\\1-", found$width), ';\n'),
      if (!is.na(found$weight)) paste0(
      '  font-weight: ', as_font_weight(found$weight), ';\n'),
      '  font-style: ', if (found$italic) "italic" else "normal", ';\n',
      '}'
    )
    paste0("data:text/css,", utils::URLencode(x))
  })
  missing <- which(lengths(fonts) == 0)
  fonts <- unlist(fonts)

  structure(fonts %||% character(), no_match = missing)
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
    google_fonts_registry[[loc]] <- do.call(
      rbind,
      lapply(fonts, function(x) {
        x <- data.frame(
          family = x$family,
          variant = unlist(x$variants),
          url = unlist(x$files),
          file = NA_character_,
          version = x$version,
          modified = x$lastModified,
          category = I(rep(list(x$category), length(x$variants)))
        )
        x$file <- paste0(
          x$family,
          "-",
          x$variant,
          sub("^.*(\\.\\w+)$", "\\1", x$url)
        )
        attr(x, "row.names") <- .set_row_names(nrow(x))
        x
      })
    )
  }
  google_fonts_registry[[loc]]
}

font_squirrel_registry <- new.env(parent = emptyenv())
get_font_squirrel_registry <- function() {
  if (is.null(font_squirrel_registry$fonts)) {
    fonts <- jsonlite::read_json(
      "https://www.fontsquirrel.com/api/fontlist/all"
    )
    font_squirrel_registry$fonts <- do.call(
      rbind,
      lapply(fonts, function(x) {
        x <- data.frame(
          family = x$family_name,
          n_variants = x$family_count,
          url = paste0(
            "https://www.fontsquirrel.com/fonts/download/",
            x$family_urlname
          ),
          category = I(list(x$classification))
        )
        attr(x, "row.names") <- .set_row_names(nrow(x))
        x
      })
    )
  }
  font_squirrel_registry$fonts
}
