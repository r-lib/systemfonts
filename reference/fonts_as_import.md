# Create import specifications for web content

If you create content in a text-based format such as HTML or SVG you
need to make sure that the font is available on the computer where it is
viewed. This can be achieved through the use of stylesheets that can
either be added with a `<link>` tag or inserted with an `@import`
statement. This function facilitates the creation of either of these (or
the bare URL to the stylesheet). It can rely on the Bunny Fonts, Google
Fonts and/or Font Library repositories for serving the fonts. If the
requested font is not found it can optionally hard code the data into
the stylesheet.

## Usage

``` r
fonts_as_import(
  family,
  italic = NULL,
  weight = NULL,
  width = NULL,
  ...,
  type = c("url", "import", "link"),
  may_embed = TRUE,
  repositories = c("Bunny Fonts", "Google Fonts", "Font Library")
)
```

## Arguments

- family:

  The name of the font families to match

- italic:

  logical indicating the font slant

- weight:

  The weight to query for, either in numbers (`0`, `100`, `200`, `300`,
  `400`, `500`, `600`, `700`, `800`, or `900`) or strings
  (`"undefined"`, `"thin"`, `"ultralight"`, `"light"`, `"normal"`,
  `"medium"`, `"semibold"`, `"bold"`, `"ultrabold"`, or `"heavy"`). `NA`
  will be interpreted as `"undefined"`/`0`

- width:

  The width to query for either in numbers (`0`, `1`, `2`, `3`, `4`,
  `5`, `6`, `7`, `8`, or `9`) or strings (`"undefined"`,
  `"ultracondensed"`, `"extracondensed"`, `"condensed"`,
  `"semicondensed"`, `"normal"`, `"semiexpanded"`, `"expanded"`,
  `"extraexpanded"`, or `"ultraexpanded"`). `NA` will be interpreted as
  `"undefined"`/`0`

- ...:

  Additional arguments passed on to the specific functions for the
  repositories. Currently:

  - **Google Fonts and Bunny Fonts:**

    - `text` A piece of text containing the glyphs required. Using this
      can severely cut down on the size of the required download

    - `display` One of `"auto"`, `"block"`, `"swap"`, `"fallback"`, or
      `"optional"`. Controls how the text is displayed while the font is
      downloading.

- type:

  The type of return value. `"url"` returns the bare url pointing to the
  style sheet. `"import"` returns the stylesheet as an import statement
  (`@import url(<url>)`). `"link"` returns the stylesheet as a link tag
  (`<link rel="stylesheet" href="<url>"/>`)

- may_embed:

  Logical. Should fonts that can't be found in the provided repositories
  be embedded as data-URLs. This is only possible if the font is
  available locally and in a `woff2`, `woff`, `otf`, or `ttf` file.

- repositories:

  The repositories to try looking for the font. Currently
  `"Bunny Fonts"`, `"Google Fonts"`, and `"Font Library"` are supported.
  Set this to `NULL` together with `may_embed = TRUE` to force embedding
  of the font data.

## Value

A character vector with stylesheet specifications according to `type`
