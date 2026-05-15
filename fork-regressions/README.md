# Fork Regression Corpus

This directory holds local PDFs and other documents that exercise regressions
fixed by this fork.

Put test documents under `data/`. The corpus contents are gitignored so private
or large PDFs do not get committed accidentally. Keep a short note in the file
or directory name describing which fork fix the document covers.

Run the corpus with:

```sh
make fork-regression-test
```

You can also run the script directly and point it at another directory:

```sh
scripts/run-fork-regression-tests.sh /path/to/corpus
```
