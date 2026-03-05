# List all fonts installed on your system

List all fonts installed on your system

## Usage

``` r
system_fonts()
```

## Value

A data frame with a row for each font and various information in each
column

## Examples

``` r
# See all monospace fonts
fonts <- system_fonts()
fonts[fonts$monospace, ]
#> # A tibble: 22 × 10
#>    path          index name  family style weight width italic monospace
#>    <chr>         <int> <chr> <chr>  <chr> <ord>  <ord> <lgl>  <lgl>    
#>  1 /usr/share/f…     0 Nimb… Nimbu… Bold… bold   norm… TRUE   TRUE     
#>  2 /usr/share/f…     0 Libe… Liber… Bold  bold   norm… FALSE  TRUE     
#>  3 /usr/share/f…     0 Nimb… Nimbu… Regu… normal norm… FALSE  TRUE     
#>  4 /usr/share/f…     0 Libe… Liber… Ital… normal norm… TRUE   TRUE     
#>  5 /usr/share/f…     0 Noto… Noto … Regu… normal norm… FALSE  TRUE     
#>  6 /usr/share/f…     0 Deja… DejaV… Book  normal norm… FALSE  TRUE     
#>  7 /usr/share/f…     0 Nimb… Nimbu… Ital… normal norm… TRUE   TRUE     
#>  8 /usr/share/f…     0 Deja… DejaV… Bold… bold   norm… FALSE  TRUE     
#>  9 /usr/share/f…     0 Noto… Noto … Regu… normal norm… FALSE  TRUE     
#> 10 /usr/share/f…     0 Deja… DejaV… Bold  bold   norm… FALSE  TRUE     
#> # ℹ 12 more rows
#> # ℹ 1 more variable: variable <lgl>
```
