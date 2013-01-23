HUE patches
===========

In this directory we maintain a series of patches of HUE in the form
`hue-patch-cdhxxx-yyy.diff` where xxx is the cloudera release version and yyy
is the cosmos release one.

These patches can be applied to the HUE installation (`/usr/share/hue` by
default) by using the `git apply` command:

    git apply -p1 /path/to/patch

Some useful additional flags are:

 * `--reject`. When the patch cannot be cleanly applied `*.rej` files are
   created beside the files to patch to ease manual resolution.
 * `--check`. Dry run.
