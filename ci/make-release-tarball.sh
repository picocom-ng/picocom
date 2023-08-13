#!/bin/bash

set -e

version=$1
ref="${2:-HEAD}"

if [[ -z ${version} ]]; then
    echo "ERROR: no version provided on command line" >&2
    exit 1
fi

tmpdir=$(mktemp -d releaseXXXXXX)
trap "rm -rf ${tmpdir}" EXIT

builddir="${tmpdir}/picocom-${version}"

echo "exporting release"
git archive --prefix "picocom-${version}/" "$ref" | tar -C "$tmpdir" -xf -
echo "VERSION=${version}" > "${builddir}/version.mk"

echo "building docs"
make -C "${builddir}" doc

echo "building CONTRIBUTORS"
cat > "${builddir}/CONTRIBUTORS" <<EOF
The following people have contributed to the development of Picocom:

EOF
git log --format=format:%an | sort -u >> "${builddir}/CONTRIBUTORS"

echo "building release tarball"
mkdir -p release
tar -C "$tmpdir" -cf "release/picocom-${version}.tar.gz" "picocom-${version}"

echo "tarball content:"
tar -tf "release/picocom-${version}.tar.gz"

echo "exctracting pdf man page"
tar --wildcards -C release -xf "release/picocom-${version}.tar.gz" --strip-components 1 "*/picocom.1.pdf" || :

echo "release files:"
ls -l release
