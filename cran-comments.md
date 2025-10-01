Patch release to address issues reported by CRAN.

* UBSAN address alignment
* Compilation issues on old macOS machines
* Writing outside of tempdir during check

## revdepcheck results

We checked 30 reverse dependencies, comparing R CMD check results across CRAN and dev versions of this package.

 * We saw 0 new problems
 * We failed to check 2 packages

### Failed to check

* OlinkAnalyze (NA)
* TransProR    (NA)
