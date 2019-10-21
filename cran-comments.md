This is a small fix addressing the compilation error on Solaris that became 
known once the package got added to CRAN. I hope submitting a new version so
quick after the old one is appropriate in this case. It also address test errors
on the Mac builder due to differences in font file types.

## Test environments
* local OS X install, R 3.6.0
* ubuntu 14.04 (on travis-ci), R 3.6.0
* win-builder (devel and release)
* rhub::check_for_cran()
* rhub::check(platform = 'ubuntu-rchk')
* rhub::check_with_sanitizers()
* rhub::check_on_solaris()

## R CMD check results

0 errors | 0 warnings | 0 note
