_make() {
    ${MAKE} -f ${MAKEFILE} "$@"
}

setup() {
    BATS_TMPDIR="$(mktemp -d batsXXXXXX)"
    MAKE=${MAKE:-make}
    MAKEFILE=${MAKEFILE:=Makefile}

    _make fakeserial
    read ptyname ptypid < <(./fakeserial)
}

@test "picocom builds successfully with features disabled" {
    for feature in CONFIGFILE HIGH_BAUD USE_FLOCK LINENOISE HELP; do
        _make distclean all FEATURE_${feature}=0
    done
}

@test "picocom builds successfully with default configuration" {
    _make distclean all FEATURE_${feature}=0
}

@test "picocom runs --help successfully" {
    ./picocom --help
}

@test "picocom runs --exit successfully" {
    timeout 5 ./picocom --exit $ptyname > "${BATS_TMPDIR}/log"
    res=$?
    (( $res == 0 ))
    grep "Terminal ready" "${BATS_TMPDIR}/log"
}

teardown() {
    kill $ptypid
    rm -rf "${BATS_TMPDIR}"
}
