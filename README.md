## littlefs: rbyd B-trees: Flexible B-trees when RAM $\ll$ block size


### A hopefully helpful little readme

To run benchmarks:

``` bash
cd benchmarks
make -f Makefile_poster build
PRECISE=1 make -f Makefile_poster bench # takes a while!
# make -f Makefile_poster plot # optional
COMB=16 make -f Makefile_poster tikz
```

To verify results (unfortunately the results seem to be slightly
unstable, uninit mem bug?):

``` bash
make diff-results
# make sync-results # optional
```

To build poster + abstract:

``` bash
make
```
