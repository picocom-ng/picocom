#!/bin/sh -e
# picocom release script by Wolfram Sang, see LICENSE.txt
tag="$(date +%Y-%m)"
sed -i "/^VERSION/ { s/=.*/= $tag/ }" Makefile Android.mk
cat > CONTRIBUTORS <<EOF
These people have contributed to picocom according to the repository at
https://gitlab.com/wsakernel/picocom. See also there for details about their
contributions.

EOF
git log --format=format:%an | sort -u >> CONTRIBUTORS
make -B doc
git commit -a -s -m "release version $tag, rebuilt docs"
git tag "$tag"
