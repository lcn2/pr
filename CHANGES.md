# Major changes to the IOCCC entry toolkit


## Release 1.0.1 2025-09-23

Move a lot of code from the [mkiocccentry
repo](https://github.com/ioccc-src/mkiocccentry), in particular that in
`soup/file_util.[ch]`, to pr.[ch] as they are file functions.

The `pr_test.c` has been updated to be very close to what was once the
`util_test.c` in the [jparse repo](https://github.com/xexyl/jparse).

Updated `PR_VERSION` to `"1.1.3 2025-09-23"`.
Updated `PR_TEST_VERSION` to `"1.1.1 2025-09-23"`.

IMPORTANT NOTE: this commit will require a commit to jparse! This cannot be done
until this commit is merged into master because otherwise it would break code.

IMPORTANT NOTE: this commit requires changes in mkiocccentry including but not
limited to the changes under pr/ (in other words it is more than a sync of the
repo). A pull request will be opened along with this but once that is merged I
can make a commit to jparse. That directory has also changed under
mkiocccentry/jparse so as to not break anything.

IMPORTANT NOTE: we really need a `pr_test.sh` so that the mkiocccentry tests
driver can run `pr_test` (this is due to what it tests). For now that test is
commented out. However as we also do `make test` under `mkiocccentry/pr` this is
probably okay for now.


## Release 1.0.0 2025-09-19

Initial release of `pr.c`, `pr.h`, and  `Makefile`.

Code has been copied over from [mkiocccentry
repo](https://github.com/ioccc-src/mkiocccentry) and from the [jparse
repo](https://github.com/xexyl/jparse).

The code in this from repo was copied out of the [mkiocccentry toolkit
repo](https://github.com/ioccc-src/mkiocccentry) and out of the  [jparse
repo](https://github.com/xexyl/jparse).

The origin of libpr dates back to code written by Landon Curt Noll
around 2008.  That 2008 code was copied into the jparse repo, and the
mkiocccentry toolkit repo by Landon Curt Noll.  While in the jparse repo,
both Landon Curt Noll and Cody Boone Ferguson added to and improved this
code base.
