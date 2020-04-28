This release fixes a bunch of issues in the compiled code, most notably the 
current solaris error as well as the valgrind errors. The configure script has
also been tweaked to better detect system libraries on macOSdevtools::check(remote = TRUE, manual = TRUE)

## Test environments
* local R installation, R 3.6.0
* ubuntu 16.04 (on travis-ci), R 3.6.0
* win-builder (devel)

## R CMD check results

0 errors | 0 warnings | 0 note

## revdepcheck results

We checked 2 reverse dependencies, comparing R CMD check results across CRAN and dev versions of this package.

 * We saw 0 new problems
 * We failed to check 0 packages
 
