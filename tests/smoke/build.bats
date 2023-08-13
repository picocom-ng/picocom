setup() {
	BATS_TMPDIR="$(mktemp -d batsXXXXXX)"
	gcc -o "${BATS_TMPDIR}/fakeserial" tests/smoke/fakeserial.c
	read ptyname ptypid < <("${BATS_TMPDIR}/fakeserial")
}

@test "picocom builds successfully with features disabled" {
	for feature in CONFIGFILE HIGH_BAUD USE_FLOCK LINENOISE HELP; do
		make realclean all FEATURE_${feature}=0
	done
}

@test "picocom builds successfully" {
	make realclean all
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
